#include"../../mems.h"
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char const *argv[])
{
    printf("-------- STARTING TEST ---------\n");
    printf("NOTE: This test should throw an error.\n");
    mems_init();
    long* ptr = (long*)mems_malloc(1000*sizeof(long));
    mems_free(ptr);
    long* phy_ptr = (long*)mems_get(ptr); // get the address of index 0
    // The physical address can be any Unsigned long number
    printf("Virtual address: %lu\tPhysical Address: %lu\n",(unsigned long)ptr,(unsigned long)phy_ptr);
    for(long i=0;i<1000;i++)
        phy_ptr[i] = i+50000000000;
    printf("Value at index 50: %ld\n", phy_ptr[50]);
    mems_finish();
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
