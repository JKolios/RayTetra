#ifndef GPUHANDLER_H_
#define GPUHANDLER_H_

//Provides GPU init and cleanup functions to RayTetra and Bench

//The maximum number of work items to use per kernel execution
//This helps avoid crashes due to insufficient GPU resources
#define WORK_ITEM_LIMIT 910080

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
extern cl_uint actualWidth;

//actual_width padded to a multiple of threadsPerWorkgroup
extern cl_uint padded_width;

//The width of the input and output buffers used
//Must be <= WORK_ITEM_LIMIT
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
//64 for AMD GPUs, a multiple of 32 determined at runtime for Nvidia GPUs, ignored for CPUs
extern size_t threadsPerWorkgroup;

//Device Name string
//Used to select between precompiled kernels
extern char deviceName[];

//Return codes from OpenCL API calls
//Used for error tracking
extern cl_int status;

//Event objects
extern cl_event exec_events[1];//Tracking kernel execution


//Function Declarations

//CL environment init
void initializeCL(int);

//Kernel Compilation or Binary Loading
void makeCLKernel(const char*,int);

//Input Processing
void allocateInput(int,int);

//Input/output buffer allocation
void allocateBuffers(void);

//Fills input buffers with data
void writeBuffers(cl_uint,cl_uint);

//Reads data from output buffers
void readBuffers(cl_uint,cl_uint);

//Execute kernels on OpenCL Device
//This version includes buffer reads and writes
void runCLKernelsWithIO(void);

//Execute kernels on OpenCL Device
//Execution Only (for benchmarking)
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
