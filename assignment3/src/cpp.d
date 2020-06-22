provider cpp {
	
	probe start_sorting_into_buckets(int,int,int);
	probe finish_sorting_into_buckets(int,int,int);
	
	probe start_seq_radix(int,int,int);
	probe finish_seq_radix(int,int,int);
	
	probe start_par_radix(int,int,int);
	probe finish_par_radix(int,int,int);
	
	probe start_copy_to_main_array(int,int,int);
	probe finish_copy_to_main_array(int,int,int);
		
	probe start_workload_distribution(int,int,int);	
	probe finish_workload_distribution(int,int,int);
};
