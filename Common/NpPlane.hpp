#ifndef _NpPlane_hpp_
#define _NpPlane_hpp_

#include "NpVector.hpp"


// NpPlane /////////////////////////////////////////////////////////////////////

struct NpPlane
{
    NpVector n;
    double d;

    NpPlane()
    { };
    
    NpPlane(const NpVector& v0, const NpVector& v1, const NpVector& v2)
    {
        NpVector dir = v1 - v2;
        NpVector c = v2 ^ v1;

        n.x =  v0.y*dir.z - v0.z*dir.y - c.x;
        n.y = -v0.x*dir.z + v0.z*dir.x - c.y;
        n.z =  v0.x*dir.y - v0.y*dir.x - c.z;
        d   =  v0 * c; 
    }
};


#endif   // _NpPlane_hpp_
