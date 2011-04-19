#ifndef _NpPluecker_hpp_
#define _NpPluecker_hpp_

#include "NpVector.hpp"


// NpPluecker //////////////////////////////////////////////////////////////////

class NpPluecker
{
private:
    NpVector d;   // direction
    NpVector c;   // cross
    
public:
    NpPluecker()
    { };
    
    // Pluecker coordinates of a ray specified by points orig and dest.
    NpPluecker(const NpVector& orig, const NpVector& dest)
        : d(dest-orig), c(dest^orig)
    { };

    // Pluecker coordinates of a ray specified by point orig and
    // direction dir. The bool parameter serves only to discriminate this
    // from the previous constructor
    NpPluecker(const NpVector& orig, const NpVector& dir, bool haveDir)
        : d(dir), c(dir^orig)
    { };

    // Permuted inner product operator
    friend double operator*(const NpPluecker& pl1, const NpPluecker& pl2);
};


inline double operator*(const NpPluecker& pl1, const NpPluecker& pl2)
{
    return (pl1.d * pl2.c + pl2.d * pl1.c);
}


#endif   // _NpPluecker_hpp_
