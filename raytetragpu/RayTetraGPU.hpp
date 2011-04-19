
#ifndef RAYTETRAGPU_H_
#define RAYTETRAGPU_H_


#include <CL/cl.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;


//Input data is stored here.
cl_double4 *orig;//Ray Origin
cl_double4 *dir;//Ray Direction
cl_double4 *vert0;//Tetrahedron vertices
cl_double4 *vert1;
cl_double4 *vert2;
cl_double4 *vert3;


//Output data is stored here.
cl_double16 *output;//See kernel file for structure 

//Ray-Tetrahedron pairs processed in parallel 
cl_int actual_width;

cl_int padded_width;

//The memory buffers that are used as input/output to the OpenCL kernel
cl_mem orig_buf;
cl_mem dir_buf;
cl_mem vert0_buf;
cl_mem vert1_buf;
cl_mem vert2_buf;
cl_mem vert3_buf;

cl_mem output_buf;


//CL Context and kernel objects
cl_context          context;
cl_device_id        *devices;
cl_command_queue    commandQueue;

cl_program program;

cl_kernel  kernel;

//Kernel source filename and kernel function name
const char *filename;
const char *kernelName;

//Device Number (for loading a premade binary)
//Default is 0 (First GPU listed in the platform)
cl_int deviceNum = 0;

cl_int status= 0;

//Input and output filenames
const char *inFileName,*outFileName;

//Function Declarations
//CL environment init
void initializeCL(void);

//Handles kernel execution and I/O 
void runCLKernels(void);

//Exports a binary image of the current kernel
void dumpBinary(cl_program);

//Releases OpenCL resources (Context, Memory etc.)
void cleanupCL(void);

//Releases the program's resources
void cleanupHost(void);

//Result printing function
void printResults(void);

//Handles error printing on fatal errors
void exitOnError(char const *error_text)
{
	printf("Error:%s\n",error_text);
	exit(1);

}


#endif 