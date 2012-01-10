CC = g++
CXXFLAGS = -Wall -O2 -Wno-unused-but-set-variable
LDLIBS = -L /usr/X11R6/lib  -lGL -lGLU -lglut  -lOpenCL


all: RandomRayTetra RayTetra RayTetraT Bench BenchSingleFill BenchT

RandomRayTetra: ../Common/NpVector.o RandomRayTetra.o

RayTetra: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHaines.o RayTetraMoller.o RayTetraSegura.o RayTetraSTP.o RayTetra.o gpuHandler.o  

RayTetraT: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHainesT.o  RayTetraSeguraT.o RayTetraT.o

Bench: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHaines.o RayTetraMoller.o RayTetraSegura.o RayTetraSTP.o gpuHandler.o

BenchSingleFill: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHaines.o RayTetraMoller.o RayTetraSegura.o RayTetraSTP.o gpuHandler.o

BenchT: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHainesT.o RayTetraSeguraT.o

clean:
	rm -f *.o ../Common/*.o RandomRayTetra RayTetra RayTetraT Bench BenchT BenchSingleFill

cleanbak:
	rm -f *~ ../Common/*~

cleanoutput:
	rm -f *.txt *.elf *.csv input* *.differences *.eps
