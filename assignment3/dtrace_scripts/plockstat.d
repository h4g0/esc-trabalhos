#!/usr/sbin/dtrace -qs 

BEGIN{
	start  = timestamp;
}

plockstat$target:::mutex-block
/execname == $1/
{
    block[pid] = timestamp;
}

plockstat$target:::mutex-spin
/execname == $1/
{
    block[pid] = timestamp;
}

plockstat$target:::mutex-acquire
/execname == $1 && block[pid] != 0/
{
    @info["wait for mutex"] = quantize(timestamp - block[pid]);
    acquire[pid] = timestamp;
    block[pid] = 0;
}

plockstat$target:::mutex-release
/execname == $1 && acquire[pid] != 0/
{
    @info["time holding mutex"] = quantize(timestamp - acquire[pid]);
    acquire[pid] = 0;
}

plockstat$target:::mutex-error
/execname == $1/
{
    block[pid] = 0;
    acquire[pid] = 0;
}

END{
    printf("info collected from the system\n");
    printa(@info);
    trunc(@info);

}



