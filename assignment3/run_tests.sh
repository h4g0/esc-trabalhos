
make

scripts=("cpc_probes1.d" "cpc_probes2.d" "cpc_probes3.d" "cpc_probes4.d" "custom_probes_omp.d" "custom_probes_seq.d" "plockstat.d" "sched_probes.d" "sysinfo_probes.d" "vminfo.d")

sizes=(1000000 10000000 100000000) 
cores=8

for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do
	
	./dtrace_scripts/$s '"seq"' > tests/$seq_$s_$size.txt &
	
	pid=$!
	
	./bin/seq $size 10 10
	
	kill -s SIGINT $pid

	done
	
	./dtrace_scripts/plockstat '"seq"' -c "./bin/seq $size 10 10" > tests/seq_plockstat_$size.txt
done	

for((c = 1; c <= $cores; c*=2))
do

export OMP_NUM_THREADS=$c

for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do

		
	./dtrace_scripts/$s '"omp"' > tests/$omp_$c_$s_$size.txt &
	
	pid=$!
	
	./bin/omp $size 1 1
	
	kill -s SIGINT $pid

	done
	
done	
done

