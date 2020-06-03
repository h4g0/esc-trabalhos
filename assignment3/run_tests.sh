
make

scripts=("cpc_probes1.d" "cpc_probes2.d" "cpc_probes3.d" "cpc_probes4.d" "sched_probes.d" "sysinfo_probes.d" "vminfo.d" "plockstat.d")

sizes=(1000000 5000000 10000000) 
cores=8
repetitions=5
kbest=5

for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do
	
	echo tests/seq-$s-$size.txt
	./dtrace_scripts/$s '"seq"' -c "./bin/seq $size $repetitions $kbest" > tests/seq-$s-$size.txt 
	done
	
	echo tests/seq-custom_probes_seq-$size.txt
	./dtrace_scripts/custom_probes_seq.d  -c "./bin/seq $size $repetitions $kbest" > tests/seq-custom_probes_seq-$size.txt 
done	


for((c = 1; c <= $cores; c*=2))
do

export OMP_NUM_THREADS=$c

for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do

		
	echo tests/omp-$c-$s-$size.txt
	./dtrace_scripts/$s '"omp"' -c "./bin/omp $size $repetitions $kbest" > tests/omp-$c-$s-$size.txt 
	
	done
	
	echo tests/omp-$c-custom_probes_omp-$size.txt
	./dtrace_scripts/custom_probes_omp.d  -c "./bin/omp $size $repetitions $kbest" > tests/omp-$c-custom_probes_omp-$size.txt 
	
done	
done

