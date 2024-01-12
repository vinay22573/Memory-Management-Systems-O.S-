#include"../../mems.h"
#include<stdio.h>
#include<stdlib.h>

/*
checking basic functions
*/
int main(int argc, char const *argv[])
{
    printf("-------- STARTING TEST ---------\n");
    mems_init();
    mems_print_stats();
    mems_finish();
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
