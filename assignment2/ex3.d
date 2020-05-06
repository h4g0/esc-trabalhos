syscall:::entry
{
    @execs[execname] = count();
    time[pid] = timestamp;
}


syscall:::open:return
/time[pid] != 0/
{
    @open_existing_file_success[pid] = count();
    time[execname] += timestamp -time[pid];
    time[pid] = 0;
}
