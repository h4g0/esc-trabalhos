#!/usr/sbin/dtrace -qs
BEGIN{
    start = timestamp;
}
syscall:::entry
/execname == $1/
{
    @syscalls[probefunc] = count();
    time_pid[pid] = timestamp;
}


syscall:::return
/time_pid[pid] != 0 & execname == $1/
{
    @time[probefunc] = sum(timestamp - time_pid[pid]);
    time_pid[pid] = 0;
}

END{
    printf("%s\n",$1);
    
    printf("time spent sice the start of this program %d\n",timestamp - start);
    
    printf("system calls frequency\n");
    
    printa(@syscalls);

    printf("\ntime spent on each system call in nanoseconds\n");

    printa(@time);
    
    trunc(@syscalls);
    trunc(@time);
}
