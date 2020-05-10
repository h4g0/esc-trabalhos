default: msdMPI.o msdOMP.o omp.o msdSEQ.o seq.o mpi.o
	/usr/local/bin/mpicc  -O3 -o bin/msd mpi.o msdMPI.o
	gcc -O3 -fopenmp -o bin/omp omp.o msdOMP.o
	gcc -O3 -fopenmp -o bin/seq seq.o msdSEQ.o
	rm *.o*

msdMPI.o:src/msdMPI.c
	gcc -c src/msdMPI.c

msdOMP.o:src/msdOMP.c
	gcc -c src/msdOMP.c

msdSEQ.o:src/msdSEQ.c
	gcc -c src/msdSEQ.c

omp.o :src/omp.d msdOMP.o
	dtrace -G -64 -s src/omp.d msdOMP.o

seq.o:src/seq.d msdSEQ.o
	dtrace -G -64 -s src/seq.d msdSEQ.o

mpi.o:src/mpi.d msdMPI.o
	dtrace -G -64 -s src/mpi.d msdMPI.o

clean: 
	rm bin/*
