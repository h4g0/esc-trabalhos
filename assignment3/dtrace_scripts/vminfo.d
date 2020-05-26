#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

vminfo:::*
/execname == $1/
{
    @info[probename] = quantize((timestamp - start) / 1000000000);
}


END{
    printf("info collected from the system\n");
    printa(@info);
    trunc(@info);



}



