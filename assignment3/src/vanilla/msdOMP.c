#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NR_BUCKETS 128
#define MAX_DIGITS 4

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

void sequential_radix_sort(int* array,int begining,int end,int digit) {	
		
	
	
	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;


	int count[NR_BUCKETS];
	int inserted[NR_BUCKETS];
	int start[NR_BUCKETS + 1];
	
	

	
	
	
	int *temp = malloc((end - begining) * sizeof(int));
	
	
	
	
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
	
	

	#pragma ivdep
	for(int i = 0; i < end - begining;i++)
		array[begining + i] = temp[i];

	free(temp);
	
	


	

	for(int i = 1; i < NR_BUCKETS + 1; i++) {
		sequential_radix_sort(array,begining + start[i-1] ,begining + start[i],digit + 1);
	}
	
	



}

void parallel_radix_sort(int* array,int begining,int end,int digit) {	
	
	

	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;

	int count[NR_BUCKETS];
	int inserted[NR_BUCKETS];
	int start[NR_BUCKETS];

	int *temp = malloc((end - begining) * sizeof(int));
	

	
	
	
	#pragma ivdep
	for(int i = 0; i < NR_BUCKETS; i++){ 
		count[i] = 0;	
		inserted[i] = 0;
		start[i] = 0;
	}
	

	
	
	
	
	int task_size = (end - begining)/NR_BUCKETS;
	
	for(int task = 0; task < NR_BUCKETS;task++) {
		

		#pragma omp task shared(array,count,task_size)
		{
		
		int task_start = task * task_size ;
		int task_end = (task + 1) * task_size ;
		

		
		if(task == NR_BUCKETS - 1)
			task_end = end;

		{
		
		int task_counts[NR_BUCKETS];	
		
		for(int i = 0; i < NR_BUCKETS; i++)
			task_counts[i] = 0;

		for(int i  = task_start; i < task_end;i++){
	
			
			task_counts[get_digit(array[i],digit)]++;
			
		}
		
		for(int i = 0; i < NR_BUCKETS;i++){
			#pragma omp atomic
			count[i] += task_counts[i];
		}
		}
		}
	}	
	

	#pragma omp taskwait
	

	for(int i = 1; i < NR_BUCKETS;i++){
		start[i] += start[i-1] + count[i-1];
	}
	
	
	
	

	for(int task = 0; task < NR_BUCKETS;task++) {
		
		#pragma task shared(array,temp,inserted,task_size)
		
		{
		int task_start = task * task_size ;
		int task_end = (task + 1) * task_size ;

		
		if(task == NR_BUCKETS - 1)
			task_end = end;

		for(int i = task_start;i < task_end;i++){
			
			
			int msdigit = get_digit(array[i],digit);
			
		
			int insert_pos;
				
			#pragma omp atomic capture
			{
			insert_pos = inserted[msdigit];
			inserted[msdigit]++;
			}
		
			
			temp[start[msdigit] + insert_pos] = array[i];

			} 
		}
	
	}
	
	#pragma omp taskwait
	
	
	
	
	#pragma ivdep
	for(int i = 0; i < end - begining;i++)
		array[begining + i] = temp[i];
	



	free(temp);
	
	
	
	
	

	for(int i = 0; i < NR_BUCKETS; i++) {
		#pragma omp task shared(array)
		
		sequential_radix_sort(array,start[i],begining + start[i] + count[i],digit + 1);

	}

	#pragma omp taskwait

	
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
	
	int size = atoi(argv[1]);
	int nr_tests = atoi(argv[2]);
	int k = 5;
	double times[nr_tests];

	int *test_array = malloc(size * sizeof(int));
	
	

	for(int test = 0; test < nr_tests; test++) {

	for(int i = 0; i < size; i++) 
		test_array[i] = abs(rand() % 2097152);
	
	double start = omp_get_wtime();	
		
	#pragma omp parallel
	#pragma omp single
	radix_sort(test_array,size);
	
	double end = omp_get_wtime();

	times[test] = end - start;

	}
	
	sort_double(times,nr_tests);

	double average = 0;
	
	for(int i = 0; i < 5; i++) 
		average += times[i];

	average /= k;
	printf("%f\n",average);
	
	return 0;
}
