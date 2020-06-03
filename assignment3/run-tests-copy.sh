
make

scripts=("cpc_probes1.d" "cpc_probes2.d" "cpc_probes3.d" "cpc_probes4.d" "sched_probes.d" "sysinfo_probes.d" "vminfo.d")

sizes=(1000000 5000000 10000000) 
cores=8
repetitions=5
kbest=5

for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do
	
	echo $s
	echo tests/seq-$s-$size.txt
	./dtrace_scripts/$s '"seq"' > tests/seq-$s-$size.txt &
	pid=$!
	
	sleep 2

	./bin/seq $size $repetitions $kbest
	
	kill -2 $pid

	done
	
	./dtrace_scripts/plockstat.d '"seq"' -c "./bin/seq $size $repetitions $kbest" > tests/seq_plockstat_$size.txt
	
	./dtrace_scripts/custom_probes_seq > tests/custom_seq_$size.txt &
	
	pid=$!
	
	sleep 2
	
	./bin/seq $size $repetitions $kbest
	
	kill -s SIGINT $pid

done	


for((c = 1; c <= $cores; c*=2))
do

export OMP_NUM_THREADS=$c

for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do

		
	./dtrace_scripts/$s '"omp"' > tests/omp-$c-$s-$size.txt &
	
	pid=$!
	
	./bin/omp $size $repetitions $kbest
	
	kill -s SIGINT $pid

	done
	
	./dtrace_scripts/plockstat.d '"omp"' -c "./bin/omp $size $repetitions $kbest" > tests/omp_plockstat_$size.txt
	
	./dtrace_scripts/custom_probes_omp > tests/custom-omp-$c-$size.txt &
	
	pid=$!
	
	./bin/omp $size $repetitions $kbest
	
	kill -s SIGINT $pid
	
done	
done

