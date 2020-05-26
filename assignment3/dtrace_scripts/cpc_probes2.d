#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}


cpc:::PAPI_l2_dcm-all-5000
/execname == $1/
{
    @info["l2 cache data misses"] = sum(5000);
}


END{
    printf("data cache misses l2\n");
    printa(@info);
    trunc(@info);
}



