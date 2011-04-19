#include "../Common/NpVector.hpp"
#include "../Common/NpUtil.h"


// Ray-Triangle intersection using Moller's algorithm
// see http://www.ce.chalmers.se/staff/tomasm/raytri/ for details


bool RayTetraMoller1(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[], 
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    // In the following, side i is the side opposite vertex i.
    // Calculate determinant showing if the ray enters/leaves through face i.
    NpVector edge1[4], edge2[4], pvec[4], qvec[4], tvec[4];
    double det[4], inv_det[4];
    int sign[4];

    enterFace = -1;
    leaveFace = -1;

    tEnter = -NpInf;
    tLeave = NpInf;

    // Face 0
    edge1[0] = vert[2] - vert[3];
    edge2[0] = vert[1] - vert[3];
    pvec[0] = dir ^ edge2[0];
    det[0] = edge1[0] * pvec[0];
    sign[0] = NpSign(det[0]);

    if (sign[0] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[0] ^ edge2[0];

        double volOrig = (vert[3] - orig) * e1xe2;
        double volVert = (vert[3] - vert[0]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }

    // Face 1
    edge1[1] = -edge1[0];
    edge2[1] = vert[0] - vert[2];
    pvec[1] =  dir ^ edge2[1];
    det[1] = edge1[1] * pvec[1];
    sign[1] = NpSign(det[1]);

    if (sign[1] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[1] ^ edge2[1];

        double volOrig = (vert[2] - orig) * e1xe2;
        double volVert = (vert[2] - vert[1]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }

    // Face 2
    edge1[2] = vert[0] - vert[1];
    edge2[2] = -edge2[0];
    pvec[2] = -pvec[0];
    det[2] = edge1[2] * pvec[2];
    sign[2] = NpSign(det[2]);

    if (sign[2] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[2] ^ edge2[2];

        double volOrig = (vert[1] - orig) * e1xe2;
        double volVert = (vert[1] - vert[2]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }

    // Face 3
    edge1[3] = -edge1[2];
    edge2[3] = -edge2[1];
    pvec[3] = -pvec[1];
    det[3] = edge1[3] * pvec[3];
    sign[3] = NpSign(det[3]);

    if (sign[3] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[3] ^ edge2[3];

        double volOrig = (vert[0] - orig) * e1xe2;
        double volVert = (vert[0] - vert[3]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }

    // Examine non-parallel faces to find enter/leave faces
    for (unsigned int i = 0; i < 4; ++i)  {
        if (sign[i] != 0)  {
            tvec[i] = orig - vert[3-i];
            qvec[i] = tvec[i] ^ edge1[i];
            inv_det[i] = 1.0 / det[i];
            double t = inv_det[i] * (edge2[i] * qvec[i]);

            if (sign[i] == 1)  {
                if (t > tLeave)  {
                    return false;
                }
                if (t > tEnter)  {
                    tEnter = t;
                    enterFace = i;
                }
            }
            else  {
                if (t < tEnter)  {
                    return false;
                }
                if (t < tLeave)  {
                    tLeave = t;
                    leaveFace = i;
                }
            }
        }
    }

    // Handle degenerate tetrahedra
    if (enterFace == -1)  {
        return false;
    }

    uEnter1 = inv_det[enterFace] * (tvec[enterFace] * pvec[enterFace]);
    uEnter2 = inv_det[enterFace] * (dir * qvec[enterFace]);
    enterPoint = orig + tEnter * dir;

    uLeave1 = inv_det[leaveFace] * (tvec[leaveFace] * pvec[leaveFace]);
    uLeave2 = inv_det[leaveFace] * (dir * qvec[leaveFace]);
    leavePoint = orig + tLeave * dir;

    return true;
}





bool RayTetraMoller2(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[], 
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    // In the following, side i is the side opposite vertex i.
    // Calculate determinant showing if the ray enters/leaves through face i.
    NpVector edgeA, edgeB, edgeC, edgeD, pvecA, pvecB;
    NpVector edge1, edge2, pvec, qvec, tvec;
    double det, inv_det;

    enterFace = -1;
    leaveFace = -1;

    // Face 0
    edge1 = edgeA = vert[2] - vert[3];
    edge2 = edgeB = vert[1] - vert[3];
    pvec = pvecA = dir ^ edgeB;
    det = edge1 * pvec;
    
    if (det > NpZero)  {
        tvec = orig - vert[3];
        uEnter1 = tvec * pvec;
        if ((uEnter1 >= 0.0)  &&  (uEnter1 <= det))  {
            qvec = tvec ^ edge1;
            uEnter2 = dir * qvec;
            if ((uEnter2 >= 0.0)  &&  (uEnter1 + uEnter2 <= det))  {
                enterFace = 0;

                inv_det = 1.0 / det;
                uEnter1 *= inv_det;
                uEnter2 *= inv_det;

                tEnter = inv_det * (edge2 * qvec);
                enterPoint = orig + tEnter * dir;
            }
        }
    }
    else if (det < -NpZero)  {
        tvec = orig - vert[3];
        uLeave1 = tvec * pvec;
        if ((uLeave1 <= 0.0)  &&  (uLeave1 >= det))  {
            qvec = tvec ^ edge1;
            uLeave2 = dir * qvec;
            if ((uLeave2 <= 0.0)  &&  (uLeave1 + uLeave2 >= det))  {
                leaveFace = 0;

                inv_det = 1.0 / det;
                uLeave1 *= inv_det;
                uLeave2 *= inv_det;

                tLeave = inv_det * (edge2 * qvec);
                leavePoint = orig + tLeave * dir;
            }
        }
    }

    // Face 1
    edge1 = -edgeA;
    edge2 = edgeD = vert[0] - vert[2];
    pvec = pvecB = dir ^ edge2;
    det = edge1 * pvec;

    if ((enterFace == -1)  &&  (det > NpZero))  {
        tvec = orig - vert[2];
        uEnter1 = tvec * pvec;
        if ((uEnter1 >= 0.0)  &&  (uEnter1 <= det))  {
            qvec = tvec ^ edge1;
            uEnter2 = dir * qvec;
            if ((uEnter2 >= 0.0)  &&  (uEnter1 + uEnter2 <= det))  {
                enterFace = 1;

                inv_det = 1.0 / det;
                uEnter1 *= inv_det;
                uEnter2 *= inv_det;

                tEnter = inv_det * (edge2 * qvec);
                enterPoint = orig + tEnter * dir;

                if (leaveFace != -1)  {
                    return true;
                }
            }
        }
    }
    else if ((leaveFace == -1)  &&  (det < -NpZero))  {
        tvec = orig - vert[2];
        uLeave1 = tvec * pvec;
        if ((uLeave1 <= 0.0)  &&  (uLeave1 >= det))  {
            qvec = tvec ^ edge1;
            uLeave2 = dir * qvec;
            if ((uLeave2 <= 0.0)  &&  (uLeave1 + uLeave2 >= det))  {
                leaveFace = 1;

                inv_det = 1.0 / det;
                uLeave1 *= inv_det;
                uLeave2 *= inv_det;

                tLeave = inv_det * (edge2 * qvec);
                leavePoint = orig + tLeave * dir;

                if (enterFace != -1)  {
                    return true;
                }
            }
        }
    }

    // Face 2
    edge1 = edgeC = vert[0] - vert[1];
    edge2 = -edgeB;
    pvec = -pvecA;
    det = edge1 * pvec;

    if ((enterFace == -1)  &&  (det > NpZero))  {
        tvec = orig - vert[1];
        uEnter1 = tvec * pvec;
        if ((uEnter1 >= 0.0)  &&  (uEnter1 <= det))  {
            qvec = tvec ^ edge1;
            uEnter2 = dir * qvec;
            if ((uEnter2 >= 0.0)  &&  (uEnter1 + uEnter2 <= det))  {
                enterFace = 2;

                inv_det = 1.0 / det;
                uEnter1 *= inv_det;
                uEnter2 *= inv_det;

                tEnter = inv_det * (edge2 * qvec);
                enterPoint = orig + tEnter * dir;

                if (leaveFace != -1)  {
                    return true;
                }
            }
        }
    }
    else if ((leaveFace == -1)  &&  (det < -NpZero))  {
        tvec = orig - vert[1];
        uLeave1 = tvec * pvec;
        if ((uLeave1 <= 0.0)  &&  (uLeave1 >= det))  {
            qvec = tvec ^ edge1;
            uLeave2 = dir * qvec;
            if ((uLeave2 <= 0.0)  &&  (uLeave1 + uLeave2 >= det))  {
                leaveFace = 2;

                inv_det = 1.0 / det;
                uLeave1 *= inv_det;
                uLeave2 *= inv_det;

                tLeave = inv_det * (edge2 * qvec);
                leavePoint = orig + tLeave * dir;

                if (enterFace != -1)  {
                    return true;
                }
            }
        }
    }

    if ((enterFace == -1)  &&  (leaveFace == -1))  {
        return false;
    }


    // Face 3
    edge1 = -edgeC;
    edge2 = -edgeD;
    pvec = -pvecB;
    det = edge1 * pvec;

    if (enterFace == -1)  {
        tvec = orig - vert[0];
        uEnter1 = tvec * pvec;

        qvec = tvec ^ edge1;
        uEnter2 = dir * qvec;

        enterFace = 3;

        inv_det = 1.0 / det;
        uEnter1 *= inv_det;
        uEnter2 *= inv_det;

        tEnter = inv_det * (edge2 * qvec);
        enterPoint = orig + tEnter * dir;

        return true;
    }
    else  {
        tvec = orig - vert[0];
        uLeave1 = tvec * pvec;

        qvec = tvec ^ edge1;
        uLeave2 = dir * qvec;

        leaveFace = 3;

        inv_det = 1.0 / det;
        uLeave1 *= inv_det;
        uLeave2 *= inv_det;

        tLeave = inv_det * (edge2 * qvec);
        leavePoint = orig + tLeave * dir;

        return true;
    }
}





bool RayTetraMoller3(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[], 
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    // In the following, side i is the side opposite vertex i.
    // Calculate determinant showing if the ray enters/leaves through face i.
    NpVector edge1[4], edge2[4], pvec[4], qvec[4], tvec[4];
    double det[4], inv_det[4];
    int sign[4];

    enterFace = -1;
    leaveFace = -1;

    tEnter = -NpInf;
    tLeave = NpInf;

    // Face 0
    edge1[0] = vert[2] - vert[3];
    edge2[0] = vert[1] - vert[3];
    pvec[0] = dir ^ edge2[0];
    det[0] = edge1[0] * pvec[0];
    sign[0] = NpSign(det[0]);

    if (sign[0] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[0] ^ edge2[0];

        double volOrig = (vert[3] - orig) * e1xe2;
        double volVert = (vert[3] - vert[0]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }
    else  {
        tvec[0] = orig - vert[3];
        qvec[0] = tvec[0] ^ edge1[0];
        inv_det[0] = 1.0 / det[0];
        double t = inv_det[0] * (edge2[0] * qvec[0]);

        if (sign[0] == 1)  {
            if (t > tLeave)  {
                return false;
            }
            if (t > tEnter)  {
                tEnter = t;
                enterFace = 0;
            }
        }
        else  {
            if (t < tEnter)  {
                return false;
            }
            if (t < tLeave)  {
                tLeave = t;
                leaveFace = 0;
            }
        }
    }

    // Face 1
    edge1[1] = -edge1[0];
    edge2[1] = vert[0] - vert[2];
    pvec[1] =  dir ^ edge2[1];
    det[1] = edge1[1] * pvec[1];
    sign[1] = NpSign(det[1]);

    if (sign[1] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[1] ^ edge2[1];

        double volOrig = (vert[2] - orig) * e1xe2;
        double volVert = (vert[2] - vert[1]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }
    else  {
        tvec[1] = orig - vert[2];
        qvec[1] = tvec[1] ^ edge1[1];
        inv_det[1] = 1.0 / det[1];
        double t = inv_det[1] * (edge2[1] * qvec[1]);

        if (sign[1] == 1)  {
            if (t > tLeave)  {
                return false;
            }
            if (t > tEnter)  {
                tEnter = t;
                enterFace = 1;
            }
        }
        else  {
            if (t < tEnter)  {
                return false;
            }
            if (t < tLeave)  {
                tLeave = t;
                leaveFace = 1;
            }
        }
    }

    // Face 2
    edge1[2] = vert[0] - vert[1];
    edge2[2] = -edge2[0];
    pvec[2] = -pvec[0];
    det[2] = edge1[2] * pvec[2];
    sign[2] = NpSign(det[2]);

    if (sign[2] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[2] ^ edge2[2];

        double volOrig = (vert[1] - orig) * e1xe2;
        double volVert = (vert[1] - vert[2]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }
    else  {
        tvec[2] = orig - vert[1];
        qvec[2] = tvec[2] ^ edge1[2];
        inv_det[2] = 1.0 / det[2];
        double t = inv_det[2] * (edge2[2] * qvec[2]);

        if (sign[2] == 1)  {
            if (t > tLeave)  {
                return false;
            }
            if (t > tEnter)  {
                tEnter = t;
                enterFace = 2;
            }
        }
        else  {
            if (t < tEnter)  {
                return false;
            }
            if (t < tLeave)  {
                tLeave = t;
                leaveFace = 2;
            }
        }
    }

    // Face 3
    edge1[3] = -edge1[2];
    edge2[3] = -edge2[1];
    pvec[3] = -pvec[1];
    det[3] = edge1[3] * pvec[3];
    sign[3] = NpSign(det[3]);

    if (sign[3] == 0)  {
        // Ray is parallel to face i. If the opposite vertex is closer
        // to face i than the ray, then they cannot intersect.
        NpVector e1xe2 = edge1[3] ^ edge2[3];

        double volOrig = (vert[0] - orig) * e1xe2;
        double volVert = (vert[0] - vert[3]) * e1xe2;

        if ((volOrig*volVert < 0.0)  ||  (volOrig > volVert))  {
            return false;
        }
    }
    else  {
        tvec[3] = orig - vert[0];
        qvec[3] = tvec[3] ^ edge1[3];
        inv_det[3] = 1.0 / det[3];
        double t = inv_det[3] * (edge2[3] * qvec[3]);

        if (sign[3] == 1)  {
            if (t > tLeave)  {
                return false;
            }
            if (t > tEnter)  {
                tEnter = t;
                enterFace = 3;
            }
        }
        else  {
            if (t < tEnter)  {
                return false;
            }
            if (t < tLeave)  {
                tLeave = t;
                leaveFace = 3;
            }
        }
    }

    // Handle degenerate tetrahedra
    if (enterFace == -1)  {
        return false;
    }

    uEnter1 = inv_det[enterFace] * (tvec[enterFace] * pvec[enterFace]);
    uEnter2 = inv_det[enterFace] * (dir * qvec[enterFace]);
    enterPoint = orig + tEnter * dir;

    uLeave1 = inv_det[leaveFace] * (tvec[leaveFace] * pvec[leaveFace]);
    uLeave2 = inv_det[leaveFace] * (dir * qvec[leaveFace]);
    leavePoint = orig + tLeave * dir;

    return true;
}
