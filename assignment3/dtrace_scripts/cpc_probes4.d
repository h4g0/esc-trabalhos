#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}


cpc:::PAPI_l2_icm-all-5000
/execname == $1/
{
    @info["l2 cache instruction misses"] = sum(5000);
}



END{
    printf("instruction cache misses l2\n");
    printa(@info);
    trunc(@info);
}



