metrics=( "cpu-cycles" "cpu-clock" "L1-dcache-load-misses" "L1-dcache-loads" "L1-dcache-store-misses" "instructions" "cache-misses" "branch-misses" "cpu-migrations" "branches" "L1-dcache-loads" "L1-dcache-load-misses" "L1-dcache-stores" "L1-dcache-store-misses" "L1-icache-loads" "LLC-loads" "LLC-load-misses" "LLC-store-misses" "dTLB-load-misses" "iTLB-load-misses" "branch-loads" "branch-load-misses")


make

for metric in ${metrics[@]};
do
	echo $metric
	
	perf record -e $metric ./bin/naive 1 2048
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event"

	perf record -e $metric ./bin/naive 2 2048
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event"

	perf record -e $metric ./bin/naive 1 512
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event" 

	perf record -e $metric ./bin/naive 2 512
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event" 

	perf record -e $metric ./bin/naive 1 128
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event" 

	perf record -e $metric ./bin/naive 2 128
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event"

	perf record -e $metric ./bin/naive 1 32
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event"

	perf record -e $metric ./bin/naive 2 32
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event"
done
