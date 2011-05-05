#include "RayTetraGPU.hpp"

//Input
void loadInput(void)
{
	std::ifstream inputFile;
	inputFile.open(inFileName);
	if(inputFile.fail()) exitOnError("Cannot open input file.");
	inputFile >> actual_width;

	//Determine the amount of false entries to pad the input arrays with.
	//Create a workgroup size of 64 
	
	
	if((actual_width % 64) != 0) padded_width = actual_width + (64 - (actual_width % 64));
	else padded_width = actual_width;
	printf("Padded Width:%d\n",padded_width);

	//Input Arrays
	orig = (cl_double4 *) malloc(padded_width * sizeof(cl_double4));
	if(orig == NULL) exitOnError("Failed to allocate input memory on host(orig)");

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

	// Input Initialization
	cl_int i;
	for(i = 0;i<actual_width;i++)
	{


		inputFile >> vert0[i].s[0];
		inputFile >> vert0[i].s[1];
		inputFile >> vert0[i].s[2];	
		vert0[i].s[3] = 0.0;

		inputFile >> vert1[i].s[0];
		inputFile >> vert1[i].s[1];
		inputFile >> vert1[i].s[2];
		vert1[i].s[3] = 0.0;

		inputFile >> vert2[i].s[0];
		inputFile >> vert2[i].s[1];
		inputFile >> vert2[i].s[2];
		vert2[i].s[3] = 0.0;

		inputFile >> vert3[i].s[0];
		inputFile >> vert3[i].s[1];
		inputFile >> vert3[i].s[2];
		vert3[i].s[3] = 0.0;

		inputFile >> orig[i].s[0];
		inputFile >> orig[i].s[1]; 
		inputFile >> orig[i].s[2]; 
		orig[i].s[3] = 0.0;

		inputFile >>dir[i].s[0];
		inputFile >>dir[i].s[1];
		inputFile >>dir[i].s[2];
		dir[i].s[3] = 0.0;

	}

	inputFile.close();
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
* \brief OpenCL related initialization 
*        Create Context, Device list, Command Queue
*        Create OpenCL memory buffer objects
*        Load CL file, compile, link CL source 
*		  Build program and kernel objects
*/
void
initializeCL(void)
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
	  
		std::string  sourceStr = convertToString(filename);
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
		dumpBinary(program);
	  
	}else{
	
	  printf("Binary image found:%s\n",binFileName);	    
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

	
}


/*
* \brief Run OpenCL program 
*		  
*        Bind host variables to kernel arguments 
*		  Run the CL kernel
*/
void runCLKernels(void)
{
	cl_event events[4];
	size_t globalThreads[1];
	size_t localThreads[1];
	
	
	localThreads[0]= DEVICE_WORK_ITEMS_PER_WAVEFRONT;  
		
	/////////////////////////////////////////////////////////////////
	// Create OpenCL memory buffers
	/////////////////////////////////////////////////////////////////

	//Input buffers
	orig_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR ,
		sizeof(cl_double4) * padded_width,
		orig, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(orig)"); 

	dir_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR ,
		sizeof(cl_double4) * padded_width,
		dir, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(dir)"); 


	vert0_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR ,
		sizeof(cl_double4) * padded_width,
		vert0, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert0)"); 

	vert1_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR ,
		sizeof(cl_double4) * padded_width,
		vert1, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert1)");

	vert2_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR ,
		sizeof(cl_double4) * padded_width,
		vert2, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert2)"); 

	vert3_buf = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR ,
		sizeof(cl_double4) * padded_width,
		vert3, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(vert3)"); 

	//Output buffers
	cartesian_buf = clCreateBuffer(
		context, 
		CL_MEM_WRITE_ONLY| CL_MEM_USE_HOST_PTR ,
		sizeof(cl_double8) * padded_width,
		cartesian, 
		&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(cartesian_buf)");
	
	barycentric_buf = clCreateBuffer(
	context, 
	CL_MEM_WRITE_ONLY| CL_MEM_USE_HOST_PTR ,
	sizeof(cl_double4) * padded_width,
	barycentric, 
	&status);
	if(status != CL_SUCCESS) exitOnError("Cannot Create buffer(barycentric_buf)");
	
	parametric_buf = clCreateBuffer(
	context, 
	CL_MEM_WRITE_ONLY| CL_MEM_USE_HOST_PTR ,
	sizeof(cl_double2) * padded_width,
	parametric, 
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

	//Break up kernel execution to 1 exec per DEVICE_WORK_ITEMS_PER_LAUNCH work items.
	cl_uint remaining_width = padded_width;
	size_t thread_offset[3] = {0,0,0};
	
	while(remaining_width > DEVICE_WORK_ITEMS_PER_LAUNCH)
	{
	
	  //Enqueue a kernel run call.	
	  globalThreads[0] = DEVICE_WORK_ITEMS_PER_LAUNCH;
	  printf("Running kernel with work_items:%zu and offset %zu\n",globalThreads[0],thread_offset[0]);
	  status = clEnqueueNDRangeKernel(
		  commandQueue,
		  kernel,
		  1,
		  thread_offset,
		  globalThreads,
		  localThreads,
		  0,
		  NULL,
		  &events[0]);
	  if(status != CL_SUCCESS) exitOnError("Enqueueing kernel onto command queue.(clEnqueueNDRangeKernel)"); 

	
	  status = clWaitForEvents(1,&events[0]);
	  if(status != CL_SUCCESS) exitOnError(" Waiting for kernel run to finish.(clWaitForEvents)");

	  status = clReleaseEvent(events[0]);
	  if(status != CL_SUCCESS) exitOnError(" clReleaseEvent. (events[0])");
	
	 thread_offset[0]+=DEVICE_WORK_ITEMS_PER_LAUNCH;
	 remaining_width-=DEVICE_WORK_ITEMS_PER_LAUNCH;
	  
	}
		
	//Final kernel run call for remaining work_items
	globalThreads[0] = remaining_width;
	printf("Running kernel with work_items:%zu and offset %zu\n",globalThreads[0],thread_offset[0]);
	
	status = clEnqueueNDRangeKernel(
		commandQueue,
		kernel,
		1,
		thread_offset,
		globalThreads,
		localThreads,
		0,
		NULL,
		&events[0]);
	if(status != CL_SUCCESS) exitOnError("Enqueueing kernel onto command queue.(clEnqueueNDRangeKernel)"); 

	
	status = clWaitForEvents(1,&events[0]);
	if(status != CL_SUCCESS) exitOnError(" Waiting for kernel run to finish.(clWaitForEvents)");

	status = clReleaseEvent(events[0]);
	if(status != CL_SUCCESS) exitOnError(" clReleaseEvent. (events[0])");
	

	
	status = clEnqueueReadBuffer(
		commandQueue,
		cartesian_buf,
		CL_FALSE,
		0,
		actual_width * sizeof(cl_double8),
		cartesian,
		0,
		NULL,
		&events[1]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	status = clEnqueueReadBuffer(
		commandQueue,
		barycentric_buf,
		CL_FALSE,
		0,
		actual_width * sizeof(cl_double4),
		barycentric,
		0,
		NULL,
		&events[2]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	
	status = clEnqueueReadBuffer(
		commandQueue,
		parametric_buf,
		CL_FALSE,
		0,
		actual_width * sizeof(cl_double2),
		parametric,
		0,
		NULL,
		&events[3]);

	if(status != CL_SUCCESS) exitOnError("clEnqueueReadBuffer failed.(clEnqueueReadBuffer)\n");
	
	status = clWaitForEvents(1, &events[1]);
	if(status != CL_SUCCESS) exitOnError(" Waiting for read buffer call to finish.(cartesian_buf)\n");

	status = clReleaseEvent(events[1]);
	if(status != CL_SUCCESS) exitOnError("clReleaseEvent. (cartesian_buf read)\n");
	
	status = clWaitForEvents(1, &events[2]);
	if(status != CL_SUCCESS) exitOnError(" Waiting for read buffer call to finish.(barycentric_buf)\n");

	status = clReleaseEvent(events[2]);
	if(status != CL_SUCCESS) exitOnError("clReleaseEvent. (barycentric_buf read)\n");
	
	status = clWaitForEvents(1, &events[3]);
	if(status != CL_SUCCESS) exitOnError(" Waiting for read buffer call to finish.(parametric_buf)\n");

	status = clReleaseEvent(events[3]);
	if(status != CL_SUCCESS) exitOnError("clReleaseEvent. (parametric_buf read)\n");
}

void dumpBinary(cl_program program)
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

// Print output to file 
void printResults()
{
	cl_int i;
	std::ofstream outputFile;
	outputFile.open(outFileName);
	for(i = 0;i<actual_width;i++){

		if((cartesian[i].s[0] != -1) && (cartesian[i].s[1] != -1) ){

			outputFile  << "true" 
				<< "   " << cartesian[i].s[0] 
			<< " " << cartesian[i].s[1]
			<< "   " << cartesian[i].s[2]
			<< " " << cartesian[i].s[3]                               
			<< " " << cartesian[i].s[4]                               
			<< "   " << cartesian[i].s[5]
			<< " " << cartesian[i].s[6]                               
			<< " " << cartesian[i].s[7]   
			<< "   " << barycentric[i].s[0] 
			<< " " << barycentric[i].s[1] 
			<< "   " << barycentric[i].s[2] 
			<< " " << barycentric[i].s[3]
			<< "   " << parametric[i].s[0] 
			<< " " << parametric[i].s[1];
		}else outputFile << "false";
		outputFile << std::endl;

	}
	outputFile.close();
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
	if(orig != NULL)
	{
		free(orig);
		orig = NULL;
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

void print_usage(void)
{
	printf("Usage:RayTetraGPU [Kernel number] [input file name] [output file name] [device number]\n");
	printf("Kernel numbers:\n");
	printf("0: Segura0\n");
	printf("1: STP0\n");
	printf("2: STP1\n");
	printf("3: STP2\n");
	printf("The default device is the first GPU listed in the platform. \n");
}


int main(int argc, char * argv[])
{
	//Parse kernel selection argument.
	if(argc < 4)
	{
		print_usage();
		exit(1);
	}else{
		if (strcmp(argv[1],"0")== 0) 
		{
			kernelName = "RayTetraSegura0";
			filename = "RayTetraSegura0.cl";
		}
		else if (strcmp(argv[1],"1")== 0) {
			kernelName = "RayTetraSTP0";
			filename = "RayTetraSTP0.cl";
		}
		else if (strcmp(argv[1],"2")== 0) {
			kernelName = "RayTetraSTP1";
			filename = "RayTetraSTP1.cl";
		}
		else if (strcmp(argv[1],"3")== 0) {
			kernelName = "RayTetraSTP2";
			filename = "RayTetraSTP2.cl";
		}
		else{
			print_usage();	
			exit(1);
		}
		inFileName = argv[2];
		outFileName= argv[3];
		if (argc==5) deviceNum=atoi(argv[4]);
	}

	// Read and process input
	loadInput(); 

	// Initialize OpenCL resources
	initializeCL();
	
	

	// Run the CL program
	runCLKernels();

	// Print output array
	printResults();

	// Release OpenCL resources 
	cleanupCL();

	// Release host resources
	cleanupHost();

	return 0;
}

