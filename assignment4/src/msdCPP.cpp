#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include<thread>

#define NR_BUCKETS 128
#define MAX_DIGITS 4

int *digits_power;
unsigned thread_count;
pthread_mutex_t mutex_digit[NR_BUCKETS];

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

	int start[NR_BUCKETS + 1];
	
	std::vector<std::vector<int>> buckets;
	
	for(unsigned i = 0; i < NR_BUCKETS;i++)
		buckets.push_back(std::vector<int>());
			
	for(int i = begining;i < end;i++){
		
		int msdigit = get_digit(array[i],digit);
		
		buckets[msdigit].push_back(array[i]);

	}

	start[0] = 0;
	
	for(int i = 1; i < NR_BUCKETS + 1;i++)
		start[i] = start[i-1] + buckets[i-1].size();

	int pos_array = 0;
	
	for(int i = 0; i < NR_BUCKETS;i++)
		for(int j = 0; j < buckets[i].size();j++)
			array[begining + pos_array++] = buckets[i][j];
	
	
	for(int i = 1; i < NR_BUCKETS + 1; i++) {
		sequential_radix_sort(array,begining + start[i-1] ,begining + start[i],digit + 1);
	}

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


void insert_into_buckets(int *array,int digit,int begining,int end,std::vector<std::vector<int>> buckets){
		
		for(int i = begining;i < end;i++){
		
			int msdigit = get_digit(array[i],digit);
		
			buckets[msdigit].push_back(array[i]);

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

	//printf("%d %d\n",t_start,t_end);
	for(int i=t_start; i < t_end;i++)
		sequential_radix_sort(array,start[i],start[i] + count[i],digit + 1);

}

class insertion_object{
	
	private:
		int *array;
		int digit;
		int begining;
		int end;
		std::vector<std::vector<int>> buckets;

	public:
		insertion_object(int *array,int digit,int begining,int end,std::vector<std::vector<int>> buckets){
				this->array = array;
				this->digit = digit;
				this->begining = begining;
				this->end = end;
				this->buckets = buckets;
		}
		void operator()(){
			insert_into_buckets(this->array,this->digit,this->begining,this->end,this->buckets);
		}

};

void parallel_radix_sort(int* array,int begining,int end,int digit) {	
	
	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;

	std::vector<std::vector<int>> buckets;

	for(unsigned i = 0; i < NR_BUCKETS;i++)
		buckets.push_back(std::vector<int>());
				
	int start[NR_BUCKETS];
	int count[NR_BUCKETS];

	int task_start[thread_count];
	int task_end[thread_count];


	
	for(unsigned i = 0; i < NR_BUCKETS;i++)
		buckets.push_back(std::vector<int>());
				
	std::thread Threads[thread_count];

	int task_size = (end - begining)/thread_count;
	
	for(int task = 0; task < thread_count;task++) {
		
		
		task_start[task] = task * task_size ;
		task_end[task] = (task + 1) * task_size ;
		
		if(task == thread_count - 1)
			task_end[task] = end;
		
		//std::thread t(insert_into_buckets,array,digit,task_start[task],task_end[task],buckets);
		Threads[task] = std::thread(new insertion_object(array,digit,begining,end,buckets));
	
	}

	for(int i = 0; i < thread_count;i++)
		Threads[i].join();
	
	for(int i = 0; i < NR_BUCKETS;i++)
		count[i] = buckets[i].size();

	start[0] = 0;
	for(int i = 1; i < NR_BUCKETS+1;i++){
		start[i] = start[i-1] + count[i-1];
	}
	

	#pragma ivdep
	for(int i = 0; i < end - begining;i++)
		array[begining + i] = temp[i];
	
	

	task_size = NR_BUCKETS/thread_count;

	work_allocation(thread_count,start,count,NR_BUCKETS);
	printf("%d\n",task_size);
	
	for(int task = 0; task < thread_count;task++) {
		
		task_start[task] = task * task_size ;
		task_end[task] = (task + 1) * task_size ;

		if(task == thread_count - 1)
			task_end[task] = NR_BUCKETS;
		
		//printf("%d %d\n",task_start,task_end);

		argument[task][0] = array;
		argument[task][1] = &count[0];
		argument[task][2] = &start[0];
		argument[task][3] = &digit;
		argument[task][4] = &task_start[task];
		argument[task][5] = &task_end[task];

		pthread_create(&thread_handles[task],NULL,n_sorts,&argument[task]);
		//sequential_radix_sort(array,start[i],begining + start[i] + count[i],digit + 1);

	}
	
	//#pragma omp taskwait
	for(int task = 0;task < thread_count;task++){
		pthread_join(thread_handles[task],NULL);
	}
	
	free(thread_handles);
}



void radix_sort(int *array,int size){
	
	parallel_radix_sort(array,0,size,1);
}

void s_radix_sort(int *array,int size){
	
	sequential_radix_sort(array,0,size,1);
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

	if(argc > 3)
		thread_count = atoi(argv[4]);	
	else
		thread_count = default_threads;
	
	
	for(int i = 0; i < thread_count;i++){

		if(pthread_mutex_init(&mutex_digit[i],NULL) != 0){
			printf("error initializing mutexes\n");
			exit(-1);
		}
	}

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
		printf("%d\n",test_array[i]);*/
	}
	
	sort_double(times,nr_tests);

	double average = 0;
	
	for(int i = 0; i < 5; i++) 
		average += times[i];

	average /= k;
	printf("%f\n",average);
	
	return 0;
}
