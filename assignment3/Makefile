default: msdMPI.o msdOMP.o msdSEQ.o omp.o seq.o mpi.o
	/usr/local/bin/mpicc  -O3 -o bin/mpi mpi.o msdMPI.o
	gcc -O3 -o bin/omp omp.o msdOMP.o -fopenmp
	gcc -O3 -o bin/seq seq.o msdSEQ.o -fopenmp
	rm *.o*

no_probes: msdMPI_no_probes.o msdOMP_no_probes.o msdSEQ_no_probes.o
	/usr/local/bin/mpicc  -O3 -o bin/mpi msdMPI_no_probes.o
	gcc -O3 -fopenmp -o bin/omp msdOMP_no_probes.o
	gcc -O3 -fopenmp -o bin/seq msdSEQ_no_probes.o
	rm *.o*

msdSEQ_no_probes.o:src/msdSEQ_no_probes.c
	gcc -c src/msdSEQ_no_probes.c
	
msdOMP_no_probes.o:src/msdOMP_no_probes.c
	gcc -c src/msdOMP_no_probes.c

msdMPI_no_probes.o:src/msdMPI_no_probes.c
	/usr/local/bin/mpicc  -c src/msdMPI_no_probes.c

msdMPI.o:src/msdMPI.c
	/usr/local/bin/mpicc -c src/msdMPI.c -fopenmp

msdOMP.o:src/msdOMP.c
	gcc -c src/msdOMP.c -fopenmp

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
