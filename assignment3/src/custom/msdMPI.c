#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/sdt.h>

#define NR_BUCKETS 128
#define MAX_DIGITS 4

int nprocesses;
int myrank;

int *digits_power;

int get_max_digit_size(int nr_buckets,int size) {
	int max_size = 1;
	for(int i = 0; i < size;i++)
		max_size *= nr_buckets;
	return max_size;
}

int get_digit(int number,int digit){
	return (number / digits_power[MAX_DIGITS - digit - 1]) % NR_BUCKETS;
}


void sort2arrays(int *array,int *array2,int size,int reverse) {
	
	if(reverse) {
	
		for(int i = 0; i < size;i++) {
		for(int v  = i; v < size;v++) {
			if(array[v] > array[i]){
			array[i]  += array[v];
			array[v]   = array[i] - array[v];
			array[i]  -= array[v];
			array2[i] += array2[v];
			array2[v]  = array2[i] - array2[v];
			array2[i] -= array2[v];

			}
		
		}	
	
		}
	
	}
	else {
	
		for(int i = 0; i < size;i++) {
		for(int v  = i; v < size;v++) {
			if(array[v] < array[i]){
			array[i]  += array[v];
			array[v]   = array[i] - array[v];
			array[i]  -= array[v];
			array2[i] += array2[v];
			array2[v]  = array2[i] - array2[v];
			array2[i] -= array2[v];

			}
		
		}	
	
		}
	
	
	
	}

}

void work_allocation(int nprocesses,int *start,int *count,int size) {
	
	int remaining = size;
	
	int reverse = 1;
	

	while(remaining > 0) {

		if(remaining > nprocesses) {
			sort2arrays(count + (size - remaining)
					,start + (size - remaining)
					,nprocesses,reverse);

			reverse    = 1 - reverse;

			remaining -= nprocesses;
		}

		else {
			sort2arrays(count + (size - remaining)
					,start + (size - remaining)
					,remaining,reverse);


			remaining = 0;
		}


	}

	return ;

}

void sequential_radix_sort(int* array,int begining,int end,int digit) {	
		
	DTRACE_PROBE2(mpi,start_seq_radix,end - begining,digit);
	
	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;


	int count[NR_BUCKETS];
	int inserted[NR_BUCKETS];
	int start[NR_BUCKETS + 1];
	
	DTRACE_PROBE2(mpi,start_sorting_into_buckets,end - begining,digit);

	
	DTRACE_PROBE2(mpi,start_allocate_temp_array,end - begining,digit);
	
	int *temp = malloc((end - begining) * sizeof(int));
	
	DTRACE_PROBE2(mpi,finish_allocate_temp_array,end - begining,digit);
	
	
	#pragma ivdep
	for(int i = 0; i < NR_BUCKETS; i++){ 
		count[i] = 0;	
		inserted[i] = 0;
		start[i] = 0;
	}
	
	start[NR_BUCKETS] = 0;
	
	for(int i  = begining; i < end;i++){
		count[get_digit(array[i],digit)]++;
	
	}	


	for(int i = 1; i < NR_BUCKETS + 1;i++){
		start[i] += start[i-1] + count[i-1];
	}

	
	for(int i = begining;i < end;i++){
		int msdigit = get_digit(array[i],digit);
		temp[start[msdigit] + inserted[msdigit]++] = array[i];

	}
	
	DTRACE_PROBE2(mpi,start_copy_to_main_array,end - begining,digit);

	#pragma ivdep
	for(int i = 0; i < end - begining;i++)
		array[begining + i] = temp[i];

	free(temp);
	
	DTRACE_PROBE2(mpi,finish_copy_to_main_array,end - begining,digit);


	DTRACE_PROBE2(mpi,finish_sorting_into_buckets,end - begining,digit);

	for(int i = 1; i < NR_BUCKETS + 1; i++) {
		sequential_radix_sort(array,begining + start[i-1] ,begining + start[i],digit + 1);
	}
	
	DTRACE_PROBE2(mpi,finish_seq_radix,end - begining,digit);



}

void parallel_radix_sort(int* array,int begining,int end,int digit) {	
		
	DTRACE_PROBE2(mpi,start_par_radix,end - begining,digit);

	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;

	int count[NR_BUCKETS];
	int inserted[NR_BUCKETS];
	int start[NR_BUCKETS];

	int *temp = malloc((end - begining) * sizeof(int));
	

	DTRACE_PROBE2(mpi,start_sorting_into_buckets,end - begining,digit);

	#pragma vectorize always
	for(int i = 0; i < NR_BUCKETS; i++){ 
		count[i] = 0;	
		inserted[i] = 0;
		start[i] = 0;
	}
	
	DTRACE_PROBE2(mpi,start_count_digits,end - begining,digit);


	for(int i  = begining; i < end;i++){
	

		count[get_digit(array[i],digit)]++;
	
	}	

	for(int i = 1; i < NR_BUCKETS;i++){
		start[i] += start[i-1] + count[i-1];
	}

	DTRACE_PROBE2(mpi,finish_count_digits,end - begining,digit);

	DTRACE_PROBE2(mpi,start_insert_into_buckets,end - begining,digit);

	for(int i = begining;i < end;i++){
		int msdigit = get_digit(array[i],digit);
		temp[start[msdigit] + inserted[msdigit]++] = array[i];

	}
	
	DTRACE_PROBE2(mpi,finish_insert_into_buckets,end - begining,digit);
	
	DTRACE_PROBE2(mpi,start_copy_to_main_array,end - begining,digit);
	#pragma vectorize always
	for(int i = 0; i < end - begining;i++)
		array[begining + i] = temp[i];



	free(temp);

	DTRACE_PROBE2(mpi,finish_copy_to_main_array,end - begining,digit);

	DTRACE_PROBE2(mpi,finish_sorting_into_buckets,end - begining,digit);

	int scatters = NR_BUCKETS/nprocesses;
	
	DTRACE_PROBE2(mpi,start_workload_distribution,end - begining,digit);

	work_allocation(nprocesses,start,count,NR_BUCKETS);

	DTRACE_PROBE2(mpi,finish_workload_distribution,end - begining,digit);


	DTRACE_PROBE2(mpi,start_scatter_workload,end - begining,digit);


	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(start,NR_BUCKETS,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(count,NR_BUCKETS,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);


	
	for(int i = 0; i < scatters;i++) {
		
		MPI_Barrier(MPI_COMM_WORLD);	
		MPI_Scatterv(array,count+i*nprocesses,start+i*nprocesses,
				MPI_INT,array + start[i*nprocesses],count[i*nprocesses],MPI_INT,0,MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);	
		

		
	}

	DTRACE_PROBE2(mpi,finish_scatter_workload,end - begining,digit);

	for(int i = 0; i < scatters;i++) 
		sequential_radix_sort(array + start[i*nprocesses],0,count[i*nprocesses],2);
	
	DTRACE_PROBE2(mpi,start_gather_workload,end - begining,digit);

	for(int i = 0; i < scatters;i++) {

		MPI_Barrier(MPI_COMM_WORLD);	
		MPI_Gatherv(array + start[i*nprocesses],count[i*nprocesses],MPI_INT,
			array,count + i*nprocesses,start + i*nprocesses,
			MPI_INT,0,MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);	
		
	}
	
	DTRACE_PROBE2(mpi,start_gather_workload,end - begining,digit);

	DTRACE_PROBE2(mpi,finish_par_radix,end - begining,digit);


}



void radix_sort(int *array,int size){
	
	parallel_radix_sort(array,0,size,1);
}

void s_radix_sort(int *array,int size){
	
	sequential_radix_sort(array,0,size,2);
}

void init(){
	
	digits_power = malloc(MAX_DIGITS * sizeof(int));

	int initial_power = 1;
	for(int i = 0; i < MAX_DIGITS;i++) {
		digits_power[i] = initial_power;
		initial_power *= NR_BUCKETS;
	}

	return ;


}

int test_sorted(int *test_array,int size) {
	
	for(int i = 1; i < size; i++) {
		if(test_array[i-1] > test_array[i]){
			printf("%d|%d:%d|%d\n",i-1,test_array[i-1],i,test_array[i]);
			return 1;

		}
	}
	return 0;
}

void sort_double(double *array,int size) {
	
	for(int i = 0; i < size;i++) {
		for(int v  = i; v < size;v++) {
			if(array[v] < array[i]){
				array[i]  += array[v];
				array[v]  = array[i] - array[v];
				array[i] -= array[v];
			}
		
		}
	
	}

}

int main(int argc,char **argv){
	
	init();
	
	
	int nr_tests = atoi(argv[1]);
	int size = atoi(argv[2]);

	MPI_Status status;

	MPI_Init(&argc,&argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &nprocesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	
	if(myrank == 0) {
	
	
	int *test_array = malloc(size * sizeof(int));
	
	
		
	
	double *times = malloc(nr_tests * sizeof(double));
	
	for(int i = 0; i < nr_tests;i++) {
		
		for(int v = 0; v < size; v++) 
			test_array[v] = abs(rand() % 2097152);

		double start = MPI_Wtime();
		
		radix_sort(test_array,size);
		
		double end = MPI_Wtime();

		times[i] = end - start;
		
		
	}

	sort_double(times,nr_tests);

	double k_best = 0;
	for(int i = 0;i < 5;i++) 
		k_best += times[i];
	
	k_best = k_best/5;

	printf("%f\n",k_best);


	}

	if(myrank > 0) {
		for(int i = 0;i < nr_tests;i++) {			
			int start[NR_BUCKETS];
			int count[NR_BUCKETS];
			int scatters = NR_BUCKETS/nprocesses;
			int **arrays = malloc(scatters * sizeof(int*));
			

			DTRACE_PROBE1(mpi,start_receive_workload,myrank);

			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(start,NR_BUCKETS,MPI_INT,0,MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			

			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(count,NR_BUCKETS,MPI_INT,0,MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);	
			
			

			for(int i = 0; i < scatters;i++) {	
			
				arrays[i] = malloc(count[myrank + i*nprocesses] * sizeof(int));


				MPI_Barrier(MPI_COMM_WORLD);	
				MPI_Scatterv(NULL,count + i*nprocesses,start + i*nprocesses,
					MPI_INT,arrays[i],count[myrank + i*nprocesses],MPI_INT,0,MPI_COMM_WORLD);
				MPI_Barrier(MPI_COMM_WORLD);	
			
			}
		
			DTRACE_PROBE1(mpi,finish_receive_workload,myrank);

			for(int i = 0; i < scatters;i++) 
				s_radix_sort(arrays[i],count[myrank + i*nprocesses]);
					
			DTRACE_PROBE1(mpi,start_send_workload,myrank);

			for(int i = 0; i < scatters;i++) {
				
			MPI_Barrier(MPI_COMM_WORLD);	
			MPI_Gatherv(arrays[i],count[myrank + i*nprocesses],MPI_INT,
				NULL,count + i*nprocesses,start + i*nprocesses,
				MPI_INT,0,MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);	
			
			}

			DTRACE_PROBE1(mpi,finish_send_workload,myrank);

		}

	}

	
	
	MPI_Finalize();

	return 0;
}
