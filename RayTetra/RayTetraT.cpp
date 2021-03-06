#include "RayTetraAlgorithmsT.hpp"
#include "../Common/NpVector.hpp"
#include "../Common/NpProgramTimer.hpp"
#include "../Common/NpCArrayAdapter.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <getopt.h>

#include <GL/glut.h>


// Struct for program arguments

struct ProgramArguments
{
    std::string algorithm;
    char* inputFileName;
    char* outputFileName;
    unsigned int repetitions;
    int print;
    int displayResult;
};


// Struct for all variables required for glut

struct GlutVars
{
    enum MouseAction  {
        Np_MA_ROTATE, Np_MA_PAN, Np_MA_ZOOM
    };

    GLdouble cameraPosX, cameraPosY, cameraPosZ,
             cameraAimX, cameraAimY, cameraAimZ,
             fovy, zNear, zFar;

    GLfloat transX, transY, transZ, rotateX, rotateY;

    int viewportWidth, viewportHeight;

    GLenum polygonMode;

    bool drawSpheres;

    int mouseInitX, mouseInitY;
    MouseAction mouseAction;

    GlutVars()
        : transX(0), transY(0), transZ(0), rotateX(0), rotateY(0),
          viewportWidth(500), viewportHeight(500), polygonMode(GL_FILL),
          drawSpheres(true)
    { }
};


// Struct for the bounding box of the scene
struct SceneBox
{
    NpVector max, min, center;

    SceneBox()
    { }

    SceneBox(NpVector v[], 
             const NpVector& orig, const NpVector& dest,
             const NpVector& enterPoint, const NpVector& leavePoint);
};



// Some global variables required for display //////////////////////////////////

NpVector v[4], orig, dest, enterPoint, leavePoint;
bool result;
GlutVars glutVars;
SceneBox sceneBox;



// Forward declarations ////////////////////////////////////////////////////////

bool ParseArgs(int argc, char* argv[], ProgramArguments& arguments);
void PrintHelp(char* argv0);
void GlutInit(int argc, char* argv[]);
void GlutDisplay();
void GlutReshape(int w, int h);
void GlutKeyboard(unsigned char key, int x, int y);
void GlutMouseClick(int button, int state, int x, int y);
void GlutMouseMotion(int x, int y);
void DisplayScene();
void DisplayPoint(const NpVector& v, float size);



// main ////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    ProgramArguments arguments;

    if (!ParseArgs(argc, argv, arguments))  {
        return -1;
    }

    // Open input and output files
    std::ifstream dataFile(arguments.inputFileName);
    if (dataFile.fail())  {
        std::cerr << "Could not open input file '" 
                  << arguments.inputFileName << "'" << std::endl;

        return -1;
    }

    int nTests = 0;
    dataFile >> nTests;

    if (arguments.displayResult == -1)  {
        std::ofstream resultsFile(arguments.outputFileName);
        if (resultsFile.fail())  {
            std::cerr << "Could not open output file '" 
                      << arguments.outputFileName << "'" << std::endl;

            return -1;
        }

        // Process all tests, if the user has not asked to display one
        std::vector< NpCArrayAdapter<NpVector, 4> > v(nTests);
        std::vector<NpVector> orig(nTests), dest(nTests), 
                              enterPoint(nTests), leavePoint(nTests);
        std::vector<int> enterFace(nTests), leaveFace(nTests);
        std::vector<double> ue1(nTests), ue2(nTests), ul1(nTests), ul2(nTests);
        std::vector<double> tEnter(nTests), tLeave(nTests);
        std::vector<bool> result(nTests);

        for (int i = 0; i < nTests; ++i)  {
            dataFile >> v[i][0] >> v[i][1] >> v[i][2] >> v[i][3] 
                     >> orig[i] >> dest[i];
        }

        NpProgramTimer timer;

         if (arguments.algorithm == "RayTetraHainesT")  {
            std::vector< NpCArrayAdapter<double, 4> > vd(nTests), vn(nTests);

            for (int i = 0; i < nTests; ++i)  {
                CalculateFacePlanes(orig[i], (dest[i]-orig[i]), 
                                    v[i], vd[i], vn[i]);
            }

            timer.Start();
            
            for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
                for (int i = 0; i < nTests; ++i)  {
                    result[i] = RayTetraHainesT(orig[i], (dest[i]-orig[i]),
                                                v[i],
                                                vd[i], vn[i],
                                                enterFace[i], leaveFace[i],
                                                enterPoint[i], leavePoint[i],
                                                ue1[i], ue2[i], ul1[i], ul2[i],
                                                tEnter[i], tLeave[i]);
                }
            }
            
            timer.Stop();
        }
        else  {
            std::vector< NpCArrayAdapter<double, 6> > u(nTests);
            std::vector< NpCArrayAdapter<int, 6> > sign(nTests);
            
            for (int i = 0; i < nTests; ++i)  {
                CalculatePluecker(orig[i], (dest[i]-orig[i]), v[i], 
                                  u[i], sign[i]);
            }

            if (arguments.algorithm.rfind('0') != std::string::npos)  {
                timer.Start();
                
                for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
                    for (int i = 0; i < nTests; ++i)  {
                        result[i] = RayTetraSegura0T(orig[i], (dest[i]-orig[i]),
                                               v[i],
                                               u[i], sign[i],
                                               enterFace[i], leaveFace[i],
                                               enterPoint[i], leavePoint[i],
                                               ue1[i], ue2[i], ul1[i], ul2[i],
                                               tEnter[i], tLeave[i]);
                    }
                }
                
                timer.Stop();
            }
            else if (arguments.algorithm.rfind('1') != std::string::npos)  {
                timer.Start();
                
                for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
                    for (int i = 0; i < nTests; ++i)  {
                        result[i] = RayTetraSegura1T(orig[i], (dest[i]-orig[i]),
                                               v[i],
                                               u[i], sign[i],
                                               enterFace[i], leaveFace[i],
                                               enterPoint[i], leavePoint[i],
                                               ue1[i], ue2[i], ul1[i], ul2[i],
                                               tEnter[i], tLeave[i]);
                    }
                }
                
                timer.Stop();
            }
            else if (arguments.algorithm.rfind('2') != std::string::npos)  {
                timer.Start();
                
                for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
                    for (int i = 0; i < nTests; ++i)  {
                        result[i] = RayTetraSegura2T(orig[i], (dest[i]-orig[i]),
                                               v[i],
                                               u[i], sign[i],
                                               enterFace[i], leaveFace[i],
                                               enterPoint[i], leavePoint[i],
                                               ue1[i], ue2[i], ul1[i], ul2[i],
                                               tEnter[i], tLeave[i]);
                    }
                }
                
                timer.Stop();
            }
        }

        if ((arguments.print == 1)  ||  (arguments.print == 3))  {
            resultsFile << timer.TotalElapsedTime() << std::endl;
        }

        if ((arguments.print == 2)  ||  (arguments.print == 3))  {
            for (int i = 0; i < nTests; ++i)  {
                resultsFile << std::boolalpha << result[i];
                if (result[i])  {
                    resultsFile << "   " << enterFace[i] << " " << leaveFace[i]
                                << "   " << enterPoint[i]
                                << "   " << leavePoint[i]
                                << "   " << ue1[i] << " " << ue2[i] 
                                << "   " << ul1[i] << " " << ul2[i]
                                << "   " << tEnter[i] << " " << tLeave[i];
                }
                resultsFile << std::endl;
            }
        }
    }
    else  {
        // Only process and display the result asked for
        if (arguments.displayResult > nTests)  {
            std::cout << "The line to display must be <= " << nTests <<std::endl;
            return -1;
        }
        
        for (int i = 0; i < arguments.displayResult-1; ++i)  {
            dataFile >> v[0] >> v[1] >> v[2] >> v[3] >> orig >> dest;
        }

        double ue1, ue2, ul1, ul2;
        double tEnter, tLeave;
        int enterFace = -2, leaveFace = -2;
        
        dataFile >> v[0] >> v[1] >> v[2] >> v[3] >> orig >> dest;

        if (arguments.algorithm == "RayTetraHainesT")  {
            double vd[4], vn[4];
            CalculateFacePlanes(orig, (dest-orig), v, vd, vn);

            result = RayTetraHainesT(orig, (dest-orig),
                                     v,
                                     vd, vn,
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
        }
        else  {
            NpCArrayAdapter<double, 6> u;
            NpCArrayAdapter<int, 6> sign;
            CalculatePluecker(orig, (dest-orig), v, u, sign);

            if (arguments.algorithm.rfind('0') != std::string::npos)  {
                result = RayTetraSegura0T(orig, (dest-orig),
                                          v,
                                          u, sign,
                                          enterFace, leaveFace,
                                          enterPoint, leavePoint,
                                          ue1, ue2, ul1, ul2,
                                          tEnter, tLeave);
            }
            else if (arguments.algorithm.rfind('1') != std::string::npos)  {
                result = RayTetraSegura1T(orig, (dest-orig),
                                          v,
                                          u, sign,
                                          enterFace, leaveFace,
                                          enterPoint, leavePoint,
                                          ue1, ue2, ul1, ul2,
                                          tEnter, tLeave);
            }
            else if (arguments.algorithm.rfind('2') != std::string::npos)  {
                result = RayTetraSegura2T(orig, (dest-orig),
                                          v,
                                          u, sign,
                                          enterFace, leaveFace,
                                          enterPoint, leavePoint,
                                          ue1, ue2, ul1, ul2,
                                          tEnter, tLeave);
            }
        }

        if (result)  {
            std::cout << enterFace << " " << leaveFace << std::endl;
            std::cout << (1-ue1-ue2) << " " << ue1 << " " << ue2 << "    " 
                      << (1-ul1-ul2) << " " << ul1 << " " << ul2 << "    "
                      << tEnter << " " << tLeave << std::endl;
            std::cout << enterPoint << "     " << leavePoint << std::endl;

            double vol = ((v[0]^v[1])*(v[2]-v[3])+(v[2]^v[3])*(v[0]-v[1]))/6.0;
            std::cout << "vol: " << vol << std::endl;
        }

        GlutInit(argc, argv);
        glutMainLoop();
    }

    return 0;
}



// Other functions /////////////////////////////////////////////////////////////

bool ParseArgs(int argc, char* argv[], ProgramArguments& arguments)
{
    // Some default values for options
    arguments.print = 3;
    arguments.displayResult = -1;
    
    const char* short_options = "m:als:p:d:h";

    bool stop = false;
    bool foundAlg = false;
    while (!stop)  {
        int c = getopt(argc, argv, short_options);

        if (c == -1)  {
            // All options are parsed
            stop = true;
        }
        else if (c == 'p')  {
            if (std::strcmp(optarg, "t") == 0)  {
                arguments.print = 1;
            }
            else if (std::strcmp(optarg, "r") == 0)  {
                arguments.print = 2;
            }
            else if (std::strcmp(optarg, "b") == 0)  {
                arguments.print = 3;
            }
            else  {
                std::cerr << "Invalid print argument." << std::endl;
                foundAlg = false;
                stop = 0;
            }
        }
        else if (((c == 'a') || (c == 's'))  &&  foundAlg)  {
            // Multiple algorithms given
            foundAlg = false;   // To signal the error
            stop = true;
        }
        else if (c == 'a')  {
            foundAlg = true;
            arguments.algorithm = "RayTetraHainesT";
        }
        else if (c == 's')  {
            foundAlg = true;
            if (std::strcmp(optarg, "0") == 0)  {
                arguments.algorithm = "RayTetraSegura0T";
            }
            else if (std::strcmp(optarg, "1") == 0)  {
                arguments.algorithm = "RayTetraSegura1T";
            }
            else if (std::strcmp(optarg, "2") == 0)  {
                arguments.algorithm = "RayTetraSegura2T";
            }
            else  {
                std::cerr << "Segura has only 3 variations!" << std::endl;
                foundAlg = false;
                stop = true;
            }
        }
        else if (c == 'd')  {
            arguments.displayResult = std::atoi(optarg);
            if (arguments.displayResult <= 0)  {
                std::cout << "The line to display must be > 0" << std::endl;
                return false;
            }
         }
        else if (c == 'h')  {
            stop = true;
            foundAlg = false;
        }
        else if (c == '?')  {
            // Unsupported option
            std::cerr << "Unsupported option '" << char(optopt) << "'" 
                      << std::endl;
            stop = true;
        }
        else  {
            std::cerr << "ERROR: getopt returned code " << c << std::endl;
            stop = 0;
        }
    }

    // Now process non-options (the two filenames and number of repetitions)
    if (foundAlg)  {
        if (optind < argc)  {
            arguments.inputFileName = argv[optind];
            ++optind;

            if (arguments.displayResult == -1)  {
                if (optind < argc)  {
                    arguments.outputFileName = argv[optind];
                    ++optind;

                    if (optind < argc)  {
                        arguments.repetitions = std::atoi(argv[optind]);
                    }
                    else  {
                        foundAlg = false;
                    }
                }
                else  {
                    foundAlg = false;
                }
            }
        }
        else  {
            foundAlg = false;
        }
    }

    if (!foundAlg)  {
        PrintHelp(argv[0]);
    }

    return foundAlg;
}



void PrintHelp(char* argv0)
{
    std::cerr << std::endl << "Usage: " << argv0 << " [algorithm] [options]" 
              << " <input file> <output file> <repetitions>" << std::endl;

    std::cerr << std::endl << "Algorithm must be one of:" << std::endl;
    std::cerr << "\t-a:   Use Haines' algorithm i." << std::endl;
    std::cerr << "\t-s i: Use Segura's algorithm i, i=0,1,2." << std::endl;
    
    std::cerr << std::endl << "Options may be:" << std::endl;
    std::cerr << "\t-h:   Print this help text and exit." << std::endl;
    std::cerr << "\t-p i: Controls what to print to the output file:" 
              << std::endl << "\t\ti=t: Print time," 
              << std::endl << "\t\ti=r: Print results," 
              << std::endl << "\t\ti=b: Print both (default)" << std::endl;
    std::cerr << "\t-d i: Only compute and display line i of the input file."
              << std::endl << "\t      In this case <output file> and "
              << "<repetitions> are ignored." << std::endl;
}



SceneBox::SceneBox(NpVector v[],
                   const NpVector& orig, const NpVector& dest,
                   const NpVector& enterPoint, const NpVector& leavePoint)
{
    NpVector all[8];
    for (unsigned int i = 0; i < 4; ++i)  {
        all[i] = v[i];
    }
    all[4] = orig;
    all[5] = dest;
    all[6] = enterPoint;
    all[7] = leavePoint;

    min = max = all[0];
    for (unsigned int i = 1; i < 8; ++i)  {
        if (all[i].x < min.x)
            min.x = all[i].x;
        else if (all[i].x > max.x)
            max.x = all[i].x;

        if (all[i].y < min.y)
            min.y = all[i].y;
        else if (all[i].y > max.y)
            max.y = all[i].y;

        if (all[i].z < min.z)
            min.z = all[i].z;
        else if (all[i].z > max.z)
            max.z = all[i].z;
    }

    center = 0.5 * (min+max);
}



void GlutInit(int argc, char* argv[])
{
    // Glut initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(glutVars.viewportWidth, glutVars.viewportHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("RayTetraTest");
    
    // Various parameters

    // Light
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat specular[]     = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_pos[]    = { 0.0f, 0.0f, 1.0f, 0.0f };
    
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Material of the model
    GLfloat mat_ambient[] =  { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat mat_diffuse[] =  { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
    glShadeModel(GL_FLAT);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Set up values for the camera, based on initial position of the model
    sceneBox = SceneBox(v, orig, dest, enterPoint, leavePoint);
    
    glutVars.fovy = 60;
    NpVector radiusVec = sceneBox.max - sceneBox.min;
    GLdouble radius = radiusVec.Measure() / 2.0;
    GLdouble d = 3*radius / tan(glutVars.fovy*M_PI/180.0);
    glutVars.zNear = d / 20.0;
    glutVars.zFar  = d * 10.0;
    
    glutVars.cameraPosX = sceneBox.center.x;
    glutVars.cameraPosY = sceneBox.center.y;
    glutVars.cameraPosZ = sceneBox.center.z + d;;
    glutVars.cameraAimX = sceneBox.center.x;
    glutVars.cameraAimY = sceneBox.center.y;
    glutVars.cameraAimZ = sceneBox.center.z;
    
    // Background color
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Antialiasing!
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

    // Glut callbacks
    glutDisplayFunc(GlutDisplay);
    glutReshapeFunc(GlutReshape);
    glutKeyboardFunc(GlutKeyboard);
    glutMouseFunc(GlutMouseClick);
    glutMotionFunc(GlutMouseMotion);
}



void GlutDisplay()
{
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up camera and viewing parameters
    glLoadIdentity();
    
    gluLookAt(glutVars.cameraPosX, glutVars.cameraPosY, glutVars.cameraPosZ,
              glutVars.cameraAimX, glutVars.cameraAimY, glutVars.cameraAimZ, 
              0.0, 1.0, 0.0);  // camera up vector
    
    // Apply pan/zoom
    glTranslatef(glutVars.transX, glutVars.transY, glutVars.transZ);
    
    // Apply rotation (relative to object's center)
    glTranslated(glutVars.cameraAimX, glutVars.cameraAimY, glutVars.cameraAimZ);
    glRotatef(glutVars.rotateX, 1.0, 0.0, 0.0);
    glRotatef(glutVars.rotateY, 0.0, 1.0, 0.0);
    glTranslated(-glutVars.cameraAimX, -glutVars.cameraAimY, 
                 -glutVars.cameraAimZ);
    
    DisplayScene();

    // Housekeeping
    glutSwapBuffers();
}



void GlutReshape(int w, int h)
{
    glutVars.viewportWidth  = w;
    glutVars.viewportHeight = h;
    
    glViewport(0, 0, GLsizei(w), GLsizei(h));
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(glutVars.fovy, float(w)/float(h), 
                   glutVars.zNear, glutVars.zFar);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



void GlutKeyboard(unsigned char key, int x, int y)
{
    switch (key)  {
        case 27:  {   // ESC key
            exit(0);
            break;
        }
        case 'h':
        case 'H':  {
            std::cout << "Keyboard commands:" << std::endl;
            std::cout << "\t'h'  this help text." << std::endl;
            std::cout << "\t'i'  display model at its initial position." 
                      << std::endl;
            std::cout << "\t'f'  fill faces on/off." << std::endl;
            std::cout << "\t's'  draw spheres on/off." << std::endl;
            break;
        }
        case 'i':
        case 'I':  {
            glutVars.rotateX = glutVars.rotateY = 0;
            glutVars.transX = glutVars.transY = glutVars.transZ = 0;
            glutPostRedisplay();
            break;
        }
        case 'f':
        case 'F':  {
            glutVars.polygonMode = 
                (glutVars.polygonMode==GL_FILL ? GL_LINE : GL_FILL);
            std::cout << "PolygonMode=" 
                      << (glutVars.polygonMode==GL_FILL ? "GL_FILL" : "GL_LINE")
                      << std::endl;
            glutPostRedisplay();
            break;
        }
        case 's':
        case 'S':  {
            glutVars.drawSpheres = !glutVars.drawSpheres;
            glutPostRedisplay();
            break;
        }
    }
}


void GlutMouseClick(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)  {
        glutVars.mouseInitX = x;
        glutVars.mouseInitY = y;
        
        if (button == GLUT_LEFT_BUTTON)  {
            // Rotate
            glutVars.mouseAction = GlutVars::Np_MA_ROTATE;
        }
        else if (button == GLUT_MIDDLE_BUTTON)  {
            // Zoom
            glutVars.mouseAction = GlutVars::Np_MA_PAN;
        }
        else if (button == GLUT_RIGHT_BUTTON)  {
            // Pan
            glutVars.mouseAction = GlutVars::Np_MA_ZOOM;
        }
    }
    else  {   // state == GLUT_UP
        // nothing to be done here; movement is handled in mouseMotion()
    }
}


void GlutMouseMotion(int x, int y)
{
    if (glutVars.mouseAction == GlutVars::Np_MA_ROTATE)  {
        glutVars.rotateX += float(y - glutVars.mouseInitY);
        glutVars.rotateY += float(x - glutVars.mouseInitX);
    }
    else if (glutVars.mouseAction == GlutVars::Np_MA_PAN)  {
        glutVars.transX += float(x - glutVars.mouseInitX) / 
                           float(glutVars.viewportWidth);
        glutVars.transY += float(glutVars.mouseInitY - y) / 
                           float(glutVars.viewportHeight);
    }
    else if (glutVars.mouseAction == GlutVars::Np_MA_ZOOM)  {
        glutVars.transZ += float(y - glutVars.mouseInitY) /
                           float(glutVars.viewportHeight);
    }
    
    glutVars.mouseInitX = x;
    glutVars.mouseInitY = y;
    
    glutPostRedisplay();
}



void DisplayScene()
{
    NpVector normal;

    // If requested, display the filled tetrahedron
    if (glutVars.polygonMode == GL_FILL)  {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3f(1.0, 1.0, 1.0);

        glBegin(GL_TRIANGLES);
        
        normal = (v[2]-v[3]) ^ (v[1]-v[3]);
        normal.MakeUnitVector();
        glNormal3d(normal.x, normal.y, normal.z);
        glVertex3d(v[3].x, v[3].y, v[3].z);
        glVertex3d(v[2].x, v[2].y, v[2].z);
        glVertex3d(v[1].x, v[1].y, v[1].z);

        normal = (v[3]-v[2]) ^ (v[0]-v[2]);
        normal.MakeUnitVector();
        glNormal3d(normal.x, normal.y, normal.z);
        glVertex3d(v[2].x, v[2].y, v[2].z);
        glVertex3d(v[3].x, v[3].y, v[3].z);
        glVertex3d(v[0].x, v[0].y, v[0].z);

        normal = (v[0]-v[1]) ^ (v[3]-v[1]);
        normal.MakeUnitVector();
        glNormal3d(normal.x, normal.y, normal.z);
        glVertex3d(v[1].x, v[1].y, v[1].z);
        glVertex3d(v[0].x, v[0].y, v[0].z);
        glVertex3d(v[3].x, v[3].y, v[3].z);

        normal = (v[1]-v[0]) ^ (v[2]-v[0]);
        normal.MakeUnitVector();
        glNormal3d(normal.x, normal.y, normal.z);
        glVertex3d(v[0].x, v[0].y, v[0].z);
        glVertex3d(v[1].x, v[1].y, v[1].z);
        glVertex3d(v[2].x, v[2].y, v[2].z);

        glEnd();

        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    // Display a wireframe model of the tetrahedron
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0, 0.0, 0.0);

    glBegin(GL_TRIANGLES);

    normal = (v[2]-v[3]) ^ (v[1]-v[3]);
    normal.MakeUnitVector();
    glNormal3d(normal.x, normal.y, normal.z);
    glVertex3d(v[3].x, v[3].y, v[3].z);
    glVertex3d(v[2].x, v[2].y, v[2].z);
    glVertex3d(v[1].x, v[1].y, v[1].z);

    normal = (v[3]-v[2]) ^ (v[0]-v[2]);
    normal.MakeUnitVector();
    glNormal3d(normal.x, normal.y, normal.z);
    glVertex3d(v[2].x, v[2].y, v[2].z);
    glVertex3d(v[3].x, v[3].y, v[3].z);
    glVertex3d(v[0].x, v[0].y, v[0].z);

    normal = (v[0]-v[1]) ^ (v[3]-v[1]);
    normal.MakeUnitVector();
    glNormal3d(normal.x, normal.y, normal.z);
    glVertex3d(v[1].x, v[1].y, v[1].z);
    glVertex3d(v[0].x, v[0].y, v[0].z);
    glVertex3d(v[3].x, v[3].y, v[3].z);

    normal = (v[1]-v[0]) ^ (v[2]-v[0]);
    normal.MakeUnitVector();
    glNormal3d(normal.x, normal.y, normal.z);
    glVertex3d(v[0].x, v[0].y, v[0].z);
    glVertex3d(v[1].x, v[1].y, v[1].z);
    glVertex3d(v[2].x, v[2].y, v[2].z);

    glEnd();

    // Display the line
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3d(orig.x, orig.y, orig.z);
    glVertex3d(dest.x, dest.y, dest.z);
    glEnd();

    // Display the vertices of the tetrahedron
    if (glutVars.drawSpheres)  {
        float pointSize = (sceneBox.max-sceneBox.min).Measure() / 100;

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glColor3f(1.0, 0.0, 0.0);
        DisplayPoint(v[0], pointSize);
        
        glColor3f(0.0, 1.0, 0.0);
        DisplayPoint(v[1], pointSize);

        glColor3f(0.0, 0.0, 1.0);
        DisplayPoint(v[2], pointSize);

        glColor3f(1.0, 0.0, 1.0);
        DisplayPoint(v[3], pointSize);

        // Display the endpoints of the line
        glColor3f(1.0, 0.0, 0.0);
        DisplayPoint(orig, pointSize);

        glColor3f(0.0, 1.0, 0.0);
        DisplayPoint(dest, pointSize);

        // Display the intersection points, if available
        if (result)  {
            glColor3f(1.0, 0.0, 0.0);
            DisplayPoint(enterPoint, pointSize*2);

            glColor3f(0.0, 1.0, 0.0);
            DisplayPoint(leavePoint, pointSize*2);
        }
    }
}



void DisplayPoint(const NpVector& v, float size)
{
    glPushMatrix();
    glTranslated(v.x, v.y, v.z);
    glutSolidSphere(size, 100, 100);
    glPopMatrix();
}

