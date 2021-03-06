#ifdef cl_khr_fp64
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

#ifdef cl_amd_fp64
    #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif


//Section 3.2 Optimizations added
__kernel void RayTetraSTP1(
				__global double4* orig,
				__global double4* dest,
				__global double4* vert0,
				__global double4* vert1,
				__global double4* vert2,
				__global double4* vert3,
				__global double8* cartesian,
				__global double4* barycentric,
				__global double2* parametric)
{
	uint tid = get_global_id(0);

	//Initialization of output vars
	int enterFace = -1;
	int leaveFace = -1;
	double uEnter1= 0;
	double uEnter2= 0;
	double uLeave1= 0;
	double uLeave2= 0;
	double tEnter= 0;
	double tLeave= 0;
	double4 enterPoint = (double4) (0,0,0,0);
	double4 leavePoint = (double4) (0,0,0,0);

  
  //Signs for each STP  var 
  int signQAB = -2, signQAC = -2, signQAD = -2, 
  signQBC = -2, signQBD = -2, signQCD = -2;
  
  int nextSign;
  
	//Point Translation
	double4 dir = dest[tid]-orig[tid];//dir = Q
	double4 A = vert0[tid] - orig[tid];
	double4 B = vert1[tid] - orig[tid];
	double4 C = vert2[tid] - orig[tid];
	double4 D = vert3[tid] - orig[tid];

	//Initial cross product calculation
	double4 QxA = cross(dir,A);
	double4 QxB = cross(dir,B);

	//Intersection test with face ABC(face3)
	//STPs needed:[QAB][QBC][QCA]
	//[QAB]
	double QAB = dot(QxA,B);
	signQAB = sign(QAB);

	//[QBC]
	double QBC = dot(QxB,C);
	signQBC = sign(QBC);
  
  
  
  //Intersection test with face ABC(face3)
  if ((signQAB == signQBC)  ||  (signQAB == 0)  ||  (signQBC == 0))  {
    
    //[QAC] == -[QCA]
    double QAC = dot(QxA,C);
    signQAC = sign(QAC);
    
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
	leavePoint =   (1-uLeave1-uLeave2)*vert0[tid] + uLeave1*vert1[tid] + uLeave2*vert2[tid];
	nextSign = -1;
      }else{
	enterFace = 3;
	uEnter1 = -QAC * invVolABC;
	uEnter2 = QAB * invVolABC;
	enterPoint =   (1-uEnter1-uEnter2)*vert0[tid] + uEnter1*vert1[tid] + uEnter2*vert2[tid];
	nextSign = 1;
      }
      //Faces remaining:BAD,CDA,DCB
      //Intersection test with face BAD(face2)
      //STPs needed:[QBA][QAD][QDB]
      //[QBA] == - [QAB]
      
      //[QAD]
      double QAD = dot(QxA,D);
      signQAD = sign(QAD);
      
      //Intersection test with face BAD
      if ((signQAD == nextSign)  ||  (signQAD == 0))  {
	
	//[QBD] == - [QDB]
	double QBD = dot(QxB,D);
	signQBD = sign(QBD);
	
	if ((-signQBD == nextSign) || ((signQBD == 0) && ((signQAD != 0)  ||  (signQAB != 0))))  {
	  // Face BAD intersects with the ray
	  double invVolBAD = 1.0/(-QAB + QAD - QBD);
	  if (nextSign == 1)  {
	    leaveFace = 2;
	    uLeave1 = -QBD * invVolBAD;
	    uLeave2 = -QAB * invVolBAD;
	    leavePoint =   (1-uLeave1-uLeave2)*vert1[tid] + uLeave1*vert0[tid] + uLeave2*vert3[tid];
	    if (dir.s0)  {
	      double invDirx = 1.0 / dir.s0;
	      tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	      tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.s1;
	      tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	      tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.s2;
	      tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	      tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	    }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	  }else{
	    enterFace = 2;
	    uEnter1 = -QBD * invVolBAD;
	    uEnter2 = -QAB * invVolBAD;
	    enterPoint =   (1-uEnter1-uEnter2)*vert1[tid] + uEnter1*vert0[tid] + uEnter2*vert3[tid];
	    if (dir.s0)  {
	      double invDirx = 1.0 / dir.s0;
	      tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	      tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	    }
	    else if (dir.s1)  {
	      double invDiry = 1.0 / dir.s1;
	      tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	      tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.s2;
	      tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	      tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	    }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	  }
	}
      }
      // Face BAD does not intersect with the ray.
      // Determine the other intersecting face between CDA, DCB
      
      //Intersection test with face CDA(face1)
      //STPs needed:[QCD][QDA][QAC]
      
      //[QCD]
      double4 QxC = cross(dir,C);
      double QCD = dot(QxC,D);
      signQCD = sign(QCD);
      if ((signQCD == nextSign)  ||((signQCD == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
	// Face CDA intersects with the ray
	double invVolCDA = 1.0/(QCD -QAD +QAC);
	if (nextSign == 1)  {
	  leaveFace = 1;
	  uLeave1 = QAC * invVolCDA;
	  uLeave2 = QCD * invVolCDA;
	  leavePoint =   (1-uLeave1-uLeave2)*vert2[tid] + uLeave1*vert3[tid] + uLeave2*vert0[tid];
	  if (dir.s0)  {
	    double invDirx = 1.0 / dir.s0;
	    tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	    tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	  }
	  else if (dir.s1)  {
	    double invDiry = 1.0 / dir.s1;
	    tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	    tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.s2;
	    tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	    tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	  }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	}else{
	  enterFace = 1;
	  uEnter1 = QAC * invVolCDA;
	  uEnter2 = QCD * invVolCDA;
	  enterPoint =   (1-uEnter1-uEnter2)*vert2[tid] + uEnter1*vert3[tid] + uEnter2*vert0[tid];
	  if (dir.s0)  {
	    double invDirx = 1.0 / dir.s0;
	    tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	    tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	  }
	  else if (dir.s1)  {
	    double invDiry = 1.0 / dir.s1;
	    tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	    tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.s2;
	    tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	    tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	  }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	}
      }else{
	//Face DCB intersects with the ray
	double QBD = dot(QxB,D);
	double invVolDCB = 1.0/(-QCD -QBC + QBD);
	if (nextSign == 1)  {
	  leaveFace = 0;
	  uLeave1 = QBD * invVolDCB;
	  uLeave2 = -QCD * invVolDCB;
	  leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] + uLeave1*vert2[tid] + uLeave2*vert1[tid];
	  if (dir.s0)  {
	    double invDirx = 1.0 / dir.s0;
	    tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	    tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	  }
	  else if (dir.s1)  {
	    double invDiry = 1.0 / dir.s1;
	    tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	    tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.s2;
	    tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	    tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	  }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	}else{
	  enterFace = 0;
	  uEnter1 = QBD * invVolDCB;
	  uEnter2 = -QCD * invVolDCB;
	  enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] + uEnter1*vert2[tid] + uEnter2*vert1[tid];
	  if (dir.s0)  {
	    double invDirx = 1.0 / dir.s0;
	    tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	    tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	  }
	  else if (dir.s1)  {
	    double invDiry = 1.0 / dir.s1;
	    tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	    tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.s2;
	    tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	    tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	  }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	}
	
      }
    }
    
  }
  
  //Intersection test with face BAD(face2)
  //STPs needed:[QBA][QAD][QDB]
  //STPs precalculated [QAB][QBC]
  //[QBA] = - [QAB]
  
  //[QAD]
  double QAD = dot(QxA,D);
  signQAD = sign(QAD);
  
  if ((signQAD == -signQAB)  ||  (signQAB == 0)  ||  (signQAD == 0))  {
    
    //[QBD] == - [QDB]
    double QBD = dot(QxB,D);
    signQBD = sign(QBD);
    
    
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
	leavePoint =   (1-uLeave1-uLeave2)*vert1[tid] + uLeave1*vert0[tid] + uLeave2*vert3[tid];
	nextSign = -1;
      }else{
	enterFace = 2;
	uEnter1 = -QBD * invVolBAD;
	uEnter2 = -QAB * invVolBAD;
	enterPoint =   (1-uEnter1-uEnter2)*vert1[tid] + uEnter1*vert0[tid] + uEnter2*vert3[tid];
	nextSign = 1;
      }
      // Determine the other intersecting face between CDA, DCB
      // Examine face CDA
      // STPs needed:[QCD][QDA][QAC]
      
      //[QAC]
      double4 QxC = cross(dir,C);
      double QAC = dot(QxA,C);
      signQAC = sign(QAC);
      if ((-signQAD == signQAC)  ||  (signQAC == 0)  ||  (signQAD == 0))  {
	
	//[QCD]
	double4 QxC = cross(dir,C);
	double QCD = dot(QxC,D);
	signQCD = sign(QCD);
	
	if ((signQCD == nextSign)  || ((signQCD == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
	  // Face CDA intersects with the ray
	  double invVolCDA = 1.0 / (QCD -QAD +QAC);
	  if (nextSign == 1)  {
	    leaveFace = 1;
	    uLeave1 =  QAC * invVolCDA;
	    uLeave2 =  QCD * invVolCDA;
	    leavePoint =   (1-uLeave1-uLeave2)*vert2[tid] + uLeave1*vert3[tid] + uLeave2*vert0[tid];
	    if (dir.s0)  {
	      double invDirx = 1.0 / dir.s0;
	      tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	      tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	    }
	    else if (dir.s1)  {
	      double invDiry = 1.0 / dir.s1;
	      tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	      tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.s2;
	      tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	      tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	    }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	  }else{
	    enterFace = 1;
	    uEnter1 =  QAC * invVolCDA;
	    uEnter2 =  QCD * invVolCDA;
	    enterPoint =   (1-uEnter1-uEnter2)*vert2[tid] + uEnter1*vert3[tid] + uEnter2*vert0[tid];
	    if (dir.s0)  {
	      double invDirx = 1.0 / dir.s0;
	      tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	      tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	    }
	    else if (dir.s1)  {
	      double invDiry = 1.0 / dir.s1;
	      tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	      tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.s2;
	      tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	      tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	    }
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
	  }
	}
      }
      // Face DCB intersects with the ray
      double QCD = dot(QxC,D);
      double QBD = dot(QxB,D);
      double invVolDCB = 1.0 / (-QCD -QBC + QBD);
      if (nextSign == 1)  {
	leaveFace = 0;
	uLeave1 =  QBD * invVolDCB;
	uLeave2 =  -QCD * invVolDCB;
	leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] + uLeave1*vert2[tid] + uLeave2*vert1[tid];
	if (dir.s0)  {
	  double invDirx = 1.0 / dir.s0;
	  tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	  tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	}
	else if (dir.s1)  {
	  double invDiry = 1.0 / dir.s1;
	  tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	  tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.s2;
	  tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	  tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	}
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
      }else{
	enterFace = 0;
	uEnter1 = QBD * invVolDCB;
	uEnter2 = -QCD * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] + uEnter1*vert2[tid] + uEnter2*vert1[tid];
	if (dir.s0)  {
	  double invDirx = 1.0 / dir.s0;
	  tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	  tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	}
	else if (dir.s1)  {
	  double invDiry = 1.0 / dir.s1;
	  tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	  tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.s2;
	  tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	  tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	}
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
      }
    }
  }
  
  
  
  //Intersection test with face CDA(face1)
  //STPs needed:[QCD][QDA][QAC]
  //STPs precalculated [QAB][QBC][QAD]
  
  //[QAC]
  double QAC = dot(QxA,C);
  signQAC = sign(QAC);
  
  if ((-signQAD == signQAC)  ||  (signQAC == 0)  ||  (signQAD == 0))  {
    // Face CDA may intersect with the ray
    
    //[QCD]
    double4 QxC = cross(dir,C);
    double QCD = dot(QxC,D);
    signQCD = sign(QCD);
    
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
      double QBD = dot(QxB,D);
      double invVolDCB = 1.0 / (-QCD - QBC + QBD);
      
      if (signCDA == 1)  {
	enterFace = 0;
	leaveFace = 1;
	uLeave1 =  QAC * invVolCDA;
	uLeave2 =  QCD * invVolCDA;
	uEnter1 =  QBD * invVolDCB;
	uEnter2 = -QCD * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] + uEnter1*vert2[tid] + uEnter2*vert1[tid];
	leavePoint =   (1-uLeave1-uLeave2)*vert2[tid] + uLeave1*vert3[tid] + uLeave2*vert0[tid];
	if (dir.s0)  {
	  double invDirx = 1.0 / dir.s0;
	  tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	  tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	}
	else if (dir.s1)  {
	  double invDiry = 1.0 / dir.s1;
	  tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	  tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.s2;
	  tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	  tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	}
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
      }
      else  {
	enterFace = 1;
	leaveFace = 0;			
	uEnter1 =  QAC * invVolCDA;
	uEnter2 = QCD * invVolCDA;
	uLeave1 = QBD * invVolDCB;
	uLeave2 =  -QCD * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert2[tid] + uEnter1*vert3[tid] + uEnter2*vert0[tid];
	leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] + uLeave1*vert2[tid] + uLeave2*vert1[tid];
	if (dir.s0)  {
	  double invDirx = 1.0 / dir.s0;
	  tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	  tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
	}
	else if (dir.s1)  {
	  double invDiry = 1.0 / dir.s1;
	  tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	  tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.s2;
	  tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	  tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
	}
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
      }
    }
  }
  
  // Three faces do not intersect with the ray, the fourth will not.
	cartesian[tid].s0 = enterFace;
	cartesian[tid].s1 = leaveFace; 
	cartesian[tid].s2 = enterPoint.s0;
	cartesian[tid].s3 = enterPoint.s1; 
	cartesian[tid].s4 = enterPoint.s2;
	cartesian[tid].s5 = leavePoint.s0;
	cartesian[tid].s6 = leavePoint.s1;
	cartesian[tid].s7 = leavePoint.s2;
	barycentric[tid].s0 = uEnter1;
	barycentric[tid].s1 = uEnter2;
	barycentric[tid].s2 = uLeave1;
	barycentric[tid].s3 = uLeave2;	 
	parametric[tid].s0 = tEnter;
	parametric[tid].s1 = tLeave;
	return;
}
