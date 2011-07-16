#ifndef GPUHANDLER_H_
#define GPUHANDLER_H_

//Provides GPU init and cleanup functions to RayTetra and Bench

//The maximum number of work items to use per kernel execution
#define DEVICE_WORK_ITEMS_PER_LAUNCH 910016

//The maximum number of chars in a device name or binary filename created at runtime
#define MAX_NAME_LENGTH 256

#include <CL/cl.h>

using namespace std;

//Input data is stored here.
extern cl_double4 *origin;//Ray Origin
extern cl_double4 *dir;//Ray Direction
extern cl_double4 *vert0;//Tetrahedron vertices
extern cl_double4 *vert1;
extern cl_double4 *vert2;
extern cl_double4 *vert3;


//Output data is stored here.
extern cl_double8 *cartesian; //Cartesian Coordinates for the entry and exit points
extern cl_double4 *barycentric; //Barycentric Coordinates for the same
extern cl_double2 *parametric; //Parametric distances of entry and exit points from orig

//Actual Ray-Tetrahedron pairs processed. 
extern cl_uint actual_width;

//actual_width padded to a multiple of DEVICE_WORK_ITEMS_PER_WAVEFRONT
extern cl_uint padded_width;

//The width of the input and output buffers used
//Must be <= DEVICE_WORK_ITEMS_PER_LAUNCH
extern cl_uint buffer_width;

//The memory buffers that are used as input/output to the OpenCL kernel
extern cl_mem orig_buf;
extern cl_mem dir_buf;
extern cl_mem vert0_buf;
extern cl_mem vert1_buf;
extern cl_mem vert2_buf;
extern cl_mem vert3_buf;

extern cl_mem cartesian_buf;
extern cl_mem barycentric_buf;
extern cl_mem parametric_buf;


//CL Context and kernel objects
extern cl_context          context;
extern cl_device_id        *devices;
extern cl_command_queue    commandQueue;

extern cl_program program;

extern cl_kernel  kernel;

//The number of work items(threads) launched (at minimum) for every work group of the target device
//64 for AMD, 32 for Nvidia GPUs
extern cl_int threadsPerWavefront;

//Device Name string
//Used to select between precompiled kernels
extern char deviceName[];

//Device Number (for loading a premade binary)
//Default is 0 (First GPU listed in the platform)
extern cl_int deviceNum;

//Return codes from OpenCL API calls
//Used for error tracking
extern cl_int status;

//Event objects
extern cl_event read_events[3];//Tracking buffer reads
extern cl_event write_events[6];//Tracking buffer writes
extern cl_event exec_events[1];//Tracking kernel execution


//Function Declarations

//CL environment init
void initializeCL();

//Kernel Compilation or Binary Loading
void makeCLKernel(const char*);

//Input Processing
void allocateInput(int);

//Input/output buffer allocation
void allocateBuffers(void);

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
