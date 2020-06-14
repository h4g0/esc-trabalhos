#!/usr/sbin/dtrace -qs
syscall::open:entry
{
    self->file  = arg0;
    self->flags = arg1;
}

syscall::open:return
/strstr(copyinstr(self->file),"/etc") != 0/
{
    printf("%s(pid:%d,uid:%d,gid:%d) tried to open file %s with flags %d called %s with return value %d\n",execname,pid,uid,gid,copyinstr(self->file),
                self->flags,probefunc,arg1);
    self->file = 0;
    self->flags = 0;
    
}
