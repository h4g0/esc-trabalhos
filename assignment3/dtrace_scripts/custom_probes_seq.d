#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

seq*:::start_sorting_into_buckets
{
    started_sorting[pid] = timestamp;
    @sizes_per_digit[arg1] = quantize(arg2);
}

seq*:::finish_sorting_into_buckets
/started_sorting[pid] != 0/
{
    @sorting_into_buckets[arg1] = sum(timestamp - started_sorting[pid]);
    started_sorting[pid] = 0;
}

seq*:::start_get_digit
{
	@get_digit[arg1] = count();
	getting_digit[pid] = timestamp;
}

seq*:::finish_get_digit
/getting_digit[pid] != 0/
{
	@time_getting_digit[arg1] = sum(timestamp - getting_digit[pid]);
	getting_digit[pid] = 0;
}

END{
    printf("time spent sorting into buckets\n");
    normalize(@sorting_into_buckets ,1000000000);
    printa(@sorting_into_buckets);
    trunc(@sorting_into_buckets);


    printf("array sizes per digit\n");
    printa(@sizes_per_digit);
    trunc(@sizes_per_digit);
    
    printf("time spend per digit\n");
    normalize(@time_getting_digit ,1000000000);
    printa(@time_getting_digit);
    trunc(@time_getting_digit);


}



