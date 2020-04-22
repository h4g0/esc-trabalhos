import os

programs = ["is","mg","ep"]
sizes = ["C","D"]
repetitions = [10,1]
compilers = [('gnuO3','5.3.0'),('gnuO2','5.3.0'),('gnuO1','5.3.0'),
    ('gnuO3','6.1.0'),('gnuO2','6.1.0'),('gnuO1','6.1.0'),
    ('gnuO3','7.2.0'),('gnuO2','7.2.0'),('gnuO1','7.2.0'),
    ('intelO3','2019'),('intelO2','2019'),('intelO1','2019')]

tests = 5

commands = list()
 
for n in compilers:
    compiler,version = n
    for i in programs:
        for x in range(len(sizes)):
            command = 'qsub -v \"repetitions=' + str(repetitions[x]) + ',tests=' + str(tests)  + ',test_file=' + str(i) + '.' + str(sizes[x]) + '.' + str(compiler) + '.' + str(version) + ',test_exe=' + str(i) + '.' + str(sizes[x])  + '.x,' + 'compiler=' + compiler + ',version=' + version + '\" omp_pbs/omp.pbs'
            
            print(command)
            commands.append(command)
print(len(commands))


for i in commands:
   os.system(i)
