#ifdef cl_khr_fp64
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

#ifdef cl_amd_fp64
    #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif


//Section 3.3 Modifications added
__kernel void RayTetraSTP2(
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

	int nextSign;
  
	//Point Translation
	double4 dir = dest[tid] -orig[tid];//dir = Q
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
  int signQAB = sign(QAB);
  
  //[QBC]
  double QBC = dot(QxB,C);
  int signQBC = sign(QBC);
  
  //[QAC]
  double QAC = dot(QxA,C);
  int signQAC = sign(QAC);
  
  
  if ((signQAB == 0)  &&  (signQBC == 0)  &&  (signQAC == 0))  {
    // Ray is on the plane of ABC
    // Examine face BAD
    
    //[QAD]
    double QAD = dot(QxA,D);
    int signQAD = sign(QAD);
    
    double4 QxD = cross(dir,D);
    
    //[QDB]
    double QDB = dot(QxD,B);
    int signQDB = sign(QDB);
    
    //[QDC]
    double QDC = dot(QxD,C);
    int signQDC = sign(QDC);	
    
    
    if ((signQAD == 0)  &&  (signQDB == 0))  {
      if (signQDC == 0)  {
	// The tetrahedron is flat and the ray is on its plane
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
	// The ray is on the AB edge
	if (signQDC == 1)  {
	  leaveFace = 1;
	  uLeave1 = 0;
	  uLeave2 = 1;
	  leavePoint = vert0[tid];
	  
	  enterFace = 0;
	  uEnter1 = 0;
	  uEnter2 = 1;
	  enterPoint = vert1[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	  uEnter1 = 0;
	  uEnter2 = 1;
	  enterPoint = vert0[tid];
	  
	  leaveFace = 0;
	  uLeave1 = 0;
	  uLeave2 = 1;
	  leavePoint = vert1[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
    else  {
      // Check if the ray intersects with face BAD
      if ((signQAD >= 0)  &&  (signQDB >= 0))  {
	// The ray leaves through BAD
	leaveFace = 2;
	double invVolBAD = 1.0 / (QAD + QDB);
	uLeave1 =  QDB * invVolBAD;
	uLeave2 =  0;
	leavePoint = (1-uLeave1)*vert1[tid] + uLeave1*vert0[tid];
	
	// Determine the other intersecting face between CDA, DCB
	if ((signQDC == 1)  ||
	  ((signQDC == 0)  &&  (signQAD == 1)))  {
	  // The ray enters through CDA
	  enterFace = 1;
	double invVolCDA = 1.0 / (-QDC - QAD);
	uEnter1 =  0;
	uEnter2 = -QDC * invVolCDA;
	enterPoint = (1-uEnter2)*vert2[tid] + uEnter2*vert0[tid];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	  tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	  tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	  tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	    // The ray enters through DCB
	    enterFace = 0;
	    double invVolDCB = 1.0 / (QDC - QDB);
	    uEnter1 = -QDB * invVolDCB;
	    uEnter2 =  QDC * invVolDCB;
	    enterPoint = uEnter1*vert2[tid] + uEnter2*vert1[tid];
	    
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	      tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	      tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	      tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
      else if ((signQAD <= 0)  &&  (signQDB <= 0))  {
	// The ray enters through BAD
	enterFace = 2;
	
	double invVolBAD = 1.0 / (QAD + QDB);
	uEnter1 =  QDB * invVolBAD;
	uEnter2 =  0;
	enterPoint = (1-uEnter1)*vert1[tid] + uEnter1*vert0[tid];
	
	// Determine the other intersecting face between CDA, DCB
	//[QDC]
	double4 QxD = cross(dir,D);
	double QDC = dot(QxD,C);
	int signQDC = sign(QDC);
	
	if ((signQDC == -1)  ||
	  ((signQDC == 0)  &&  (signQAD == -1)))  {
	  // The ray leaves through CDA
	  leaveFace = 1;
	double invVolCDA = 1.0 / (- QDC - QAD);
	uLeave1 =  0;
	uLeave2 = -QDC * invVolCDA;
	leavePoint =   (1-uLeave2)*vert2[tid] + uLeave2*vert0[tid];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	  tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	  tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	  tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	    // The ray leaves through DCB
	    leaveFace = 0;
	    double invVolDCB = 1.0 / (QDC - QDB);
	    uLeave1 = -QDB * invVolDCB;
	    uLeave2 =  QDC * invVolDCB;
	    leavePoint = uLeave1*vert2[tid] + uLeave2*vert1[tid];
	    
	    if (dir.x)  {
	      double invDirx = 1.0 / dir.x;
	      tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	      tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	    }
	    else if (dir.y)  {
	      double invDiry = 1.0 / dir.y;
	      tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	      tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	    }
	    else  {
	      double invDirz = 1.0 / dir.z;
	      tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	      tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
      else  {
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
  else  {
    // Check if ray intersects with face ABC
    if ((signQAB >= 0)  &&  (signQBC >= 0)  &&  (signQAC <= 0))  {
      leaveFace = 3;
      double invVolABC = 1.0 / (QAB + QBC - QAC);
      uLeave1 = -QAC * invVolABC;
      uLeave2 =  QAB * invVolABC;
      leavePoint =   (1-uLeave1-uLeave2)*vert0[tid] 
      + uLeave1*vert1[tid] + uLeave2*vert2[tid];
      
      nextSign = -1;
      // The other intersecting face is determined after the next 'else'
    }
    else if ((signQAB <= 0)  &&  (signQBC <= 0)  &&  (signQAC >= 0))  {
      enterFace = 3;
      double invVolABC = 1.0 / (QAB + QBC - QAC);
      uEnter1 = -QAC * invVolABC;
      uEnter2 =  QAB * invVolABC;
      enterPoint =   (1-uEnter1-uEnter2)*vert0[tid] 
      + uEnter1*vert1[tid] + uEnter2*vert2[tid];
      
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
	double QAD = dot(QxA,D);
	int signQAD = sign(QAD);
	
	if (signQAD == -nextSign)  {
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
	
	//[QDB]
	double4 QxD = cross(dir,D);
	double QDB = dot(QxD,B);
	int signQDB = sign(QDB);
	
	if (signQDB == -nextSign)  {
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
	
	// Face BAD intersects with the ray
	double invVolBAD = 1.0 / (QAD + QDB - QAB);
	if (nextSign == 1)  {
	  leaveFace = 2;
	  uLeave1 =  QDB * invVolBAD;
	  uLeave2 = -QAB * invVolBAD;
	  leavePoint =   (1-uLeave1-uLeave2)*vert1[tid]
	  + uLeave1*vert0[tid] + uLeave2*vert3[tid];
	  
	  nextSign = -1;
	}
	else  {
	  enterFace = 2;
	  uEnter1 =  QDB * invVolBAD;
	  uEnter2 = -QAB * invVolBAD;
	  enterPoint =   (1-uEnter1-uEnter2)*vert1[tid]
	  + uEnter1*vert0[tid] + uEnter2*vert3[tid];
	  
	  nextSign = 1;
	}
	
	// Determine the other intersecting face between CDA, DCB
	//[QDC]
	double QDC = dot(QxD,C);
	int signQDC = sign(QDC);
	
	if ((signQDC == -nextSign)  ||
	  ((signQDC == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
	  // Face CDA intersects with the ray
	  double invVolCDA = 1.0 / (QAC - QDC - QAD);
	if (nextSign == 1)  {
	  leaveFace = 1;
	  uLeave1 =  QAC * invVolCDA;
	  uLeave2 = -QDC * invVolCDA;
	  leavePoint =   (1-uLeave1-uLeave2)*vert2[tid] 
	  + uLeave1*vert3[tid] + uLeave2*vert0[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	  uEnter1 =  QAC * invVolCDA;
	  uEnter2 = -QDC * invVolCDA;
	  enterPoint =   (1-uEnter1-uEnter2)*vert2[tid] 
	  + uEnter1*vert3[tid] + uEnter2*vert0[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	  else  {
	    // Face DCB intersects with the ray
	    double invVolDCB = 1.0 / (QDC - QBC - QDB);
	    if (nextSign == 1)  {
	      leaveFace = 0;
	      uLeave1 = -QDB * invVolDCB;
	      uLeave2 =  QDC * invVolDCB;
	      leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] 
	      + uLeave1*vert2[tid] + uLeave2*vert1[tid];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig[tid].x) * invDirx;
		tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig[tid].y) * invDiry;
		tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig[tid].z) * invDirz;
		tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	      enterFace = 0;
	      uEnter1 = -QDB * invVolDCB;
	      uEnter2 =  QDC * invVolDCB;
	      enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] 
	      + uEnter1*vert2[tid] + uEnter2*vert1[tid];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig[tid].x) * invDirx;
		tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig[tid].y) * invDiry;
		tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig[tid].z) * invDirz;
		tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
      else if (nextFace == 1)  {
	// Examine face CDA
	
	double4 QxD = cross(dir,D);
	double QDC = dot(QxD,C);
	int signQDC = sign(QDC);
	
	if (signQDC == nextSign)  {
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
	
	//[QAD]
	double QAD = dot(QxA,D);
	int signQAD = sign(QAD);
	
	if (signQAD == nextSign)  {
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
	
	// Face CDA intersects with the ray
	double invVolCDA = 1.0 / (QAC - QDC - QAD);
	if (nextSign == 1)  {
	  leaveFace = 1;
	  uLeave1 =  QAC * invVolCDA;
	  uLeave2 = -QDC * invVolCDA;
	  leavePoint =   (1-uLeave1-uLeave2)*vert2[tid] 
	  + uLeave1*vert3[tid] + uLeave2*vert0[tid];
	  
	  nextSign = -1;
	}
	else  {
	  enterFace = 1;
	  uEnter1 =  QAC * invVolCDA;
	  uEnter2 = -QDC * invVolCDA;
	  enterPoint =   (1-uEnter1-uEnter2)*vert2[tid] 
	  + uEnter1*vert3[tid] + uEnter2*vert0[tid];
	  
	  nextSign = 1;
	}
	
	// Determine the other intersecting face between BAD, DCB
	//[QDB]
	double QDB = dot(QxD,B);
	int signQDB = sign(QDB);
	
	if ((signQDB == nextSign)  ||
	  ((signQDB == 0)  &&  ((signQAB != 0)  ||  (signQAD != 0))))  {
	  // Face BAD intersects with the ray
	  double invVolBAD = 1.0 / (QAD + QDB - QAB);
	if (nextSign == 1)  {
	  leaveFace = 2;
	  uLeave1 =  QDB * invVolBAD;
	  uLeave2 = -QAB * invVolBAD;
	  leavePoint =   (1-uLeave1-uLeave2)*vert1[tid]
	  + uLeave1*vert0[tid] + uLeave2*vert3[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	  enterFace = 2;
	  uEnter1 =  QDB * invVolBAD;
	  uEnter2 = -QAB * invVolBAD;
	  enterPoint =   (1-uEnter1-uEnter2)*vert1[tid]
	  + uEnter1*vert0[tid] + uEnter2*vert3[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	  else  {
	    // Face DCB intersects with the ray
	    double invVolDCB = 1.0 / (QDC - QBC - QDB);
	    if (nextSign == 1)  {
	      leaveFace = 0;
	      uLeave1 = -QDB * invVolDCB;
	      uLeave2 =  QDC * invVolDCB;
	      leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] 
	      + uLeave1*vert2[tid] + uLeave2*vert1[tid];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig[tid].x) * invDirx;
		tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig[tid].y) * invDiry;
		tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig[tid].z) * invDirz;
		tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	      enterFace = 0;
	      uEnter1 = -QDB * invVolDCB;
	      uEnter2 =  QDC * invVolDCB;
	      enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] 
	      + uEnter1*vert2[tid] + uEnter2*vert1[tid];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig[tid].x) * invDirx;
		tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig[tid].y) * invDiry;
		tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig[tid].z) * invDirz;
		tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
      else  {
	// Examine face DCB
	//[QDC]
	double4 QxD = cross(dir,D);
	double QDC = dot(QxD,C);
	int signQDC = sign(QDC);	
	
	
	if (signQDC == -nextSign)  {
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
	
	double QDB = dot(QxD,B);
	int signQDB = sign(QDB);
	
	if (signQDB == nextSign)  {
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
	
	// Face DCB intersects with the ray
	double invVolDCB = 1.0 / (QDC - QBC - QDB);
	if (nextSign == 1)  {
	  leaveFace = 0;
	  uLeave1 = -QDB * invVolDCB;
	  uLeave2 =  QDC * invVolDCB;
	  leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] 
	  + uLeave1*vert2[tid] + uLeave2*vert1[tid];
	  
	  nextSign = -1;
	}
	else  {
	  enterFace = 0;
	  uEnter1 = -QDB * invVolDCB;
	  uEnter2 =  QDC * invVolDCB;
	  enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] 
	  + uEnter1*vert2[tid] + uEnter2*vert1[tid];
	  
	  nextSign = 1;
	}
	
	// Determine the other intersecting face between BAD, CDA
	//[QAD]
	double QAD = dot(QxA,D);
	int signQAD = sign(QAD);
	
	if ((signQAD == nextSign)  ||
	  ((signQAD == 0)  &&  ((signQAB != 0)  ||  (signQDB != 0))))  {
	  // Face BAD intersects with the ray
	  double invVolBAD = 1.0 / (QAD + QDB - QAB);
	if (nextSign == 1)  {
	  leaveFace = 2;
	  uLeave1 =  QDB * invVolBAD;
	  uLeave2 = -QAB * invVolBAD;
	  leavePoint =   (1-uLeave1-uLeave2)*vert1[tid]
	  + uLeave1*vert0[tid] + uLeave2*vert3[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	  enterFace = 2;
	  uEnter1 =  QDB * invVolBAD;
	  uEnter2 = -QAB * invVolBAD;
	  enterPoint =   (1-uEnter1-uEnter2)*vert1[tid]
	  + uEnter1*vert0[tid] + uEnter2*vert3[tid];
	  
	  if (dir.x)  {
	    double invDirx = 1.0 / dir.x;
	    tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	    tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	  }
	  else if (dir.y)  {
	    double invDiry = 1.0 / dir.y;
	    tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	    tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	  }
	  else  {
	    double invDirz = 1.0 / dir.z;
	    tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	    tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	  else  {
	    // Face CDA intersects with the ray
	    double invVolCDA = 1.0 / (QAC - QDC - QAD);
	    if (nextSign == 1)  {
	      leaveFace = 1;
	      uLeave1 =  QAC * invVolCDA;
	      uLeave2 = -QDC * invVolCDA;
	      leavePoint =   (1-uLeave1-uLeave2)*vert2[tid] 
	      + uLeave1*vert3[tid] + uLeave2*vert0[tid];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig[tid].x) * invDirx;
		tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig[tid].y) * invDiry;
		tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig[tid].z) * invDirz;
		tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	      uEnter1 =  QAC * invVolCDA;
	      uEnter2 = -QDC * invVolCDA;
	      enterPoint =   (1-uEnter1-uEnter2)*vert2[tid] 
	      + uEnter1*vert3[tid] + uEnter2*vert0[tid];
	      
	      if (dir.x)  {
		double invDirx = 1.0 / dir.x;
		tLeave = (leavePoint.x - orig[tid].x) * invDirx;
		tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	      }
	      else if (dir.y)  {
		double invDiry = 1.0 / dir.y;
		tLeave = (leavePoint.y - orig[tid].y) * invDiry;
		tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	      }
	      else  {
		double invDirz = 1.0 / dir.z;
		tLeave = (leavePoint.z - orig[tid].z) * invDirz;
		tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
  }
  // Face ABC intersects with the ray.
  // Determine the other intersecting face between BAD, CDA, DCB
  // Examine face BAD
  //[QAD]
  double QAD = dot(QxA,D);
  int signQAD = sign(QAD);
  
  
  double QDB = 0;
  int signQDB = -2;
  
  if ((signQAD == nextSign)  ||  (signQAD == 0))  {
    // Face BAD may intersect with the ray
    double4 QxD = cross(dir,D);
    double QDB = dot(QxD,B);
    int signQDB = sign(QDB);
    
    if ((signQDB == nextSign)  ||  
      ((signQDB == 0)  &&  
      ((signQAD != 0)  ||  (signQAB != 0))))  {
      // Face BAD intersects with the ray
      double invVolBAD = 1.0 / (QAD + QDB - QAB);
    if (nextSign == 1)  {
      leaveFace = 2;
      uLeave1 =  QDB * invVolBAD;
      uLeave2 = -QAB * invVolBAD;
      leavePoint =   (1-uLeave1-uLeave2)*vert1[tid]
      + uLeave1*vert0[tid] + uLeave2*vert3[tid];
      
      if (dir.x)  {
	double invDirx = 1.0 / dir.x;
	tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	tEnter = (enterPoint.x - orig[tid].x) * invDirx;
      }
      else if (dir.y)  {
	double invDiry = 1.0 / dir.y;
	tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	tEnter = (enterPoint.y - orig[tid].y) * invDiry;
      }
      else  {
	double invDirz = 1.0 / dir.z;
	tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
      enterFace = 2;
      uEnter1 =  QDB * invVolBAD;
      uEnter2 = -QAB * invVolBAD;
      enterPoint =   (1-uEnter1-uEnter2)*vert1[tid]
      + uEnter1*vert0[tid] + uEnter2*vert3[tid];
      
      if (dir.x)  {
	double invDirx = 1.0 / dir.x;
	tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	tEnter = (enterPoint.x - orig[tid].x) * invDirx;
      }
      else if (dir.y)  {
	double invDiry = 1.0 / dir.y;
	tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	tEnter = (enterPoint.y - orig[tid].y) * invDiry;
      }
      else  {
	double invDirz = 1.0 / dir.z;
	tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
  //[QDC]
  double4 QxD = cross(dir,D);
  double QDC = dot(QxD,C);
  int signQDC = sign(QDC);
  
  if ((signQDC == -nextSign)  ||
    ((signQDC == 0)  &&  ((signQAD != 0)  ||  (signQAC != 0))))  {
    // Face CDA intersects with the ray
    double invVolCDA = 1.0 / (QAC - QDC - QAD);
  if (nextSign == 1)  {
    leaveFace = 1;
    uLeave1 =  QAC * invVolCDA;
    uLeave2 = -QDC * invVolCDA;
    leavePoint =   (1-uLeave1-uLeave2)*vert2[tid] 
    + uLeave1*vert3[tid] + uLeave2*vert0[tid];
    
    if (dir.x)  {
      double invDirx = 1.0 / dir.x;
      tLeave = (leavePoint.x - orig[tid].x) * invDirx;
      tEnter = (enterPoint.x - orig[tid].x) * invDirx;
    }
    else if (dir.y)  {
      double invDiry = 1.0 / dir.y;
      tLeave = (leavePoint.y - orig[tid].y) * invDiry;
      tEnter = (enterPoint.y - orig[tid].y) * invDiry;
    }
    else  {
      double invDirz = 1.0 / dir.z;
      tLeave = (leavePoint.z - orig[tid].z) * invDirz;
      tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
    uEnter1 =  QAC * invVolCDA;
    uEnter2 = -QDC * invVolCDA;
    enterPoint =   (1-uEnter1-uEnter2)*vert2[tid] 
    + uEnter1*vert3[tid] + uEnter2*vert0[tid];
    
    if (dir.x)  {
      double invDirx = 1.0 / dir.x;
      tLeave = (leavePoint.x - orig[tid].x) * invDirx;
      tEnter = (enterPoint.x - orig[tid].x) * invDirx;
    }
    else if (dir.y)  {
      double invDiry = 1.0 / dir.y;
      tLeave = (leavePoint.y - orig[tid].y) * invDiry;
      tEnter = (enterPoint.y - orig[tid].y) * invDiry;
    }
    else  {
      double invDirz = 1.0 / dir.z;
      tLeave = (leavePoint.z - orig[tid].z) * invDirz;
      tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
    else  {
      // Face DCB intersects with the ray
      if (signQDB == -2)  {
	QDB = dot(QxD,B);
      }
      
      double invVolDCB = 1.0 / (QDC - QBC - QDB);
      if (nextSign == 1)  {
	leaveFace = 0;
	uLeave1 = -QDB * invVolDCB;
	uLeave2 =  QDC * invVolDCB;
	leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] 
	+ uLeave1*vert2[tid] + uLeave2*vert1[tid];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	  tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	  tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	  tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
	enterFace = 0;
	uEnter1 = -QDB * invVolDCB;
	uEnter2 =  QDC * invVolDCB;
	enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] 
	+ uEnter1*vert2[tid] + uEnter2*vert1[tid];
	
	if (dir.x)  {
	  double invDirx = 1.0 / dir.x;
	  tLeave = (leavePoint.x - orig[tid].x) * invDirx;
	  tEnter = (enterPoint.x - orig[tid].x) * invDirx;
	}
	else if (dir.y)  {
	  double invDiry = 1.0 / dir.y;
	  tLeave = (leavePoint.y - orig[tid].y) * invDiry;
	  tEnter = (enterPoint.y - orig[tid].y) * invDiry;
	}
	else  {
	  double invDirz = 1.0 / dir.z;
	  tLeave = (leavePoint.z - orig[tid].z) * invDirz;
	  tEnter = (enterPoint.z - orig[tid].z) * invDirz;
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
