#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

cpc:::PAPI_l1_icm-all-5000
/execname == $1/
{
    @info["l1 cache instruction misses"] = sum(5000);
}


END{
    printf("instruction cache misses l1\n");
    printa(@info);
    trunc(@info);
}



