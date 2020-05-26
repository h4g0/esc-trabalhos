default: msdOMP.o msdSEQ.o msdPT.o msdCPP.o
	gcc -O3 -fopenmp -o bin/omp msdOMP.o
	gcc -O3 -fopenmp -o bin/seq msdSEQ.o
	gcc -g -Wall -O3 -fopenmp -o bin/pt msdPT.o -lpthread
	g++ -g -Wall -O3 -fopenmp -o bin/cpp msdCPP.o -lpthread

	rm *.o*

msdPT.o:src/msdPT.c
	gcc -c src/msdPT.c

msdOMP.o:src/msdOMP.c
	gcc -c src/msdOMP.c

msdSEQ.o:src/msdSEQ.c
	gcc -c src/msdSEQ.c

msdCPP.o:src/msdCPP.cpp
	g++ -std=c++11 -c src/msdCPP.cpp

clean: 
	rm bin/*
