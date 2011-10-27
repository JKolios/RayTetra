

#include "../Common/NpVector.hpp"
#include "../Common/NpUtil.h"

// Ray--tetrahedron intersection algorithm using scalar triple products
//Basic Algorithm according to Section 3.1 

bool RayTetraSTP0(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[],
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave)
{
  enterFace = -1;
  leaveFace = -1;
  
  //Point Translation
  //Q == (orig + dir - orig) == dir
  NpVector A = vert[0] - orig;
  NpVector B = vert[1] - orig;
  NpVector C = vert[2] - orig;
  NpVector D = vert[3] - orig;
  
  //Initial cross product calculation
  NpVector QxA = dir^A;
  NpVector QxB = dir^B;
  
  //Intersection test with face ABC(face3)
  //STPs needed:[QAB][QBC][QCA]
  //[QAB]
  double QAB = QxA * B;
  int signQAB = NpSign(QAB);
  
  //[QBC]
  double QBC = QxB * C;
  int signQBC = NpSign(QBC);
  
  //[QAC]
  double QAC = QxA * C;
  int signQAC = NpSign(QAC);
  
  if ((signQAB != 0)  ||  (signQBC != 0)  ||  (signQAC != 0))  {
    if ((signQAB <= 0)  &&  (signQBC <= 0)  &&  (signQAC >= 0))  {
      enterFace = 3;
      
      double invVolABC = 1.0 / (QAB + QBC - QAC);
      uEnter1 = -QAC * invVolABC;
      uEnter2 = QAB * invVolABC;
      enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
      + uEnter1*vert[1] + uEnter2*vert[2];
    }
    else if ((signQAB >= 0)  &&  (signQBC >= 0)  &&  (signQAC <= 0))  {
      leaveFace = 3;
      
      double invVolABC = 1.0 / (QAB + QBC - QAC);
      uLeave1 = -QAC * invVolABC;
      uLeave2 = QAB * invVolABC;
      leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
      + uLeave1*vert[1] + uLeave2*vert[2];
    }
  }
  
  //Intersection test with face BAD(face2)
  //STPs needed:[QBA][QAD][QDB]
  //Available: [QAB][QBC][QAC]
  
  //[QBA] == - [QAB]
  
  //[QAD]
  double QAD = QxA * D;
  int signQAD = NpSign(QAD);
  
  
  //[QDB] == - [QBD]
  //[QBD] is selected in order to avoid computing QxD.
  double QBD = QxB * D;
  int signQBD = NpSign(QBD);
  
  if ((signQAB != 0)  ||  (signQAD != 0)  ||  (signQBD != 0))  {
    if ((enterFace == -1)  &&
      (signQAB >= 0)  &&  (signQAD <= 0)  &&  (signQBD >= 0))  {
      enterFace = 2;
    
    double invVolBAD = 1.0/(-QAB + QAD - QBD);
    uEnter1 = -QBD * invVolBAD;
    uEnter2 = -QAB * invVolBAD;
    enterPoint =   (1-uEnter1-uEnter2)*vert[1]
    + uEnter1*vert[0] + uEnter2*vert[3];
    
    if (leaveFace != -1)  {
      if (dir.x)  {
	double invDirx = 1.0 / dir.x;
	tEnter = (enterPoint.x - orig.x) * invDirx;
	tLeave = (leavePoint.x - orig.x) * invDirx;
      }
      else if (dir.y)  {
	double invDiry = 1.0 / dir.y;
	tEnter = (enterPoint.y - orig.y) * invDiry;
	tLeave = (leavePoint.y - orig.y) * invDiry;
      }
      else  {
	double invDirz = 1.0 / dir.z;
	tEnter = (enterPoint.z - orig.z) * invDirz;
	tLeave = (leavePoint.z - orig.z) * invDirz;
      }
      
      return true;
    }
      }
      else if ((leaveFace == -1)  &&
	(signQAB <= 0)  &&  (signQAD >= 0)  &&  (signQBD <= 0))  {
	leaveFace = 2;
      
      double invVolBAD = 1.0 / (-QAB + QAD - QBD);
      uLeave1 = -QBD * invVolBAD;
      uLeave2 = -QAB * invVolBAD;
      leavePoint =   (1-uLeave1-uLeave2)*vert[1]
      + uLeave1*vert[0] + uLeave2*vert[3];
      
      if (enterFace != -1)  {
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	}
	
	return true;
      }
	}
  }
  
  //Intersection test with face CDA(face1)
  //STPs needed:[QCD][QDA][QAC]
  //Available: [QAB][QBC][QAC][QAD][QBD]
  
  //[QCD]
  NpVector QxC = dir^C;
  double QCD = QxC * D;
  int signQCD = NpSign(QCD);
  
  //[QDA] = - [QAD]
    
  if ((signQCD != 0)  ||  (signQAD != 0)  ||  (signQAC != 0))  {
    if ((enterFace == -1)  &&
      (signQCD <= 0)  &&  (signQAD >= 0)  &&  (signQAC <= 0))  {
      enterFace = 1;
    
    double invVolCDA = 1.0 / (QAC + QCD - QAD);
    uEnter1 =  QAC * invVolCDA;
    uEnter2 =  QCD * invVolCDA;
    enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
    + uEnter1*vert[3] + uEnter2*vert[0];
    
    if (leaveFace != -1)  {
      if (dir.x)  {
	double invDirx = 1.0 / dir.x;
	tEnter = (enterPoint.x - orig.x) * invDirx;
	tLeave = (leavePoint.x - orig.x) * invDirx;
      }
      else if (dir.y)  {
	double invDiry = 1.0 / dir.y;
	tEnter = (enterPoint.y - orig.y) * invDiry;
	tLeave = (leavePoint.y - orig.y) * invDiry;
      }
      else  {
	double invDirz = 1.0 / dir.z;
	tEnter = (enterPoint.z - orig.z) * invDirz;
	tLeave = (leavePoint.z - orig.z) * invDirz;
      }
      
      return true;
    }
      }
      else if ((leaveFace == -1)  &&
	(signQCD >= 0)  &&  (signQAD <= 0)  &&  (signQAC >= 0))  {
	leaveFace = 1;
      
      double invVolCDA = 1.0 / (QAC + QCD - QAD);
      uLeave1 =  QAC * invVolCDA;
      uLeave2 =   QCD* invVolCDA;
      leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
      + uLeave1*vert[3] + uLeave2*vert[0];
      
      if (enterFace != -1)  {
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	}
	
	return true;
      }
	}
  }
  
  
  // If no intersecting face has been found so far, then there is none,
  // otherwise DCB (face0) is the remaining one
  // STPs relevant to DCB:[QDC][QCB][QBD]
  if ((enterFace == -1)  &&  (leaveFace == -1))  {
    return false;
  }
  
  double invVolDCB = 1.0 / (-QCD -QBC + QBD);
  if (enterFace == -1)  {
    enterFace = 0;
    
    uEnter1 = QBD * invVolDCB;
    uEnter2 = -QCD * invVolDCB;
    enterPoint =   (1-uEnter1-uEnter2)*vert[3] 
    + uEnter1*vert[2] + uEnter2*vert[1];
    
    if (dir.x)  {
      double invDirx = 1.0 / dir.x;
      tEnter = (enterPoint.x - orig.x) * invDirx;
      tLeave = (leavePoint.x - orig.x) * invDirx;
    }
    else if (dir.y)  {
      double invDiry = 1.0 / dir.y;
      tEnter = (enterPoint.y - orig.y) * invDiry;
      tLeave = (leavePoint.y - orig.y) * invDiry;
    }
    else  {
      double invDirz = 1.0 / dir.z;
      tEnter = (enterPoint.z - orig.z) * invDirz;
      tLeave = (leavePoint.z - orig.z) * invDirz;
    }
    
    return true;
  }
  else  {
    leaveFace = 0;
    
    uLeave1 = QBD * invVolDCB;
    uLeave2 = -QCD * invVolDCB;
    leavePoint =   (1-uLeave1-uLeave2)*vert[3] 
    + uLeave1*vert[2] + uLeave2*vert[1];
    
    if (dir.x)  {
      double invDirx = 1.0 / dir.x;
      tEnter = (enterPoint.x - orig.x) * invDirx;
      tLeave = (leavePoint.x - orig.x) * invDirx;
    }
    else if (dir.y)  {
      double invDiry = 1.0 / dir.y;
      tEnter = (enterPoint.y - orig.y) * invDiry;
      tLeave = (leavePoint.y - orig.y) * invDiry;
    }
    else  {
      double invDirz = 1.0 / dir.z;
      tEnter = (enterPoint.z - orig.z) * invDirz;
      tLeave = (leavePoint.z - orig.z) * invDirz;
    }
    
    return true;
  }
}

//Section 3.2 Optimizations added
bool RayTetraSTP1(
  const NpVector& orig, const NpVector& dir,
  const NpVector vert[],
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave)
{
  enterFace = -1;
  leaveFace = -1;
  
  //Signs for each STP  var 
  int signQAB = -2, signQAC = -2, signQAD = -2, 
  signQBC = -2, signQBD = -2, signQCD = -2;
  
  int nextSign;
  
  //Point Translation
  //Q == (orig + dir - orig) == dir
  NpVector A = vert[0] - orig;
  NpVector B = vert[1] - orig;
  NpVector C = vert[2] - orig;
  NpVector D = vert[3] - orig;
  
  //Initial cross product calculation
  NpVector QxA = dir^A;
  NpVector QxB = dir^B;
  
  //Intersection test with face ABC(face3)
  //STPs needed:[QAB][QBC][QCA]
  //[QAB]
  double QAB = QxA * B;
  signQAB = NpSign(QAB);
  
  //[QBC]
  double QBC = QxB * C;
  signQBC = NpSign(QBC);
  
  
  
  //Intersection test with face ABC
  if ((signQAB == signQBC)  ||  (signQAB == 0)  ||  (signQBC == 0))  {
    
    //[QAC]
    double QAC = QxA * C;
    signQAC = NpSign(QAC);
    
    int signABC = signQAB;
    if (signABC == 0)  {
      signABC = signQBC;
      if (signABC == 0)  {
	signABC = -signQAC;
      }
    }
    
    if ((signABC != 0)  && ((-signQAC == signABC)  ||  (signQBC == 0)))  {
      //Face ABC is intersected by the ray
      double invVolABC = 1.0/(QAB + QBC - QAC);
      if (signABC == 1)  { 
	leaveFace = 3;
	uLeave1 = -QAC * invVolABC;
	uLeave2 = QAB * invVolABC;
	leavePoint =   (1-uLeave1-uLeave2)*vert[0] + uLeave1*vert[1] + uLeave2*vert[2];
	nextSign = -1;
      }else{
	enterFace = 3;
	uEnter1 = -QAC * invVolABC;
	uEnter2 = QAB * invVolABC;
	enterPoint =   (1-uEnter1-uEnter2)*vert[0] + uEnter1*vert[1] + uEnter2*vert[2];
	nextSign = 1;
      }
      //Faces remaining:BAD,CDA,DCB
      //Intersection test with face BAD
      //STPs needed:[QBA][QAD][QDB]
      //[QBA] == - [QAB]
      
      //[QAD]
      double QAD = QxA * D;
      signQAD = NpSign(QAD);
      
      //Intersection test with face BAD
      if ((signQAD == nextSign)  ||  (signQAD == 0))  {
	
	//[QBD] == - [QDB]
	double QBD = QxB * D;
	signQBD = NpSign(QBD);
	
	if ((-signQBD == nextSign) || ((signQBD == 0) && ((signQAD != 0)  ||  (signQAB != 0))))  {
	  // Face BAD intersects with the ray
	  double invVolBAD = 1.0/(-QAB + QAD - QBD);
	  if (nextSign == 1)  {
	    leaveFace = 2;
	    uLeave1 = -QBD * invVolBAD;
	    uLeave2 = -QAB * invVolBAD;
	    leavePoint =   (1-uLeave1-uLeave2)*vert[1] + uLeave1*vert[0] + uLeave2*vert[3];
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tEnter = (enterPoint.x - orig.x) * invDirx;
	      tLeave = (leavePoint.x - orig.x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tEnter = (enterPoint.y - orig.y) * invDiry;
	      tLeave = (leavePoint.y - orig.y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tEnter = (enterPoint.z - orig.z) * invDirz;
	      tLeave = (leavePoint.z - orig.z) * invDirz;
	    }
	    return true;
	  }else{
	    enterFace = 2;
	    uEnter1 = -QBD * invVolBAD;
	    uEnter2 = -QAB * invVolBAD;
	    enterPoint =   (1-uEnter1-uEnter2)*vert[1] + uEnter1*vert[0] + uEnter2*vert[3];
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tEnter = (enterPoint.x - orig.x) * invDirx;
	      tLeave = (leavePoint.x - orig.x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tEnter = (enterPoint.y - orig.y) * invDiry;
	      tLeave = (leavePoint.y - orig.y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tEnter = (enterPoint.z - orig.z) * invDirz;
	      tLeave = (leavePoint.z - orig.z) * invDirz;
	    }
	    return true;
	  }
	}
      }
      // Face BAD does not intersect with the ray.
      // Determine the other intersecting face between CDA, DCB
      
      //Intersection test with face CDA(face1)
      //STPs needed:[QCD][QDA][QAC]
      
      //[QCD]
      NpVector QxC = dir^C;
      double QCD = QxC * D;
      signQCD = NpSign(QCD);
      if ((signQCD == nextSign)  ||((signQCD == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
	// Face CDA intersects with the ray
	double invVolCDA = 1.0/(QCD -QAD +QAC);
	if (nextSign == 1)  {
	  leaveFace = 1;
	  uLeave1 = QAC * invVolCDA;
	  uLeave2 = QCD * invVolCDA;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[2] + uLeave1*vert[3] + uLeave2*vert[0];
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	  }
	  return true;
	}else{
	  enterFace = 1;
	  uEnter1 = QAC * invVolCDA;
	  uEnter2 = QCD * invVolCDA;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[2] + uEnter1*vert[3] + uEnter2*vert[0];
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	  }
	  return true;
	}
      }else{
	//Face DCB intersects with the ray
	double QBD = QxB * D;
	double invVolDCB = 1.0/(-QCD -QBC + QBD);
	if (nextSign == 1)  {
	  leaveFace = 0;
	  uLeave1 = QBD * invVolDCB;
	  uLeave2 = -QCD * invVolDCB;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[3] + uLeave1*vert[2] + uLeave2*vert[1];
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	  }
	  return true;
	}else{
	  enterFace = 0;
	  uEnter1 = QBD * invVolDCB;
	  uEnter2 = -QCD * invVolDCB;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[3] + uEnter1*vert[2] + uEnter2*vert[1];
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	  }
	  return true;
	}
	
      }
    }
    
  }
  
  //Intersection test with face BAD(face2)
  //STPs needed:[QBA][QAD][QDB]
  //STPs precalculated [QAB][QBC]
  //[QBA] = - [QAB]
  
  //[QAD]
  double QAD = QxA * D;
  signQAD = NpSign(QAD);
  
  if ((signQAD == -signQAB)  ||  (signQAB == 0)  ||  (signQAD == 0))  {
    
    //[QBD] == - [QDB]
    double QBD = QxB * D;
    signQBD = NpSign(QBD);
    
    
    int signBAD = -signQAB;
    if (signBAD == 0)  {
      signBAD = signQAD;
      if (signBAD == 0)  {
	signBAD = -signQBD;
      }
    }
    if ((signBAD != 0)  && ((-signQBD == signBAD)  ||  (signQBD == 0)))  {
      // Face BAD intersects with the ray
      double invVolBAD = 1.0/(QAD -QAB -QBD);
      if (signBAD == 1)  {
	leaveFace = 2;
	uLeave1 = -QBD * invVolBAD;
	uLeave2 = -QAB * invVolBAD;
	leavePoint =   (1-uLeave1-uLeave2)*vert[1] + uLeave1*vert[0] + uLeave2*vert[3];
	nextSign = -1;
      }else{
	enterFace = 2;
	uEnter1 = -QBD * invVolBAD;
	uEnter2 = -QAB * invVolBAD;
	enterPoint =   (1-uEnter1-uEnter2)*vert[1] + uEnter1*vert[0] + uEnter2*vert[3];
	nextSign = 1;
      }
      // Determine the other intersecting face between CDA, DCB
      // Examine face CDA
      // STPs needed:[QCD][QDA][QAC]
      
      //[QAC]
      NpVector QxC = dir^C;
      double QAC = QxA * C;
      signQAC = NpSign(QAC);
      if ((-signQAD == signQAC)  ||  (signQAC == 0)  ||  (signQAD == 0))  {
	
	//[QCD]
	NpVector QxC = dir^C;
	double QCD = QxC * D;
	signQCD = NpSign(QCD);
	
	if ((signQCD == nextSign)  || ((signQCD == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
	  // Face CDA intersects with the ray
	  double invVolCDA = 1.0 / (QCD -QAD +QAC);
	  if (nextSign == 1)  {
	    leaveFace = 1;
	    uLeave1 =  QAC * invVolCDA;
	    uLeave2 =  QCD * invVolCDA;
	    leavePoint =   (1-uLeave1-uLeave2)*vert[2] + uLeave1*vert[3] + uLeave2*vert[0];
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tEnter = (enterPoint.x - orig.x) * invDirx;
	      tLeave = (leavePoint.x - orig.x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tEnter = (enterPoint.y - orig.y) * invDiry;
	      tLeave = (leavePoint.y - orig.y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tEnter = (enterPoint.z - orig.z) * invDirz;
	      tLeave = (leavePoint.z - orig.z) * invDirz;
	    }
	    return true;
	  }else{
	    enterFace = 1;
	    uEnter1 =  QAC * invVolCDA;
	    uEnter2 =  QCD * invVolCDA;
	    enterPoint =   (1-uEnter1-uEnter2)*vert[2] + uEnter1*vert[3] + uEnter2*vert[0];
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tEnter = (enterPoint.x - orig.x) * invDirx;
	      tLeave = (leavePoint.x - orig.x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tEnter = (enterPoint.y - orig.y) * invDiry;
	      tLeave = (leavePoint.y - orig.y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tEnter = (enterPoint.z - orig.z) * invDirz;
	      tLeave = (leavePoint.z - orig.z) * invDirz;
	    }
	    return true;
	  }
	}
      }
      // Face DCB intersects with the ray
      double QCD = QxC * D;
      double QBD = QxB * D;
      double invVolDCB = 1.0 / (-QCD -QBC + QBD);
      if (nextSign == 1)  {
	leaveFace = 0;
	uLeave1 =  QBD * invVolDCB;
	uLeave2 =  -QCD * invVolDCB;
	leavePoint =   (1-uLeave1-uLeave2)*vert[3] + uLeave1*vert[2] + uLeave2*vert[1];
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	}
	return true;
      }else{
	enterFace = 0;
	uEnter1 = QBD * invVolDCB;
	uEnter2 = -QCD * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert[3] + uEnter1*vert[2] + uEnter2*vert[1];
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	}
	return true;
      }
    }
  }
  
  
  
  //Intersection test with face CDA(face1)
  //STPs needed:[QCD][QDA][QAC]
  //STPs precalculated [QAB][QBC][QAD]
  
  //[QAC]
  double QAC = QxA * C;
  signQAC = NpSign(QAC);
  
  if ((-signQAD == signQAC)  ||  (signQAC == 0)  ||  (signQAD == 0))  {
    // Face CDA may intersect with the ray
    
    //[QCD]
    NpVector QxC = dir^C;
    double QCD = QxC * D;
    signQCD = NpSign(QCD);
    
    int signCDA = signQAC;
    if (signCDA == 0)  {
      signCDA = -signQAD;
      if (signCDA == 0)  {
	signCDA = signQCD;
      }
    }
    
    if ((signCDA != 0)  && ((signQCD == signCDA)  ||  (signQCD == 0)))  {
      // Face CDA intersects with the ray
      // Face DCB also intersects with the ray
      double invVolCDA = 1.0 / (QAC + QCD - QAD);
      double QBD = QxB * D;
      double invVolDCB = 1.0 / (-QCD - QBC + QBD);
      
      if (signCDA == 1)  {
	enterFace = 0;
	leaveFace = 1;
	uLeave1 =  QAC * invVolCDA;
	uLeave2 =  QCD * invVolCDA;
	uEnter1 =  QBD * invVolDCB;
	uEnter2 = -QCD * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert[3] + uEnter1*vert[2] + uEnter2*vert[1];
	leavePoint =   (1-uLeave1-uLeave2)*vert[2] + uLeave1*vert[3] + uLeave2*vert[0];
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	}
	return true;
      }
      else  {
	enterFace = 1;
	leaveFace = 0;			
	uEnter1 =  QAC * invVolCDA;
	uEnter2 = QCD * invVolCDA;
	uLeave1 = QBD * invVolDCB;
	uLeave2 =  -QCD * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert[2] + uEnter1*vert[3] + uEnter2*vert[0];
	leavePoint =   (1-uLeave1-uLeave2)*vert[3] + uLeave1*vert[2] + uLeave2*vert[1];
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	}
	return true;
      }
    }
  }
  
  // Three faces do not intersect with the ray, the fourth will not.
  return false;
}

//Section 3.3 Modifications added
bool RayTetraSTP2(
const NpVector& orig, const NpVector& dir,
  const NpVector vert[],
  int& enterFace, int& leaveFace,
  NpVector& enterPoint, NpVector& leavePoint,
  double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
  double& tEnter, double& tLeave)
{
  leaveFace = -1;
  enterFace = -1;
  
  
  int nextSign;
  
  //Point Translation
  //Q == (orig + dir - orig) == dir
  NpVector A = vert[0] - orig;
  NpVector B = vert[1] - orig;
  NpVector C = vert[2] - orig;
  NpVector D = vert[3] - orig;
  
  //Initial cross product calculation
  NpVector QxA = dir^A;
  NpVector QxB = dir^B;
  
  //Intersection test with face ABC(face3)
  //STPs needed:[QAB][QBC][QCA]
  //[QAB]
  double QAB = QxA * B;
  int signQAB = NpSign(QAB);
  
  //[QBC]
  double QBC = QxB * C;
  int signQBC = NpSign(QBC);
  
  //[QAC]
  double QAC = QxA * C;
  int signQAC = NpSign(QAC);
  
  
  if ((signQAB == 0)  &&  (signQBC == 0)  &&  (signQAC == 0))  {
    // Ray is on the plane of ABC
    // Examine face BAD
    
    //[QAD]
    double QAD = QxA * D;
    int signQAD = NpSign(QAD);
    
    NpVector QxD = dir^D;
    
    //[QDB]
    double QDB = QxD * B;
    int signQDB = NpSign(QDB);
    
    //[QDC]
    double QDC = QxD * C;
    int signQDC = NpSign(QDC);	
    
    
    if ((signQAD == 0)  &&  (signQDB == 0))  {
      if (signQDC == 0)  {
	// The tetrahedron is flat and the ray is on its plane
	return false;
      }
      else  {
	// The ray is on the AB edge
	if (signQDC == 1)  {
	  leaveFace = 1;
	  uLeave1 = 0;
	  uLeave2 = 1;
	  leavePoint = vert[0];
	  
	  enterFace = 0;
	  uEnter1 = 0;
	  uEnter2 = 1;
	  enterPoint = vert[1];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
	else  {
	  enterFace = 1;
	  uEnter1 = 0;
	  uEnter2 = 1;
	  enterPoint = vert[0];
	  
	  leaveFace = 0;
	  uLeave1 = 0;
	  uLeave2 = 1;
	  leavePoint = vert[1];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
      }
    }
    else  {
      // Check if the ray intersects with face BAD
      if ((signQAD >= 0)  &&  (signQDB >= 0))  {
	// The ray leaves through BAD
	leaveFace = 2;
	double invVolBAD = 1.0 / (QAD + QDB);
	uLeave1 =  QDB * invVolBAD;
	uLeave2 =  0;
	leavePoint = (1-uLeave1)*vert[1] + uLeave1*vert[0];
	
	// Determine the other intersecting face between CDA, DCB
	if ((signQDC == 1)  ||
	  ((signQDC == 0)  &&  (signQAD == 1)))  {
	  // The ray enters through CDA
	  enterFace = 1;
	double invVolCDA = 1.0 / (-QDC - QAD);
	uEnter1 =  0;
	uEnter2 = -QDC * invVolCDA;
	enterPoint = (1-uEnter2)*vert[2] + uEnter2*vert[0];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	}
	
	return true;
	  }
	  else  {
	    // The ray enters through DCB
	    enterFace = 0;
	    double invVolDCB = 1.0 / (QDC - QDB);
	    uEnter1 = -QDB * invVolDCB;
	    uEnter2 =  QDC * invVolDCB;
	    enterPoint = uEnter1*vert[2] + uEnter2*vert[1];
	    
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tLeave = (leavePoint.x - orig.x) * invDirx;
	      tEnter = (enterPoint.x - orig.x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tLeave = (leavePoint.y - orig.y) * invDiry;
	      tEnter = (enterPoint.y - orig.y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tLeave = (leavePoint.z - orig.z) * invDirz;
	      tEnter = (enterPoint.z - orig.z) * invDirz;
	    }
	    
	    return true;
	  }
      }
      else if ((signQAD <= 0)  &&  (signQDB <= 0))  {
	// The ray enters through BAD
	enterFace = 2;
	
	double invVolBAD = 1.0 / (QAD + QDB);
	uEnter1 =  QDB * invVolBAD;
	uEnter2 =  0;
	enterPoint = (1-uEnter1)*vert[1] + uEnter1*vert[0];
	
	// Determine the other intersecting face between CDA, DCB
	//[QDC]
	NpVector QxD = dir^D;
	double QDC = QxD * C;
	int signQDC = NpSign(QDC);
	
	if ((signQDC == -1)  ||
	  ((signQDC == 0)  &&  (signQAD == -1)))  {
	  // The ray leaves through CDA
	  leaveFace = 1;
	double invVolCDA = 1.0 / (- QDC - QAD);
	uLeave1 =  0;
	uLeave2 = -QDC * invVolCDA;
	leavePoint =   (1-uLeave2)*vert[2] + uLeave2*vert[0];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	}
	
	return true;
	  }
	  else  {
	    // The ray leaves through DCB
	    leaveFace = 0;
	    double invVolDCB = 1.0 / (QDC - QDB);
	    uLeave1 = -QDB * invVolDCB;
	    uLeave2 =  QDC * invVolDCB;
	    leavePoint = uLeave1*vert[2] + uLeave2*vert[1];
	    
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tLeave = (leavePoint.x - orig.x) * invDirx;
	      tEnter = (enterPoint.x - orig.x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tLeave = (leavePoint.y - orig.y) * invDiry;
	      tEnter = (enterPoint.y - orig.y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tLeave = (leavePoint.z - orig.z) * invDirz;
	      tEnter = (enterPoint.z - orig.z) * invDirz;
	    }
	    
	    return true;
	  }
      }
      else  {
	return false;
      }
    }
  }
  else  {
    // Check if ray intersects with face ABC
    if ((signQAB >= 0)  &&  (signQBC >= 0)  &&  (signQAC <= 0))  {
      leaveFace = 3;
      double invVolABC = 1.0 / (QAB + QBC - QAC);
      uLeave1 = -QAC * invVolABC;
      uLeave2 =  QAB * invVolABC;
      leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
      + uLeave1*vert[1] + uLeave2*vert[2];
      
      nextSign = -1;
      // The other intersecting face is determined after the next 'else'
    }
    else if ((signQAB <= 0)  &&  (signQBC <= 0)  &&  (signQAC >= 0))  {
      enterFace = 3;
      double invVolABC = 1.0 / (QAB + QBC - QAC);
      uEnter1 = -QAC * invVolABC;
      uEnter2 =  QAB * invVolABC;
      enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
      + uEnter1*vert[1] + uEnter2*vert[2];
      
      nextSign = 1;
      // The other intersecting face is determined after the next 'else'
    }
    else  {
      // Ray does not intersect with ABC,
      // determine the next face to examine
      
      int nextFace;
      
      if (QAB+QBC-QAC > 0)  {
	if (signQBC == -1)  {
	  if (signQAC == 1)  {
	    nextFace = 2;
	    nextSign = -1;
	  }
	  else if (signQAB == -1)  {
	    nextFace = 1;
	    nextSign = -1;
	  }
	  else  {
	    nextFace = 0;
	    nextSign = 1;
	  }
	}
	else if (signQAC == 1)  {
	  if (signQAB == -1)  {
	    nextFace = 0;
	    nextSign = -1;
	  }
	  else  {
	    nextFace = 1;
	    nextSign = 1;
	  }
	}
	else  {
	  nextFace = 2;
	  nextSign = 1;
	}
      }
      else  {
	if (signQBC == 1)  {
	  if (signQAC == -1)  {
	    nextFace = 2;
	    nextSign = 1;
	  }
	  else if (signQAB == 1)  {
	    nextFace = 1;
	    nextSign = 1;
	  }
	  else  {
	    nextFace = 0;
	    nextSign = -1;
	  }
	}
	else if (signQAC == -1)  {
	  if (signQAB == 1)  {
	    nextFace = 0;
	    nextSign = 1;
	  }
	  else  {
	    nextFace = 1;
	    nextSign = -1;
	  }
	}
	else  {
	  nextFace = 2;
	  nextSign = -1;
	}
      }
      
      // Examine nextFace
      if (nextFace == 2)  {
	// Examine face BAD
	//[QAD]
	double QAD = QxA * D;
	int signQAD = NpSign(QAD);
	
	if (signQAD == -nextSign)  {
	  return false;
	}
	
	//[QDB]
	NpVector QxD = dir^D;
	double QDB = QxD * B;
	int signQDB = NpSign(QDB);
	
	if (signQDB == -nextSign)  {
	  return false;
	}
	
	// Face BAD intersects with the ray
	double invVolBAD = 1.0 / (QAD + QDB - QAB);
	if (nextSign == 1)  {
	  leaveFace = 2;
	  uLeave1 =  QDB * invVolBAD;
	  uLeave2 = -QAB * invVolBAD;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[1]
	  + uLeave1*vert[0] + uLeave2*vert[3];
	  
	  nextSign = -1;
	}
	else  {
	  enterFace = 2;
	  uEnter1 =  QDB * invVolBAD;
	  uEnter2 = -QAB * invVolBAD;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[1]
	  + uEnter1*vert[0] + uEnter2*vert[3];
	  
	  nextSign = 1;
	}
	
	// Determine the other intersecting face between CDA, DCB
	//[QDC]
	double QDC = QxD * C;
	int signQDC = NpSign(QDC);
	
	if ((signQDC == -nextSign)  ||
	  ((signQDC == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
	  // Face CDA intersects with the ray
	  double invVolCDA = 1.0 / (QAC - QDC - QAD);
	if (nextSign == 1)  {
	  leaveFace = 1;
	  uLeave1 =  QAC * invVolCDA;
	  uLeave2 = -QDC * invVolCDA;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
	  + uLeave1*vert[3] + uLeave2*vert[0];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
	else  {
	  enterFace = 1;
	  uEnter1 =  QAC * invVolCDA;
	  uEnter2 = -QDC * invVolCDA;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
	  + uEnter1*vert[3] + uEnter2*vert[0];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
	  }
	  else  {
	    // Face DCB intersects with the ray
	    double invVolDCB = 1.0 / (QDC - QBC - QDB);
	    if (nextSign == 1)  {
	      leaveFace = 0;
	      uLeave1 = -QDB * invVolDCB;
	      uLeave2 =  QDC * invVolDCB;
	      leavePoint =   (1-uLeave1-uLeave2)*vert[3] 
	      + uLeave1*vert[2] + uLeave2*vert[1];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig.x) * invDirx;
		tEnter = (enterPoint.x - orig.x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig.y) * invDiry;
		tEnter = (enterPoint.y - orig.y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig.z) * invDirz;
		tEnter = (enterPoint.z - orig.z) * invDirz;
	      }
	      
	      return true;
	    }
	    else  {
	      enterFace = 0;
	      uEnter1 = -QDB * invVolDCB;
	      uEnter2 =  QDC * invVolDCB;
	      enterPoint =   (1-uEnter1-uEnter2)*vert[3] 
	      + uEnter1*vert[2] + uEnter2*vert[1];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig.x) * invDirx;
		tEnter = (enterPoint.x - orig.x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig.y) * invDiry;
		tEnter = (enterPoint.y - orig.y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig.z) * invDirz;
		tEnter = (enterPoint.z - orig.z) * invDirz;
	      }
	      
	      return true;
	    }
	  }
      }
      else if (nextFace == 1)  {
	// Examine face CDA
	
	NpVector QxD = dir^D;
	double QDC = QxD * C;
	int signQDC = NpSign(QDC);
	
	if (signQDC == nextSign)  {
	  return false;
	}
	
	//[QAD]
	double QAD = QxA * D;
	int signQAD = NpSign(QAD);
	
	if (signQAD == nextSign)  {
	  return false;
	}
	
	// Face CDA intersects with the ray
	double invVolCDA = 1.0 / (QAC - QDC - QAD);
	if (nextSign == 1)  {
	  leaveFace = 1;
	  uLeave1 =  QAC * invVolCDA;
	  uLeave2 = -QDC * invVolCDA;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
	  + uLeave1*vert[3] + uLeave2*vert[0];
	  
	  nextSign = -1;
	}
	else  {
	  enterFace = 1;
	  uEnter1 =  QAC * invVolCDA;
	  uEnter2 = -QDC * invVolCDA;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
	  + uEnter1*vert[3] + uEnter2*vert[0];
	  
	  nextSign = 1;
	}
	
	// Determine the other intersecting face between BAD, DCB
	//[QDB]
	double QDB = QxD * B;
	int signQDB = NpSign(QDB);
	
	if ((signQDB == nextSign)  ||
	  ((signQDB == 0)  &&  ((signQAB != 0)  ||  (signQAD != 0))))  {
	  // Face BAD intersects with the ray
	  double invVolBAD = 1.0 / (QAD + QDB - QAB);
	if (nextSign == 1)  {
	  leaveFace = 2;
	  uLeave1 =  QDB * invVolBAD;
	  uLeave2 = -QAB * invVolBAD;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[1]
	  + uLeave1*vert[0] + uLeave2*vert[3];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
	else  {
	  enterFace = 2;
	  uEnter1 =  QDB * invVolBAD;
	  uEnter2 = -QAB * invVolBAD;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[1]
	  + uEnter1*vert[0] + uEnter2*vert[3];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
	  }
	  else  {
	    // Face DCB intersects with the ray
	    double invVolDCB = 1.0 / (QDC - QBC - QDB);
	    if (nextSign == 1)  {
	      leaveFace = 0;
	      uLeave1 = -QDB * invVolDCB;
	      uLeave2 =  QDC * invVolDCB;
	      leavePoint =   (1-uLeave1-uLeave2)*vert[3] 
	      + uLeave1*vert[2] + uLeave2*vert[1];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig.x) * invDirx;
		tEnter = (enterPoint.x - orig.x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig.y) * invDiry;
		tEnter = (enterPoint.y - orig.y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig.z) * invDirz;
		tEnter = (enterPoint.z - orig.z) * invDirz;
	      }
	      
	      return true;
	    }
	    else  {
	      enterFace = 0;
	      uEnter1 = -QDB * invVolDCB;
	      uEnter2 =  QDC * invVolDCB;
	      enterPoint =   (1-uEnter1-uEnter2)*vert[3] 
	      + uEnter1*vert[2] + uEnter2*vert[1];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig.x) * invDirx;
		tEnter = (enterPoint.x - orig.x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig.y) * invDiry;
		tEnter = (enterPoint.y - orig.y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig.z) * invDirz;
		tEnter = (enterPoint.z - orig.z) * invDirz;
	      }
	      
	      return true;
	    }
	  }
      }
      else  {
	// Examine face DCB
	//[QDC]
	NpVector QxD = dir ^D;
	double QDC = QxD * C;
	int signQDC = NpSign(QDC);	
	
	
	if (signQDC == -nextSign)  {
	  return false;
	}
	
	double QDB = QxD * B;
	int signQDB = NpSign(QDB);
	
	if (signQDB == nextSign)  {
	  return false;
	}
	
	// Face DCB intersects with the ray
	double invVolDCB = 1.0 / (QDC - QBC - QDB);
	if (nextSign == 1)  {
	  leaveFace = 0;
	  uLeave1 = -QDB * invVolDCB;
	  uLeave2 =  QDC * invVolDCB;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[3] 
	  + uLeave1*vert[2] + uLeave2*vert[1];
	  
	  nextSign = -1;
	}
	else  {
	  enterFace = 0;
	  uEnter1 = -QDB * invVolDCB;
	  uEnter2 =  QDC * invVolDCB;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[3] 
	  + uEnter1*vert[2] + uEnter2*vert[1];
	  
	  nextSign = 1;
	}
	
	// Determine the other intersecting face between BAD, CDA
	//[QAD]
	double QAD = QxA * D;
	int signQAD = NpSign(QAD);
	
	if ((signQAD == nextSign)  ||
	  ((signQAD == 0)  &&  ((signQAB != 0)  ||  (signQDB != 0))))  {
	  // Face BAD intersects with the ray
	  double invVolBAD = 1.0 / (QAD + QDB - QAB);
	if (nextSign == 1)  {
	  leaveFace = 2;
	  uLeave1 =  QDB * invVolBAD;
	  uLeave2 = -QAB * invVolBAD;
	  leavePoint =   (1-uLeave1-uLeave2)*vert[1]
	  + uLeave1*vert[0] + uLeave2*vert[3];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
	else  {
	  enterFace = 2;
	  uEnter1 =  QDB * invVolBAD;
	  uEnter2 = -QAB * invVolBAD;
	  enterPoint =   (1-uEnter1-uEnter2)*vert[1]
	  + uEnter1*vert[0] + uEnter2*vert[3];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig.x) * invDirx;
	    tEnter = (enterPoint.x - orig.x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig.y) * invDiry;
	    tEnter = (enterPoint.y - orig.y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig.z) * invDirz;
	    tEnter = (enterPoint.z - orig.z) * invDirz;
	  }
	  
	  return true;
	}
	  }
	  else  {
	    // Face CDA intersects with the ray
	    double invVolCDA = 1.0 / (QAC - QDC - QAD);
	    if (nextSign == 1)  {
	      leaveFace = 1;
	      uLeave1 =  QAC * invVolCDA;
	      uLeave2 = -QDC * invVolCDA;
	      leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
	      + uLeave1*vert[3] + uLeave2*vert[0];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig.x) * invDirx;
		tEnter = (enterPoint.x - orig.x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig.y) * invDiry;
		tEnter = (enterPoint.y - orig.y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig.z) * invDirz;
		tEnter = (enterPoint.z - orig.z) * invDirz;
	      }
	      
	      return true;
	    }
	    else  {
	      enterFace = 1;
	      uEnter1 =  QAC * invVolCDA;
	      uEnter2 = -QDC * invVolCDA;
	      enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
	      + uEnter1*vert[3] + uEnter2*vert[0];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig.x) * invDirx;
		tEnter = (enterPoint.x - orig.x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig.y) * invDiry;
		tEnter = (enterPoint.y - orig.y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig.z) * invDirz;
		tEnter = (enterPoint.z - orig.z) * invDirz;
	      }
	      
	      return true;
	    }
	  } 
      }
    }
  }
  // Face ABC intersects with the ray.
  // Determine the other intersecting face between BAD, CDA, DCB
  // Examine face BAD
  //[QAD]
  double QAD = QxA * D;
  int signQAD = NpSign(QAD);
  
  
  double QDB = 0;
  int signQDB = -2;
  
  if ((signQAD == nextSign)  ||  (signQAD == 0))  {
    // Face BAD may intersect with the ray
    NpVector QxD = dir ^D;
    double QDB = QxD * B;
    int signQDB = NpSign(QDB);
    
    if ((signQDB == nextSign)  ||  
      ((signQDB == 0)  &&  
      ((signQAD != 0)  ||  (signQAB != 0))))  {
      // Face BAD intersects with the ray
      double invVolBAD = 1.0 / (QAD + QDB - QAB);
    if (nextSign == 1)  {
      leaveFace = 2;
      uLeave1 =  QDB * invVolBAD;
      uLeave2 = -QAB * invVolBAD;
      leavePoint =   (1-uLeave1-uLeave2)*vert[1]
      + uLeave1*vert[0] + uLeave2*vert[3];
      
      if (dir.x)  {
	double invDirx = 1.0 / dir.x;
	tLeave = (leavePoint.x - orig.x) * invDirx;
	tEnter = (enterPoint.x - orig.x) * invDirx;
      }
      else if (dir.y)  {
	double invDiry = 1.0 / dir.y;
	tLeave = (leavePoint.y - orig.y) * invDiry;
	tEnter = (enterPoint.y - orig.y) * invDiry;
      }
      else  {
	double invDirz = 1.0 / dir.z;
	tLeave = (leavePoint.z - orig.z) * invDirz;
	tEnter = (enterPoint.z - orig.z) * invDirz;
      }
      
      return true;
    }
    else  {
      enterFace = 2;
      uEnter1 =  QDB * invVolBAD;
      uEnter2 = -QAB * invVolBAD;
      enterPoint =   (1-uEnter1-uEnter2)*vert[1]
      + uEnter1*vert[0] + uEnter2*vert[3];
      
      if (dir.x)  {
	double invDirx = 1.0 / dir.x;
	tLeave = (leavePoint.x - orig.x) * invDirx;
	tEnter = (enterPoint.x - orig.x) * invDirx;
      }
      else if (dir.y)  {
	double invDiry = 1.0 / dir.y;
	tLeave = (leavePoint.y - orig.y) * invDiry;
	tEnter = (enterPoint.y - orig.y) * invDiry;
      }
      else  {
	double invDirz = 1.0 / dir.z;
	tLeave = (leavePoint.z - orig.z) * invDirz;
	tEnter = (enterPoint.z - orig.z) * invDirz;
      }
      
      return true;
    }
      }           
  }
  
  // Face BAD does not intersect with the ray.
  // Determine the other intersecting face between CDA, DCB
  //[QDC]
  NpVector QxD = dir ^D;
  double QDC = QxD * C;
  int signQDC = NpSign(QDC);
  
  if ((signQDC == -nextSign)  ||
    ((signQDC == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
    // Face CDA intersects with the ray
    double invVolCDA = 1.0 / (QAC - QDC - QAD);
  if (nextSign == 1)  {
    leaveFace = 1;
    uLeave1 =  QAC * invVolCDA;
    uLeave2 = -QDC * invVolCDA;
    leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
    + uLeave1*vert[3] + uLeave2*vert[0];
    
    if (dir.x)  {
      double invDirx = 1.0 / dir.x;
      tLeave = (leavePoint.x - orig.x) * invDirx;
      tEnter = (enterPoint.x - orig.x) * invDirx;
    }
    else if (dir.y)  {
      double invDiry = 1.0 / dir.y;
      tLeave = (leavePoint.y - orig.y) * invDiry;
      tEnter = (enterPoint.y - orig.y) * invDiry;
    }
    else  {
      double invDirz = 1.0 / dir.z;
      tLeave = (leavePoint.z - orig.z) * invDirz;
      tEnter = (enterPoint.z - orig.z) * invDirz;
    }
    
    return true;
  }
  else  {
    enterFace = 1;
    uEnter1 =  QAC * invVolCDA;
    uEnter2 = -QDC * invVolCDA;
    enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
    + uEnter1*vert[3] + uEnter2*vert[0];
    
    if (dir.x)  {
      double invDirx = 1.0 / dir.x;
      tLeave = (leavePoint.x - orig.x) * invDirx;
      tEnter = (enterPoint.x - orig.x) * invDirx;
    }
    else if (dir.y)  {
      double invDiry = 1.0 / dir.y;
      tLeave = (leavePoint.y - orig.y) * invDiry;
      tEnter = (enterPoint.y - orig.y) * invDiry;
    }
    else  {
      double invDirz = 1.0 / dir.z;
      tLeave = (leavePoint.z - orig.z) * invDirz;
      tEnter = (enterPoint.z - orig.z) * invDirz;
    }
    
    return true;
  }
    }
    else  {
      // Face DCB intersects with the ray
      if (signQDB == -2)  {
	QDB = QxD * B;
      }
      
      double invVolDCB = 1.0 / (QDC - QBC - QDB);
      if (nextSign == 1)  {
	leaveFace = 0;
	uLeave1 = -QDB * invVolDCB;
	uLeave2 =  QDC * invVolDCB;
	leavePoint =   (1-uLeave1-uLeave2)*vert[3] 
	+ uLeave1*vert[2] + uLeave2*vert[1];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	}
	
	return true;
      }
      else  {
	enterFace = 0;
	uEnter1 = -QDB * invVolDCB;
	uEnter2 =  QDC * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert[3] 
	+ uEnter1*vert[2] + uEnter2*vert[1];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig.x) * invDirx;
	  tEnter = (enterPoint.x - orig.x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig.y) * invDiry;
	  tEnter = (enterPoint.y - orig.y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig.z) * invDirz;
	  tEnter = (enterPoint.z - orig.z) * invDirz;
	}
	
	return true;
      }
    }
}