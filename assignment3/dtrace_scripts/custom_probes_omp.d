#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

omp*:::start_sorting_into_buckets
{
    started_sorting[pid,arg1] = timestamp;
    started_section[pid,arg1,"sorting_into_buckets"] = timestamp;
    @sizes_per_digit[arg1] = quantize(arg2);
}

omp*:::finish_sorting_into_buckets
/started_sorting[pid,arg1] != 0/
{
    @sorting_into_buckets[arg1] = sum(timestamp - started_sorting[pid,arg1]);
    started_sorting[pid,arg1] = 0;
    @time_spent_in_section["sorting into buckets"] = sum(timestamp - started_section[pid,arg1,"sorting_into_buckets"]);
    started_section[pid,arg1,"sorting_into_buckets"] = 0;
}


omp*:::start_copy_to_main_array
{
	started_section[pid,arg1,"copy_to_main_array"] = timestamp;
}

omp*:::finish_copy_to_main_array
/started_section[pid,arg1,"copy_to_main_array"] != 0/ 
{
	@time_spent_in_section["copy to main array"] = sum(timestamp - started_section[pid,arg1,"copy_to_main_array"]);
	started_section[pid,arg1,"copy_to_main_array"] = 0;
}

omp*:::start_allocate_temp_array
{
	started_section[pid,arg1,"allocate_temp_array"] = timestamp;
}

omp*:::finish_allocate_temp_array
/started_section[pid,arg1,"allocate_temp_array"] != 0/ 
{
	@time_spent_in_section["allocate temp array"] = sum(timestamp - started_section[pid,arg1,"allocate_temp_array"]);
	started_section[pid,arg1,"allocate_temp_array"] = 0;
}


omp*:::start_seq_radix
{
	@get_digit[arg1] = count();
	getting_digit[pid,arg1] = timestamp;
	started_section[pid,arg1,"recusrive_calls"] = timestamp;
}

omp*:::finish_seq_radix
/getting_digit[pid,arg1] != 0/
{
	@time_getting_digit[arg1] = sum(timestamp - getting_digit[pid,arg1]);
	@time_spent_in_section["recursive calls"] = sum(timestamp - started_section[pid,arg1,"total"]);
	getting_digit[pid,arg1] = 0;
	started_section[pid,arg1,"recursive_calls"] = 0;
}

END{

    @time_spent_in_section["total"] = sum(timestamp - start);
    printf("time spent sorting into buckets\n");
    normalize(@sorting_into_buckets ,1000000000);
    printa(@sorting_into_buckets);
    trunc(@sorting_into_buckets);


    printf("array sizes per digit\n");
    printa(@sizes_per_digit);
    trunc(@sizes_per_digit);
    
    printf("time spent per digit\n");
    normalize(@time_getting_digit ,1000000000);
    printa(@time_getting_digit);
    trunc(@time_getting_digit);

    printf("calls by digit\n");
    printa(@get_digit);
    trunc(@get_digit);

    printf("time spent in different sections of the program\n");
    normalize(@time_spent_in_section ,1000000000);
    printa(@time_spent_in_section);
    trunc(@time_spent_in_section);
}



