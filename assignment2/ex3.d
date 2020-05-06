#!/usr/sbin/dtrace -qs
syscall:::entry
/execname == $1/
{
    @syscalls[probefunc] = count();
    time_pid[pid] = timestamp;
}


syscall:::return
/time_pid[pid] != 0 & execname == $1/
{
    @time[execname] = sum(timestamp - time_pid[pid]);
    time_pid[pid] = 0;
}

END{
    printf("%s\n",$1);
    printa(@time);
    trunc(@time);
}
