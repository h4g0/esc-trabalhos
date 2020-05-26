#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

cpc:::PAPI_l1_dcm-all-5000
/execname == $1/
{
    @info["l1 cache data misses"] = sum(5000);
}

cpc:::PAPI_l1_icm-all-5000
/execname == $1/
{
    @info["l1 cache instruction misses"] = sum(5000);
}

cpc:::PAPI_l2_dcm-all-5000
/execname == $1/
{
    @info["l2 cache data misses"] = sum(5000);
}

cpc:::PAPI_l2_icm-all-5000
/execname == $1/
{
    @info["l2 cache instruction misses"] = sum(5000);
}



END{
    printf("cache misses\n");
    printa(@info);
    trunc(@info);
}



