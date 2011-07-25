#ifdef cl_khr_fp64
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

#ifdef cl_amd_fp64
    #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif


//Segura Algorithm with Scalar Triple Product Calc
__kernel void RayTetraSTP0(
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
	int leaveface = -1;
	int enterface = -1;
	double uLeave1= 0;
	double uLeave2= 0;
	double uEnter1= 0;
	double uEnter2= 0;
	double tLeave= 0;
	double tEnter= 0;
	double4 leavePoint = (double4) (0,0,0,0);
	double4 enterPoint = (double4) (0,0,0,0);


	//Point Translation
	double4 dir = dest[tid]-orig[tid]; //dir = Q
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

	//[QAC] = - [QCA]
	double QAC = dot(QxA,C);
	int signQAC = sign(QAC);


	if ((signQAB!=0) && (signQAC!=0)&& (signQBC!=0))
	{
		double invVolABC = 1.0 / (QAB + QBC - QAC);
		if (( signQAB > 0.0)&& (signQAC < 0.0)&& ( signQBC > 0.0)){
			//face 3 is the entry face
			leaveface = 3;
			uLeave1 = -QAC * invVolABC;
			uLeave2 =  QAB * invVolABC;
			leavePoint = (1 - uLeave1- uLeave2 ) * vert0[tid] + uLeave1 * vert1[tid] + uLeave2 * vert2[tid];

		}else if (( signQAB < 0.0)&& (signQAC > 0.0)&& ( signQBC < 0.0)){
			//face 3 is the exit face
			enterface = 3;
			uEnter1 = -QAC * invVolABC;
			uEnter2 =  QAB * invVolABC;
			enterPoint = (1 - uEnter1- uEnter2 ) * vert0[tid] + uEnter1 * vert1[tid] + uEnter2 * vert2[tid];
		}
	}

	//Intersection test with face BAD(face2)
	//STPs needed:[QBA][QAD][QDB]
	//Available: [QAB][QBC][QAC]

	//[QBA] == - [QAB]

	//[QAD]
	double QAD = dot(QxA,D);
	int signQAD = sign(QAD);


	//[QDB] == - [QBD]
	//[QBD] is selected in order to avoid computing QxD.
	double QBD = dot(QxB,D);
	int signQBD = sign(QBD);

	if ((signQAB!=0) && (signQAD!=0)&& (signQBD!=0))
	{
		double invVolBAD = 1.0 / (QAD - QBD - QAB);
		if ((leaveface == -1) && ( signQAB < 0.0)&& ( signQAD > 0.0)&& (signQBD < 0.0)){
			//face 2 is the entry face
			leaveface = 2;
			uLeave1 = -QBD * invVolBAD;
			uLeave2 = -QAB * invVolBAD;
			leavePoint =   (1-uLeave1-uLeave2)*vert1[tid] + uLeave1*vert0[tid] + uLeave2*vert3[tid];

		}else if ((enterface == -1) && ( signQAB > 0.0)&& ( signQAD < 0.0)&& (signQBD > 0.0)){
			//face 2 is the exit face
			enterface = 2;
			uEnter1 =  -QBD * invVolBAD;
			uEnter2 =  -QAB * invVolBAD;
			enterPoint =   (1-uEnter1-uEnter2)*vert1[tid] + uEnter1*vert0[tid] + uEnter2*vert3[tid];
		}
	}

	//Intersection test with face CDA(face1)
	//STPs needed:[QCD][QDA][QAC]
	//Available: [QAB][QBC][QAC][QAD][QBD]

	//[QCD]
	double4 QxC = cross(dir,C);
	double QCD = dot(QxC,D);
	int signQCD = sign(QCD);

	//[QDA] = - [QAD]


	if ((signQAD!=0) && (signQAC!=0)&& (signQCD!=0))
	{
		double invVolCDA = 1.0 / (QAC + QCD - QAD);
		if ((leaveface == -1) && ( signQAD < 0.0)&& ( signQAC > 0.0)&& (signQCD > 0.0)){
			//face 1 is the entry face
			leaveface = 1;
			uLeave1 =  QAC * invVolCDA;
			uLeave2 =  QCD * invVolCDA;
			leavePoint = (1-uLeave1-uLeave2)*vert2[tid] + uLeave1*vert3[tid] + uLeave2 * vert0[tid];
		}else if ((enterface == -1) && ( signQAD > 0.0)&& ( signQAC < 0.0)&& (signQCD < 0.0)){
			//face 1 is the exit face
			enterface = 1;
			uEnter1 =  QAC * invVolCDA;
			uEnter2 =  QCD * invVolCDA;
			enterPoint =   (1-uEnter1-uEnter2)* vert2[tid] + uEnter1 * vert3[tid] + uEnter2*vert0[tid];
		}
	}

	// If no intersecting face has been found so far, then there is none,
	// otherwise DCB (face0) is the remaining one
	//STPs relevant to DCB:[QDC][QCB][QBD]
	//Available: [QAB][QBC][QAC][QAD][QBD][QCD]

	if ((signQBC!=0)&& (signQBD!=0) && (signQCD!=0))
	{
		double invVolDCB = 1.0 / (-QCD - QBC + QBD);
		if ((leaveface == -1) && ( signQBC < 0.0)&& (signQBD > 0.0)&& ( signQCD < 0.0)){
			//face 0 is the entry face
			leaveface = 0;
			uLeave1 =  QBD * invVolDCB;
			uLeave2 = -QCD * invVolDCB;
			leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] + uLeave1*vert2[tid] + uLeave2*vert1[tid];
		}else if ((enterface == -1) && ( signQBC > 0.0)&& (signQBD < 0.0)&& ( signQCD > 0.0)){
			//face 0 is the exit face
			enterface = 0;
			uEnter1 =  QBD * invVolDCB;
			uEnter2 = -QCD * invVolDCB;
			enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] + uEnter1*vert2[tid] + uEnter2*vert1[tid];
		}
	}

	//Calculating the parametric distances tLeave and tEnter

	if (dir.s0)  {
		double invDirx = 1.0 / dir.s0;
		tLeave = (leavePoint.s0 - orig[tid].s0) * invDirx;
		tEnter = (enterPoint.s0 - orig[tid].s0) * invDirx;
	}
	else if (dir.s1)  {
		double invDiry = 1.0 / dir.s1;
		tLeave = (leavePoint.s1 - orig[tid].s1) * invDiry;
		tEnter = (enterPoint.s1 - orig[tid].s1) * invDiry;
	}
	else  {
		double invDirz = 1.0 / dir.s2;
		tLeave = (leavePoint.s2 - orig[tid].s2) * invDirz;
		tEnter = (enterPoint.s2 - orig[tid].s2) * invDirz;
	}

	tLeave = (leaveface != -1)? tLeave : 0;
	tEnter = (enterface != -1)? tEnter : 0;


	cartesian[tid].s0 = enterface;
	cartesian[tid].s1 = leaveface; 
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
}
				
