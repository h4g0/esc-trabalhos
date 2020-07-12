metrics=( "cpu-cycles" "cpu-clock" "L1-dcache-load-misses" "cache-references" "cache-misses" "LLC-loads" "LLC-load-misses" "dTLB-load-misses" "branches" "branch-misses")

period=100000
make

for metric in ${metrics[@]};
do
	echo $metric
	
	perf record -e $metric -c $period ./bin/naive 1 2048
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event"

	perf record -e $metric -c $period ./bin/naive 2 2048
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event"

	perf record -e $metric -c $period ./bin/naive 1 512
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event" 

	perf record -e $metric -c $period ./bin/naive 2 512
	perf report --stdio --show-nr-samples --dsos=naive | grep "Event" 

done
