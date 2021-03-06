provider mpi {
	
	probe start_sorting_into_buckets(int,int,int);
	probe finish_sorting_into_buckets(int,int,int);
	
	probe start_seq_radix(int,int,int);
	probe finish_seq_radix(int,int,int);
	
	probe start_par_radix(int,int,int);
	probe finish_par_radix(int,int,int);
	
	probe start_count_digits(int,int,int);
	probe finish_count_digits(int,int,int);
	
	probe start_insert_into_buckets(int,int,int);
	probe finish_insert_into_buckets(int,int,int);
	
	probe start_copy_to_main_array(int,int,int);
	probe finish_copy_to_main_array(int,int,int);
	
	probe start_allocate_temp_array(int,int,int);	
	probe finish_allocate_temp_array(int,int,int);	

	probe start_workload_distribution(int,int,int);	
	probe finish_workload_distribution(int,int,int);

	probe start_scatter_workload(int,int,int);	
	probe finish_scatter_workload(int,int,int);

	probe start_gather_workload(int,int,int);	
	probe finish_gather_workload(int,int,int);

	probe start_receive_workload(int);	
	probe finish_receive_workload(int);

	probe start_send_workload(int);	
	probe finish_send_workload(int);
};
