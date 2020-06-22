#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include<thread>
#include<mutex>
#include<bits/stdc++.h>
#include<sys/sdt.h>
#include "cpp.h"

#define NR_BUCKETS 128
#define MAX_DIGITS 4


int *digits_power;
unsigned thread_count;
std::mutex bucket_mutex[NR_BUCKETS];

int get_max_digit_size(int nr_buckets,int size) {
	int max_size = 1;
	for(int i = 0; i < size;i++)
		max_size *= nr_buckets;
	return max_size;
}

int get_digit(int number,int digit){
	
	return (number / digits_power[MAX_DIGITS - digit - 1]) % NR_BUCKETS;
	
}

void sequential_radix_sort(int* array,int begining,int end,int digit,int  bucket) {	
		

	CPP_START_SEQ_RADIX(end - begining,digit,bucket);

	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;

	int start[NR_BUCKETS + 1];
	
	std::vector<std::vector<int>> buckets;
	
	for(unsigned i = 0; i < NR_BUCKETS;i++)
		buckets.push_back(std::vector<int>());


	CPP_START_SORTING_INTO_BUCKETS(end - begining,digit,bucket);
	
	for(int i = begining;i < end;i++){
		
		int msdigit = get_digit(array[i],digit);
		
		buckets[msdigit].push_back(array[i]);

	}

	start[0] = 0;
	
	for(int i = 1; i < NR_BUCKETS + 1;i++)
		start[i] = start[i-1] + buckets[i-1].size();

	int pos_array = 0;
	
	CPP_START_COPY_TO_MAIN_ARRAY(end - begining,digit,bucket);

	for(int i = 0; i < NR_BUCKETS;i++)
		for(int j = 0; j < buckets[i].size();j++)
			array[begining + pos_array++] = buckets[i][j];
	
	CPP_FINISH_COPY_TO_MAIN_ARRAY(end - begining,digit,bucket);

	CPP_FINISH_SORTING_INTO_BUCKETS(end - begining,digit,bucket);

	for(int i = 1; i < NR_BUCKETS + 1; i++) {
		sequential_radix_sort(array,begining + start[i-1] ,begining + start[i],digit + 1,bucket);
	}

	CPP_FINISH_SEQ_RADIX(end - begining,digit,bucket);


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


void insert_into_buckets(int *array,int digit,int begining,int end,std::vector<std::vector<int>> *buckets){
		
		int element;
		for(int i = begining;i < end;i++){
		
			element = array[i];
			int msdigit = get_digit(element,digit);
		
			bucket_mutex[msdigit].lock();
			(*buckets)[msdigit].push_back(element);
			bucket_mutex[msdigit].unlock();
		}
		
}

void *n_sorts(void *args){
	int **arguments = (int**) args;

	int *array = arguments[0];
	int *count = arguments[1];
	int *start = arguments[2];
	int digit = *arguments[3];
	int t_start = *arguments[4];
	int t_end = *arguments[5];

	for(int i=t_start; i < t_end;i++)
		sequential_radix_sort(array,start[i],start[i] + count[i],digit + 1,i);

}

class insertion_into_buckets{
	
	public:
		void operator()(int *array,int digit,int begining,int end,std::vector<std::vector<int>> *buckets){
			insert_into_buckets(array,digit,begining,end,buckets);
		}

};


class recursive_call{
	

	public:
		void operator()(int *array,int *start,int *count,int digit,int begining,int end){
			for(int i = begining; i < end;i++)
				sequential_radix_sort(array,start[i],start[i] + count[i],digit,i);
		}

};


void parallel_radix_sort(int* array,int begining,int end,int digit) {	


	CPP_START_PAR_RADIX(end - begining,digit,0);


	if(end <= begining + 1){
		CPP_FINISH_PAR_RADIX(end - begining,digit,0);
		return ;
	}

	if(digit == MAX_DIGITS){
		CPP_FINISH_PAR_RADIX(end - begining,digit,0);
		return ;
	}
	
	std::vector<std::vector<int>> buckets;

	for(unsigned i = 0; i < NR_BUCKETS;i++)
		buckets.push_back(std::vector<int>());
				
	int start[NR_BUCKETS];
	int count[NR_BUCKETS];

	int task_start[thread_count];
	int task_end[thread_count];

	CPP_START_SORTING_INTO_BUCKETS(end - begining,digit,0);
	
		
	std::thread Insertions[thread_count];

	int task_size = (end - begining)/thread_count;
	for(int task = 0; task < thread_count;task++) {
		
		
		task_start[task] = task * task_size + begining;
		task_end[task] = (task + 1) * task_size + begining;
		
		if(task == thread_count - 1)
			task_end[task] = end;
		
		//std::thread t(insert_into_buckets,array,digit,task_start[task],task_end[task],buckets);
		Insertions[task] = std::thread(insertion_into_buckets(),array,digit,task_start[task],task_end[task],&buckets);
	
	}



	
	for(int i = 0; i < thread_count;i++)
		Insertions[i].join();
	
	for(int i = 0; i < NR_BUCKETS;i++)
		count[i] = buckets[i].size();
	

	start[0] = begining;
	for(int i = 1; i < NR_BUCKETS;i++){
		start[i] = start[i-1] + count[i-1];
	}
	
	CPP_START_COPY_TO_MAIN_ARRAY(end - begining,digit,0);

	int pos_array=begining,bucket_size;
	int *curr_bucket;
	for(int bucket = 0; bucket < NR_BUCKETS;bucket++){
		curr_bucket = &buckets[bucket][0];
		bucket_size = buckets[bucket].size();
		#pragma GCC IVDEP
		for(int i = 0; i < bucket_size;i++)
			array[pos_array++] = curr_bucket[i];
	}
	

	CPP_FINISH_COPY_TO_MAIN_ARRAY(end - begining,digit,0);

	CPP_FINISH_SORTING_INTO_BUCKETS(end - begining,digit,0);

	task_size = NR_BUCKETS/thread_count;


	CPP_START_WORKLOAD_DISTRIBUTION(end - begining,digit,0);

	work_allocation(thread_count,start,count,NR_BUCKETS);
	
	CPP_FINISH_WORKLOAD_DISTRIBUTION(end - begining,digit,0);


	std::thread RecursiveCalls[thread_count];

	for(int task = 0; task < thread_count;task++) {
		
		
		task_start[task] = task * task_size ;
		task_end[task] = (task + 1) * task_size ;
		
		if(task == thread_count - 1)
			task_end[task] = NR_BUCKETS;
		
		//std::thread t(insert_into_buckets,array,digit,task_start[task],task_end[task],buckets);
		RecursiveCalls[task] = std::thread(recursive_call(),array,start,count,digit + 1,task_start[task],task_end[task]);
	}

	for(int i = 0; i < thread_count;i++)
		RecursiveCalls[i].join();
	
	CPP_FINISH_PAR_RADIX(end - begining,digit,0);

	
}



void radix_sort(int *array,int size){
	
	parallel_radix_sort(array,0,size,1);
}

void s_radix_sort(int *array,int size){
	
	sequential_radix_sort(array,0,size,1,0);
}

void init(){
	
	digits_power = (int *) malloc(MAX_DIGITS * sizeof(int));

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
	int k = atoi(argv[3]);

	unsigned default_threads = 4;

	if(argc > 4)
		thread_count = atoi(argv[4]);	
	else
		thread_count = default_threads;
	

	double times[nr_tests];

	int *test_array = (int *) malloc(size * sizeof(int));
	
	for(int test = 0; test < nr_tests; test++) {

	for(int i = 0; i < size; i++) 
		test_array[i] = abs(rand() % 2097152);
	
	double start = omp_get_wtime();	
		
	#pragma omp parallel
	#pragma omp single
	radix_sort(test_array,size);
	
	double end = omp_get_wtime();

	times[test] = end - start;

	printf("sorted %d\n",test_sorted(test_array,size));
	
	/*for(unsigned i = 0; i < size;i++)
		printf("%d\n",test_array[i]);
	
	*/
	}
	sort_double(times,nr_tests);

	double average = 0;
	
	for(int i = 0; i < k; i++) 
		average += times[i];

	average /= k;
	printf("%f\n",average);
	
	return 0;
}
