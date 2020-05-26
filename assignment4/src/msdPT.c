#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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


void *count_digits(void *args){
		
		int **arguments = (int**) args;

		int *array = arguments[0];
		int *count = arguments[1];
		int digit = *arguments[2];
		int t_start = *arguments[3];
		int t_end = *arguments[4];

		int task_counts[NR_BUCKETS];	
		
		for(int i = 0; i < NR_BUCKETS; i++)
			task_counts[i] = 0;

		for(int i  = t_start; i < t_end;i++)
			task_counts[get_digit(array[i],digit)]++;
					
		for(int i = 0; i < NR_BUCKETS;i++){
			pthread_mutex_lock(&mutex_digit[i]);
			count[i] += task_counts[i];
			pthread_mutex_unlock(&mutex_digit[i]);
		}
		
		
}

void *insert_into_buckets(void *args){
		
		int **arguments = (int**) args;

		int *array = arguments[0];
		int *temp = arguments[1];
		int *start = arguments[2];
		int *inserted = arguments[3];
		int digit = *arguments[4];
		int t_start = *arguments[5];
		int t_end = *arguments[6];

		for(int i = t_start;i < t_end;i++){
		
			int msdigit = get_digit(array[i],digit);
			int insert_pos;
				
			
			pthread_mutex_lock(&mutex_digit[msdigit]);
			insert_pos = inserted[msdigit];
			inserted[msdigit]++;
			pthread_mutex_unlock(&mutex_digit[msdigit]);
					
			
			temp[start[msdigit] + insert_pos] = array[i];

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

void parallel_radix_sort(int* array,int begining,int end,int digit) {	
	
	if(end <= begining + 1)
		return ;
	
	if(digit == MAX_DIGITS)
		return ;

	long thread;
	pthread_t *thread_handles;
	thread_handles = malloc(thread_count * sizeof(pthread_t));
	
	int count[NR_BUCKETS];
	int inserted[NR_BUCKETS];
	int start[NR_BUCKETS];
	
	int task_start[thread_count];
	int task_end[thread_count];
	
	int *temp = malloc((end - begining) * sizeof(int));
	
	#pragma ivdep
	for(int i = 0; i < NR_BUCKETS; i++){ 
		count[i] = 0;	
		inserted[i] = 0;
		start[i] = 0;
	}
	
	int max_arguments = 10;

	int *argument[thread_count][max_arguments];

	int task_size = (end - begining)/thread_count;
	
	for(int task = 0; task < thread_count;task++) {
		
		
		task_start[task] = task * task_size ;
		task_end[task] = (task + 1) * task_size ;
		
		if(task == thread_count - 1)
			task_end[task] = end;
		
		argument[task][0] = array;
		argument[task][1] = &count[0];
		argument[task][2] = &digit;
		argument[task][3] = &task_start[task];
		argument[task][4] = &task_end[task];

		//#pragma omp task shared(array,count,task_size)
		pthread_create(&thread_handles[task],NULL,count_digits,&argument[task]);
		//count_digits(&argument[task]);
	}

	//#pragma omp taskwait
	
	for(int task = 0;task < thread_count;task++){
		pthread_join(thread_handles[task],NULL);
	}

	for(int i = 1; i < NR_BUCKETS;i++){
		start[i] += start[i-1] + count[i-1];
	}
	


	for(int task = 0; task < thread_count;task++) {
		
		task_start[task] = task * task_size ;
		task_end[task] = (task + 1) * task_size ;

		
		if(task == thread_count - 1)
			task_end[task] = end;
		
		argument[task][0] = array;
		argument[task][1] = temp;
		argument[task][2] = &start[0];
		argument[task][3] = &inserted[0];
		argument[task][4] = &digit;
		argument[task][5] = &task_start[task];
		argument[task][6] = &task_end[task];

		//#pragma task shared(array,temp,inserted,task_size)
		pthread_create(&thread_handles[task],NULL,insert_into_buckets,&argument[task]);
		//insert_into_buckets(&argument[task]);
	}

	//#pragma omp taskwait
	
	for(int task = 0;task < thread_count;task++){
		pthread_join(thread_handles[task],NULL);
	}

	#pragma ivdep
	for(int i = 0; i < end - begining;i++)
		array[begining + i] = temp[i];
	
	free(temp);
	

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
	int k = atoi(argv[3]);

	unsigned default_threads = 4;

	if(argc > 2)
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
