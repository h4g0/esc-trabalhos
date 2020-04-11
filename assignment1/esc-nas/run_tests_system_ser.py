import os

programs = ["is","mg","cg"]
sizes = ["S","W","A","B","C","D"]
repetitions = [100,10,10,1,1,1]
compilers = [('gnuO3','5.3.0')]

tests = 5

commands = list()
 
for n in compilers:
    compiler,version = n
    for i in programs:
        for x in range(len(sizes)):
            command = 'qsub -v \"repetitions=' + str(repetitions[x]) + ',tests=' + str(tests)  + ',test_file=' + str(i) + '.' + str(sizes[x]) + '.' + str(compiler) + '.' + str(version) + ',test_exe=' + str(i) + '.' + str(sizes[x])  + '.x,' + 'compiler=' + compiler + ',version=' + version + '\" ser_pbs/system_ser.pbs'
            
            print(command)
            commands.append(command)
print(len(commands))


for i in commands:
   os.system(i)
