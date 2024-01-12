#include"../../mems.h"
#include<stdio.h>
#include<stdlib.h>

// testing mems_get and reuse of memory
int main(int argc, char const *argv[])
{
    printf("-------- STARTING TEST ---------\n");
    mems_init();
    int* ptr[4];
    for(int i=0;i<4;i++)
        ptr[i] = (int*)mems_malloc(500*sizeof(int));

    // getting the address at index 100
    int* phy_ptr= (int*) mems_get(&ptr[1][100]);
    phy_ptr[0] = 5000;

    mems_free(ptr[1]);
    mems_free(ptr[2]);
    ptr[2] = (int*)mems_malloc(500*sizeof(int));
    ptr[1] = (int*)mems_malloc(500*sizeof(int));

    // getting the address at index 0
    phy_ptr= (int*) mems_get(&ptr[2][0]);
    printf("The Secret Number is: %d\n\n[NOTE: This should be 5000]\n", phy_ptr[100]);

    
    mems_finish();
    printf("--------- ENDING TEST ----------\n");
    return 0;
}
