provider omp {
	
	probe start_sorting_into_buckets(int,int);
	probe finish_sorting_into_buckets(int,int);
	
	probe start_seq_radix(int,int);
	probe finish_seq_radix(int,int);
	
	probe start_par_radix(int,int);
	probe finish_par_radix(int,int);
	
	probe start_count_digits(int,int);
	probe finish_count_digits(int,int);
	
	probe start_insert_into_buckets(int,int);
	probe finish_insert_into_buckets(int,int);
	
	probe start_copy_to_main_array(int,int);
	probe finish_copy_to_main_array(int,int);
	
	probe start_allocate_temp_array(int,int);	
	probe finish_allocate_temp_array(int,int);	
};
