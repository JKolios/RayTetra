#include "RayTetraAlgorithms.hpp"
#include "../Common/NpVector.hpp"
#include "../Common/NpProgramTimer.hpp"
#include "../Common/NpCArrayAdapter.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <getopt.h>

//GPU init and cleanup functions
#include "gpuHandler.hpp"

using namespace std;


// Struct for program arguments

struct ProgramArguments
{
    char* inputFileName;
    char* outputFileName;
    unsigned int repetitions;
};



// Forward declarations ////////////////////////////////////////////////////////

bool ParseArgs(int argc, char* argv[], ProgramArguments& arguments);
void PrintHelp(char* argv0);
void fillGPUInput(unsigned int nTests,const std::vector< NpCArrayAdapter<NpVector, 4> > v, const std::vector<NpVector> orig, const std::vector<NpVector> dest);



// main ////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    ProgramArguments arguments;

    if (!ParseArgs(argc, argv, arguments))  {
        return -1;
    }

    // Open input and output files
    std::ifstream dataFile(arguments.inputFileName);
    if (dataFile.fail())  {
        std::cerr << "Could not open input file '" 
                  << arguments.inputFileName << "'" << std::endl;
        return -1;
    }

    unsigned int nTests = 0;
    dataFile >> nTests;

    std::ofstream resultsFile(arguments.outputFileName,ios_base::app);
    if (resultsFile.fail())  {
        std::cerr << "Could not open output file '" 
                  << arguments.outputFileName << "'" << std::endl;
        return -1;
    }

    std::vector< NpCArrayAdapter<NpVector, 4> > v(nTests);
    std::vector<NpVector> orig(nTests), dest(nTests);
    NpVector enterPoint, leavePoint;
    int enterFace, leaveFace;
    double ue1, ue2, ul1, ul2;
    double tEnter, tLeave;
    bool result;

    for (unsigned int i = 0; i < nTests; ++i)  {
        dataFile >> v[i][0] >> v[i][1] >> v[i][2] >> v[i][3] 
                 >> orig[i] >> dest[i];
    }

    std::cout << "Haines...   ";
    NpProgramTimer timerHaines;
    timerHaines.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraHaines(orig[i], (dest[i]-orig[i]),
                                    v[i],
                                    enterFace, leaveFace,
                                    enterPoint, leavePoint,
                                    ue1, ue2, ul1, ul2,
                                    tEnter, tLeave);
        }
    }
    timerHaines.Stop();
    resultsFile << timerHaines.TotalElapsedTime() << ',';
    std::cout << timerHaines.TotalElapsedTime() << std::endl;

    std::cout << "Moller1...   ";
    NpProgramTimer timerMoller1;
    timerMoller1.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraMoller1(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerMoller1.Stop();
    resultsFile << timerMoller1.TotalElapsedTime() << ',';
    std::cout << timerMoller1.TotalElapsedTime() << std::endl;

    std::cout << "Moller2...   ";
    NpProgramTimer timerMoller2;
    timerMoller2.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraMoller2(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerMoller2.Stop();
    resultsFile << timerMoller2.TotalElapsedTime() << ',';
    std::cout << timerMoller2.TotalElapsedTime() << std::endl;

    std::cout << "Moller3...   ";
    NpProgramTimer timerMoller3;
    timerMoller3.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraMoller3(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerMoller3.Stop();
    resultsFile << timerMoller3.TotalElapsedTime() << ',';
    std::cout << timerMoller3.TotalElapsedTime() << std::endl;

    std::cout << "Segura0...  ";
    NpProgramTimer timerSegura0;
    timerSegura0.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSegura0(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerSegura0.Stop();
    resultsFile << timerSegura0.TotalElapsedTime() << ',';
    std::cout << timerSegura0.TotalElapsedTime() << std::endl;

    std::cout << "Segura1...  ";
    NpProgramTimer timerSegura1;
    timerSegura1.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSegura1(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerSegura1.Stop();
    resultsFile << timerSegura1.TotalElapsedTime() << ',';
    std::cout << timerSegura1.TotalElapsedTime() << std::endl;

    std::cout << "Segura2...  ";
    NpProgramTimer timerSegura2;
    timerSegura2.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSegura2(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerSegura2.Stop();
    resultsFile << timerSegura2.TotalElapsedTime() << ',';
    std::cout << timerSegura2.TotalElapsedTime() << std::endl;
    
    
        std::cout << "STP0...  ";
    
    NpProgramTimer timerSTP0;
    timerSTP0.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSTP0(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerSTP0.Stop();
    resultsFile << timerSTP0.TotalElapsedTime()<< ',';
    std::cout << timerSTP0.TotalElapsedTime() << std::endl;


     std::cout << "STP1...  ";

    NpProgramTimer timerSTP1;
    timerSTP1.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSTP1(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerSTP1.Stop();
    resultsFile << timerSTP1.TotalElapsedTime()<< ',';
    std::cout << timerSTP1.TotalElapsedTime() << std::endl;
    
         std::cout << "STP2...  ";

    NpProgramTimer timerSTP2;
    timerSTP2.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSTP2(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    	       
    timerSTP2.Stop();
    resultsFile << timerSTP2.TotalElapsedTime() << ",";
    std::cout << timerSTP2.TotalElapsedTime() << std::endl;
    
    std::cout << "Setting up GPU...  "; 
    initializeCL();
    
    makeCLKernel("RayTetraSegura0");
    allocateInput(nTests);
    fillGPUInput(nTests,v,orig,dest);
    allocateBuffers();

    std::cout << "GPU Segura 0...  ";

    NpProgramTimer timerGPUSegura0;
    timerGPUSegura0.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r) runCLKernels();       
    timerGPUSegura0.Stop();
    resultsFile << timerGPUSegura0.TotalElapsedTime()<< ',';
    std::cout << timerGPUSegura0.TotalElapsedTime() << std::endl;
    
    cleanupCL();
    cleanupHost();
    

    initializeCL();
    makeCLKernel("RayTetraSTP0");
    allocateInput(nTests);
    fillGPUInput(nTests,v,orig,dest);
    allocateBuffers();

    std::cout << "GPU STP0...  ";

    NpProgramTimer timerGPUSTP0;
    timerGPUSTP0.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r) runCLKernels();       
    timerGPUSTP0.Stop();
    resultsFile << timerGPUSTP0.TotalElapsedTime()<< ',';
    std::cout << timerGPUSTP0.TotalElapsedTime() << std::endl;
    
    cleanupCL();
    cleanupHost();
    
    initializeCL();
    makeCLKernel("RayTetraSTP1");
    allocateInput(nTests);
    fillGPUInput(nTests,v,orig,dest);
    allocateBuffers();

    std::cout << "GPU STP1...  ";

    NpProgramTimer timerGPUSTP1;
    timerGPUSTP1.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r) runCLKernels();       
    timerGPUSTP1.Stop();
    resultsFile << timerGPUSTP1.TotalElapsedTime()<< ',';
    std::cout << timerGPUSTP1.TotalElapsedTime() << std::endl;
    
    cleanupCL();
    cleanupHost();
    
    initializeCL();
    makeCLKernel("RayTetraSTP2");
    allocateInput(nTests);
    fillGPUInput(nTests,v,orig,dest);
    allocateBuffers();

    std::cout << "GPU STP2...  ";

    NpProgramTimer timerGPUSTP2;
    timerGPUSTP2.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r) runCLKernels();       
    timerGPUSTP2.Stop();
    resultsFile << timerGPUSTP2.TotalElapsedTime()<< ',';
    std::cout << timerGPUSTP2.TotalElapsedTime() << std::endl;
    
    cleanupCL();
    cleanupHost();
    
    
    resultsFile << std::endl;
    

    return 0;
}



// Other functions /////////////////////////////////////////////////////////////

bool ParseArgs(int argc, char* argv[], ProgramArguments& arguments)
{
    if (argc != 4)  {
        PrintHelp(argv[0]);
        return false;
    }

    arguments.inputFileName = argv[1];
    arguments.outputFileName = argv[2];
    arguments.repetitions = std::atoi(argv[3]);

    return true;
}



void PrintHelp(char* argv0)
{
    std::cerr << std::endl << "Usage: " << argv0 
              << " <input file> <output file> <repetitions>" << std::endl;
}

//Converts input data to a format suitable for GPU calc
void fillGPUInput(unsigned int nTests,const std::vector< NpCArrayAdapter<NpVector, 4> > v,const std::vector<NpVector> orig, const std::vector<NpVector> dest)
{
  	for(unsigned int i = 0;i<nTests;i++)
	{

		vert0[i].s[0] = v[i][0].x;
		vert0[i].s[1] = v[i][0].y;
		vert0[i].s[2] = v[i][0].z;	
		vert0[i].s[3] = 0.0;

		vert1[i].s[0] = v[i][1].x;
		vert1[i].s[1] = v[i][1].y;
		vert1[i].s[2] = v[i][1].z;	
		vert1[i].s[3] = 0.0;

		vert2[i].s[0] = v[i][2].x;
		vert2[i].s[1] = v[i][2].y;
		vert2[i].s[2] = v[i][2].z;	
		vert2[i].s[3] = 0.0;

		vert3[i].s[0] = v[i][3].x;
		vert3[i].s[1] = v[i][3].y;
		vert3[i].s[2] = v[i][3].z;	
		vert3[i].s[3] = 0.0;

		
		origin[i].s[0] = orig[i].x;
		origin[i].s[1] = orig[i].y; 
		origin[i].s[2] = orig[i].z; 
		origin[i].s[3] = 0.0;

		dir[i].s[0] = dest[i].x;
		dir[i].s[1] = dest[i].y;
		dir[i].s[2] = dest[i].z;
		dir[i].s[3] = 0.0;

	}
}

