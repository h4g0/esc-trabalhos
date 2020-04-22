import os

##working mpi implementations
## gnu mpich2 eth - 1.5
## intel mpich2 eht - 1.5
##
programs = ["sp-mz","bt-mz"]
sizes = ["S","W","A","B","C"]
repetitions = [10,1,1,1,1]
compilers = [('gnu_eth','1.8.4')]

tests = 5
nr_cores = 2
cores_machine = 32

commands = list()
 
for n in compilers:
    compiler,version = n
    for i in programs:
        for x in range(len(sizes)):
            command = 'qsub -v \"repetitions=' + str(repetitions[x]) + ',tests=' + str(tests)  + ',test_file=' + str(i) + '.' + str(sizes[x]) + '.' + str(compiler) + '.' + str(version) + ',test_exe=' + str(i) + '.' + str(sizes[x])  + '.' + str(nr_cores) + ',' + 'compiler=' + compiler + ',version=' + version + ',cores=' + str(nr_cores) + ",cores_machine=" + str(cores_machine) + '\" hybrid_pbs/system_hybrid.pbs'
            
            print(command)
            commands.append(command)
print(len(commands))

for i in commands:
   os.system(i)
