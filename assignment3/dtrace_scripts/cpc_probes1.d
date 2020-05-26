#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

cpc:::PAPI_l1_dcm-all-5000
/execname == $1/
{
    @info["l1 cache data misses"] = sum(5000);
}


END{
    printf("data cache misses l1\n");
    printa(@info);
    trunc(@info);
}



