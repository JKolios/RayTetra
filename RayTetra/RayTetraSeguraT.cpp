#include "../Common/NpVector.hpp"
#include "../Common/NpUtil.h"
#include "../Common/NpPluecker.hpp"


static const int AB = 0,
                 AC = 1,
                 BC = 2,
                 AD = 3,
                 DB = 4,
                 DC = 5;


void CalculatePluecker(const NpVector& orig, const NpVector& dir,
                       const NpVector vert[],
                       double* u, int* sign)
{
    NpPluecker plPQ(orig, dir, true);
    
    NpPluecker plAB(vert[0], vert[1]);
    u[AB] = plPQ * plAB;
    sign[AB] = NpSign(u[AB]);

    NpPluecker plAC(vert[0], vert[2]);
    u[AC] = plPQ * plAC;
    sign[AC] = NpSign(u[AC]);
    
    NpPluecker plBC(vert[1], vert[2]);
    u[BC] = plPQ * plBC;
    sign[BC] = NpSign(u[BC]);

    NpPluecker plAD(vert[0], vert[3]);
    u[AD] = plPQ * plAD;
    sign[AD] = NpSign(u[AD]);

    NpPluecker plDB(vert[3], vert[1]);
    u[DB] = plPQ * plDB;
    sign[DB] = NpSign(u[DB]);
    
    NpPluecker plDC(vert[3], vert[2]);
    u[DC] = plPQ * plDC;
    sign[DC] = NpSign(u[DC]);
}



// Ray-Triangle intersection using Segura's algorithm

bool RayTetraSegura0T(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    const double* u, const int* sign,
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    enterFace = -1;
    leaveFace = -1;

    // In the following: P=orig, Q=dest, A,B,C,D=vert[i], i=0,1,2,3.
/*
    // Debug ///////////////////////////////////////////////////////////////////
    {
    std::cout << "ABC:  " << sign[AB] << " " << sign[BC] << " " << -sign[AC] 
              << std::endl;
    std::cout << "BAD:  " << -sign[AB] << " " << sign[AD] << " " << sign[DB] 
              << std::endl;
    std::cout << "CDA:  " << -sign[DC] << " " << -sign[AD] << " " << sign[AC] 
              << std::endl;
    std::cout << "DCB:  " << sign[DC] << " " << -sign[BC] << " " << -sign[DB] 
              << std::endl;
    }
    // End debug ///////////////////////////////////////////////////////////////
*/
    // Examine face ABC
    if ((sign[AB] != 0)  ||  (sign[BC] != 0)  ||  (sign[AC] != 0))  {
        if ((sign[AB] >= 0)  &&  (sign[BC] >= 0)  &&  (sign[AC] <= 0))  {
            enterFace = 3;

            double invVolABC = 1.0 / (u[AB] + u[BC] - u[AC]);
            uEnter1 = -u[AC] * invVolABC;
            uEnter2 =  u[AB] * invVolABC;
            enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
                         + uEnter1*vert[1] + uEnter2*vert[2];
        }
        else if ((sign[AB] <= 0)  &&  (sign[BC] <= 0)  &&  (sign[AC] >= 0))  {
            leaveFace = 3;

            double invVolABC = 1.0 / (u[AB] + u[BC] - u[AC]);
            uLeave1 = -u[AC] * invVolABC;
            uLeave2 =  u[AB] * invVolABC;
            leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
                         + uLeave1*vert[1] + uLeave2*vert[2];
        }
    }
    
    // Examine face BAD
    if ((sign[AB] != 0)  ||  (sign[AD] != 0)  ||  (sign[DB] != 0))  {
        if ((enterFace == -1)  &&
            (sign[AB] <= 0)  &&  (sign[AD] >= 0)  &&  (sign[DB] >= 0))  {
            enterFace = 2;

            double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
            uEnter1 =  u[DB] * invVolBAD;
            uEnter2 = -u[AB] * invVolBAD;
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
                 (sign[AB] >= 0)  &&  (sign[AD] <= 0)  &&  (sign[DB] <= 0))  {
            leaveFace = 2;

            double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
            uLeave1 =  u[DB] * invVolBAD;
            uLeave2 = -u[AB] * invVolBAD;
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
    if ((sign[DC] != 0)  ||  (sign[AD] != 0)  ||  (sign[AC] != 0))  {
        if ((enterFace == -1)  &&
            (sign[DC] <= 0)  &&  (sign[AD] <= 0)  &&  (sign[AC] >= 0))  {
            enterFace = 1;

            double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
            uEnter1 =  u[AC] * invVolCDA;
            uEnter2 = -u[DC] * invVolCDA;
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
                 (sign[DC] >= 0)  &&  (sign[AD] >= 0)  &&  (sign[AC] <= 0))  {
            leaveFace = 1;

            double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
            uLeave1 =  u[AC] * invVolCDA;
            uLeave2 = -u[DC] * invVolCDA;
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

    double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
    if (enterFace == -1)  {
        enterFace = 0;

        uEnter1 = -u[DB] * invVolDCB;
        uEnter2 =  u[DC] * invVolDCB;
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

        uLeave1 = -u[DB] * invVolDCB;
        uLeave2 =  u[DC] * invVolDCB;
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


bool RayTetraSegura1T(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    const double* u, const int* sign,
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    enterFace = -1;
    leaveFace = -1;

    int nextSign = 0;

    // Examine face ABC
    if ((sign[AC] == -sign[AB])  ||  (sign[AC] == 0)  ||  (sign[AB] == 0))  {
        // Face ABC may intersect with the ray
        int signABC = sign[AB];
        if (signABC == 0)  {
            signABC = -sign[AC];
            if (signABC == 0)  {
                signABC = sign[BC];
            }
        }

        if ((signABC != 0)  && 
            ((sign[BC] == signABC)  ||  (sign[BC] == 0)))  {
            // Face ABC intersects with the ray
            double invVolABC = 1.0 / (u[AB] + u[BC] - u[AC]);
            if (signABC == 1)  {
                enterFace = 3;
                uEnter1 = -u[AC] * invVolABC;
                uEnter2 =  u[AB] * invVolABC;
                enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
                             + uEnter1*vert[1] + uEnter2*vert[2];
                
                nextSign = -1;
            }
            else  {
                leaveFace = 3;
                uLeave1 = -u[AC] * invVolABC;
                uLeave2 =  u[AB] * invVolABC;
                leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
                             + uLeave1*vert[1] + uLeave2*vert[2];

                nextSign = 1;
            }

            // Determine the other intersecting face between BAD, CDA, DCB
            // Examine face BAD
            if ((sign[AD] == nextSign)  ||  (sign[AD] == 0))  {
                // Face BAD may intersect with the ray
                if ((sign[DB] == nextSign)  ||  
                    ((sign[DB] == 0)  &&
                     ((sign[AD] != 0)  ||  (sign[AB] != 0))))  {
                    // Face BAD intersects with the ray
                    double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
                    if (nextSign == 1)  {
                        enterFace = 2;
                        uEnter1 =  u[DB] * invVolBAD;
                        uEnter2 = -u[AB] * invVolBAD;
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
                        uLeave1 =  u[DB] * invVolBAD;
                        uLeave2 = -u[AB] * invVolBAD;
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
            if ((sign[DC] == -nextSign)  ||
                ((sign[DC] == 0)  &&  ((sign[AD] != 0)  ||  (sign[AC] != 0))))  {
                // Face CDA intersects with the ray
                double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
                if (nextSign == 1)  {
                    enterFace = 1;
                    uEnter1 =  u[AC] * invVolCDA;
                    uEnter2 = -u[DC] * invVolCDA;
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
                    uLeave1 =  u[AC] * invVolCDA;
                    uLeave2 = -u[DC] * invVolCDA;
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
                double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
                if (nextSign == 1)  {
                    enterFace = 0;
                    uEnter1 = -u[DB] * invVolDCB;
                    uEnter2 =  u[DC] * invVolDCB;
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
                    uLeave1 = -u[DB] * invVolDCB;
                    uLeave2 =  u[DC] * invVolDCB;
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
    if ((sign[AD] == -sign[AB])  ||  (sign[AB] == 0)  ||  (sign[AD] == 0))  {
        // Face BAD may intersect with the ray
        int signBAD = -sign[AB];
        if (signBAD == 0)  {
            signBAD = sign[AD];
            if (signBAD == 0)  {
                signBAD = sign[DB];
            }
        }

        if ((signBAD != 0)  &&
            ((sign[DB] == signBAD)  ||  (sign[DB] == 0)))  {
            // Face BAD intersects with the ray
            double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
            if (signBAD == 1)  {
                enterFace = 2;
                uEnter1 =  u[DB] * invVolBAD;
                uEnter2 = -u[AB] * invVolBAD;
                enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                             + uEnter1*vert[0] + uEnter2*vert[3];

                nextSign = -1;
            }
            else  {
                leaveFace = 2;
                uLeave1 =  u[DB] * invVolBAD;
                uLeave2 = -u[AB] * invVolBAD;
                leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                             + uLeave1*vert[0] + uLeave2*vert[3];

                nextSign = 1;
            }

            // Determine the other intersecting face between CDA, DCB
            if ((sign[DC] == -nextSign)  ||
                ((sign[DC] == 0)  &&  ((sign[AD] != 0)  ||  (sign[AC] != 0))))  {
                // Face CDA intersects with the ray
                double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
                if (nextSign == 1)  {
                    enterFace = 1;
                    uEnter1 =  u[AC] * invVolCDA;
                    uEnter2 = -u[DC] * invVolCDA;
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
                    uLeave1 =  u[AC] * invVolCDA;
                    uLeave2 = -u[DC] * invVolCDA;
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
                double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
                if (nextSign == 1)  {
                    enterFace = 0;
                    uEnter1 = -u[DB] * invVolDCB;
                    uEnter2 =  u[DC] * invVolDCB;
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
                    uLeave1 = -u[DB] * invVolDCB;
                    uLeave2 =  u[DC] * invVolDCB;
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
    if ((-sign[AD] == sign[AC])  ||  (sign[AC] == 0)  ||  (sign[AD] == 0))  {
        // Face CDA may intersect with the ray
        int signCDA = sign[AC];
        if (signCDA == 0)  {
            signCDA = -sign[AD];
            if (signCDA == 0)  {
                signCDA = -sign[DC];
            }
        }

        if ((signCDA != 0)  &&
            ((-sign[DC] == signCDA)  ||  (sign[DC] == 0)))  {
            // Face CDA intersects with the ray
            // Face DCB also intersects with the ray
            double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
            double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
            
            if (signCDA == 1)  {
                enterFace = 1;
                uEnter1 =  u[AC] * invVolCDA;
                uEnter2 = -u[DC] * invVolCDA;
                enterPoint =   (1-uEnter1-uEnter2)*vert[2]
                             + uEnter1*vert[3] + uEnter2*vert[0];
                
                leaveFace = 0;
                uLeave1 = -u[DB] * invVolDCB;
                uLeave2 =  u[DC] * invVolDCB;
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
                uLeave1 =  u[AC] * invVolCDA;
                uLeave2 = -u[DC] * invVolCDA;
                leavePoint =   (1-uLeave1-uLeave2)*vert[2]
                             + uLeave1*vert[3] + uLeave2*vert[0];
                
                enterFace = 0;
                uEnter1 = -u[DB] * invVolDCB;
                uEnter2 =  u[DC] * invVolDCB;
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


bool RayTetraSegura2T(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[],
    const double* u, const int* sign,
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    enterFace = -1;
    leaveFace = -1;

    int nextSign;

    // Examine face ABC
    if ((sign[AB] != 0)  ||  (sign[BC] != 0)  ||  (sign[AC] != 0))  {
        // Check if ray intersects with face ABC
        if ((sign[AB] >= 0)  &&  (sign[BC] >= 0)  &&  (sign[AC] <= 0))  {
            enterFace = 3;
            double invVolABC = 1.0 / (u[AB] + u[BC] - u[AC]);
            uEnter1 = -u[AC] * invVolABC;
            uEnter2 =  u[AB] * invVolABC;
            enterPoint =   (1-uEnter1-uEnter2)*vert[0] 
                         + uEnter1*vert[1] + uEnter2*vert[2];

            nextSign = -1;
            // The other intersecting face is determined after the next 'else'
        }
        else if ((sign[AB] <= 0)  &&  (sign[BC] <= 0)  &&  (sign[AC] >= 0))  {
            leaveFace = 3;
            double invVolABC = 1.0 / (u[AB] + u[BC] - u[AC]);
            uLeave1 = -u[AC] * invVolABC;
            uLeave2 =  u[AB] * invVolABC;
            leavePoint =   (1-uLeave1-uLeave2)*vert[0] 
                         + uLeave1*vert[1] + uLeave2*vert[2];

            nextSign = 1;
            // The other intersecting face is determined after the next 'else'
        }
        else  {
            // Ray does not intersect with ABC,
            // determine the next face to examine

            int nextFace;

            if (u[AB]+u[BC]-u[AC] > 0)  {
                if (sign[BC] == -1)  {
                    if (sign[AC] == 1)  {
                        nextFace = 2;
                        nextSign = -1;
                    }
                    else if (sign[AB] == -1)  {
                        nextFace = 1;
                        nextSign = -1;
                    }
                    else  {
                        nextFace = 0;
                        nextSign = 1;
                    }
                }
                else if (sign[AC] == 1)  {
                    if (sign[AB] == -1)  {
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
                if (sign[BC] == 1)  {
                    if (sign[AC] == -1)  {
                        nextFace = 2;
                        nextSign = 1;
                    }
                    else if (sign[AB] == 1)  {
                        nextFace = 1;
                        nextSign = 1;
                    }
                    else  {
                        nextFace = 0;
                        nextSign = -1;
                    }
                }
                else if (sign[AC] == -1)  {
                    if (sign[AB] == 1)  {
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
                if (sign[AD] == -nextSign)  {
                    return false;
                }

                if (sign[DB] == -nextSign)  {
                    return false;
                }

                // Face BAD intersects with the ray
                double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
                if (nextSign == 1)  {
                    enterFace = 2;
                    uEnter1 =  u[DB] * invVolBAD;
                    uEnter2 = -u[AB] * invVolBAD;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[1]
                                 + uEnter1*vert[0] + uEnter2*vert[3];

                    nextSign = -1;
                }
                else  {
                    leaveFace = 2;
                    uLeave1 =  u[DB] * invVolBAD;
                    uLeave2 = -u[AB] * invVolBAD;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[1]
                                 + uLeave1*vert[0] + uLeave2*vert[3];

                    nextSign = 1;
                }

                // Determine the other intersecting face between CDA, DCB
                if ((sign[DC] == -nextSign)  ||
                    ((sign[DC] == 0)  &&  
                     ((sign[AD] != 0)  ||  (sign[AC] != 0))))  {
                    // Face CDA intersects with the ray
                    double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
                    if (nextSign == 1)  {
                        enterFace = 1;
                        uEnter1 =  u[AC] * invVolCDA;
                        uEnter2 = -u[DC] * invVolCDA;
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
                        uLeave1 =  u[AC] * invVolCDA;
                        uLeave2 = -u[DC] * invVolCDA;
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
                    double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
                    if (nextSign == 1)  {
                        enterFace = 0;
                        uEnter1 = -u[DB] * invVolDCB;
                        uEnter2 =  u[DC] * invVolDCB;
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
                        uLeave1 = -u[DB] * invVolDCB;
                        uLeave2 =  u[DC] * invVolDCB;
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
                if (sign[DC] == nextSign)  {
                    return false;
                }

                if (sign[AD] == nextSign)  {
                    return false;
                }
                
                // Face CDA intersects with the ray
                double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
                if (nextSign == 1)  {
                    enterFace = 1;
                    uEnter1 =  u[AC] * invVolCDA;
                    uEnter2 = -u[DC] * invVolCDA;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[2] 
                                 + uEnter1*vert[3] + uEnter2*vert[0];

                    nextSign = -1;
                }
                else  {
                    leaveFace = 1;
                    uLeave1 =  u[AC] * invVolCDA;
                    uLeave2 = -u[DC] * invVolCDA;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[2] 
                                 + uLeave1*vert[3] + uLeave2*vert[0];

                    nextSign = 1;
                }

                // Determine the other intersecting face between BAD, DCB
                if ((sign[DB] == nextSign)  ||
                    ((sign[DB] == 0)  &&  
                     ((sign[AB] != 0)  ||  (sign[AD] != 0))))  {
                    // Face BAD intersects with the ray
                    double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
                    if (nextSign == 1)  {
                        enterFace = 2;
                        uEnter1 =  u[DB] * invVolBAD;
                        uEnter2 = -u[AB] * invVolBAD;
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
                        uLeave1 =  u[DB] * invVolBAD;
                        uLeave2 = -u[AB] * invVolBAD;
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
                    double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
                    if (nextSign == 1)  {
                        enterFace = 0;
                        uEnter1 = -u[DB] * invVolDCB;
                        uEnter2 =  u[DC] * invVolDCB;
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
                        uLeave1 = -u[DB] * invVolDCB;
                        uLeave2 =  u[DC] * invVolDCB;
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
                if (sign[DC] == -nextSign)  {
                    return false;
                }

                if (sign[DB] == nextSign)  {
                    return false;
                }

                // Face DCB intersects with the ray
                double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
                if (nextSign == 1)  {
                    enterFace = 0;
                    uEnter1 = -u[DB] * invVolDCB;
                    uEnter2 =  u[DC] * invVolDCB;
                    enterPoint =   (1-uEnter1-uEnter2)*vert[3] 
                                 + uEnter1*vert[2] + uEnter2*vert[1];

                    nextSign = -1;
                }
                else  {
                    leaveFace = 0;
                    uLeave1 = -u[DB] * invVolDCB;
                    uLeave2 =  u[DC] * invVolDCB;
                    leavePoint =   (1-uLeave1-uLeave2)*vert[3] 
                                 + uLeave1*vert[2] + uLeave2*vert[1];

                    nextSign = 1;
                }
                
                // Determine the other intersecting face between BAD, CDA
                if ((sign[AD] == nextSign)  ||
                    ((sign[AD] == 0)  &&  
                     ((sign[AB] != 0)  ||  (sign[DB] != 0))))  {
                    // Face BAD intersects with the ray
                    double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
                    if (nextSign == 1)  {
                        enterFace = 2;
                        uEnter1 =  u[DB] * invVolBAD;
                        uEnter2 = -u[AB] * invVolBAD;
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
                        uLeave1 =  u[DB] * invVolBAD;
                        uLeave2 = -u[AB] * invVolBAD;
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
                    double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
                    if (nextSign == 1)  {
                        enterFace = 1;
                        uEnter1 =  u[AC] * invVolCDA;
                        uEnter2 = -u[DC] * invVolCDA;
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
                        uLeave1 =  u[AC] * invVolCDA;
                        uLeave2 = -u[DC] * invVolCDA;
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
        
        // Face ABC intersects with the ray.
        // Determine the other intersecting face between BAD, CDA, DCB
        // Examine face BAD
        if ((sign[AD] == nextSign)  ||  (sign[AD] == 0))  {
            // Face BAD may intersect with the ray
            if ((sign[DB] == nextSign)  ||  
                ((sign[DB] == 0)  &&  
                 ((sign[AD] != 0)  ||  (sign[AB] != 0))))  {
                // Face BAD intersects with the ray
                double invVolBAD = 1.0 / (u[AD] + u[DB] - u[AB]);
                if (nextSign == 1)  {
                    enterFace = 2;
                    uEnter1 =  u[DB] * invVolBAD;
                    uEnter2 = -u[AB] * invVolBAD;
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
                    uLeave1 =  u[DB] * invVolBAD;
                    uLeave2 = -u[AB] * invVolBAD;
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
        if ((sign[DC] == -nextSign)  ||
            ((sign[DC] == 0)  &&  ((sign[AD] != 0)  ||  (sign[AC] != 0))))  {
            // Face CDA intersects with the ray
            double invVolCDA = 1.0 / (u[AC] - u[DC] - u[AD]);
            if (nextSign == 1)  {
                enterFace = 1;
                uEnter1 =  u[AC] * invVolCDA;
                uEnter2 = -u[DC] * invVolCDA;
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
                uLeave1 =  u[AC] * invVolCDA;
                uLeave2 = -u[DC] * invVolCDA;
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
            double invVolDCB = 1.0 / (u[DC] - u[BC] - u[DB]);
            if (nextSign == 1)  {
                enterFace = 0;
                uEnter1 = -u[DB] * invVolDCB;
                uEnter2 =  u[DC] * invVolDCB;
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
                uLeave1 = -u[DB] * invVolDCB;
                uLeave2 =  u[DC] * invVolDCB;
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
        // Ray is on the plane of ABC
        // Examine face BAD
        if ((sign[AD] == 0)  &&  (sign[DB] == 0))  {
            if (sign[DC] == 0)  {
                // The tetrahedron is flat and the ray is on its plane
                return false;
            }
            else  {
                // The ray is on the AB edge
                if (sign[DC] == 1)  {
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
            if ((sign[AD] >= 0)  &&  (sign[DB] >= 0))  {
                // The ray enters through BAD
                enterFace = 2;
                double invVolBAD = 1.0 / (u[AD] + u[DB]);
                uEnter1 =  u[DB] * invVolBAD;
                uEnter2 =  0;
                enterPoint = (1-uEnter1)*vert[1] + uEnter1*vert[0];
                
                // Determine the other intersecting face between CDA, DCB
                if ((sign[DC] == 1)  ||
                    ((sign[DC] == 0)  &&  (sign[AD] == 1)))  {
                    // The ray leaves through CDA
                    leaveFace = 1;
                    double invVolCDA = 1.0 / (-u[DC] - u[AD]);
                    uLeave1 =  0;
                    uLeave2 = -u[DC] * invVolCDA;
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
                    double invVolDCB = 1.0 / (u[DC] - u[DB]);
                    uLeave1 = -u[DB] * invVolDCB;
                    uLeave2 =  u[DC] * invVolDCB;
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
            else if ((sign[AD] <= 0)  &&  (sign[DB] <= 0))  {
                // The ray leaves through BAD
                leaveFace = 2;

                double invVolBAD = 1.0 / (u[AD] + u[DB]);
                uLeave1 =  u[DB] * invVolBAD;
                uLeave2 =  0;
                leavePoint = (1-uLeave1)*vert[1] + uLeave1*vert[0];
                
                // Determine the other intersecting face between CDA, DCB
                if ((sign[DC] == -1)  ||
                    ((sign[DC] == 0)  &&  (sign[AD] == -1)))  {
                    // The ray enters through CDA
                    enterFace = 1;
                    double invVolCDA = 1.0 / (- u[DC] - u[AD]);
                    uEnter1 =  0;
                    uEnter2 = -u[DC] * invVolCDA;
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
                    double invVolDCB = 1.0 / (u[DC] - u[DB]);
                    uEnter1 = -u[DB] * invVolDCB;
                    uEnter2 =  u[DC] * invVolDCB;
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
}
