
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
	
	./bin/seq $size $repetitions $kbest &
	pid=$!
	dtrace -p $pid -s dtrace_scripts/custom_probes_seq.d > tests/seq-custom_probes_seq-$size.txt

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
	
	./bin/omp $size $repetitions $kbest &
	pid=$!
	dtrace -p $pid -s dtrace_scripts/custom_probes_omp.d > tests/omp-custom_probes_omp-$c-$size.txt
	
done	
done

for((c = 1; c <= $cores; c*=2))
do


for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do

		
	echo tests/pt-$c-$s-$size.txt
	./dtrace_scripts/$s '"pt"' -c "./bin/pt $size $repetitions $kbest $c" > tests/pt-$c-$s-$size.txt 
	
	done
	
	echo tests/pt-$c-custom_probes_pt-$size.txt
	
	./bin/pt $size $repetitions $kbest $c &
	pid=$!
	dtrace -p $pid -s dtrace_scripts/custom_probes_pt.d > tests/omp-custom_probes_pt-$c-$size.txt
	
done	
done

for((c = 1; c <= $cores; c*=2))
do


for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do

		
	echo tests/cpp-$c-$s-$size.txt
	./dtrace_scripts/$s '"cpp"' -c "./bin/cpp $size $repetitions $kbest $c" > tests/cpp-$c-$s-$size.txt 
	
	done
	
	echo tests/cpp-$c-custom_probes_cpp-$size.txt
	
	./bin/cpp $size $repetitions $kbest $c &
	pid=$!
	dtrace -p $pid -s dtrace_scripts/custom_probes_cpp.d > tests/cpp-custom_probes_cpp-$c-$size.txt
	
done	
done

scripts=("cpc_probes1.d" "cpc_probes2.d" "cpc_probes3.d" "cpc_probes4.d" "sched_probes.d" "sysinfo_probes.d" "vminfo.d" "plockstat.d" "libmpi_probes.d")


for((c = 1; c <= $cores; c*=2))
do


for size in ${sizes[@]};
do
	for s in ${scripts[@]};
	do

		
	echo tests/mpi-$c-$s-$size.txt
	./dtrace_scripts/$s '"mpi"' -c "/usr/local/bin/mpirun -np $c ./bin/mpi $size $repetitions $kbest"
	##/usr/local/bin/mpirun -np $c dtrace -s dtrace_scripts/$s -c './bin/mpi $size $repetitions $kbest' >> tests/mpi-$c-$s-$size.txt
	
	done
	
	echo tests/omp-$c-custom_probes_omp-$size.txt
	
	/usr/local/bin/mpirun -np $c ./bin/mpi $size $repetitions $kbest &
	pid=$!
	dtrace -p $pid -s dtrace_scripts/custom_probes_mpi.d > tests/mpi-custom_probes_mpi-$c-$size.txt
done	
done
