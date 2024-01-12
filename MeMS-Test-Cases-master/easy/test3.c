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
    char* ptr = (char*)mems_malloc(1234);
    char* phy_ptr= (char*) mems_get(ptr); // get the address of index 0
    // The physical address can be any Unsigned long number
    printf("Virtual address: %lu\tPhysical Address: %lu\n",(unsigned long)ptr,(unsigned long)phy_ptr);
    printf("NOTE: The physical address can be any Unsigned Long Number\n");
    mems_finish();
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
