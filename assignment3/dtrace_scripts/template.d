#!/usr/sbin/dtrace -qs
BEGIN{
    CREATE_FILE_FLAG = 64;
}


syscall::open:entry
/(arg1 & CREATE_FILE_FLAG) != CREATE_FILE_FLAG/
{
    self->flags = arg1;
    @open_existing_file[pid,execname] = count();
}


syscall::open:entry
/(arg1 & CREATE_FILE_FLAG) == CREATE_FILE_FLAG/
{
    self->flags = arg1;
    @create_new_file[pid,execname] = count();
}

syscall::open:return
/(self->flags & CREATE_FILE_FLAG) == CREATE_FILE_FLAG & errno == 0/
{
    @create_file_success[pid,execname] = count();
    self->flags = 0;
}

syscall::open:return
/(self->flags & CREATE_FILE_FLAG) == CREATE_FILE_FLAG & errno != 0/
{
    @create_file_faillure[pid,execname] = count();
    self->flags = 0;
}

syscall::open:return
/(self->flags & CREATE_FILE_FLAG) != CREATE_FILE_FLAG & errno == 0/
{
    @open_existing_file_success[pid,execname] = count();
    self->flags = 0;
}


syscall::open:return
/(self->flags & CREATE_FILE_FLAG) != CREATE_FILE_FLAG & errno != 0/
{
    @open_existing_file_faillure[pid,execname] = count();
    self->flags = 0;
}


profile:::tick-$1s{
    
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

    printf("Faillure to open existing files\n");
    printa(@open_existing_file_faillure);
    trunc(@open_existing_file_faillure);
    printf("---------------------------------------------\n");

    printf("Faillure to create news files\n");
    printa(@create_file_faillure);
    trunc(@create_file_faillure);


    printf("---------------------------------------------\n");

    printf("Success to open existing files\n");
    printa(@open_existing_file_success);
    trunc(@open_existing_file_success);
    printf("---------------------------------------------\n");

    printf("Success to create news files\n");
    printa(@create_file_success);
    trunc(@create_file_success);

    printf("---------------------------------------------\n");

}

END{
    trunc(@open_existing_file);
    trunc(@create_new_file);
    trunc(@open_existing_file_faillure);
    trunc(@create_file_faillure);
    trunc(@open_existing_file_success);
    trunc(@create_file_success);
}



