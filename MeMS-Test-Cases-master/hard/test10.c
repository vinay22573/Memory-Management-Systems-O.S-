#include"../../mems.h"
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char const *argv[])
{
    printf("-------- STARTING TEST ---------\n");
    mems_init();
    int* ptr[6];
    for(int i=0;i<6;i++)
        ptr[i] = (int*)mems_malloc(500*sizeof(int));
    mems_free(ptr[2]);
    mems_print_stats();
    mems_free(ptr[3]);
    mems_print_stats();
    mems_finish();
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
