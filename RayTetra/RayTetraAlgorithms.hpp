#ifndef _RayTetraAlgorithms_hpp_
#define _RayTetraAlgorithms_hpp_

class NpVector;


// A typedef for the ray-tetrahedron intersection algorithms
typedef bool (*RayTetraAlgo)(
  const NpVector&, const NpVector&,
  const NpVector [],
  int&, int&,
  NpVector&, NpVector&,
  double&, double&, double&, double&,
  double&, double&);


// The various algorithms that we will test
bool RayTetraMoller1(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[],
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraMoller2(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[], 
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraMoller3(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[], 
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraHaines(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[], 
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraSegura0(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[], 
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraSegura1(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[], 
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraSegura2(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[], 
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraSTP0(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[],
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraSTP1(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[],
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

bool RayTetraSTP2(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[],
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave);

#endif   // _RayTetraAlgorithms_hpp_
