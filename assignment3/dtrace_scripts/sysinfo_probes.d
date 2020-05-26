#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

sysinfo:::nthreads
/execname == $1/

{
    @info["number of threads created"] = count();
}

sysinfo:::pswitch
/execname == $1/

{
    @info["number of cpu switches between threads"] = count();
}

sysinfo:::procovf
/execname == $1/

{
    @info["failled to create process"] = count();
}

sysinfo:::bwrite
/execname == $1/

{
    @info["number of physical writes to buffer"] = count();
    @q_info["size of writes"] = quantize(arg1);

}

sysinfo:::bread
/execname == $1/

{
    @info["number of physical reads to buffer"] = count();
    @q_info["size of reads"] = quantize(arg1);

}

sysinfo:::inv_swtch
/execname == $1/
{
    @info["number of times threads where forced to give up cpu"] = count();
}



sysinfo:::sysfork
/execname == $1/
{
    @info["number of fork system calls"] = count();
}


sysinfo:::sysexec
/execname == $1/
{
    @info["number of exec system calls"] = count();
}


END{
    printf("info collected from the system\n");
    printa(@info);
    printa(@q_info);
    trunc(@info);
    trunc(@q_info);



}



