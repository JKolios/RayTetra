#pragma OPENCL EXTENSION cl_amd_fp64 : enable

//Base Segura Algorithm
__kernel void RayTetraSegura0(
				__constant double4* orig,
				__constant double4* dest,
				__constant double4* vert0,
				__constant double4* vert1,
				__constant double4* vert2,
				__constant double4* vert3,
				__global double8* cartesian,
				__global double4* barycentric,
				__global double2* parametric)
{
	uint tid = get_global_id(0);

	//Initialization of output vars
	int enterface = -1;
	int leaveface = -1;
	double uEnter1= 0;
	double uEnter2= 0;
	double uLeave1= 0;
	double uLeave2= 0;
	double tEnter= 0;
	double tLeave= 0;
	double4 enterPoint = (double4) (0,0,0,0);
	double4 leavePoint = (double4) (0,0,0,0);

	double4 dir = dest[tid]-orig[tid];


	//Pluecker coordinates for the ray
	double4 cross_ray =  cross(orig[tid],dir);
	double8 rayPluecker = (double8) (dir.s0,dir.s1,dir.s2,cross_ray.s0,cross_ray.s1,cross_ray.s2,0.0,0.0);


	//Edge AB
	double4 dir_vert =  vert0[tid] - vert1[tid] ;
	//The cross product of dir_vert with a point on the line 
	double4 cross_vert = cross(vert1[tid],dir_vert);
	double8 vertPluecker = (double8) (dir_vert.s0,dir_vert.s1,dir_vert.s2,cross_vert.s0,cross_vert.s1,cross_vert.s2,0.0,0.0);;
	//The permuted inner product for AB
	double uAB = (rayPluecker.s0 * vertPluecker.s3) + (rayPluecker.s1 * vertPluecker.s4) + (rayPluecker.s2 * vertPluecker.s5) + (rayPluecker.s3 * vertPluecker.s0) + (rayPluecker.s4 * vertPluecker.s1) + (rayPluecker.s5 * vertPluecker.s2                                                                                       ); 
	double signAB = sign(uAB);

	//Edge AC
	dir_vert =  vert0[tid] - vert2[tid] ;
	//The cross product of dir_vert with a point on the line 
	cross_vert = cross(vert2[tid],dir_vert);
	vertPluecker = (double8) (dir_vert.s0,dir_vert.s1,dir_vert.s2,cross_vert.s0,cross_vert.s1,cross_vert.s2,0.0,0.0);;
	//The permuted inner product for AC
	double uAC = (rayPluecker.s0 * vertPluecker.s3) + (rayPluecker.s1 * vertPluecker.s4) + (rayPluecker.s2 * vertPluecker.s5) + (rayPluecker.s3 * vertPluecker.s0) + (rayPluecker.s4 * vertPluecker.s1) + (rayPluecker.s5 * vertPluecker.s2                                                                                       ); 
	double signAC = sign(uAC);

	//Edge BC
	dir_vert =  vert1[tid] - vert2[tid] ;
	//The cross product of dir_vert with a point on the line 
	cross_vert = cross(vert2[tid],dir_vert);
	vertPluecker = (double8) (dir_vert.s0,dir_vert.s1,dir_vert.s2,cross_vert.s0,cross_vert.s1,cross_vert.s2,0.0,0.0);;
	//The permuted inner product for BC
	double uBC = (rayPluecker.s0 * vertPluecker.s3) + (rayPluecker.s1 * vertPluecker.s4) + (rayPluecker.s2 * vertPluecker.s5) + (rayPluecker.s3 * vertPluecker.s0) + (rayPluecker.s4 * vertPluecker.s1) + (rayPluecker.s5 * vertPluecker.s2                                                                                       ); 
	double signBC = sign(uBC);

	//Intersection check with face3 (ABC)

	if ((signAB!=0) && (signAC!=0)&& (signBC!=0))
	{
		double invVolABC = 1.0 / (uAB + uBC - uAC);
		if (( signAB > 0.0)&& (-signAC > 0.0)&& ( signBC > 0.0)){
			//face 3 is the entry face
			enterface = 3;
			uEnter1 = -uAC * invVolABC;
			uEnter2 =  uAB * invVolABC;
			enterPoint = (1 - uEnter1- uEnter2 ) * vert0[tid] + uEnter1 * vert1[tid] + uEnter2 * vert2[tid];

		}else if (( signAB < 0.0)&& (-signAC < 0.0)&& ( signBC < 0.0)){
			//face 3 is the exit face
			leaveface = 3;
			uLeave1 = -uAC * invVolABC;
			uLeave2 =  uAB * invVolABC;
			leavePoint = (1 - uLeave1- uLeave2 ) * vert0[tid] + uLeave1 * vert1[tid] + uLeave2 * vert2[tid];
		}
	}

	//Edge AD
	dir_vert =  vert0[tid] - vert3[tid] ;
	//The cross product of dir_vert with a point on the line 
	cross_vert = cross(vert3[tid],dir_vert);
	vertPluecker = (double8) (dir_vert.s0,dir_vert.s1,dir_vert.s2,cross_vert.s0,cross_vert.s1,cross_vert.s2,0.0,0.0);;
	//The permuted inner product for AD
	double uAD = (rayPluecker.s0 * vertPluecker.s3) + (rayPluecker.s1 * vertPluecker.s4) + (rayPluecker.s2 * vertPluecker.s5) + (rayPluecker.s3 * vertPluecker.s0) + (rayPluecker.s4 * vertPluecker.s1) + (rayPluecker.s5 * vertPluecker.s2                                                                                       ); 
	double signAD = sign(uAD);


	//Edge DB
	dir_vert =  vert3[tid] - vert1[tid] ;
	//The cross product of dir_vert with a point on the line 
	cross_vert = cross(vert1[tid],dir_vert);
	vertPluecker = (double8) (dir_vert.s0,dir_vert.s1,dir_vert.s2,cross_vert.s0,cross_vert.s1,cross_vert.s2,0.0,0.0);;
	//The permuted inner product for DB
	double uDB = (rayPluecker.s0 * vertPluecker.s3) + (rayPluecker.s1 * vertPluecker.s4) + (rayPluecker.s2 * vertPluecker.s5) + (rayPluecker.s3 * vertPluecker.s0) + (rayPluecker.s4 * vertPluecker.s1) + (rayPluecker.s5 * vertPluecker.s2                                                                                       ); 
	double signDB = sign(uDB);

	//Intersection with face2 (BAD)

	if ((signAB!=0) && (signAD!=0)&& (signDB!=0))
	{
		double invVolBAD = 1.0 / (uAD + uDB - uAB);
		if ((enterface == -1) && ( -signAB > 0.0)&& ( signAD > 0.0)&& (signDB > 0.0)){
			//face 2 is the entry face
			enterface = 2;
			uEnter1 =  uDB * invVolBAD;
			uEnter2 = -uAB * invVolBAD;
			enterPoint =   (1-uEnter1-uEnter2)*vert1[tid] + uEnter1*vert0[tid] + uEnter2*vert3[tid];

		}else if ((leaveface == -1) && ( -signAB < 0.0)&& ( signAD < 0.0)&& (signDB < 0.0)){
			//face 2 is the exit face
			leaveface = 2;
			uLeave1 =  uDB * invVolBAD;
			uLeave2 = -uAB * invVolBAD;
			leavePoint =   (1-uLeave1-uLeave2)*vert1[tid] + uLeave1*vert0[tid] + uLeave2*vert3[tid];
		}
	}

	//Edge DC
	dir_vert =  vert3[tid] - vert2[tid] ;
	//The cross product of dir_vert with a point on the line 
	cross_vert = cross(vert2[tid],dir_vert);
	vertPluecker = (double8) (dir_vert.s0,dir_vert.s1,dir_vert.s2,cross_vert.s0,cross_vert.s1,cross_vert.s2,0.0,0.0);;
	//The permuted inner product for DC
	double uDC = (rayPluecker.s0 * vertPluecker.s3) + (rayPluecker.s1 * vertPluecker.s4) + (rayPluecker.s2 * vertPluecker.s5) + (rayPluecker.s3 * vertPluecker.s0) + (rayPluecker.s4 * vertPluecker.s1) + (rayPluecker.s5 * vertPluecker.s2                                                                                       ); 
	double signDC = sign(uDC);

	//Intersection with face1 (CDA)

	if ((signAD!=0) && (signAC!=0)&& (signDC!=0))
	{
		double invVolCDA = 1.0 / (uAC - uDC - uAD);
		if ((enterface == -1) && ( -signAD > 0.0)&& ( signAC > 0.0)&& (-signDC > 0.0)){
			//face 1 is the entry face
			enterface = 1;
			uEnter1 =  uAC * invVolCDA;
			uEnter2 = -uDC * invVolCDA;
			enterPoint = (1-uEnter1-uEnter2)*vert2[tid] + uEnter1*vert3[tid] + uEnter2 * vert0[tid];
		}else if ((leaveface == -1) && ( -signAD < 0.0)&& ( signAC < 0.0)&& (-signDC < 0.0)){
			//face 1 is the exit face
			leaveface = 1;
			uLeave1 =  uAC * invVolCDA;
			uLeave2 = -uDC * invVolCDA;
			leavePoint =   (1-uLeave1-uLeave2)* vert2[tid] + uLeave1 * vert3[tid] + uLeave2*vert0[tid];
		}
	}

	//Intersection with face0 (DCB)

	if ((signBC!=0)&& (signDB!=0) && (signDC!=0))
	{
		double invVolDCB = 1.0 / (uDC - uBC - uDB);
		if ((enterface == -1) && ( -signBC > 0.0)&& (-signDB > 0.0)&& ( signDC > 0.0)){
			//face 0 is the entry face
			enterface = 0;
			uEnter1 = -uDB * invVolDCB;
			uEnter2 =  uDC * invVolDCB;
			enterPoint =   (1-uEnter1-uEnter2)*vert3[tid] + uEnter1*vert2[tid] + uEnter2*vert1[tid];
		}else if ((leaveface == -1) && ( -signBC < 0.0)&& ( -signDB < 0.0)&& ( signDC < 0.0)){
			//face 0 is the exit face
			leaveface = 0;
			uLeave1 = -uDB * invVolDCB;
			uLeave2 =  uDC * invVolDCB;
			leavePoint =   (1-uLeave1-uLeave2)*vert3[tid] + uLeave1*vert2[tid] + uLeave2*vert1[tid];
		}
	}

	//Calculating the parametric distances tEnter and tLeave

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

	tEnter = (enterface != -1)? tEnter : 0;
	tLeave = (leaveface != -1)? tLeave : 0;


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

