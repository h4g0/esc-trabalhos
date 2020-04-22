import os

##working mpi implementations
## gnu mpich2 eth - 1.5
## intel mpich2 eht - 1.5
##
programs = ["is","mg","ep"]
sizes = ["C","D"]
repetitions = [1,1]
compilers = [('gnu_mpich_eth','1.2.7'),('gnu_eth','1.8.2'),('gnu_mpich2_eth','1.5'),('gnu_eth','1.6.3'),
    ('intel_mpich2_eth','1.5'),('intel_mpich_eth','1.2.7'),('intel_eth','1.8.2'),('intel_eth','1.6.3')]

##compilers = [('gnu_mpich_eth','1.2.7')]
tests = 5
nr_cores = 32

commands = list()
 
for n in compilers:
    compiler,version = n
    for i in programs:
        for x in range(len(sizes)):
            command = 'qsub -v \"repetitions=' + str(repetitions[x]) + ',tests=' + str(tests)  + ',test_file=' + str(i) + '.' + str(sizes[x]) + '.' + str(compiler) + '.' + str(version) + ',test_exe=' + str(i) + '.' + str(sizes[x])  + '.' + str(nr_cores) + ',' + 'compiler=' + compiler + ',version=' + version + ',cores=' + str(nr_cores) + '\" mpi_pbs/mpi.pbs'
            
            print(command)
            commands.append(command)
print(len(commands))

for i in commands:
   os.system(i)
