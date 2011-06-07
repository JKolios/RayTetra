#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "gpuHandler.hpp"
       
//Bind host variables to kernel arguments and run the CL kernel
void runCLKernels(void)
{
	
	cl_event read_events[3];
	cl_event write_events[6];
	cl_event exec_events[1];
	
	size_t globalThreads[1];
	size_t localThreads[1];
	
	
	localThreads[0]= DEVICE_WORK_ITEMS_PER_WAVEFRONT;  
		

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

	//Break up kernel execution to 1 exec per DEVICE_WORK_ITEMS_PER_LAUNCH work items.
	cl_uint remaining_width = padded_width;
	cl_uint buffer_offset = 0;
		
	while(remaining_width > DEVICE_WORK_ITEMS_PER_LAUNCH)
	{	  
	  	status = clEnqueueWriteBuffer(
		  commandQueue,
		  orig_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * buffer_width,
		  origin + buffer_offset,
		  0,
		  NULL,
		  &write_events[0]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (orig_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  dir_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * buffer_width,
		  dir + buffer_offset,
		  0,
		  NULL,
		  &write_events[1]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (dir_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert0_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * buffer_width,
		  vert0 + buffer_offset,
		  0,
		  NULL,
		  &write_events[2]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert0_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert1_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * buffer_width,
		  vert1 + buffer_offset,
		  0,
		  NULL,
		  &write_events[3]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert1_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert2_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * buffer_width,
		  vert2 + buffer_offset,
		  0,
		  NULL,
		  &write_events[4]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert2_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert3_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * buffer_width,
		  vert3 + buffer_offset,
		  0,
		  NULL,
		  &write_events[5]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert3_buf)");
	
	status = clWaitForEvents(6, write_events);
	if(status != CL_SUCCESS) exitOnError(" Waiting for write buffer calls to finish.\n");
	
	  //Enqueue a kernel run call.	
	  globalThreads[0] = DEVICE_WORK_ITEMS_PER_LAUNCH;
	  //printf("Running kernel with work_items:%zu \n",globalThreads[0]);
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
	  if(status != CL_SUCCESS) exitOnError(" Waiting for kernel run to finish.(clWaitForEvents)");

	  status = clReleaseEvent(exec_events[0]);
	  if(status != CL_SUCCESS) exitOnError(" clReleaseEvent. (exec_events[0])");
	  
	  status = clEnqueueReadBuffer(
		commandQueue,
		cartesian_buf,
		CL_FALSE,
		0,
		buffer_width * sizeof(cl_double8),
		cartesian + buffer_offset,
		0,
		NULL,
		&read_events[0]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	status = clEnqueueReadBuffer(
		commandQueue,
		barycentric_buf,
		CL_FALSE,
		0,
		buffer_width * sizeof(cl_double4),
		barycentric+ buffer_offset,
		0,
		NULL,
		&read_events[1]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	
	status = clEnqueueReadBuffer(
		commandQueue,
		parametric_buf,
		CL_FALSE,
		0,
		buffer_width * sizeof(cl_double2),
		parametric+ buffer_offset,
		0,
		NULL,
		&read_events[2]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	status = clWaitForEvents(3, read_events);
	if(status != CL_SUCCESS) exitOnError(" Waiting for read buffer calls to finish.\n");
	
	 remaining_width -= DEVICE_WORK_ITEMS_PER_LAUNCH;
	 buffer_offset += DEVICE_WORK_ITEMS_PER_LAUNCH;
	  
	}
		
	//Final kernel run call for remaining work_items
	globalThreads[0] = remaining_width;
	//printf("Running kernel with work_items:%zu\n",globalThreads[0]);
	
		 status = clEnqueueWriteBuffer(
		  commandQueue,
		  orig_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * remaining_width,
		  origin + buffer_offset,
		  0,
		  NULL,
		  &write_events[0]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (orig_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  dir_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * remaining_width,
		  dir + buffer_offset,
		  0,
		  NULL,
		  &write_events[1]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (dir_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert0_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * remaining_width,
		  vert0 + buffer_offset,
		  0,
		  NULL,
		  &write_events[2]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert0_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert1_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * remaining_width,
		  vert1 + buffer_offset,
		  0,
		  NULL,
		  &write_events[3]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert1_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert2_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * remaining_width,
		  vert2 + buffer_offset,
		  0,
		  NULL,
		  &write_events[4]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert2_buf)");
	
		status = clEnqueueWriteBuffer(
		  commandQueue,
		  vert3_buf,
		  CL_FALSE,
		  0,
		  sizeof(cl_double4) * remaining_width,
		  vert3 + buffer_offset,
		  0,
		  NULL,
		  &write_events[5]); 				  
	if(status != CL_SUCCESS) exitOnError("Writing to input buffer. (vert3_buf)");
	
	status = clWaitForEvents(6, write_events);
	if(status != CL_SUCCESS) exitOnError(" Waiting for write buffer calls to finish.\n");
	
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
	if(status != CL_SUCCESS) exitOnError(" Waiting for kernel run to finish.(clWaitForEvents)");

	status = clReleaseEvent(exec_events[0]);
	if(status != CL_SUCCESS) exitOnError(" clReleaseEvent. (exec_events[0])");
	

	
	status = clEnqueueReadBuffer(
		commandQueue,
		cartesian_buf,
		CL_FALSE,
		0,
		remaining_width * sizeof(cl_double8),
		cartesian + buffer_offset,
		0,
		NULL,
		&read_events[0]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	status = clEnqueueReadBuffer(
		commandQueue,
		barycentric_buf,
		CL_FALSE,
		0,
		remaining_width * sizeof(cl_double4),
		barycentric+ buffer_offset,
		0,
		NULL,
		&read_events[1]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	
	status = clEnqueueReadBuffer(
		commandQueue,
		parametric_buf,
		CL_FALSE,
		0,
		remaining_width * sizeof(cl_double2),
		parametric+ buffer_offset,
		0,
		NULL,
		&read_events[2]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	status = clWaitForEvents(3, read_events);
	if(status != CL_SUCCESS) exitOnError(" Waiting for read buffer calls to finish.\n");

}

void allocateInput(int actual_width)
{
	
	//Determine the amount of false entries to pad the input arrays with.
	//Create a workgroup size of 64 
	if((actual_width % 64) != 0) padded_width = actual_width + (64 - (actual_width % 64));
	else padded_width = actual_width;
	//printf("Padded Width:%d\n",padded_width);
		
	//Input Arrays
	origin = (cl_double4 *) malloc(padded_width * sizeof(cl_double4));
	if(origin == NULL) exitOnError("Failed to allocate input memory on host(origin)");

	dir = (cl_double4 *) malloc(padded_width * sizeof(cl_double4));
	if(dir == NULL) exitOnError("Failed to allocate input memory on host(dir)");

	vert0 = (cl_double4 *) malloc(padded_width * sizeof(cl_double4));
	if(vert0 == NULL) exitOnError("Failed to allocate input memory on host(vert0)");

	vert1 = (cl_double4 *) malloc(padded_width * sizeof(cl_double4));
	if(vert1 == NULL) exitOnError("Failed to allocate input memory on host(vert1)");

	vert2 = (cl_double4 *) malloc(padded_width * sizeof(cl_double4));
	if(vert2 == NULL) exitOnError("Failed to allocate input memory on host(vert2)");

	vert3 = (cl_double4 *) malloc(padded_width * sizeof(cl_double4));
	if(vert3 == NULL) exitOnError("Failed to allocate input memory on host(vert3)");

	//Output Arrays
	cartesian = (cl_double8*)malloc(padded_width * sizeof(cl_double8));
	if(cartesian == NULL) exitOnError("Failed to allocate output memory on host(cartesian)");
	
	barycentric = (cl_double4*)malloc(padded_width * sizeof(cl_double4));
	if(barycentric == NULL) exitOnError("Failed to allocate output memory on host(barycentric)");
	
	parametric = (cl_double2*)malloc(padded_width * sizeof(cl_double2));
	if(parametric == NULL) exitOnError("Failed to allocate output memory on host(parametric)");
	
}


/*
* Converts the contents of a file into a string
*/
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

/*
* 	  OpenCL related initialization 
*        Create Context, Device list, Command Queue
*        Create OpenCL memory buffer objects
*        Load CL file, compile, link CL source 
*		  Build program and kernel objects
*/
void initializeCL(const char *kernelName)
{
	size_t deviceListSize;

	/*
	* Have a look at the available platforms and pick either
	* the AMD one if available or a reasonable default.
	*/

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
			char pbuff[100];
			status = clGetPlatformInfo(
				platforms[i],
				CL_PLATFORM_VENDOR,
				sizeof(pbuff),
				pbuff,
				NULL);
			if(status != CL_SUCCESS) exitOnError(" Getting Platform Info. (clGetPlatformInfo)");
			platform = platforms[i];
			if(!strcmp(pbuff, "Advanced Micro Devices, Inc."))
			{
				break;
			}
		}
		delete platforms;
	}
	if(NULL == platform) exitOnError(" NULL platform found.");
	/*
	* If we could find our platform, use it. Otherwise use just available platform.
	*/

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };


	/////////////////////////////////////////////////////////////////
	// Create an OpenCL context
	/////////////////////////////////////////////////////////////////
	context = clCreateContextFromType(cps, 
		CL_DEVICE_TYPE_GPU, 
		NULL, 
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError(" Creating Context. (clCreateContextFromType).");

	/* First, get the size of device list data */
	status = clGetContextInfo(context, 
		CL_CONTEXT_DEVICES, 
		0, 
		NULL, 
		&deviceListSize);
	if(status != CL_SUCCESS) exitOnError(" Error: Getting Context Info(device list size, clGetContextInfo).");

	/////////////////////////////////////////////////////////////////
	// Detect OpenCL devices
	/////////////////////////////////////////////////////////////////
	devices = (cl_device_id *)malloc(deviceListSize);
	if(devices == 0) exitOnError(" Error: No devices found.");

	/* Now, get the device list data */
	status = clGetContextInfo(
		context, 
		CL_CONTEXT_DEVICES, 
		deviceListSize, 
		devices, 
		NULL);
	if(status != CL_SUCCESS) exitOnError("Getting Context Info (device list, clGetContextInfo).");

	/////////////////////////////////////////////////////////////////
	// Create an OpenCL command queue
	/////////////////////////////////////////////////////////////////
	commandQueue = clCreateCommandQueue(
		context, 
		devices[0], 
		0, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Creating Command Queue. (clCreateCommandQueue).");
	
	//Kernel Loading/Compilation	
	char binFileName[255];
	char deviceName[255];
	status = clGetDeviceInfo(devices[deviceNum],CL_DEVICE_NAME,255,deviceName,NULL);
	if (status != CL_SUCCESS) exitOnError("Cannot get device name for given device number");
	printf("Using Device:%s\n",deviceName);
	sprintf(binFileName,"%s_%s.elf",kernelName,deviceName);
	std::fstream inBinFile(binFileName, (ios::in|ios::binary|ios::ate));
	
	if(inBinFile.fail()) {
		inBinFile.close();	
		printf("No binary image found for specified kernel and device,compiling a new one.\n");
	  
		char sourceFileName[255]; 
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
		status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
		if(status != CL_SUCCESS) exitOnError("Building program from source (clBuildProgram)");

		//Export Binary Images to file
		dumpBinary(program,kernelName);
	  
	}else{
	
	  //printf("Binary image found:%s\n",binFileName);	    
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
	  status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
	  if(status != CL_SUCCESS) exitOnError("Building program from binary(clBuildProgram)");
	}

	// get a kernel object handle for a kernel with the given name 
	kernel = clCreateKernel(program,kernelName,&status);
	if(status != CL_SUCCESS) exitOnError(" Creating Kernel from program. (clCreateKernel)");
	
	buffer_width = (padded_width <= DEVICE_WORK_ITEMS_PER_LAUNCH) ? padded_width:DEVICE_WORK_ITEMS_PER_LAUNCH;
	//printf("Buffer Width:%d\n",buffer_width);

	// Create OpenCL memory buffers
	//Input buffers
	orig_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * buffer_width,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(orig)"); 

	dir_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * buffer_width,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(dir)"); 


	vert0_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * buffer_width,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert0)"); 

	vert1_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * buffer_width,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert1)");

	vert2_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * buffer_width,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert2)"); 

	vert3_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY,
		sizeof(cl_double4) * buffer_width,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert3)"); 

	//Output buffers
	cartesian_buf = clCreateBuffer(
		context, 
		CL_MEM_WRITE_ONLY,
		sizeof(cl_double8) * buffer_width,
		NULL, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(cartesian_buf)");

	barycentric_buf = clCreateBuffer(
	  context, 
	  CL_MEM_WRITE_ONLY,
	  sizeof(cl_double4) * buffer_width,
	  NULL, 
	  &status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(barycentric_buf)");

	parametric_buf = clCreateBuffer(
	  context, 
	  CL_MEM_WRITE_ONLY,
	  sizeof(cl_double2) * buffer_width,
	  NULL, 
	  &status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(parametric_buf)");
	
	
}


void dumpBinary(cl_program program,const char * kernelName)
{
 	//Extract binary file
	cl_uint deviceCount = 0;
	status = clGetProgramInfo(program,CL_PROGRAM_NUM_DEVICES,sizeof(cl_uint),&deviceCount,NULL);
	if(status != CL_SUCCESS) exitOnError("Getting number of devices for the program(clGetProgramInfo)");

	size_t *binSize = (size_t*)malloc(sizeof(size_t)*deviceCount);
	status = clGetProgramInfo(program,CL_PROGRAM_BINARY_SIZES,sizeof(binSize),binSize,NULL);
	if(status != CL_SUCCESS) exitOnError("Getting binary sizes for the program(clGetProgramInfo)");

	char **bin = ( char**)malloc(sizeof(char*)*deviceCount);
	for(cl_uint i = 0;i<deviceCount;i++) bin[i] = (char*)malloc(binSize[i]);

	status = clGetProgramInfo(program,CL_PROGRAM_BINARIES,sizeof(binSize),bin,NULL);
	if(status != CL_SUCCESS) exitOnError("Getting program binaries(clGetProgramInfo)");

	char binFileName[255];
	for(cl_uint i = 0;i<deviceCount;i++)
	{
	  char deviceName[255];
	  status = clGetDeviceInfo(devices[i],CL_DEVICE_NAME,255,deviceName,NULL);
	  if (status != CL_SUCCESS) exitOnError("Cannot get device name for given device number");
	  printf("Exporting Kernel for Device:%s\n",deviceName);
	  sprintf(binFileName,"%s_%s.elf",kernelName,deviceName);
	  std::fstream outBinFile(binFileName, (std::fstream::out | std::fstream::binary));
	  if(outBinFile.fail()) exitOnError("Cannot open binary file");
	  outBinFile.write(bin[i],binSize[i]);
	  outBinFile.close();
	}
}


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
	if(status != CL_SUCCESS) exitOnError("Error: In clReleaseMemObject (vert0_buf)\n");

	status = clReleaseMemObject(vert1_buf);
	if(status != CL_SUCCESS) exitOnError("Error: In clReleaseMemObject (vert1_buf)\n");

	status = clReleaseMemObject(vert2_buf);
	if(status != CL_SUCCESS) exitOnError("Error: In clReleaseMemObject (vert2_buf)\n");

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

}


//Releases program's resources 
void cleanupHost(void)
{
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
	if(devices != NULL)
	{
		free(devices);
		devices = NULL;
	}

}

void exitOnError(const char *error_text)
{
	printf("Error:%s\n",error_text);
	printf("CL Status:%d\n",status);
	exit(1);

}



