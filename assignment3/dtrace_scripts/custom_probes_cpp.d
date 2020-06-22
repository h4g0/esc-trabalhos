#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

cpp*:::start_sorting_into_buckets
{
    
    started_section[arg2,arg1,"sorting_into_buckets"] = timestamp;
    @sizes_per_digit[arg1] = quantize(arg0);
}

cpp*:::finish_sorting_into_buckets
/started_section[arg2,arg1,"sorting_into_buckets"] != 0/
{

    @sorting_into_buckets[arg1] = sum(timestamp - started_section[arg2,arg1,"sorting_into_buckets"]);
    @time_spent_in_section["sorting into buckets"] = sum(timestamp - started_section[arg2,arg1,"sorting_into_buckets"]);
    started_section[arg2,arg1,"sorting_into_buckets"] = 0;
}


cpp*:::start_copy_to_main_array
{
	started_section[arg2,arg1,"copy_to_main_array"] = timestamp;
}

cpp*:::finish_copy_to_main_array
/started_section[arg2,arg1,"copy_to_main_array"] != 0/ 
{
	@time_spent_in_section["copy to main array"] = sum(timestamp - started_section[arg2,arg1,"copy_to_main_array"]);
	started_section[arg2,arg1,"copy_to_main_array"] = 0;
}

cpp*:::start_par_radix
{
	@get_digit[arg1] = count();
	started_section[arg2,arg1,"recursive_calls"] = timestamp;
}

cpp*:::finish_par_radix
/started_section[arg2,arg1,"recursive_calls"] != 0/
{
	@time_getting_digit[arg1] = sum(timestamp - started_section[arg2,arg1,"recursive_calls"]);
	@time_spent_in_section["recursive calls"] = sum(timestamp - started_section[arg2,arg1,"recursive_calls"]);
	started_section[arg2,arg1,"recursive_calls"] = 0;
}

cpp*:::start_seq_radix
{
	@get_digit[arg1] = count();
	started_section[arg2,arg1,"recursive_calls"] = timestamp;
}

cpp*:::finish_seq_radix
/started_section[arg2,arg1,"recursive_calls"] != 0/
{
	@time_getting_digit[arg1] = sum(timestamp - started_section[arg2,arg1,"recursive_calls"]);
	@time_spent_in_section["recursive calls"] = sum(timestamp - started_section[arg2,arg1,"recursive_calls"]);
	started_section[arg2,arg1,"recursive_calls"] = 0;
}

cpp*:::start_workload_distribution
{
    started_section[arg2,arg1,"workload_distribution"] = timestamp;

}

cpp*:::finish_workload_distribution
/started_section[arg2,arg1,"workload_distribution"] != 0/
{
    @time_spent_in_section["workload distribution"] = sum(timestamp - started_section[arg2,arg1,"workload_distribution"]);
	started_section[arg2,arg1,"workload_distribution"] = 0;
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



