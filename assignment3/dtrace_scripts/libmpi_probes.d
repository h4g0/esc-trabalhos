#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}
 

pid$target:libmpi:MPI_Send:entry
{
    time_spent["send",pid] = timestamp;
}

pid$target:libmpi:MPI_Send:return
/time_spent["send",pid] != 0/
{
    @sizes["send"] = quantize(arg2);
    @time["send"] = sum(timestamp - time_spent["send",pid]);
    time_spent["send",pid] = 0;
}

pid$target:libmpi:MPI_Bcast:entry
{
    time_spent["bcast",pid] = timestamp;
}

pid$target:libmpi:MPI_Bcast:return
/time_spent["bcast",pid] != 0/
{

    @sizes["bcast"] = quantize(arg2);
    @time["bcast"] = sum(timestamp - time_spent["bcast",pid]);
    time_spent["bcast",pid] = 0;
}




pid$target:libmpi:MPI_Recv:entry
{
    time_spent["recv",pid] = timestamp;
}

pid$target:libmpi:MPI_Recv:return
/time_spent["recv",pid] != 0/
{
    @sizes["recv"] = quantize(arg2);
    @time["recv"] = sum(timestamp - time_spent["recv",pid]);
    time_spent["recv",pid] = 0;
}

pid$target:libmpi:MPI_Gatherv:entry
{
    time_spent["gatherv",pid] = timestamp;
}

pid$target:libmpi:MPI_Gatherv:return
/time_spent["gatherv",pid] != 0/
{
    @sizes["gatherv"] = quantize(arg2);
    @time["gatherv"] = sum(timestamp - time_spent["gatherv",pid]);
    time_spent["gatherv",pid] = 0;
}

END{
    printf("Communication times using different primitives\n");
    normalize(@time ,1000000000);
    printa(@time);
    trunc(@time);


    printf("Integers sent or received for each function call\n");
    printa(@sizes);
    trunc(@sizes);
}
