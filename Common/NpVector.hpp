#ifndef _NpVector_hpp_
#define _NpVector_hpp_

#include "NpUtil.h"

#include <cmath>
#include <iostream>


// NpVector ////////////////////////////////////////////////////////////////////

class NpVector  
{
public:
    double x;
    double y;
    double z;

    NpVector();
    NpVector(double xx, double yy, double zz);

    double Measure2() const;
    double Measure() const;
    double CosOfAngleWith(const NpVector& vec) const;
    double AngleWith(const NpVector& vec) const;
    void MakeUnitVector();

    // Some operators for vectors
    NpVector& operator+=(const NpVector& vec);        // Vector addition
    NpVector& operator-=(const NpVector& vec);        // Vector subtraction
    bool      operator==(const NpVector& vec) const;  // Equality test
};


// Vector negation
NpVector operator-(const NpVector& vec);

// Vector addition
NpVector operator+(const NpVector& vec1, const NpVector& vec2);

// Vector subtraction
NpVector operator-(const NpVector& vec1, const NpVector& vec2);

// Dot product
double operator*(const NpVector& vec1, const NpVector& vec2);
double Dot(const NpVector& vec1, const NpVector& vec2);

// Scalar product
NpVector operator*(double c, const NpVector& vec);

// Cross product
NpVector operator^(const NpVector& vec1, const NpVector& vec2);
NpVector Cross(const NpVector& vec1, const NpVector& vec2);

// Scalar triple product
double ScalarTriple(const NpVector& vec1, const NpVector& vec2,
                    const NpVector& vec3);


// Input/Output of vectors
std::istream& operator>>(std::istream& istr, NpVector& vec);
std::ostream& operator<<(std::ostream& ostr, const NpVector& vec);



// Inline functions ////////////////////////////////////////////////////////////

inline NpVector::NpVector()
    : x(0), y(0), z(0)
{
}


inline NpVector::NpVector(double xx, double yy, double zz)
    : x(xx), y(yy), z(zz)
{
}


inline double NpVector::Measure2() const
{
    return (x*x + y*y + z*z);
}


inline double NpVector::Measure() const
{
    return sqrt(Measure2());
}


inline double NpVector::CosOfAngleWith(const NpVector& vec) const
{
    return ((*this)*vec) / (Measure()*vec.Measure());
}


inline double NpVector::AngleWith(const NpVector& vec) const
{
    double cos = CosOfAngleWith(vec);
    return NpSign(cos) * std::acos(cos);
}


inline void NpVector::MakeUnitVector()
{
    double measure = Measure();
    x /= measure;
    y /= measure;
    z /= measure;
}


inline NpVector& NpVector::operator+=(const NpVector& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;

    return *this;
}


inline NpVector& NpVector::operator-=(const NpVector& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;

    return *this;
}


inline bool NpVector::operator==(const NpVector& vec) const
{
    return ( (x == vec.x)  &&  (y == vec.y)  &&  (z == vec.z) );
}


inline NpVector operator-(const NpVector& vec)
{
    return NpVector(-vec.x, -vec.y, -vec.z);
}


inline NpVector operator+(const NpVector& vec1, const NpVector& vec2)
{
    return NpVector(vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z);
}


inline NpVector operator-(const NpVector& vec1, const NpVector& vec2)
{
    return NpVector(vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z);
}


inline double operator*(const NpVector& vec1, const NpVector& vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
}


inline double Dot(const NpVector& vec1, const NpVector& vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
}


inline NpVector operator*(double c, const NpVector& vec)
{
    return NpVector(c*vec.x, c*vec.y, c*vec.z);
}


inline NpVector operator^(const NpVector& vec1, const NpVector& vec2)
{
    return NpVector(vec1.y*vec2.z - vec2.y*vec1.z,
                    vec1.z*vec2.x - vec2.z*vec1.x,
                    vec1.x*vec2.y - vec2.x*vec1.y);
}


inline NpVector Cross(const NpVector& vec1, const NpVector& vec2)
{
    return NpVector(vec1.y*vec2.z - vec2.y*vec1.z,
                    vec1.z*vec2.x - vec2.z*vec1.x,
                    vec1.x*vec2.y - vec2.x*vec1.y);
}


inline double ScalarTriple(const NpVector& vec1, const NpVector& vec2,
                           const NpVector& vec3)
{
    return (vec1.x*(vec2.y*vec3.z - vec3.y*vec2.z) + 
            vec1.y*(vec2.z*vec3.x - vec3.z*vec2.x) +
            vec1.z*(vec2.x*vec3.y - vec3.x*vec2.y));
}


inline std::istream& operator>>(std::istream& istr, NpVector& vec)
{
    istr >> vec.x >> vec.y >> vec.z;
    return istr;
}


inline std::ostream& operator<<(std::ostream& ostr, const NpVector& vec)
{
    ostr << vec.x << " " << vec.y << " " << vec.z;
    return ostr;
}


#endif   // _NpVector_hpp_
