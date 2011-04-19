CC = g++
CXXFLAGS = -Wall -O2
LDLIBS = -L $AMDAPPSDKROOT/lib/x86_64 -lOpenCL


all:  raytetra 

raytetra: RayTetraGPU

dumpBinary:DumpBinary

clean:
	rm -f *.o RayTetraGPU

