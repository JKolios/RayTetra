CC = g++
CXXFLAGS = -Wall -O2
LDLIBS = -L/usr/X11R6/lib -lGL -lGLU -lglut


all: RandomRayTetra RayTetra RayTetraT Bench BenchT

RandomRayTetra: ../Common/NpVector.o RandomRayTetra.o

RayTetra: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHaines.o RayTetraMoller.o RayTetraSegura.o RayTetraSTP.o RayTetra.o 

RayTetraT: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHainesT.o  RayTetraSeguraT.o RayTetraT.o

Bench: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHaines.o RayTetraMoller.o RayTetraSegura.o RayTetraSTP.o

BenchT: ../Common/NpProgramTimer.o ../Common/NpVector.o RayTetraHainesT.o RayTetraSeguraT.o

clean:
	rm -f *.o ../Common/*.o RandomRayTetra RayTetra RayTetraT Bench BenchT

cleanbak:
	rm -f *~ ../Common/*~
