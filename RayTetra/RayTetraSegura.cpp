#include "../Common/NpVector.hpp"
#include "../Common/NpUtil.h"
#include "../Common/NpPluecker.hpp"


// Ray-Triangle intersection using Segura's algorithm

bool RayTetraSegura0(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    enterFace = -1;
    leaveFace = -1;

    // In the following: P=orig, Q=dest, A,B,C,D=vert[i], i=0,1,2,3.
    NpPluecker plPQ(orig, dir, true);
/*
    // Debug ///////////////////////////////////////////////////////////////////
    {
    double uAB = plPQ * NpPluecker(vert[0], vert[1]);
    int signAB = NpSign(uAB);

    double uBC = plPQ * NpPluecker(vert[1], vert[2]);
    int signBC = NpSign(uBC);

    double uAC = plPQ * NpPluecker(vert[0], vert[2]);
    int signAC = NpSign(uAC);
    
    double uAD = plPQ * NpPluecker(vert[0], vert[3]);
    int signAD = NpSign(uAD);

    double uDB = plPQ * NpPluecker(vert[3], vert[1]);
    int signDB = NpSign(uDB);

    double uDC = plPQ * NpPluecker(vert[3], vert[2]);
    int signDC = NpSign(uDC);

    std::cout << "ABC:  " << signAB << " " << signBC << " " << -signAC 
              << std::endl;
    std::cout << "BAD:  " << -signAB << " " << signAD << " " << signDB 
              << std::endl;
    std::cout << "CDA:  " << -signDC << " " << -signAD << " " << signAC 
              << std::endl;
    std::cout << "DCB:  " << signDC << " " << -signBC << " " << -signDB 
              << std::endl;
    }
    // End debug ///////////////////////////////////////////////////////////////
*/
    // Examine face ABC
    double uAB = plPQ * NpPluecker(vert[0], vert[1]);
    int signAB = NpSign(uAB);

    double uBC = plPQ * NpPluecker(vert[1], vert[2]);
    int signBC = NpSign(uBC);

    double uAC = plPQ * NpPluecker(vert[0], vert[2]);
    int signAC = NpSign(uAC);
    
    if ((signAB != 0)  ||  (signBC != 0)  ||  (signAC != 0))  {
        if ((signAB >= 0)  &&  (signBC >= 0)  &&  (signAC <= 0))  {
            enterFace = 3;

            double invVolABC = 1.0 / (uAB + uBC - uAC);
            uEnter1 = -uAC * invVolABC;
            uEnter2 =  uAB * invVolABC;
            enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
                         + uEnter1*vert[1] + uEnter2*vert[2];
        }
        else if ((signAB <= 0)  &&  (signBC <= 0)  &&  (signAC >= 0))  {
            leaveFace = 3;

            double invVolABC = 1.0 / (uAB + uBC - uAC);
            uLeave1 = -uAC * invVolABC;
            uLeave2 =  uAB * invVolABC;
            leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
                         + uLeave1*vert[1] + uLeave2*vert[2];
        }
    }
    
    // Examine face BAD
    double uAD = plPQ * NpPluecker(vert[0], vert[3]);
    int signAD = NpSign(uAD);

    double uDB = plPQ * NpPluecker(vert[3], vert[1]);
    int signDB = NpSign(uDB);

    if ((signAB != 0)  ||  (signAD != 0)  ||  (signDB != 0))  {
        if ((enterFace == -1)  &&
            (signAB <= 0)  &&  (signAD >= 0)  &&  (signDB >= 0))  {
            enterFace = 2;

            double invVolBAD = 1.0 / (uAD + uDB - uAB);
            uEnter1 =  uDB * invVolBAD;
            uEnter2 = -uAB * invVolBAD;
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
                 (signAB >= 0)  &&  (signAD <= 0)  &&  (signDB <= 0))  {
            leaveFace = 2;

            double invVolBAD = 1.0 / (uAD + uDB - uAB);
            uLeave1 =  uDB * invVolBAD;
            uLeave2 = -uAB * invVolBAD;
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
    
    // Examine face CDA
    double uDC = plPQ * NpPluecker(vert[3], vert[2]);
    int signDC = NpSign(uDC);

    if ((signDC != 0)  ||  (signAD != 0)  ||  (signAC != 0))  {
        if ((enterFace == -1)  &&
            (signDC <= 0)  &&  (signAD <= 0)  &&  (signAC >= 0))  {
            enterFace = 1;

            double invVolCDA = 1.0 / (uAC - uDC - uAD);
            uEnter1 =  uAC * invVolCDA;
            uEnter2 = -uDC * invVolCDA;
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
                 (signDC >= 0)  &&  (signAD >= 0)  &&  (signAC <= 0))  {
            leaveFace = 1;

            double invVolCDA = 1.0 / (uAC - uDC - uAD);
            uLeave1 =  uAC * invVolCDA;
            uLeave2 = -uDC * invVolCDA;
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

    // Examine face DCB
    // If no intersecting face has been found so far, then there is none,
    // otherwise DCB is the remaining one
    if ((enterFace == -1)  &&  (leaveFace == -1))  {
        return false;
    }

    double invVolDCB = 1.0 / (uDC - uBC - uDB);
    if (enterFace == -1)  {
        enterFace = 0;

        uEnter1 = -uDB * invVolDCB;
        uEnter2 =  uDC * invVolDCB;
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

        uLeave1 = -uDB * invVolDCB;
        uLeave2 =  uDC * invVolDCB;
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


bool RayTetraSegura1(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    enterFace = -1;
    leaveFace = -1;

    double uAB = 0, uAC = 0, uDB = 0, uDC = 0, uBC = 0, uAD = 0;
                    // Keep the compiler happy about uninitialized variables
    int signAB = -2, signAC = -2, signDB = -2, 
        signDC = -2, signBC = -2, signAD = -2;

    // In the following: P=orig, Q=dest, A,B,C,D=vert[i], i=0,1,2,3.
    NpPluecker plPQ(orig, dir, true);

    int nextSign = 0;

    // Examine face ABC
    uAB = plPQ * NpPluecker(vert[0], vert[1]);
    signAB = NpSign(uAB);

    uAC = plPQ * NpPluecker(vert[0], vert[2]);
    signAC = NpSign(uAC);
    
    if ((signAC == -signAB)  ||  (signAC == 0)  ||  (signAB == 0))  {
        // Face ABC may intersect with the ray
        uBC = plPQ * NpPluecker(vert[1], vert[2]);
        signBC = NpSign(uBC);

        int signABC = signAB;
        if (signABC == 0)  {
            signABC = -signAC;
            if (signABC == 0)  {
                signABC = signBC;
            }
        }

        if ((signABC != 0)  && 
            ((signBC == signABC)  ||  (signBC == 0)))  {
            // Face ABC intersects with the ray
            double invVolABC = 1.0 / (uAB + uBC - uAC);
            if (signABC == 1)  {
                enterFace = 3;
                uEnter1 = -uAC * invVolABC;
                uEnter2 =  uAB * invVolABC;
                enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
                             + uEnter1*vert[1] + uEnter2*vert[2];
                
                nextSign = -1;
            }
            else  {
                leaveFace = 3;
                uLeave1 = -uAC * invVolABC;
                uLeave2 =  uAB * invVolABC;
                leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
                             + uLeave1*vert[1] + uLeave2*vert[2];

                nextSign = 1;
            }

            // Determine the other intersecting face between BAD, CDA, DCB
            // Examine face BAD
            uAD = plPQ * NpPluecker(vert[0], vert[3]);
            signAD = NpSign(uAD);

            if ((signAD == nextSign)  ||  (signAD == 0))  {
                // Face BAD may intersect with the ray
                uDB = plPQ * NpPluecker(vert[3], vert[1]);
                signDB = NpSign(uDB);

                if ((signDB == nextSign)  ||  
                    ((signDB == 0)  &&  
                     ((signAD != 0)  ||  (signAB != 0))))  {
                    // Face BAD intersects with the ray
                    double invVolBAD = 1.0 / (uAD + uDB - uAB);
                    if (nextSign == 1)  {
                        enterFace = 2;
                        uEnter1 =  uDB * invVolBAD;
                        uEnter2 = -uAB * invVolBAD;
                        enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                                     + uEnter1*vert[0] + uEnter2*vert[3];

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
                        leaveFace = 2;
                        uLeave1 =  uDB * invVolBAD;
                        uLeave2 = -uAB * invVolBAD;
                        leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                                     + uLeave1*vert[0] + uLeave2*vert[3];

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
            uDC = plPQ * NpPluecker(vert[3], vert[2]);
            signDC = NpSign(uDC);

            if ((signDC == -nextSign)  ||
                ((signDC == 0)  &&  ((signAD != 0)  ||  (signAC != 0))))  {
                // Face CDA intersects with the ray
                double invVolCDA = 1.0 / (uAC - uDC - uAD);
                if (nextSign == 1)  {
                    enterFace = 1;
                    uEnter1 =  uAC * invVolCDA;
                    uEnter2 = -uDC * invVolCDA;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
                                 + uEnter1*vert[3] + uEnter2*vert[0];

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
                    leaveFace = 1;
                    uLeave1 =  uAC * invVolCDA;
                    uLeave2 = -uDC * invVolCDA;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
                                 + uLeave1*vert[3] + uLeave2*vert[0];

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
            else  {
                // Face DCB intersects with the ray
                if (signDB == -2)  {
                    uDB = plPQ * NpPluecker(vert[3], vert[1]);
                }

                double invVolDCB = 1.0 / (uDC - uBC - uDB);
                if (nextSign == 1)  {
                    enterFace = 0;
                    uEnter1 = -uDB * invVolDCB;
                    uEnter2 =  uDC * invVolDCB;
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
                    uLeave1 = -uDB * invVolDCB;
                    uLeave2 =  uDC * invVolDCB;
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
        }
    }

    // Examine face BAD
    uAD = plPQ * NpPluecker(vert[0], vert[3]);
    signAD = NpSign(uAD);

    if ((signAD == -signAB)  ||  (signAB == 0)  ||  (signAD == 0))  {
        // Face BAD may intersect with the ray
        uDB = plPQ * NpPluecker(vert[3], vert[1]);
        signDB = NpSign(uDB);

        int signBAD = -signAB;
        if (signBAD == 0)  {
            signBAD = signAD;
            if (signBAD == 0)  {
                signBAD = signDB;
            }
        }

        if ((signBAD != 0)  &&
            ((signDB == signBAD)  ||  (signDB == 0)))  {
            // Face BAD intersects with the ray
            double invVolBAD = 1.0 / (uAD + uDB - uAB);
            if (signBAD == 1)  {
                enterFace = 2;
                uEnter1 =  uDB * invVolBAD;
                uEnter2 = -uAB * invVolBAD;
                enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                             + uEnter1*vert[0] + uEnter2*vert[3];

                nextSign = -1;
            }
            else  {
                leaveFace = 2;
                uLeave1 =  uDB * invVolBAD;
                uLeave2 = -uAB * invVolBAD;
                leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                             + uLeave1*vert[0] + uLeave2*vert[3];

                nextSign = 1;
            }

            // Determine the other intersecting face between CDA, DCB
            uDC = plPQ * NpPluecker(vert[3], vert[2]);
            signDC = NpSign(uDC);

            if ((signDC == -nextSign)  ||
                ((signDC == 0)  &&  ((signAD != 0)  ||  (signAC != 0))))  {
                // Face CDA intersects with the ray
                double invVolCDA = 1.0 / (uAC - uDC - uAD);
                if (nextSign == 1)  {
                    enterFace = 1;
                    uEnter1 =  uAC * invVolCDA;
                    uEnter2 = -uDC * invVolCDA;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
                                 + uEnter1*vert[3] + uEnter2*vert[0];

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
                    leaveFace = 1;
                    uLeave1 =  uAC * invVolCDA;
                    uLeave2 = -uDC * invVolCDA;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
                                 + uLeave1*vert[3] + uLeave2*vert[0];

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
            else  {
                // Face DCB intersects with the ray
                if (signBC == -2)  {
                    uBC = plPQ * NpPluecker(vert[1], vert[2]);
                }

                double invVolDCB = 1.0 / (uDC - uBC - uDB);
                if (nextSign == 1)  {
                    enterFace = 0;
                    uEnter1 = -uDB * invVolDCB;
                    uEnter2 =  uDC * invVolDCB;
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
                    uLeave1 = -uDB * invVolDCB;
                    uLeave2 =  uDC * invVolDCB;
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
        }
    }

    // Examine face CDA
    if ((-signAD == signAC)  ||  (signAC == 0)  ||  (signAD == 0))  {
        // Face CDA may intersect with the ray
        uDC = plPQ * NpPluecker(vert[3], vert[2]);
        signDC = NpSign(uDC);

        int signCDA = signAC;
        if (signCDA == 0)  {
            signCDA = -signAD;
            if (signCDA == 0)  {
                signCDA = -signDC;
            }
        }

        if ((signCDA != 0)  &&
            ((-signDC == signCDA)  ||  (signDC == 0)))  {
            // Face CDA intersects with the ray
            // Face DCB also intersects with the ray
            double invVolCDA = 1.0 / (uAC - uDC - uAD);

            if (signBC == -2)  {
                uBC = plPQ * NpPluecker(vert[1], vert[2]);
            }
            if (signDB == -2)  {
                uDB = plPQ * NpPluecker(vert[3], vert[1]);
            }
            double invVolDCB = 1.0 / (uDC - uBC - uDB);
            
            if (signCDA == 1)  {
                enterFace = 1;
                uEnter1 =  uAC * invVolCDA;
                uEnter2 = -uDC * invVolCDA;
                enterPoint =   (1-uEnter1-uEnter2)*vert[2]
                             + uEnter1*vert[3] + uEnter2*vert[0];
                
                leaveFace = 0;
                uLeave1 = -uDB * invVolDCB;
                uLeave2 =  uDC * invVolDCB;
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
            else  {
                leaveFace = 1;
                uLeave1 =  uAC * invVolCDA;
                uLeave2 = -uDC * invVolCDA;
                leavePoint =   (1-uLeave1-uLeave2)*vert[2]
                             + uLeave1*vert[3] + uLeave2*vert[0];
                
                enterFace = 0;
                uEnter1 = -uDB * invVolDCB;
                uEnter2 =  uDC * invVolDCB;
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
        }
    }

    // Three faces do not intersect with the ray, the fourth will not.
    return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


bool RayTetraSegura2(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    enterFace = -1;
    leaveFace = -1;

    int nextSign;

    // In the following: P=orig, Q=dest, A,B,C,D=vert[i], i=0,1,2,3.
    NpPluecker plPQ(orig, dir, true);

    // Examine face ABC
    double uAB = plPQ * NpPluecker(vert[0], vert[1]);
    int signAB = NpSign(uAB);

    double uAC = plPQ * NpPluecker(vert[0], vert[2]);
    int signAC = NpSign(uAC);
    
    double uBC = plPQ * NpPluecker(vert[1], vert[2]);
    int signBC = NpSign(uBC);

    if ((signAB == 0)  &&  (signBC == 0)  &&  (signAC == 0))  {
        // Ray is on the plane of ABC
        // Examine face BAD
        double uAD = plPQ * NpPluecker(vert[0], vert[3]);
        int signAD = NpSign(uAD);

        double uDB = plPQ * NpPluecker(vert[3], vert[1]);
        int signDB = NpSign(uDB);
        
        double uDC = plPQ * NpPluecker(vert[3], vert[2]);
        int signDC = NpSign(uDC);

        if ((signAD == 0)  &&  (signDB == 0))  {
            if (signDC == 0)  {
                // The tetrahedron is flat and the ray is on its plane
                return false;
            }
            else  {
                // The ray is on the AB edge
                if (signDC == 1)  {
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
        else  {
            // Check if the ray intersects with face BAD
            if ((signAD >= 0)  &&  (signDB >= 0))  {
                // The ray enters through BAD
                enterFace = 2;
                double invVolBAD = 1.0 / (uAD + uDB);
                uEnter1 =  uDB * invVolBAD;
                uEnter2 =  0;
                enterPoint = (1-uEnter1)*vert[1] + uEnter1*vert[0];
                
                // Determine the other intersecting face between CDA, DCB
                if ((signDC == 1)  ||
                    ((signDC == 0)  &&  (signAD == 1)))  {
                    // The ray leaves through CDA
                    leaveFace = 1;
                    double invVolCDA = 1.0 / (-uDC - uAD);
                    uLeave1 =  0;
                    uLeave2 = -uDC * invVolCDA;
                    leavePoint = (1-uLeave2)*vert[2] + uLeave2*vert[0];

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
                    // The ray leaves through DCB
                    leaveFace = 0;
                    double invVolDCB = 1.0 / (uDC - uDB);
                    uLeave1 = -uDB * invVolDCB;
                    uLeave2 =  uDC * invVolDCB;
                    leavePoint = uLeave1*vert[2] + uLeave2*vert[1];

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
            else if ((signAD <= 0)  &&  (signDB <= 0))  {
                // The ray leaves through BAD
                leaveFace = 2;

                double invVolBAD = 1.0 / (uAD + uDB);
                uLeave1 =  uDB * invVolBAD;
                uLeave2 =  0;
                leavePoint = (1-uLeave1)*vert[1] + uLeave1*vert[0];
                
                // Determine the other intersecting face between CDA, DCB
                uDC = plPQ * NpPluecker(vert[3], vert[2]);
                signDC = NpSign(uDC);

                if ((signDC == -1)  ||
                    ((signDC == 0)  &&  (signAD == -1)))  {
                    // The ray enters through CDA
                    enterFace = 1;
                    double invVolCDA = 1.0 / (- uDC - uAD);
                    uEnter1 =  0;
                    uEnter2 = -uDC * invVolCDA;
                    enterPoint =   (1-uEnter2)*vert[2] + uEnter2*vert[0];

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
                    // The ray enters through DCB
                    enterFace = 0;
                    double invVolDCB = 1.0 / (uDC - uDB);
                    uEnter1 = -uDB * invVolDCB;
                    uEnter2 =  uDC * invVolDCB;
                    enterPoint = uEnter1*vert[2] + uEnter2*vert[1];

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
            else  {
                return false;
            }
        }
    }
    /* Copied up to here */
    else  {
        // Check if ray intersects with face ABC
        if ((signAB >= 0)  &&  (signBC >= 0)  &&  (signAC <= 0))  {
            enterFace = 3;
            double invVolABC = 1.0 / (uAB + uBC - uAC);
            uEnter1 = -uAC * invVolABC;
            uEnter2 =  uAB * invVolABC;
            enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
                         + uEnter1*vert[1] + uEnter2*vert[2];

            nextSign = -1;
            // The other intersecting face is determined after the next 'else'
        }
        else if ((signAB <= 0)  &&  (signBC <= 0)  &&  (signAC >= 0))  {
            leaveFace = 3;
            double invVolABC = 1.0 / (uAB + uBC - uAC);
            uLeave1 = -uAC * invVolABC;
            uLeave2 =  uAB * invVolABC;
            leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
                         + uLeave1*vert[1] + uLeave2*vert[2];

            nextSign = 1;
            // The other intersecting face is determined after the next 'else'
        }
        else  {
            // Ray does not intersect with ABC,
            // determine the next face to examine

            int nextFace;

            if (uAB+uBC-uAC > 0)  {
                if (signBC == -1)  {
                    if (signAC == 1)  {
                        nextFace = 2;
                        nextSign = -1;
                    }
                    else if (signAB == -1)  {
                        nextFace = 1;
                        nextSign = -1;
                    }
                    else  {
                        nextFace = 0;
                        nextSign = 1;
                    }
                }
                else if (signAC == 1)  {
                    if (signAB == -1)  {
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
                if (signBC == 1)  {
                    if (signAC == -1)  {
                        nextFace = 2;
                        nextSign = 1;
                    }
                    else if (signAB == 1)  {
                        nextFace = 1;
                        nextSign = 1;
                    }
                    else  {
                        nextFace = 0;
                        nextSign = -1;
                    }
                }
                else if (signAC == -1)  {
                    if (signAB == 1)  {
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
                double uAD = plPQ * NpPluecker(vert[0], vert[3]);
                int signAD = NpSign(uAD);

                if (signAD == -nextSign)  {
                    return false;
                }

                double uDB = plPQ * NpPluecker(vert[3], vert[1]);
                int signDB = NpSign(uDB);

                if (signDB == -nextSign)  {
                    return false;
                }

                // Face BAD intersects with the ray
                double invVolBAD = 1.0 / (uAD + uDB - uAB);
                if (nextSign == 1)  {
                    enterFace = 2;
                    uEnter1 =  uDB * invVolBAD;
                    uEnter2 = -uAB * invVolBAD;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                                 + uEnter1*vert[0] + uEnter2*vert[3];

                    nextSign = -1;
                }
                else  {
                    leaveFace = 2;
                    uLeave1 =  uDB * invVolBAD;
                    uLeave2 = -uAB * invVolBAD;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                                 + uLeave1*vert[0] + uLeave2*vert[3];

                    nextSign = 1;
                }

                // Determine the other intersecting face between CDA, DCB
                double uDC = plPQ * NpPluecker(vert[3], vert[2]);
                int signDC = NpSign(uDC);

                if ((signDC == -nextSign)  ||
                    ((signDC == 0)  &&  ((signAD != 0)  ||  (signAC != 0))))  {
                    // Face CDA intersects with the ray
                    double invVolCDA = 1.0 / (uAC - uDC - uAD);
                    if (nextSign == 1)  {
                        enterFace = 1;
                        uEnter1 =  uAC * invVolCDA;
                        uEnter2 = -uDC * invVolCDA;
                        enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
                                     + uEnter1*vert[3] + uEnter2*vert[0];

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
                        leaveFace = 1;
                        uLeave1 =  uAC * invVolCDA;
                        uLeave2 = -uDC * invVolCDA;
                        leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
                                     + uLeave1*vert[3] + uLeave2*vert[0];

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
                else  {
                    // Face DCB intersects with the ray
                    double invVolDCB = 1.0 / (uDC - uBC - uDB);
                    if (nextSign == 1)  {
                        enterFace = 0;
                        uEnter1 = -uDB * invVolDCB;
                        uEnter2 =  uDC * invVolDCB;
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
                        uLeave1 = -uDB * invVolDCB;
                        uLeave2 =  uDC * invVolDCB;
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
            }
            else if (nextFace == 1)  {
                // Examine face CDA
                double uDC = plPQ * NpPluecker(vert[3], vert[2]);
                int signDC = NpSign(uDC);

                if (signDC == nextSign)  {
                    return false;
                }

                double uAD = plPQ * NpPluecker(vert[0], vert[3]);
                int signAD = NpSign(uAD);

                if (signAD == nextSign)  {
                    return false;
                }
                
                // Face CDA intersects with the ray
                double invVolCDA = 1.0 / (uAC - uDC - uAD);
                if (nextSign == 1)  {
                    enterFace = 1;
                    uEnter1 =  uAC * invVolCDA;
                    uEnter2 = -uDC * invVolCDA;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
                                 + uEnter1*vert[3] + uEnter2*vert[0];

                    nextSign = -1;
                }
                else  {
                    leaveFace = 1;
                    uLeave1 =  uAC * invVolCDA;
                    uLeave2 = -uDC * invVolCDA;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
                                 + uLeave1*vert[3] + uLeave2*vert[0];

                    nextSign = 1;
                }

                // Determine the other intersecting face between BAD, DCB
                double uDB = plPQ * NpPluecker(vert[3], vert[1]);
                int signDB = NpSign(uDB);

                if ((signDB == nextSign)  ||
                    ((signDB == 0)  &&  ((signAB != 0)  ||  (signAD != 0))))  {
                    // Face BAD intersects with the ray
                    double invVolBAD = 1.0 / (uAD + uDB - uAB);
                    if (nextSign == 1)  {
                        enterFace = 2;
                        uEnter1 =  uDB * invVolBAD;
                        uEnter2 = -uAB * invVolBAD;
                        enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                                     + uEnter1*vert[0] + uEnter2*vert[3];
            
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
                        leaveFace = 2;
                        uLeave1 =  uDB * invVolBAD;
                        uLeave2 = -uAB * invVolBAD;
                        leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                                     + uLeave1*vert[0] + uLeave2*vert[3];
            
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
                else  {
                    // Face DCB intersects with the ray
                    double invVolDCB = 1.0 / (uDC - uBC - uDB);
                    if (nextSign == 1)  {
                        enterFace = 0;
                        uEnter1 = -uDB * invVolDCB;
                        uEnter2 =  uDC * invVolDCB;
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
                        uLeave1 = -uDB * invVolDCB;
                        uLeave2 =  uDC * invVolDCB;
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
            }
            else  {
                // Examine face DCB
                double uDC = plPQ * NpPluecker(vert[3], vert[2]);
                int signDC = NpSign(uDC);

                if (signDC == -nextSign)  {
                    return false;
                }

                double uDB = plPQ * NpPluecker(vert[3], vert[1]);
                int signDB = NpSign(uDB);

                if (signDB == nextSign)  {
                    return false;
                }

                // Face DCB intersects with the ray
                double invVolDCB = 1.0 / (uDC - uBC - uDB);
                if (nextSign == 1)  {
                    enterFace = 0;
                    uEnter1 = -uDB * invVolDCB;
                    uEnter2 =  uDC * invVolDCB;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[3] 
                                 + uEnter1*vert[2] + uEnter2*vert[1];

                    nextSign = -1;
                }
                else  {
                    leaveFace = 0;
                    uLeave1 = -uDB * invVolDCB;
                    uLeave2 =  uDC * invVolDCB;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[3] 
                                 + uLeave1*vert[2] + uLeave2*vert[1];

                    nextSign = 1;
                }
                
                // Determine the other intersecting face between BAD, CDA
                double uAD = plPQ * NpPluecker(vert[0], vert[3]);
                int signAD = NpSign(uAD);

                if ((signAD == nextSign)  ||
                    ((signAD == 0)  &&  ((signAB != 0)  ||  (signDB != 0))))  {
                    // Face BAD intersects with the ray
                    double invVolBAD = 1.0 / (uAD + uDB - uAB);
                    if (nextSign == 1)  {
                        enterFace = 2;
                        uEnter1 =  uDB * invVolBAD;
                        uEnter2 = -uAB * invVolBAD;
                        enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                                     + uEnter1*vert[0] + uEnter2*vert[3];
            
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
                        leaveFace = 2;
                        uLeave1 =  uDB * invVolBAD;
                        uLeave2 = -uAB * invVolBAD;
                        leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                                     + uLeave1*vert[0] + uLeave2*vert[3];
            
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
                else  {
                    // Face CDA intersects with the ray
                    double invVolCDA = 1.0 / (uAC - uDC - uAD);
                    if (nextSign == 1)  {
                        enterFace = 1;
                        uEnter1 =  uAC * invVolCDA;
                        uEnter2 = -uDC * invVolCDA;
                        enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
                                     + uEnter1*vert[3] + uEnter2*vert[0];
            
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
                        leaveFace = 1;
                        uLeave1 =  uAC * invVolCDA;
                        uLeave2 = -uDC * invVolCDA;
                        leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
                                     + uLeave1*vert[3] + uLeave2*vert[0];
            
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
    }
     
    // Face ABC intersects with the ray.
    // Determine the other intersecting face between BAD, CDA, DCB
    // Examine face BAD
    double uAD = plPQ * NpPluecker(vert[0], vert[3]);
    int signAD = NpSign(uAD);

    double uDB = 0;
    int signDB = -2;

    if ((signAD == nextSign)  ||  (signAD == 0))  {
        // Face BAD may intersect with the ray
        uDB = plPQ * NpPluecker(vert[3], vert[1]);
        signDB = NpSign(uDB);

        if ((signDB == nextSign)  ||  
            ((signDB == 0)  &&  
             ((signAD != 0)  ||  (signAB != 0))))  {
            // Face BAD intersects with the ray
            double invVolBAD = 1.0 / (uAD + uDB - uAB);
            if (nextSign == 1)  {
                enterFace = 2;
                uEnter1 =  uDB * invVolBAD;
                uEnter2 = -uAB * invVolBAD;
                enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                             + uEnter1*vert[0] + uEnter2*vert[3];

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
                leaveFace = 2;
                uLeave1 =  uDB * invVolBAD;
                uLeave2 = -uAB * invVolBAD;
                leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                             + uLeave1*vert[0] + uLeave2*vert[3];

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
    double uDC = plPQ * NpPluecker(vert[3], vert[2]);
    int signDC = NpSign(uDC);

    if ((signDC == -nextSign)  ||
        ((signDC == 0)  &&  ((signAD != 0)  ||  (signAC != 0))))  {
        // Face CDA intersects with the ray
        double invVolCDA = 1.0 / (uAC - uDC - uAD);
        if (nextSign == 1)  {
            enterFace = 1;
            uEnter1 =  uAC * invVolCDA;
            uEnter2 = -uDC * invVolCDA;
            enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
                         + uEnter1*vert[3] + uEnter2*vert[0];

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
            leaveFace = 1;
            uLeave1 =  uAC * invVolCDA;
            uLeave2 = -uDC * invVolCDA;
            leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
                         + uLeave1*vert[3] + uLeave2*vert[0];

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
    else  {
        // Face DCB intersects with the ray
        if (signDB == -2)  {
            uDB = plPQ * NpPluecker(vert[3], vert[1]);
        }

        double invVolDCB = 1.0 / (uDC - uBC - uDB);
        if (nextSign == 1)  {
            enterFace = 0;
            uEnter1 = -uDB * invVolDCB;
            uEnter2 =  uDC * invVolDCB;
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
            uLeave1 = -uDB * invVolDCB;
            uLeave2 =  uDC * invVolDCB;
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
}
