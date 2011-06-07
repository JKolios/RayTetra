 //Provides GPU init and cleanup functions to RayTetra

#ifndef GPUHANDLER_H_
#define GPUHANDLER_H_

//The number of work items launched in every Compute Unit of the target device
//at a given time.
#define DEVICE_WORK_ITEMS_PER_WAVEFRONT 64

//The maximum number of work items to use per kernel execution
#define DEVICE_WORK_ITEMS_PER_LAUNCH 910016

#include <CL/cl.h>

using namespace std;

//Input data is stored here.
cl_double4 *origin;//Ray Origin
cl_double4 *dir;//Ray Direction
cl_double4 *vert0;//Tetrahedron vertices
cl_double4 *vert1;
cl_double4 *vert2;
cl_double4 *vert3;


//Output data is stored here.
cl_double8 *cartesian; //Cartesian Coordinates for the entry and exit points
cl_double4 *barycentric; //Barycentric Coordinates for the same
cl_double2 *parametric; //Parametric distances of entry and exit points from orig

//Actual Ray-Tetrahedron pairs processed. 
cl_uint actual_width;

//actual_width padded to a multiple of DEVICE_WORK_ITEMS_PER_WAVEFRONT
cl_uint padded_width;

//The width of the input and output buffers used
//Must be <= DEVICE_WORK_ITEMS_PER_LAUNCH
cl_uint buffer_width;

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

//Device Number (for loading a premade binary)
//Default is 0 (First GPU listed in the platform)
cl_int deviceNum = 0;

//Return codes from OpenCL API calls
//Used for error tracking
cl_int status= 0;

//Function Declarations

//CL environment init
void initializeCL(const char*);

//Input Processing
void allocateInput(int);

//Execute kernels on OpenCL Device
void runCLKernels(void);

//Exports a binary image of the current kernel
void dumpBinary(cl_program,const char*);

//Releases OpenCL resources (Context, Memory etc.)
void cleanupCL(void);

//Releases the program's resources
void cleanupHost(void);

//Handles error printing on fatal errors
void exitOnError(char const *);


#endif 