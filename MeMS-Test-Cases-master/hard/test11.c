#include"../../mems.h"
#include<stdio.h>
#include<stdlib.h>

// testing the reusability
int main(int argc, char const *argv[])
{
    printf("-------- STARTING TEST ---------\n");
    mems_init();
    int num = 16;
    int num2 = num/2;
    int size = PAGE_SIZE/8;
    int one_piece = (PAGE_SIZE*3)/32;
    int* ptr[num];

    for(int i=0;i<num;i++)
        ptr[i] = (int*)mems_malloc(size);

    int i;
    for(i=0;i<num;i+=2){
        mems_free(ptr[i++]);
        mems_free(ptr[i++]);
    }

    for(int i=0;i<num2;i++)
        ptr[i] = (int*)mems_malloc(one_piece);

    for(int i=0;i<num2;i++)
        ptr[i] = (int*)mems_malloc((size-one_piece));
    
    mems_print_stats();
    mems_finish();
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
