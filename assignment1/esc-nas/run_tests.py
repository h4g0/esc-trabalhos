import os



programs = ["mg","cg","is"]
sizes = ["B","C"]
repetitions = [10,1]
compilers = ['intelO1','intelO2','intelO3']
tests = 5

commands = list()
 
for compiler in compilers:
    for i in programs:
        for x in range(len(sizes)):
            command = 'qsub -v \"repetitions=' + str(repetitions[x]) + ',tests=' + str(tests)  + ',test_file=' + str(i) + '.' + str(sizes[x]) + '.' + str(compiler) + ',test_exe=' + str(i) + '.' + str(sizes[x])  + '.x\" ser_pbs/testSER_' + str(compiler) + '.pbs'
            
            print(command)
            commands.append(command)
print(len(commands))


for i in commands:
	os.system(i)
