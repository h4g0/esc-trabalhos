compiler = g++
options = -ggdb -O3
default: naive/naive.c sd/main.o sd/sort.o sd/sort.h
	export SORT=true
	$(compiler) $(options) -o bin/sort  sd/main.o sd/sort.o sd/sort.h 
	$(compiler) $(options) -o bin/naive naive/naive.c
	rm sd/*.o

sd/main.o: sd/main.c sd/sort.h
	g++ -c sd/main.c
	mv main.o sd
sd/sort.o: sd/sort.c sd/sort.h 
	g++ -c sd/sort.c
	mv sort.o sd
clean: 
	rm bin/*
