import os



programs = ["mg","ep"]
sizes = ["S","W","A","B","C","D"]
repetitions = [1000,1000,100,100,10,1]
compilers = ['gnuO3']
tests = 5

commands = list()
 
for compiler in compilers:
    for i in programs:
        for x in range(len(sizes)):
            command = 'qsub -v \"repetitions=' + str(repetitions[x]) + ',tests=' + str(tests)  + ',test_file=' + str(i) + '.' + str(sizes[x]) + '.' + str(compiler) + ',test_exe=' + str(i) + '.' + str(sizes[x])  + '.x\" system_pbs/test_systemOMP.pbs'
            
            print(command)
            commands.append(command)
print(len(commands))


for i in commands:
	os.system(i)
