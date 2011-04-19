#ifndef _RayTetraAlgorithms_hpp_
#define _RayTetraAlgorithms_hpp_

class NpVector;


void CalculateFacePlanes(const NpVector& orig, const NpVector& dir,
                         const NpVector vert[], 
                         double vd[], double vn[]);

void CalculatePluecker(const NpVector& orig, const NpVector& dir,
                       const NpVector vert[],
                       double* u, int* sign);


// The various algorithms that we will test
bool RayTetraHainesT(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    double vd[], double vn[],
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave);

bool RayTetraSegura0T(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    const double* u, const int* sign,
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave);

bool RayTetraSegura1T(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[], 
    const double* u, const int* sign,
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave);

bool RayTetraSegura2T(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[], 
    const double* u, const int* sign,
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave);

#endif   // _RayTetraAlgorithms_hpp_
