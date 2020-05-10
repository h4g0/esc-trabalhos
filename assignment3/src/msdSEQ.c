#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sdt.h>

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
		
	DTRACE_PROBE2(seq,start_seq_radix,end - begining,digit);
	
	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;


	int count[NR_BUCKETS];
	int inserted[NR_BUCKETS];
	int start[NR_BUCKETS + 1];
	
	DTRACE_PROBE2(seq,start_sorting_into_buckets,end - begining,digit);

	
	DTRACE_PROBE2(seq,start_allocate_temp_array,end - begining,digit);
	
	int *temp = malloc((end - begining) * sizeof(int));
	
	DTRACE_PROBE2(seq,finish_allocate_temp_array,end - begining,digit);
	
	
	#pragma ivdep
	for(int i = 0; i < NR_BUCKETS; i++){ 
		count[i] = 0;	
		inserted[i] = 0;
		start[i] = 0;
	}
	
	start[NR_BUCKETS] = 0;
	
	for(int i  = begining; i < end;i++){
		
		DTRACE_PROBE3(seq,start_get_digit,digit,MAX_DIGITS,NR_BUCKETS);
		
		count[get_digit(array[i],digit)]++;
		
		DTRACE_PROBE3(seq,finish_get_digit,digit,MAX_DIGITS,NR_BUCKETS);
	
	}	


	for(int i = 1; i < NR_BUCKETS + 1;i++){
		start[i] += start[i-1] + count[i-1];
	}

	
	for(int i = begining;i < end;i++){
		
		DTRACE_PROBE3(seq,start_get_digit,digit,MAX_DIGITS,NR_BUCKETS);
		
		int msdigit = get_digit(array[i],digit);
		temp[start[msdigit] + inserted[msdigit]++] = array[i];
		
		DTRACE_PROBE3(seq,finish_get_digit,digit,MAX_DIGITS,NR_BUCKETS);

	}
	
	DTRACE_PROBE2(seq,start_copy_to_main_array,end - begining,digit);

	#pragma ivdep
	for(int i = 0; i < end - begining;i++)
		array[begining + i] = temp[i];

	free(temp);
	
	DTRACE_PROBE2(seq,finish_copy_to_main_array,end - begining,digit);


	DTRACE_PROBE2(seq,finish_sorting_into_buckets,end - begining,digit);

	for(int i = 1; i < NR_BUCKETS + 1; i++) {
		sequential_radix_sort(array,begining + start[i-1] ,begining + start[i],digit + 1);
	}
	
	DTRACE_PROBE2(seq,finish_seq_radix,end - begining,digit);



}


void radix_sort(int *array,int size){
	
	sequential_radix_sort(array,0,size,1);
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
