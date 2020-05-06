module dtrace.esc-trabalhos.assignment2.ex1;


syscall::open:entry
{
    self->file  = arg0;
    self->flags = arg1;
    /* printf("%s(pid:%d,uid:%d,gid:%d), called %s\n",execname,pid,uid,gid,probefunc); */
}

syscall::open:return
{
    printf("%s(pid:%d,uid:%d,gid:%d) %s with flags %d called %s\n",execname,pid,uid,gid,copyinstr(self->file),
                self->flags,probefunc);
    self->file = 0;
    self->flags = 0;
    
}