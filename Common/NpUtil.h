#ifndef _NpUtil_h_
#define _NpUtil_h_


#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197169399375105820974944592308
#endif

#include <cstdlib>


const double NpZero = 1e-6;
const double NpInf = 1.7976931348623157e+308;


inline int NpSign(const double x)
{
    return (x>NpZero ? 1 : (x<-NpZero ? -1 : 0));
}


inline double NpDegToRad(const double d)
{
    return (d * M_PI / 180);
}


inline double NpRadToDeg(const double r)
{
    return (180 * r / M_PI);
}


inline int NpRand(int max)
{
    // Returns a random integer in [0, max)
    int r = static_cast<int>((static_cast<double>(rand()) / RAND_MAX) * max);
    return (r == max ? max-1 : r);
}


inline int NpRand(int min, int max)
{
    // Returns a random integer in [min, max)
    int d = max-min;
    int r = static_cast<int>((static_cast<double>(rand()) / RAND_MAX) * d);
    return (r == d ? max-1 : r+min);
}


inline double NpRand(double max)
{
    // Returns a random double in [0, max)
    double r = (static_cast<double>(rand()) / RAND_MAX) * max;
    if (r == max)  {
        return (max - static_cast<int>(max));
    }
    else  {
        return r;
    }
}


inline double NpRand(double min, double max)
{
    // Returns a random double in [0, max)
    double d = max-min;
    double r = (static_cast<double>(rand()) / RAND_MAX) * d;
    if (r == d)  {
        return (max - static_cast<int>(max));
    }
    else  {
        return r+min;
    }
}


#endif  // _NpUtil_h_
