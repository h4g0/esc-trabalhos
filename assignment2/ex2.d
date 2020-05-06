BEGIN{

}

syscall::open:entry
/arg1 == 64/
{
    self->flags = arg1;
    @open_existing_file[pid,execname] = count();
    /* printf("%s(pid:%d,uid:%d,gid:%d), called %s\n",execname,pid,uid,gid,probefunc); */
}


syscall::open:entry
/arg1 != 64/
{
    self->flags = arg1;
    @create_new_file[pid,execname] = count();
    /* printf("%s(pid:%d,uid:%d,gid:%d), called %s\n",execname,pid,uid,gid,probefunc); */
}

syscall::open:return
/self->flags == 64 & errno == 0/
{
    @create_file_success[pid,execname] = count();
    self->flags = 0;
}

syscall::open:return
/self->flags == 64 & errno != 0/
{
    @create_file_faillure[pid,execname] = count();
    self->flags = 0;
}

syscall::open:return
/self->flags != 64 & errno == 0/
{
    @open_existing_file_success[pid,execname] = count();
    self->flags = 0;
}


syscall::open:return
/self->flags != 64 & errno != 0/
{
    @open_existing_file_faillure[pid,execname] = count();
    self->flags = 0;
}


profile:::tick-10s{
    
    printf("Date: %Y\n",walltimestamp);

    printf("---------------------------------------------\n");
    
    printf("Number of tentatives to open existing files\n");
    printa(@open_existing_file);
    trunc(@open_existing_file);

    printf("---------------------------------------------\n");

    printf("Number of tentatives to create new files\n");
    printa(@create_new_file);
    trunc(@create_new_file);

    printf("---------------------------------------------\n");

    printf("Faillures to open existing files\n");
    printa(@open_existing_file_faillure);
    trunc(@open_existing_file_faillure);
    printf("---------------------------------------------\n");

    printf("Faillures to create news files\n");
    printa(@create_file_faillure);
    trunc(@create_file_faillure);


    printf("---------------------------------------------\n");

}

END{
    trunc(@open_existing_file);
    trunc(@create_new_file);
    trunc(@open_existing_file_faillure);
    trunc(@create_file_faillure);
}


