#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

sched:::on-cpu
/execname == $1/
{
    self->ts = timestamp;
}

sched:::off-cpu
/self->ts && execname == $1/
{
    @strech_time_cpu[cpu] = quantize(timestamp - self->ts);
    @total_time_cpu[cpu] = sum(timestamp - self->ts);
    self->ts = 0;
}

END{
    printf("time spent per strech per  cpu\n");
    normalize(@strech_time_cpu,1000000000);
    printa(@strech_time_cpu);
    trunc(@strech_time_cpu);
    printf("time per cpu\n");
    normalize(@total_time_cpu,1000000000);
    printa(@total_time_cpu);
    trunc(@total_time_cpu);
}



