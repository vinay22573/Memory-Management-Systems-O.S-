#include"../../mems.h"
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char const *argv[])
{
    printf("-------- STARTING TEST ---------\n");
    mems_init();
    long* ptr = (long*)mems_malloc(1000*sizeof(long));
    mems_print_stats();
    mems_free(ptr);
    mems_print_stats();
    mems_finish();
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
