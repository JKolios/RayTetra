#include "RayTetraAlgorithms.hpp"
#include "../Common/NpVector.hpp"
#include "../Common/NpProgramTimer.hpp"
#include "../Common/NpCArrayAdapter.hpp"
//GPU init and cleanup functions
#include "gpuHandler.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <getopt.h>

#include <GL/glut.h>


// Struct for program arguments

struct ProgramArguments
{
    RayTetraAlgo algorithm;
    char* inputFileName;
    char* outputFileName;
    unsigned int repetitions;
    int print;
    int displayResult;
    bool gpuNeeded;
    const char *gpuAlgName;
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
        
        //If a GPU algorithm has been selected, initialise the GPU
        if(arguments.gpuNeeded) {
	  	
	 allocateInput(nTests);
	
	 // Converting input to raytetragpu's input format
	for(int i = 0;i<nTests;i++)
	{

		vert0[i].s[0] = v[i][0].x;
		vert0[i].s[1] = v[i][0].y;
		vert0[i].s[2] = v[i][0].z;	
		vert0[i].s[3] = 0.0;

		vert1[i].s[0] = v[i][1].x;
		vert1[i].s[1] = v[i][1].y;
		vert1[i].s[2] = v[i][1].z;	
		vert1[i].s[3] = 0.0;

		vert2[i].s[0] = v[i][2].x;
		vert2[i].s[1] = v[i][2].y;
		vert2[i].s[2] = v[i][2].z;	
		vert2[i].s[3] = 0.0;

		vert3[i].s[0] = v[i][3].x;
		vert3[i].s[1] = v[i][3].y;
		vert3[i].s[2] = v[i][3].z;	
		vert3[i].s[3] = 0.0;

		
		origin[i].s[0] = orig[i].x;
		origin[i].s[1] = orig[i].y; 
		origin[i].s[2] = orig[i].z; 
		origin[i].s[3] = 0.0;

		dir[i].s[0] = dest[i].x;
		dir[i].s[1] = dest[i].y;
		dir[i].s[2] = dest[i].z;
		dir[i].s[3] = 0.0;

	}
		
	initializeCL(arguments.gpuAlgName);	  
	}

        NpProgramTimer timer;
	timer.Start();
        for (unsigned int r = 0; r < arguments.repetitions; ++r)  {
          if(arguments.gpuNeeded){  
	    
	    runCLKernels();	    
	    for(int j=0;j < nTests;++j)
	    {
	      //cout << cartesian[j].s[0] << " " << cartesian[j].s[1]<<endl;
	      result[j] = true ? ((cartesian[j].s[0] != -1) &&(cartesian[j].s[1] != -1)) : false;
	      if(result[j])
	      {
		
		  enterFace[j] = cartesian[j].s[0];
		  leaveFace[j] = cartesian[j].s[1];
		  enterPoint[j] = NpVector(cartesian[j].s[2],cartesian[j].s[3],cartesian[j].s[4]);
		  leavePoint[j] = NpVector(cartesian[j].s[5],cartesian[j].s[6],cartesian[j].s[7]);
		  ue1[j] = barycentric[j].s[0];
		  ue2[j] = barycentric[j].s[1];
		  ul1[j] = barycentric[j].s[2];
		  ul2[j] = barycentric[j].s[3];
		  tEnter[j] = parametric[j].s[0];
		  tLeave[j] = parametric[j].s[1];
	      
	    }
	}
	    	    
	  }else{
	  for (int i = 0; i < nTests; ++i)  {
	    result[i] = arguments.algorithm(orig[i], (dest[i]-orig[i]),
                                                v[i],
                                                enterFace[i], leaveFace[i],
                                                enterPoint[i], leavePoint[i],
                                                ue1[i], ue2[i], ul1[i], ul2[i],
                                                tEnter[i], tLeave[i]);

	    
	  }
            }
        }
        timer.Stop();
        

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
	
	if(arguments.gpuNeeded) {
	  	
	 allocateInput(1);

	vert0[0].s[0] = v[0].x;
	vert0[0].s[1] = v[0].y;
	vert0[0].s[2] = v[0].z;	
	vert0[0].s[3] = 0.0;

	vert1[0].s[0] = v[1].x;
	vert1[0].s[1] = v[1].y;
	vert1[0].s[2] = v[1].z;	
	vert1[0].s[3] = 0.0;

	vert2[0].s[0] = v[2].x;
	vert2[0].s[1] = v[2].y;
	vert2[0].s[2] = v[2].z;	
	vert2[0].s[3] = 0.0;

	vert3[0].s[0] = v[3].x;
	vert3[0].s[1] = v[3].y;
	vert3[0].s[2] = v[3].z;	
	vert3[0].s[3] = 0.0;

		
	origin[0].s[0] = orig.x;
	origin[0].s[1] = orig.y; 
	origin[0].s[2] = orig.z; 
	origin[0].s[3] = 0.0;

	dir[0].s[0] = dest.x;
	dir[0].s[1] = dest.y;
	dir[0].s[2] = dest.z;
	dir[0].s[3] = 0.0;


		
	initializeCL(arguments.gpuAlgName);	  
	}
	
	if(arguments.gpuNeeded){  
	    
		  runCLKernels();	    
		  result = true ? ((cartesian[0].s[0] != -1) &&(cartesian[0].s[1] != -1)) : false;		
		  enterFace = cartesian[0].s[0];
		  leaveFace = cartesian[0].s[1];
		  enterPoint = NpVector(cartesian[0].s[2],cartesian[0].s[3],cartesian[0].s[4]);
		  leavePoint = NpVector(cartesian[0].s[5],cartesian[0].s[6],cartesian[0].s[7]);
		  ue1 = barycentric[0].s[0];
		  ue2 = barycentric[0].s[1];
		  ul1 = barycentric[0].s[2];
		  ul2 = barycentric[0].s[3];
		  tEnter = parametric[0].s[0];
		  tLeave = parametric[0].s[1];
		  
	      
	    }else{

        result = arguments.algorithm(orig, (dest-orig),
                                     v,
                                     enterFace, leaveFace,
                                     enterPoint, leavePoint,
                                     ue1, ue2, ul1, ul2,
                                     tEnter, tLeave);
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

    if(arguments.gpuNeeded){
      
      cleanupHost();
      cleanupCL();
      
    }
    return 0;
}



// Other functions /////////////////////////////////////////////////////////////

bool ParseArgs(int argc, char* argv[], ProgramArguments& arguments)
{
    // Some default values for options
    arguments.print = 3;
    arguments.displayResult = -1;
    arguments.gpuNeeded = false;
    
    const char* short_options = "m:a:s:t:g:p:d:h";

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
        else if (((c == 'm') || (c == 'a') || (c == 's')|| (c == 'u'))  &&
                 foundAlg)  {
            // Multiple algorithms given
            foundAlg = false;   // To signal the error
            stop = true;
        }
        else if (c == 'm')  {
            foundAlg = true;
            if (std::strcmp(optarg, "1") == 0)  {
                arguments.algorithm = RayTetraMoller1;
            }
            else if (std::strcmp(optarg, "2") == 0)  {
                arguments.algorithm = RayTetraMoller2;
            }
            else if (std::strcmp(optarg, "3") == 0)  {
                arguments.algorithm = RayTetraMoller3;
            }
            else  {
                std::cerr << "Moller has only 3 variations!" << std::endl;
                foundAlg = false;
                stop = true;
            }
        }
        else if (c == 'a')  {
            foundAlg = true;
            arguments.algorithm = RayTetraHaines;
        }
        else if (c == 's')  {
            foundAlg = true;
            if (std::strcmp(optarg, "0") == 0)  {
                arguments.algorithm = RayTetraSegura0;
            }
            else if (std::strcmp(optarg, "1") == 0)  {
                arguments.algorithm = RayTetraSegura1;
            }
            else if (std::strcmp(optarg, "2") == 0)  {
                arguments.algorithm = RayTetraSegura2;
            }
            else  {
                std::cerr << "Segura has only 3 variations!" << std::endl;
                foundAlg = false;
                stop = true;
            }
        }
        else if (c == 't')  {
            foundAlg = true;
            if (std::strcmp(optarg, "0") == 0)  {
                arguments.algorithm = RayTetraSTP0;
            }
            else if (std::strcmp(optarg, "1") == 0)  {
                arguments.algorithm = RayTetraSTP1;
            }
            else if (std::strcmp(optarg, "2") == 0)  {
                arguments.algorithm = RayTetraSTP2;
            }
            else  {
                std::cerr << "STP has only 3 variations!" << std::endl;
                foundAlg = false;
                stop = true;
            }
	}
         else if (c == 'g')  {
            foundAlg = true;
	    arguments.gpuNeeded = true;
            if (std::strcmp(optarg, "0") == 0)  {
                arguments.gpuAlgName = "RayTetraSegura0";
            }
            else if (std::strcmp(optarg, "1") == 0)  {
                arguments.gpuAlgName = "RayTetraSTP0";

            }
            else if (std::strcmp(optarg, "2") == 0)  {
                arguments.gpuAlgName = "RayTetraSTP1";
            }
            else if (std::strcmp(optarg, "3") == 0)  {
                arguments.gpuAlgName = "RayTetraSTP2";
            }
            else  {
                std::cerr << "There are only 4 variations of GPU Algorithms." << std::endl;
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
    std::cerr << "\t-m i: Use Moller's algorithm i, i=1,2,3." << std::endl;
    std::cerr << "\t-a:   Use Haines' algorithm." << std::endl;
    std::cerr << "\t-s i: Use Segura's algorithm i, i=0,1,2." << std::endl;
    std::cerr << "\t-t i: Use Segura's algorithm with STP calc i, i=0,1,2." << std::endl;
    std::cerr << "\t-g i: Use OpenCl GPU algorithm i, i=0,1,2,3" << std::endl;
    std::cerr << "\t\t 0=Segura0,1=STP0,2=STP1,3=STP2." << std::endl;
    
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

