#include"../../mems.h"
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char const *argv[])
{
    printf("-------- STARTING TEST ---------\n");
    mems_init();
    int* ptr[10];
    for(int i=0;i<10;i++)
        ptr[i] = (int*)mems_malloc(100*sizeof(int));
    
    int sequence[] = {9, 2, 3, 1, 7, 8, 0, 5, 4, 6};
    for(int i=0;i<sizeof(sequence)/sizeof(int);i++)
       mems_free(ptr[i]);
    mems_print_stats();
    
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
