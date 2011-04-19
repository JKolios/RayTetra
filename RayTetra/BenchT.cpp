#include "RayTetraAlgorithmsT.hpp"
#include "../Common/NpVector.hpp"
#include "../Common/NpProgramTimer.hpp"
#include "../Common/NpCArrayAdapter.hpp"
#include "../Common/NpPlane.hpp"
#include "../Common/NpPluecker.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <getopt.h>


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

    std::ofstream resultsFile(arguments.outputFileName);
    if (resultsFile.fail())  {
        std::cerr << "Could not open output file '" 
                  << arguments.outputFileName << "'" << std::endl;
        return -1;
    }

    std::vector< NpCArrayAdapter<NpVector, 4> > v(nTests);
    std::vector< NpCArrayAdapter<double, 4> > vd(nTests), vn(nTests);
    std::vector< NpCArrayAdapter<double, 6> > u(nTests);
    std::vector< NpCArrayAdapter<int, 6> > sign(nTests);
    std::vector<NpVector> orig(nTests), dest(nTests);
    NpVector enterPoint, leavePoint;
    int enterFace, leaveFace;
    double ue1, ue2, ul1, ul2;
    double tEnter, tLeave;
    bool result;

    for (unsigned int i = 0; i < nTests; ++i)  {
        dataFile >> v[i][0] >> v[i][1] >> v[i][2] >> v[i][3] 
                 >> orig[i] >> dest[i];

        CalculateFacePlanes(orig[i], (dest[i]-orig[i]), v[i], vd[i], vn[i]);
        CalculatePluecker(orig[i], (dest[i]-orig[i]), v[i], u[i], sign[i]);
    }

    std::cout << "Haines...   ";
    NpProgramTimer timerHaines;
    timerHaines.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraHainesT(orig[i], (dest[i]-orig[i]),
                                     v[i],
                                     vd[i], vn[i],
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
    }
    timerHaines.Stop();
    resultsFile << timerHaines.TotalElapsedTime() << '\t';
    std::cout << timerHaines.TotalElapsedTime() << std::endl;

    std::cout << "Segura0...  ";
    NpProgramTimer timerSegura0;
    timerSegura0.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSegura0T(orig[i], (dest[i]-orig[i]),
                                      v[i],
                                      u[i], sign[i],
                                      enterFace, leaveFace,
                                      enterPoint, leavePoint,
                                      ue1, ue2, ul1, ul2,
                                      tEnter, tLeave);
        }
    }
    timerSegura0.Stop();
    resultsFile << timerSegura0.TotalElapsedTime() << '\t';
    std::cout << timerSegura0.TotalElapsedTime() << std::endl;
    
    std::cout << "Segura1...  ";
    NpProgramTimer timerSegura1;
    timerSegura1.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSegura1T(orig[i], (dest[i]-orig[i]),
                                      v[i],
                                      u[i], sign[i],
                                      enterFace, leaveFace,
                                      enterPoint, leavePoint,
                                      ue1, ue2, ul1, ul2,
                                      tEnter, tLeave);
        }
    }
    timerSegura1.Stop();
    resultsFile << timerSegura1.TotalElapsedTime() << '\t';
    std::cout << timerSegura1.TotalElapsedTime() << std::endl;

    std::cout << "Segura2...  ";
    NpProgramTimer timerSegura2;
    timerSegura2.Start();
    for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
        for (unsigned int i = 0; i < nTests; ++i)  {
            result = RayTetraSegura2T(orig[i], (dest[i]-orig[i]),
                                      v[i],
                                      u[i], sign[i],
                                      enterFace, leaveFace,
                                      enterPoint, leavePoint,
                                      ue1, ue2, ul1, ul2,
                                      tEnter, tLeave);
        }
    }
    timerSegura2.Stop();
    resultsFile << timerSegura2.TotalElapsedTime() << '\t';
    std::cout << timerSegura2.TotalElapsedTime() << std::endl;

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

