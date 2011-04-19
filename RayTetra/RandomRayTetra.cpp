#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "../Common/NpVector.hpp"
#include "../Common/NpUtil.h"


// Structures of tetrahedron and line //////////////////////////////////////////

struct NpTetrahedron
{
    NpVector v[4];

    NpTetrahedron()
        {  v[0] = NpVector(); v[1] = NpVector(); 
           v[2] = NpVector(); v[3] = NpVector();  }
    
    NpTetrahedron(const NpVector& v0, const NpVector& v1, 
                  const NpVector& v2, const NpVector& v3)
        {  v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;  }

    // Gives the tetrahedron point 
    // u0*v[0] + u1*v[1] + u2*v[2] + (1-u0-u1-u2)*v[2]
    NpVector TetrahedronPoint(double u0, double u1, double u2) const
        {  return (u0*v[0] + u1*v[1] + u2*v[2] + (1-u0-u1-u2)*v[2]);  }
};

std::ostream& operator<<(std::ostream& ostr, const NpTetrahedron& tetrahedron);



struct NpLine
{
    NpVector orig, dest;

    NpLine()
        : orig(NpVector()), dest(NpVector())
        { }

    NpLine(const NpVector& o, const NpVector& d)
        : orig(o), dest(d)
        { }
};

std::ostream& operator<<(std::ostream& ostr, const NpLine& line);



// Generate a random vertex with coordinates within a min/max box
NpVector RandomVector(const NpVector& min, const NpVector& max);

// Generate a random tetrahedron inside a min/max bounding box
NpTetrahedron RandomTetrahedron(const NpVector& min, const NpVector& max);

// Generate a line intersecting the given tetrahedron at a random point
NpLine IntersectingLine(const NpTetrahedron& t);

// Generate a line non-intersecting the given tetrahedron at a random point
NpLine NonIntersectingLine(const NpTetrahedron& t);

// Print a help message when the input arguments are incorrect
void PrintHelp(char* arg0);



// main ////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    if ((argc != 4)  &&  (argc != 6))  {
        PrintHelp(argv[0]);
        return 1;
    }

    // The file containing the lines
    std::ofstream out(argv[1]);
    if (out.fail())  {
        std::cerr << "Cannot open output file" << std::endl;
        return 1;
    }

    int nIntersectingLines = 0;
    int nNonIntersectingLines = 0;

    // Parse other arguments
    int arg = 2;
    while (arg < argc)  {
        if (strcmp(argv[arg], "-i") == 0)  {
            ++arg;
            if (arg < argc)  {
                nIntersectingLines = std::atoi(argv[arg]);
                ++arg;
            }
            else  {
                PrintHelp(argv[0]);
                return 1;
            }
        }
        else if (strcmp(argv[arg], "-n") == 0)  {
            ++arg;
            if (arg < argc)  {
                nNonIntersectingLines = std::atoi(argv[arg]);
                ++arg;
            }
            else  {
                PrintHelp(argv[0]);
                return 1;
            }
        }
        else  {
            PrintHelp(argv[0]);
            return 1;
        }
    }

    // Now generate the lines
    
    // Bounding box of all tetrahedra
    NpVector min(-10.0, -10.0, -10.0);
    NpVector max(10.0, 10.0, 10.0);

    int nTotalLines = nIntersectingLines + nNonIntersectingLines;

    out << nTotalLines << std::endl;

    double intersectingPercent = double(nIntersectingLines) / nTotalLines * 100;

    int generatedIntersectingLines = 0;
    int generatedNonIntersectingLines = 0;
    for (int i = 0; i < nTotalLines; ++i)  {
        NpTetrahedron tr = RandomTetrahedron(min, max);
        NpLine ln;

        if (generatedIntersectingLines == nIntersectingLines)  {
            ln = NonIntersectingLine(tr);
            ++generatedNonIntersectingLines;
        }
        else if (generatedNonIntersectingLines == nNonIntersectingLines)  {
            ln = IntersectingLine(tr);
            ++generatedIntersectingLines;
        }
        else  {
            double chance = NpRand(100);
            if (chance < intersectingPercent)  {
                ln = IntersectingLine(tr);
                ++generatedIntersectingLines;
            }
            else  {
                ln = NonIntersectingLine(tr);
                ++generatedNonIntersectingLines;
            }
        }
        out << tr << "    " << ln << std::endl;
    }

    return 0;
}



// Implementation //////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& ostr, const NpTetrahedron& tetrahedron)
{
    ostr << tetrahedron.v[0] << "   " << tetrahedron.v[1] << "   " 
         << tetrahedron.v[2] << "   " << tetrahedron.v[3];
    return ostr;
}



std::ostream& operator<<(std::ostream& ostr, const NpLine& line)
{
    ostr << line.orig << "   " << line.dest;
    return ostr;
}



NpVector RandomVector(const NpVector& min, const NpVector& max)
{
    return NpVector(NpRand(min.x, max.x),
                    NpRand(min.y, max.y),
                    NpRand(min.z, max.z));
}



NpTetrahedron RandomTetrahedron(const NpVector& min, const NpVector& max)
{
    NpTetrahedron tetra(RandomVector(min, max),
                        RandomVector(min, max),
                        RandomVector(min, max),
                        RandomVector(min, max));

    double vol = (tetra.v[0]-tetra.v[1]) * 
                 ((tetra.v[0]-tetra.v[2]) ^ (tetra.v[0]-tetra.v[3]));
    if (vol < 0)  {
        NpVector tmp(tetra.v[2]);
        tetra.v[2] = tetra.v[3];
        tetra.v[3] = tmp;
    }

    return tetra;
}



NpLine IntersectingLine(const NpTetrahedron& t)
{
    static const int faceVertices[4][3] = { {3, 2, 1},
                                            {2, 3, 0},
                                            {1, 0, 3},
                                            {0, 1, 2} };

    // Select two random faces of the tetrahedron 
    // (possibly the same face twice, to produce tangent lines)
    int face1 = NpRand(4);
    int face2 = NpRand(4);

    // Find two points inside the selected faces
    NpVector pt1, pt2;
    double u1, u2;

    u1 = NpRand(0.0, 1.0);
    u2 = NpRand(0.0, 1.0-u1);
    pt1 = (1-u1-u2) * t.v[faceVertices[face1][0]] +
                 u1 * t.v[faceVertices[face1][1]] +
                 u2 * t.v[faceVertices[face1][2]];

    do  {
        u1 = NpRand(0.0, 1.0);
        u2 = NpRand(0.0, 1.0-u1);
        pt2 = (1-u1-u2) * t.v[faceVertices[face2][0]] +
                     u1 * t.v[faceVertices[face2][1]] +
                     u2 * t.v[faceVertices[face2][2]];
    }  while (pt2 == pt1);

    NpVector dir = pt2 - pt1;

    // Generate two points on the line
    double t0 = NpRand(-10.0, 0.0);
    NpVector linePt0 = pt1 + t0*dir;

    double t1 = NpRand(0.0, 10.0);
    NpVector linePt1 = pt2 + t1*dir;

    return NpLine(linePt0, linePt1);
}



NpLine NonIntersectingLine(const NpTetrahedron& t)
{
    // Find a point outside the tetrahedron
    double u0, u1, u2;
    do  {
        u0 = NpRand(-10.0, 10.0);
        u1 = NpRand(-10.0, 10.0);
        u2 = NpRand(-10.0, 10.0);
    }  while ((u0 >= 0.0)  &&  (u0 <= 1.0)  &&
              (u1 >= 0.0)  &&  (u1 <= 1.0)  &&
              (u2 >= 0.0)  &&  (u1 <= 1.0)  &&
              (u0+u1+u2 >= 0.0)  &&  (u0+u1+u2 <= 1.0));

    NpVector tetrahedronPt = t.TetrahedronPoint(u0, u1, u2);

    // Find a random direction
    NpVector dir = RandomVector(NpVector(-1, -1, -1), NpVector(1, 1, 1));
    
    // Generate two points on the line
    double t0 = NpRand(-10.0, 0.0);
    NpVector linePt0 = tetrahedronPt + t0*dir;

    double t1 = NpRand(0.0, 10.0);
    NpVector linePt1 = tetrahedronPt + t1*dir;

    return NpLine(linePt0, linePt1);
}



void PrintHelp(char* arg0)
{
    std::cerr << "Usage:  " << arg0 
              << " <output file> -i <k> -n <k>" << std::endl;
}

