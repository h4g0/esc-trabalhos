import os

##working mpi implementations
## gnu mpich2 eth - 1.5
## intel mpich2 eht - 1.5
##
programs = ["is","mg","ep"]
sizes = ["S","W","A","B","C","D"]
repetitions = [100,100,10,10,1,1]

compilers = [('gnu_eth','1.8.2')]
tests = 5
nr_cores = 32

commands = list()
 
for n in compilers:
    compiler,version = n
    for i in programs:
        for x in range(len(sizes)):
            command = 'qsub -v \"repetitions=' + str(repetitions[x]) + ',tests=' + str(tests)  + ',test_file=' + str(i) + '.' + str(sizes[x]) + '.' + str(compiler) + '.' + str(version) + ',test_exe=' + str(i) + '.' + str(sizes[x])  + '.' + str(nr_cores) + ',' + 'compiler=' + compiler + ',version=' + version + ',cores=' + str(nr_cores) + '\" mpi_pbs/system_mpi.pbs'
            
            print(command)
            commands.append(command)
print(len(commands))


for i in commands:
   os.system(i)
