#include "../Common/NpVector.hpp"
#include "../Common/NpUtil.h"
#include "../Common/NpPlane.hpp"


// Ray-Triangle intersection using Haines's algorithm

bool RayTetraHaines(
    const NpVector& orig, const NpVector& dir,
    const NpVector vert[], 
    int& enterFace, int& leaveFace,
    NpVector& enterPoint, NpVector& leavePoint,
    double& uEnter1, double& uEnter2, double& uLeave1, double& uLeave2,
    double& tEnter, double& tLeave)
{
    static const int faceVertices[4][3] = { {3, 2, 1},
                                            {2, 3, 0},
                                            {1, 0, 3},
                                            {0, 1, 2} };

    tEnter = -NpInf;
    tLeave = NpInf;

    enterFace = -1;   // Keep the compiler happy about
    leaveFace = -1;   // uninitialized variables

    // Test each plane in polyhedron
    for (unsigned int i = 0; i < 4; ++i)  {
        // Calculate face equation, faceNormal*(x,y,z) + faceD;
        NpPlane facePlane(vert[faceVertices[i][0]],
                          vert[faceVertices[i][1]],
                          vert[faceVertices[i][2]]);

        // Compute intersection point T and sidedness
        double vd = dir * facePlane.n;
        double vn = (orig * facePlane.n) + facePlane.d;

        if (vd == 0.0)  {
            // ray is parallel to plane - 
            // check if ray origin is inside plane's half-space
            if (vn > 0.0)  {
                // ray origin is outside half-space
                return false;
            }
        }
        else  {
            // ray not parallel - get distance to plane
            double t = -vn / vd;
            if (vd < 0.0)  {
                // front face - T is a near point
                if (t > tLeave)  {
                    return false;
                }
                if (t > tEnter)  {
                    // hit near face, update normal
                    tEnter = t;
                    enterFace = i;
                }
            }
            else  {
                // back face - T is a far point
                if (t < tEnter)  {
                    return false;
                }
                if (t < tLeave)  {
                    // hit far face, update normal
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
    
    // survived all tests, ray intersects tetrahedron
    enterPoint = orig + tEnter * dir;
    leavePoint = orig + tLeave * dir;

    // Calculate barycentric coordinates of enterPoint
    const NpVector& aEnter = vert[faceVertices[enterFace][0]];

    NpVector abEnter = vert[faceVertices[enterFace][1]] - aEnter;
    NpVector acEnter = vert[faceVertices[enterFace][2]] - aEnter;
    NpVector apEnter = enterPoint - aEnter;
    NpVector normalEnter = abEnter ^ acEnter;
    double invLengthEnter = 1.0 / normalEnter.Measure2();

    uEnter1 = invLengthEnter * ((apEnter ^ acEnter) * normalEnter);
    uEnter2 = invLengthEnter * ((abEnter ^ apEnter) * normalEnter);
    
    // Calculate barycentric coordinates of leavePoint
    const NpVector& aLeave = vert[faceVertices[leaveFace][0]];

    NpVector abLeave = vert[faceVertices[leaveFace][1]] - aLeave;
    NpVector acLeave = vert[faceVertices[leaveFace][2]] - aLeave;
    NpVector apLeave = leavePoint - aLeave;
    NpVector normalLeave = abLeave ^ acLeave;
    double invLengthLeave = 1.0 / normalLeave.Measure2();

    uLeave1 = invLengthLeave * ((apLeave ^ acLeave) * normalLeave);
    uLeave2 = invLengthLeave * ((abLeave ^ apLeave) * normalLeave);
    
    return true;
}
