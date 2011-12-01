#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include <CL/cl.h>

#include "gpuHandler.hpp"

//Input data is stored here.
cl_double4 *origin;//Ray Origin
cl_double4 *dir;//Ray Direction
cl_double4 *vert0;//Tetrahedron vertices 0-3
cl_double4 *vert1;
cl_double4 *vert2;
cl_double4 *vert3;

//Output data is stored here.
cl_double8 *cartesian; //Cartesian Coordinates for the entry and exit points
cl_double4 *barycentric; //Barycentric Coordinates for the same
cl_double2 *parametric; //Parametric distances of entry and exit points from orig

//Actual Ray-Tetrahedron pairs processed. 
cl_uint actualWidth;

//actualWidth padded to a multiple of threadsPerWorkgroup
cl_uint paddedWidth;

//The width of the input and output buffers used
//Must be <= WORK_ITEM_LIMIT
cl_uint bufferWidth;

//The memory buffers that are used as input/output to the OpenCL kernel
cl_mem orig_buf;
cl_mem dir_buf;
cl_mem vert0_buf;
cl_mem vert1_buf;
cl_mem vert2_buf;
cl_mem vert3_buf;

cl_mem cartesian_buf;
cl_mem barycentric_buf;
cl_mem parametric_buf;

//CL Context and kernel objects
cl_context          context;
cl_device_id        *devices;
cl_command_queue    commandQueue;

cl_program program;

cl_kernel  kernel;

//The number of work items(threads) launched (at minimum) for every work group of the target device
//64 for AMD GPUs, a multiple of 32 determined at runtime for Nvidia GPUs, ignored for CPUs
size_t threadsPerWorkgroup = 1;

//Device Name string
//Used to select between precompiled kernels
char deviceName[MAX_NAME_LENGTH];

//Return codes from OpenCL API calls
//Used for error tracking
cl_int status;

      
//Run the CL kernel
//Handles buffer IO
void runCLKernelsWithIO(void)
{
	size_t globalThreads[1];
	size_t localThreads[1];
		
	localThreads[0]= threadsPerWorkgroup;  
		
	//Break up kernel execution to 1 exec per WORK_ITEM_LIMIT work items.
	cl_uint remainingWidth = paddedWidth;
	cl_uint offset = 0;
	cl_event exec_events[1];//An event that tracks kernel execution
		
	while(remainingWidth > WORK_ITEM_LIMIT)
	{	  
	  
	  writeBuffers(offset,WORK_ITEM_LIMIT);
	  //Enqueue a kernel run call.	
	  globalThreads[0] = WORK_ITEM_LIMIT;
	  
	  status = clEnqueueNDRangeKernel(
		  commandQueue,
		  kernel,
		  1,
		  NULL,
		  globalThreads,
		  localThreads,
		  0,
		  NULL,
		  &exec_events[0]);
	  if(status != CL_SUCCESS) exitOnError("Enqueueing kernel onto command queue.(clEnqueueNDRangeKernel)"); 

	
	  status = clWaitForEvents(1,&exec_events[0]);
	  if(status != CL_SUCCESS) exitOnError("Waiting for kernel run to finish.(clWaitForEvents)");
	  
	  readBuffers(offset,WORK_ITEM_LIMIT);
	
	remainingWidth -= WORK_ITEM_LIMIT;
	offset += WORK_ITEM_LIMIT;
	  
	}
		
	//Final kernel run call for remaining work_items
	globalThreads[0] = remainingWidth;
	
	writeBuffers(offset,remainingWidth);
		
	status = clEnqueueNDRangeKernel(
		commandQueue,
		kernel,
		1,
		NULL,
		globalThreads,
		localThreads,
		0,
		NULL,
		&exec_events[0]);
	if(status != CL_SUCCESS) exitOnError("Enqueueing kernel onto command queue.(clEnqueueNDRangeKernel)"); 

	
	status = clWaitForEvents(1,&exec_events[0]);
	if(status != CL_SUCCESS) exitOnError("Waiting for kernel run to finish.(clWaitForEvents)");
	
	readBuffers(offset,remainingWidth);
	
	status = clReleaseEvent(exec_events[0]);
	if(status != CL_SUCCESS) exitOnError("Releasing event object exec_events[0].(clWaitForEvents)");

}

//Run the CL kernel
//Buffer I/O must be handled separately
void runCLKernels(size_t runWidth)
{
	//cout << "Running " << runWidth << " threads." << endl;	
	size_t globalThreads[1];
	size_t localThreads[1];
		
	localThreads[0]= threadsPerWorkgroup;  			
	globalThreads[0] =  runWidth;
	cl_event exec_events[1];//An event that tracks kernel execution
	
	status = clEnqueueNDRangeKernel(
		commandQueue,
		kernel,
		1,
		NULL,
		globalThreads,
		localThreads,
		0,
		NULL,
		&exec_events[0]);
	if(status != CL_SUCCESS) exitOnError("Enqueueing kernel onto command queue.(clEnqueueNDRangeKernel)"); 

	
	status = clWaitForEvents(1,&exec_events[0]);
	if(status != CL_SUCCESS) exitOnError("Waiting for kernel run to finish.(clWaitForEvents)");
	
	status = clReleaseEvent(exec_events[0]);
	if(status != CL_SUCCESS) exitOnError("Releasing event object exec_events[0].(clWaitForEvents)");
}

//Determines the size of the input arrays needed and allocates them
void allocateInput(int actual_width,int deviceNum)
{
	//The size of the input arrays and buffers must be a multiple of the workgroup size
	if(threadsPerWorkgroup == 32)
	{//if this is an Nvidia GPU use the maximum workgroup size allowed by the kernel
	  
	    //Determine the maximum workgroup size allowed on the current device for this kernel.
	    size_t maxGroupSize;
      
	    status = clGetKernelWorkGroupInfo(kernel, 
					  devices[deviceNum], 
					  CL_KERNEL_WORK_GROUP_SIZE,
					  sizeof(size_t),
					  &maxGroupSize, 
					  NULL);
	    if(status != CL_SUCCESS) exitOnError("Cannot get maximum workgroup size for given kernel.(clGetKernelWorkGroupInfo)\n");
	
	    threadsPerWorkgroup = maxGroupSize;
	    //cout << "Work group size: " << maxGroupSize << endl;
	}
	    
	//Determine the amount of false entries to pad the input arrays with.
	//Create a workgroup size of threadsPerWorkgroup 
	if((actual_width % threadsPerWorkgroup) != 0) paddedWidth = actual_width + (threadsPerWorkgroup - (actual_width % threadsPerWorkgroup));
	else paddedWidth = actual_width;
	
	//Memory used to store vector objects must be stored in 16 byte aligned addresses.
	//Mostly needed for 32bit systems.
	#if defined (_WIN32)
	
	//Input Arrays
	origin = (cl_double4 *) _aligned_malloc(paddedWidth * sizeof(cl_double4),16);
	if(origin == NULL) exitOnError("Failed to allocate input memory on host(origin)");

	dir = (cl_double4 *) _aligned_malloc(paddedWidth * sizeof(cl_double4),16);
	if(dir == NULL) exitOnError("Failed to allocate input memory on host(dir)");

	vert0 = (cl_double4 *) _aligned_malloc(paddedWidth * sizeof(cl_double4),16);
	if(vert0 == NULL) exitOnError("Failed to allocate input memory on host(vert0)");

	vert1 = (cl_double4 *) _aligned_malloc(paddedWidth * sizeof(cl_double4),16);
	if(vert1 == NULL) exitOnError("Failed to allocate input memory on host(vert1)");

	vert2 = (cl_double4 *) _aligned_malloc(paddedWidth * sizeof(cl_double4),16);
	if(vert2 == NULL) exitOnError("Failed to allocate input memory on host(vert2)");

	vert3 = (cl_double4 *) _aligned_malloc(paddedWidth * sizeof(cl_double4),16);
	if(vert3 == NULL) exitOnError("Failed to allocate input memory on host(vert3)");

	//Output Arrays
	cartesian = (cl_double8*)_aligned_malloc(paddedWidth * sizeof(cl_double8),16);
	if(cartesian == NULL) exitOnError("Failed to allocate output memory on host(cartesian)");
	
	barycentric = (cl_double4*)_aligned_malloc(paddedWidth * sizeof(cl_double4),16);
	if(barycentric == NULL) exitOnError("Failed to allocate output memory on host(barycentric)");
	
	parametric = (cl_double2*)_aligned_malloc(paddedWidth * sizeof(cl_double2),16);
	if(parametric == NULL) exitOnError("Failed to allocate output memory on host(parametric)");
	

      #elif defined _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600	
	
	//Input Arrays
	status = posix_memalign((void**)&origin,16,paddedWidth * sizeof(cl_double4));
	if(status != 0) exitOnError("Failed to allocate input memory on host(origin)");

	status = posix_memalign((void**)&dir,16,paddedWidth * sizeof(cl_double4));
	if(status != 0) exitOnError("Failed to allocate input memory on host(dir)");

	status = posix_memalign((void**)&vert0,16,paddedWidth * sizeof(cl_double4));
	if(status != 0) exitOnError("Failed to allocate input memory on host(vert0)");

	status = posix_memalign((void**)&vert1,16,paddedWidth * sizeof(cl_double4));
	if(status != 0) exitOnError("Failed to allocate input memory on host(vert1)");

	status = posix_memalign((void**)&vert2,16,paddedWidth * sizeof(cl_double4));
	if(status != 0) exitOnError("Failed to allocate input memory on host(vert2)");

	status = posix_memalign((void**)&vert3,16,paddedWidth * sizeof(cl_double4));
	if(status != 0) exitOnError("Failed to allocate input memory on host(vert3)");

	//Output Arrays
	status = posix_memalign((void**)&cartesian,16,paddedWidth * sizeof(cl_double8));
	if(status != 0) exitOnError("Failed to allocate input memory on host(cartesian)");
	
	status = posix_memalign((void**)&barycentric,16,paddedWidth * sizeof(cl_double4));
	if(status != 0) exitOnError("Failed to allocate input memory on host(berycentric)");
	
	status = posix_memalign((void**)&parametric,16,paddedWidth * sizeof(cl_double2));
	if(status != 0) exitOnError("Failed to allocate input memory on host(parametric)");

	#else

	origin = (cl_double4 *) malloc(paddedWidth * sizeof(cl_double4));
	if(origin == NULL) exitOnError("Failed to allocate input memory on host(origin)");

	dir = (cl_double4 *) malloc(paddedWidth * sizeof(cl_double4));
	if(dir == NULL) exitOnError("Failed to allocate input memory on host(dir)");

	vert0 = (cl_double4 *) malloc(paddedWidth * sizeof(cl_double4));
	if(vert0 == NULL) exitOnError("Failed to allocate input memory on host(vert0)");

	vert1 = (cl_double4 *) malloc(paddedWidth * sizeof(cl_double4));
	if(vert1 == NULL) exitOnError("Failed to allocate input memory on host(vert1)");

	vert2 = (cl_double4 *) malloc(paddedWidth * sizeof(cl_double4));
	if(vert2 == NULL) exitOnError("Failed to allocate input memory on host(vert2)");

	vert3 = (cl_double4 *) malloc(paddedWidth * sizeof(cl_double4));
	if(vert3 == NULL) exitOnError("Failed to allocate input memory on host(vert3)");

	//Output Arrays
	cartesian = (cl_double8*)malloc(paddedWidth * sizeof(cl_double8));
	if(cartesian == NULL) exitOnError("Failed to allocate output memory on host(cartesian)");
	
	barycentric = (cl_double4*)malloc(paddedWidth * sizeof(cl_double4));
	if(barycentric == NULL) exitOnError("Failed to allocate output memory on host(barycentric)");
	
	parametric = (cl_double2*)malloc(paddedWidth * sizeof(cl_double2));
	if(parametric == NULL) exitOnError("Failed to allocate output memory on host(parametric)");
	#endif
	
}

//Create Input/Output buffers and assign them as kernel arguments
void allocateBuffers(void)
{
	
	bufferWidth = (paddedWidth <= WORK_ITEM_LIMIT) ? paddedWidth:WORK_ITEM_LIMIT;

	// Create OpenCL memory buffers
	//Input buffers
	orig_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * bufferWidth,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(orig)"); 

	dir_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * bufferWidth,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(dir)"); 


	vert0_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * bufferWidth,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert0)"); 

	vert1_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * bufferWidth,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert1)");

	vert2_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * bufferWidth,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert2)"); 

	vert3_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * bufferWidth,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert3)"); 

	//Output buffers
	cartesian_buf = clCreateBuffer(
		context, 
		CL_MEM_WRITE_ONLY,
		sizeof(cl_double8) * bufferWidth,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(cartesian_buf)");

	barycentric_buf = clCreateBuffer(
	  context, 
	  CL_MEM_WRITE_ONLY,
	  sizeof(cl_double4) * bufferWidth,
	  NULL, 
	  &status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(barycentric_buf)");

	parametric_buf = clCreateBuffer(
	  context, 
	  CL_MEM_WRITE_ONLY,
	  sizeof(cl_double2) * bufferWidth,
	  NULL, 
	  &status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(parametric_buf)");
	
	//Assign the buffers as kernel arguments
	status = clSetKernelArg(
		kernel, 
		0, 
		sizeof(cl_mem), 
		(void *)&orig_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (orig_buf)"); 

	status = clSetKernelArg(
		kernel, 
		1, 
		sizeof(cl_mem), 
		(void *)&dir_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (dir_buf)"); 

	status = clSetKernelArg(
		kernel, 
		2, 
		sizeof(cl_mem), 
		(void *)&vert0_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (vert0_buf)");

	status = clSetKernelArg(
		kernel, 
		3, 
		sizeof(cl_mem), 
		(void *)&vert1_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (vert1_buf)"); 

	status = clSetKernelArg(
		kernel, 
		4, 
		sizeof(cl_mem), 
		(void *)&vert2_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (vert2_buf)"); 

	status = clSetKernelArg(
		kernel, 
		5, 
		sizeof(cl_mem), 
		(void *)&vert3_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (vert3_buf)"); 

	status = clSetKernelArg(
		kernel, 
		6, 
		sizeof(cl_mem), 
		(void *)&cartesian_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (cartesian_buf)"); 
	
	status = clSetKernelArg(
		kernel, 
		7, 
		sizeof(cl_mem), 
		(void *)&barycentric_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (cartesian_buf)"); 
	
		status = clSetKernelArg(
		kernel, 
		8, 
		sizeof(cl_mem), 
		(void *)&parametric_buf);
	if(status != CL_SUCCESS) exitOnError("Setting kernel argument. (cartesian_buf)"); 

}

//Fills input buffers with data
void writeBuffers(cl_uint offset,cl_uint entriesToWrite)
{
	//cout << "Writing " << entriesToWrite << " entries into buffers with offset "<< offset << endl;	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  orig_buf,
		  CL_TRUE,
		  0,
		  sizeof(cl_double4) * entriesToWrite,
		  origin + offset,
		  0,
		  NULL,
		  NULL); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (orig_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  dir_buf,
		  CL_TRUE,
		  0,
		  sizeof(cl_double4) * entriesToWrite,
		  dir + offset,
		  0,
		  NULL,
		  NULL); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (dir_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert0_buf,
		  CL_TRUE,
		  0,
		  sizeof(cl_double4) * entriesToWrite,
		  vert0 + offset,
		  0,
		  NULL,
		  NULL); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert0_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert1_buf,
		  CL_TRUE,
		  0,
		  sizeof(cl_double4) * entriesToWrite,
		  vert1 + offset,
		  0,
		  NULL,
		  NULL); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert1_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert2_buf,
		  CL_TRUE,
		  0,
		  sizeof(cl_double4) * entriesToWrite,
		  vert2 + offset,
		  0,
		  NULL,
		  NULL); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert2_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert3_buf,
		  CL_TRUE,
		  0,
		  sizeof(cl_double4) * entriesToWrite,
		  vert3 + offset,
		  0,
		  NULL,
		  NULL); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert3_buf)");
  
}

//Reads data from output buffers
void readBuffers(cl_uint offset,cl_uint entriesToRead)
{
  
	//cout << "Reading " << entriesToRead << " entries from buffers with offset "<< offset << endl;	  
	status = clEnqueueReadBuffer(
		commandQueue,
		cartesian_buf,
		CL_TRUE,
		0,
		entriesToRead * sizeof(cl_double8),
		cartesian + offset,
		0,
		NULL,
		NULL);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	status = clEnqueueReadBuffer(
		commandQueue,
		barycentric_buf,
		CL_TRUE,
		0,
		entriesToRead * sizeof(cl_double4),
		barycentric + offset,
		0,
		NULL,
		NULL);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	
	status = clEnqueueReadBuffer(
		commandQueue,
		parametric_buf,
		CL_TRUE,
		0,
		entriesToRead * sizeof(cl_double2),
		parametric + offset,
		0,
		NULL,
		NULL);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
  
}

//OpenCL related initialization 
//Create Context, Device list, Command Queue
void initializeCL(int deviceNum)
{
	size_t deviceListSize;
	//Identify available platforms and select one
	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if(status != CL_SUCCESS) exitOnError("Getting Platforms. (clGetPlatformsIDs)");
	if(numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id *)malloc(numPlatforms*sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		if(status != CL_SUCCESS) exitOnError("Getting Platform Ids. (clGetPlatformsIDs)");
		for(cl_uint i=0; i < numPlatforms; ++i)
		{
			char pbuff[MAX_NAME_LENGTH];
			status = clGetPlatformInfo(
				platforms[i],
				CL_PLATFORM_VENDOR,
				sizeof(pbuff),
				pbuff,
				NULL);
			if(status != CL_SUCCESS) exitOnError("Getting Platform Info. (clGetPlatformInfo)");
			platform = platforms[i];
			//AMD or Nvidia Platforms are preferred
			//If none of them are found the first available platform is used
			if((!strcmp(pbuff, "Advanced Micro Devices, Inc.")) ||(!strcmp(pbuff,"NVIDIA Corporation")))
			{
				break;
			}
		}
		delete platforms;
	}
	if(NULL == platform) exitOnError("NULL platform found.");

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

	// Create an OpenCL context
	context = clCreateContextFromType(cps, 
		CL_DEVICE_TYPE_ALL, 
		NULL, 
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Creating Context. (clCreateContextFromType).");

	//First, get the size of device list data
	status = clGetContextInfo(context, 
		CL_CONTEXT_DEVICES, 
		0, 
		NULL, 
		&deviceListSize);
	if(status != CL_SUCCESS) exitOnError("Getting Context Info(device list size, clGetContextInfo).");

	//Detect OpenCL devices
	devices = (cl_device_id *)malloc(deviceListSize);
	if(deviceListSize == 0) exitOnError("No devices found.");
	if((deviceListSize/sizeof(size_t)) < (deviceNum +1)) exitOnError("No device found for given device number.");

	//Now, get the device list data
	status = clGetContextInfo(
		context, 
		CL_CONTEXT_DEVICES, 
		deviceListSize, 
		devices, 
		NULL);
	if(status != CL_SUCCESS) exitOnError("Getting Context Info (device list, clGetContextInfo).");

	//Create an OpenCL command queue on the device with the given deviceNum
	commandQueue = clCreateCommandQueue(
		context, 
		devices[deviceNum], 
		0, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Creating Command Queue. (clCreateCommandQueue).");
	
	//Identify the target device
	status = clGetDeviceInfo(devices[deviceNum],CL_DEVICE_NAME,MAX_NAME_LENGTH,deviceName,NULL);
	if (status != CL_SUCCESS) exitOnError("Cannot get device name for given device number(clGetDeviceInfo)");
	
	printf("Using Device:%s\n",deviceName);
	
	//Identify the device's vendor (used to set threadsPerWorkgroup)
	char deviceVendorName[MAX_NAME_LENGTH];
	status = clGetDeviceInfo(devices[deviceNum],CL_DEVICE_VENDOR,MAX_NAME_LENGTH * sizeof(char),deviceVendorName,NULL);
	if (status != CL_SUCCESS) exitOnError("Cannot get device vendor's name for given device number(clGetDeviceInfo)");
		
	//Identify the device's type (used to set threadsPerWorkgroup)
	cl_device_type deviceType;
	status = clGetDeviceInfo(devices[deviceNum],CL_DEVICE_TYPE,sizeof(cl_device_type),&deviceType,NULL);
	if (status != CL_SUCCESS) exitOnError("Cannot get device type for given device number(clGetDeviceInfo)");
	
	if (deviceType == CL_DEVICE_TYPE_CPU) threadsPerWorkgroup = 1;
	else if(!strcmp(deviceVendorName,"Advanced Micro Devices, Inc.")) threadsPerWorkgroup = 64;
	else if(!strcmp(deviceVendorName,"NVIDIA Corporation")) threadsPerWorkgroup = 32;
  
}


//Load CL file, compile, link CL source
//Try to find a precompiled binary
//Build program and kernel objects	
void makeCLKernel(const char *kernelName,int deviceNum)
{
	//Kernel Loading/Compilation		
	
	//Attempt to load precompiled binary file for target device
	char binFileName[MAX_NAME_LENGTH];
	sprintf(binFileName,"%s_%s.elf",kernelName,deviceName);
	std::fstream inBinFile(binFileName, (ios::in|ios::binary|ios::ate));
	
	//Compile
	if(inBinFile.fail()) {
		inBinFile.close();	
		printf("No binary image found for specified kernel.Compiling from source.\n");
	  
		char sourceFileName[MAX_NAME_LENGTH]; 
		sprintf(sourceFileName,"%s.cl",kernelName);
		std::string  sourceStr = convertToString(sourceFileName);
		if (sourceStr == "NULL") exitOnError("Cannot read kernel source file");
		const char *source = (char*)sourceStr.c_str(); 
		size_t sourceSize[]    = { strlen(source) };

		program = clCreateProgramWithSource(
		context, 
		1, 
		&source,
		sourceSize,
		&status);
		if(status != CL_SUCCESS) exitOnError("Loading Source into cl_program (clCreateProgramWithSource)");
	
		// create a cl program executable for all the devices specified 
		status = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if (status != CL_SUCCESS) {
		  
		  //Build failed.
		  printf("Error building program(clBuildProgram)\nCompiler Output:\n");
		  
		  //Retrieve the size of the build log
		  char* build_log;
		  size_t log_size;
		  status = clGetProgramBuildInfo(program, devices[deviceNum], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		  if(status != CL_SUCCESS) exitOnError("Cannot get compiler log size.(clGetProgramBuildInfo)");
		  build_log = (char*)malloc(log_size+1);
		  
		  // Get and display the build log
		  status = clGetProgramBuildInfo(program, devices[deviceNum], CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
		  if(status != CL_SUCCESS) exitOnError("Cannot get compiler log.(clGetProgramBuildInfo)");
		  build_log[log_size] = '\0';
		  printf("%s\n",build_log);
		  free (build_log);
		  printf("End of Compiler Output.\n");
		  exit(1);
				  
		}

		//Export Binary Images to file
		dumpBinary(program,kernelName);
	  
	}else{
		    
	  size_t  *binSize = (size_t*)malloc(sizeof(size_t));	  
	  char* bin;  
	  *binSize = inBinFile.tellg();
	  inBinFile.seekg (0, ios::beg);
	  bin = new char[*binSize];
	  inBinFile.read(bin,*binSize);
	  inBinFile.close();
		  
	  program = clCreateProgramWithBinary(
	    context, 
	    1, 
	    &devices[deviceNum],
	    binSize,
	    (const unsigned char **)&bin,
	    NULL,
	  &status);
	  if(status != CL_SUCCESS) exitOnError("Loading Binary into cl_program (clCreateProgramWithBinary)");
	    
	  // create a cl program executable for all the devices specified 
	  status = clBuildProgram(program,1, &devices[deviceNum], NULL, NULL, NULL);
	  if (status != CL_SUCCESS) {
		  
		  //Build failed.
		  printf("Error building program(clBuildProgram)\nCompiler Output:\n");
		  
		  //Retrieve the size of the build log
		  char* build_log;
		  size_t log_size;
		  status = clGetProgramBuildInfo(program, devices[deviceNum], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		  if(status != CL_SUCCESS) exitOnError("Cannot get compiler log size.(clGetProgramBuildInfo)");
		  build_log = (char*)malloc(log_size+1);
		  
		  // Get and display the build log
		  status = clGetProgramBuildInfo(program, devices[deviceNum], CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
		  if(status != CL_SUCCESS) exitOnError("Cannot get compiler log.(clGetProgramBuildInfo)");
		  build_log[log_size] = '\0';
		  printf("%s\n",build_log);
		  free (build_log);
		  printf("End of Compiler Output.");
		  exit(1);
				  
		}

	}
	
	// get a kernel object handle for a kernel with the given name 
	kernel = clCreateKernel(program,kernelName,&status);
	if(status != CL_SUCCESS) exitOnError("Creating Kernel from program. (clCreateKernel)");

}

//Converts the contents of a file into a string
//Used to feed kernel source code to the OpenCL Compiler
std::string convertToString(const char *filename)
{
	size_t size;
	char*  str;
	std::string s;

	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = f.tellg();
		f.seekg(0, std::fstream::beg);

		str = new char[size+1];
		if(!str)
		{
			f.close();
			return NULL;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';

		s = str;
		delete[] str;
		return s;
	}
	return "NULL";

  
}

//Dumps a compiled kernel to a binary file
void dumpBinary(cl_program program,const char *kernelName)
{
	
	//Get number of devices
	cl_uint deviceCount = 0;
	status = clGetProgramInfo(program,CL_PROGRAM_NUM_DEVICES,sizeof(cl_uint),&deviceCount,NULL);
	if(status != CL_SUCCESS) exitOnError("Getting number of devices for the program(clGetProgramInfo)");
	
	//Get sizes of compiled binaries for said devices
	size_t *binSize = (size_t*)malloc(sizeof(size_t)*deviceCount);
	status = clGetProgramInfo(program,CL_PROGRAM_BINARY_SIZES,(sizeof(size_t)*deviceCount),binSize,NULL);
	if(status != CL_SUCCESS) exitOnError("Getting binary sizes for the program(clGetProgramInfo)");

	char **bin = ( char**)malloc(sizeof(char*)*deviceCount);
	for(cl_uint i = 0;i<deviceCount;i++) bin[i] = (char*)malloc(binSize[i]);

	//Retrieve compiled binaries
	status = clGetProgramInfo(program,CL_PROGRAM_BINARIES,(sizeof(size_t)*deviceCount),bin,NULL);
	if(status != CL_SUCCESS) exitOnError("Getting program binaries(clGetProgramInfo)");

	//Export binaries to files, appending CL_DEVICE_NAME to each filename
	char binFileName[MAX_NAME_LENGTH];
	for(cl_uint i = 0;i<deviceCount;i++)
	{
	  char deviceName[MAX_NAME_LENGTH];
	  status = clGetDeviceInfo(devices[i],CL_DEVICE_NAME,MAX_NAME_LENGTH,deviceName,NULL);
	  if (status != CL_SUCCESS) exitOnError("Cannot get device name for given device number");
	  printf("Binary image of kernel %s created for device %s.\n",kernelName,deviceName);
	  sprintf(binFileName,"%s_%s.elf",kernelName,deviceName);
	  std::fstream outBinFile(binFileName, (std::fstream::out | std::fstream::binary));
	  if(outBinFile.fail()) exitOnError("Cannot open binary file");
	  outBinFile.write(bin[i],binSize[i]);
	  outBinFile.close();
	}
      for(cl_uint i = 0;i<deviceCount;i++) free(bin[i]);
}

//Releases all CL objects
void cleanupCL(void)
{

	status = clReleaseKernel(kernel);
	if(status != CL_SUCCESS) exitOnError("In clReleaseKernel \n");

	status = clReleaseProgram(program);
	if(status != CL_SUCCESS) exitOnError("In clReleaseProgram\n");

	status = clReleaseMemObject(orig_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (orig_buf)\n");

	status = clReleaseMemObject(dir_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (dir_buf)\n");

	status = clReleaseMemObject(vert0_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (vert0_buf)\n");

	status = clReleaseMemObject(vert1_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (vert1_buf)\n");

	status = clReleaseMemObject(vert2_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (vert2_buf)\n");

	status = clReleaseMemObject(vert3_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (vert3_buf)\n");

	status = clReleaseMemObject(cartesian_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (cartesian_buf)\n");
	
	status = clReleaseMemObject(barycentric_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (barycentric_buf)\n");
	
	status = clReleaseMemObject(parametric_buf);
	if(status != CL_SUCCESS) exitOnError("In clReleaseMemObject (parametric_buf)\n");
		
	status = clReleaseCommandQueue(commandQueue);
	if(status != CL_SUCCESS) exitOnError("In clReleaseCommandQueue\n");

	status = clReleaseContext(context);
	if(status != CL_SUCCESS) exitOnError("In clReleaseContext\n");
	
	if(devices != NULL)
	{
		free(devices);
		devices = NULL;
	}

}


//Releases all objects created in host memory (input and output arrays)
void cleanupHost(void)
{
#if defined (_WIN32)
	if(origin != NULL)
	{
		_aligned-free(origin);
		origin = NULL;
	}
	if(dir != NULL)
	{
		_aligned-free(dir);
		dir = NULL;
	}
	if(vert0 != NULL)
	{
		_aligned-free(vert0);
		vert0 = NULL;
	}
	if(vert1 != NULL)
	{
		_aligned-free(vert1);
		vert1 = NULL;
	}
	if(vert2 != NULL)
	{
		_aligned-free(vert2);
		vert2 = NULL;
	}
	if(vert3 != NULL)
	{
		_aligned-free(vert3);
		vert3 = NULL;
	}

	if(cartesian != NULL)
	{
		_aligned-free(cartesian);
		cartesian = NULL;
	}
	if(barycentric != NULL)
	{
		_aligned-free(barycentric);
		barycentric = NULL;
	}
	if(parametric != NULL)
	{
		_aligned-free(parametric);
		parametric = NULL;
	}

#else
	if(origin != NULL)
	{
		free(origin);
		origin = NULL;
	}
	if(dir != NULL)
	{
		free(dir);
		dir = NULL;
	}
	if(vert0 != NULL)
	{
		free(vert0);
		vert0 = NULL;
	}
	if(vert1 != NULL)
	{
		free(vert1);
		vert1 = NULL;
	}
	if(vert2 != NULL)
	{
		free(vert2);
		vert2 = NULL;
	}
	if(vert3 != NULL)
	{
		free(vert3);
		vert3 = NULL;
	}

	if(cartesian != NULL)
	{
		free(cartesian);
		cartesian = NULL;
	}
	if(barycentric != NULL)
	{
		free(barycentric);
		barycentric = NULL;
	}
	if(parametric != NULL)
	{
		free(parametric);
		parametric = NULL;
	}
#endif
	
}

//Prints error messages with corresponding CL Status codes
void exitOnError(const char *error_text)
{
	fprintf(stderr,"Error:%s\n",error_text);
	fprintf(stderr,"CL Status:%d\n",status);
	exit(1);

}
