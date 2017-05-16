# This Makefile requires GNU make, which is called gmake on Solaris systems
#
# 'make'        : builds and runs the project
# 'make clean'  : remove build products

PROG = bin/*
OBJS = obj/*

# This is the compiler to use.  Note that, for MPI, it is not GCC
CC = mpic++

# Library details
LDLIBS  = -L/usr/lib/openmpi/lib -L/usr/lib -lm -ljpeg -lmpi
# Include paths
INCLUDE = -ITools -I/usr/lib/openmpi/include

# Phony targets don't do auto incremental builds
.PHONY: clean Prac4 run

# The default target
all:    clean Prac4 run

# The other targets...
clean:
	rm -f -r $(PROG) $(OBJS)

Prac4:
	$(CC) $(INCLUDE) -c Prac4.cpp -o obj/Prac4.o
	$(CC) $(INCLUDE) -c Tools/JPEG.cpp -o obj/JPEG.o
	$(CC) $(INCLUDE) -c Tools/Timer.cpp -o obj/Timer.o
	$(CC) -o bin/Prac4 obj/Prac4.o obj/JPEG.o obj/Timer.o $(LDLIBS)

run:
	mpirun -np 5 bin/Prac4

