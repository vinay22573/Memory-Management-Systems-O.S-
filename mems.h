#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdint.h>
// code where mems malloc is faulty
#define PAGE_SIZE 4096
typedef struct subChainNode subNode;
typedef struct mainChainNode {
    struct mainChainNode *prev;
    subNode *nextSubChain;
    size_t size; //memory size 
    size_t startAddress;
    size_t endAddress;
    void *mems_physical_address;
    int mchainNo;
    struct mainChainNode *next;
} mcNode;
typedef struct subChainNode { 
    subNode *prev;  
    int type; // if process 1 if hole 0
    size_t startAddress;
    size_t endAddress;
    int schainNo;
    struct subChainNode *next;  
} subNode;


// Declaring some global variables

mcNode *first;// gloablly accesible pointer to the first node of main node
mcNode *last;// gloablly accesible pointer to the last node of main node
mcNode *for_getting_ptr_on_main_node_to_get_physical_address;
int getNo_of_Mapped_pages; // globally getting the no of pages used in mainnode
subNode *process_ptr;//maybe we will need it somewhere
subNode *abcd;// this is a pointer which you will need in checking subnode capacity declaring it gloabally is necessary to change it from anywhere in the code , this pointer points on the subnode which has the capacity to hold the new process
void *startptr;// declaring them globally to access and change them easily
void *endptr;// these pointers are required for replacing malloc with our custom defined malloc/getchunksize



// It initialises all the required global parameters
void mems_init(){
    first = NULL;
    last  = first;
    startptr = NULL;
    endptr = NULL;
    abcd = NULL;
    getNo_of_Mapped_pages = -1;
    process_ptr = NULL;
    for_getting_ptr_on_main_node_to_get_physical_address = NULL;  
}






// It calculates the number of pages when size is given
int getNo_of_pages(size_t size) {
    int no_pages = 0;
    int page_size = PAGE_SIZE;
    int q = size / page_size;
    if (size % page_size == 0) {
        no_pages = q;
    } else {
        no_pages = q + 1;
    }
    return no_pages;
}






// if a main node type pointer is given this function basically checks that this mainnode ptr have any subnode of the size required
int check_subnode_capacity(mcNode *mainNodePtr, size_t size) {
    mcNode *c = mainNodePtr;
    while (c) {
        subNode *s = c->nextSubChain;
        while (s) {
            if (s->type == 0) { // HOLE
                size_t hole_size = (size_t)(s->endAddress - s->startAddress + 1);
                if (hole_size >= size) {
                    abcd = s;
                    return 1;
                }
            }
            s = s->next;
        }
        c = c->next;
    }
    return 0;
}




// Substitute of malloc
void *getchunksize(size_t size) {
    if (startptr == NULL) {
        int get_pages_No = getNo_of_pages(size); // Calculate the number of pages needed
        startptr = mmap(NULL, PAGE_SIZE * get_pages_No, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (startptr == MAP_FAILED) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }
        endptr = startptr + (PAGE_SIZE * get_pages_No) - 1; // End pointer should point to the end of this memory
        startptr = startptr + size;
        return startptr - size;
    } 
    // this else if handles the case when first memory was unable to meet the new demand of size so we were forced to make a new memory
    else if (endptr - startptr + 1 < size) { // Check if available space is less than the required size
        int get_pages_No = getNo_of_pages(size); // Calculate the number of pages needed
        void *newstartptr = mmap(NULL, PAGE_SIZE * get_pages_No, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (newstartptr == MAP_FAILED) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }
        endptr = newstartptr + (PAGE_SIZE * get_pages_No) - 1; // End pointer should point to the end of this memory
        startptr = newstartptr;
        return newstartptr;
    } else {
        startptr = startptr + size; // Update startptr
        return startptr - size; // Return the start pointer to the beginning of the allocated chunk
    }
}





// It adds a newnode by mmap calling to the mainchain node iniitialises its subchain to null with the size of mmap called pages after adding a new node it updates the last pointer to last added node and returns the physical address obtained from mmap , if not created create a mainnode, if mainnode created then add a node in that Doubly LL
mcNode* add_mcNode(size_t processsize,void *mem) {
    // Allocate memory for the new mcNode
    mcNode *t = (mcNode *)getchunksize(sizeof(mcNode));
    t->next = NULL;
    t->size = processsize;
    t->prev = last;
    t->nextSubChain = NULL;
    t->mems_physical_address = mem;
    // Update the first and last pointers if necessary
    if (last) {
        last->next = t;
    } else {
        first = last  = t;
    }
    // Calculate startAddress and mchainNo
    if (t->prev) {
        t->startAddress = (size_t)((t->prev)->endAddress + 1);
        t->mchainNo = (t->prev)->mchainNo + 1;
    } else {
        t->startAddress = (size_t)1000;
        t->mchainNo = 1;
    }
    // Calculate endAddress
    t->endAddress = t->startAddress + processsize - (size_t)1;
    
    // Initialize the subChain
    subNode *snt = (subNode *)getchunksize(sizeof(subNode));
    snt->prev = NULL;
    snt->next = NULL;
    snt->type = 0;
    snt->startAddress = t->startAddress;
    snt->endAddress = t->endAddress;
    snt->schainNo = 1; 
    t->nextSubChain = snt;
    last = t;
    return last; // Return a pointer to the last node
}




// Function assumes that the ptr holds the capacity for size
// It splits the hole in hole and process and if the process is created at start then it updates the mainnode's nextsubchain to point on the process node
void *splitter(size_t size, subNode *hole_ptr, mcNode *mn){
    subNode* prcs_ptr = (subNode *)getchunksize(sizeof(subNode));
    prcs_ptr->next = prcs_ptr->prev = NULL;
    prcs_ptr->schainNo = 0; // NOTE: This is a dummy subchain number which is just to initialize it, it will be updated once where to fit this node is known.
    prcs_ptr->type = 1;
    size_t nsoh = (size_t)((hole_ptr->endAddress) - (hole_ptr->startAddress) + 1) /* current hole size before splitting */ - size; // nsoh = new size of hole after splitting
    if(nsoh==0){
        hole_ptr->type = 1;
        prcs_ptr = hole_ptr;
    }else{
        prcs_ptr->endAddress = prcs_ptr->startAddress = (size_t)0;// intialising both the address of prcs ptr
        prcs_ptr->next = hole_ptr;// chahe prev of hole null ho ya na ho
        if(hole_ptr->prev){
            hole_ptr->prev->next = prcs_ptr;
        }
        prcs_ptr->prev = hole_ptr->prev;
        hole_ptr->prev = prcs_ptr;
        prcs_ptr->startAddress = hole_ptr->startAddress;
        prcs_ptr->endAddress = (size_t)((hole_ptr->startAddress) + size - 1);
        hole_ptr->startAddress = (size_t)(prcs_ptr->endAddress + 1);
        prcs_ptr->schainNo = hole_ptr->schainNo;
        // hole_ptr->schainNo = prcs_ptr->schainNo + 1; // dont write this redundant line while loop will handle this
    }
    if(mn->startAddress == prcs_ptr->startAddress){
        mn->nextSubChain = prcs_ptr;
    }
    process_ptr = prcs_ptr;
    subNode *sb = prcs_ptr;
    int sno = prcs_ptr->schainNo;
    while(sb){
        sb->schainNo = sno;
        sb = sb->next;
        sno++;
    }
    return process_ptr;
}




// If a virtual address is given it finds the mainnode and then its subnode which contains this virtual address.
subNode *findSubNodeFromVirtualAddress(void* virtualAddr) {
    mcNode* mc = first;  // Start from the first mainChain node
    subNode* sub = NULL;  // Initialize sub to NULL

    while (mc) {
        // Check if the virtual address is within the range of the mainChain node
        if ((size_t)virtualAddr >= mc->startAddress && (size_t)virtualAddr <= mc->endAddress) {
            sub = mc->nextSubChain;  // Start from the first subChain node
            for_getting_ptr_on_main_node_to_get_physical_address = mc;
            break;
        }
        mc = mc->next;  // Move to the next mainChain node
    }

    while (sub) {
        // Check if the virtual address is within the range of the subChain node
        if ((size_t)virtualAddr >= sub->startAddress &&  (size_t)virtualAddr <= sub->endAddress) {
            // The virtual address is within this subChain node
            return sub;
        }
        sub = sub->next;
    }

    return NULL;
}





// It has so many works to do , it utilizes all the helper functions defined above 
void *mems_malloc(size_t size) {
    mcNode *c;
    subNode *sn = NULL;
    subNode *snb = NULL;
    mcNode *tc = NULL;
    void *ptr = NULL;
    // First, check if there's enough capacity in existing subnodes
    c = first;
    while (c) {
        sn = c->nextSubChain;
        int result = check_subnode_capacity(c, size);
        if (result == 1) {
            sn = abcd;
            snb  = splitter(size, sn,c);
            return (void *)snb->startAddress; // Return immediately after splitting
        }
        c = c->next;
    }
    // If no existing subnode is suitable, allocate new memory
    int no_pages = getNo_of_pages(size);
    void *mem = mmap(NULL, no_pages * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    tc = add_mcNode(no_pages * PAGE_SIZE, mem);
    sn = tc->nextSubChain;
    snb = splitter(size, sn,tc);
    return (void *)snb->startAddress;
}
void mems_print_stats() {
    mcNode *mainNode = first;
    subNode *subChain = NULL;
    size_t space_unused = 0;
    int pages = 0;
    printf("Printing Stats [mems_print_stats]\n");
    printf("MEMS SYSTEM STATS\n\n");
    int mainChainLength = 0;
    while (mainNode) {
        // Print main chain info
        subChain = mainNode->nextSubChain;
        printf("MAIN%d[%ld:%ld]-> ", mainNode->mchainNo, mainNode->startAddress, mainNode->endAddress);
        // if this does not work then you can make a global variable and update it in getNo_of_pages function since that function will be called only when we need more pages for sure.
        pages += (int)((mainNode->size) / (size_t)PAGE_SIZE);
        mainChainLength++;
        while (subChain) {
            if (subChain->type == 0) {
                printf("H%d[%ld:%ld] <-> ", subChain->schainNo, subChain->startAddress, subChain->endAddress);
                space_unused += (subChain->endAddress - subChain->startAddress + (size_t)1);
                
            }
            else {
                // printf("chk");
                printf("P%d[%ld:%ld] <-> ", subChain->schainNo, subChain->startAddress, subChain->endAddress);
            }
            subChain = subChain->next;
        }
        // Print "NULL" when the subchain ends
        printf("NULL\n");
        mainNode = mainNode->next;
        // Move to the next main chain
        if (mainNode) {
            printf("\n");
        }
    }

    getNo_of_Mapped_pages = pages;
    printf("\nPages used: %d\n", getNo_of_Mapped_pages);
    printf("Space unused: %ld\n", space_unused);
    printf("\n----------------------------------------\n");
}


// int getpagesMapped(){
//     int pages = 0;
//     mcNode *mainNode = first;
//     while (mainNode){
//         pages += (int)((mainNode->size) / (size_t)PAGE_SIZE);
//         mainNode = mainNode->next;
//     }
//     getNo_of_Mapped_pages = pages;
//     retun pages;
// }


void *mems_get(void *v_ptr){
    findSubNodeFromVirtualAddress(v_ptr);
    mcNode *mnp = for_getting_ptr_on_main_node_to_get_physical_address;
    void *rturn = (void *)((uintptr_t)mnp->mems_physical_address + ((uintptr_t)v_ptr - mnp->startAddress));
    return rturn;
}

// it firstly makes the subNode whose pointer is given to it to hole type, then checks adjacent subNodes if anyof them comes out to be a hole then it merges them with the existing node
void Holemerger(subNode *ptr) {
    subNode *temp = NULL;
    subNode *prevfree = NULL;
    if (ptr->type == 1) {
        printf("Node type before mems_free:%d.\n",ptr->type);
        ptr->type = 0;
        printf("Change the type from 1 (process) to 0 (hole) :%d.\n\n\n", ptr->type);
    }else{
        printf("Node is already a hole:%d\n", ptr->type);
    }
    if (ptr->prev != NULL && ptr->prev->type == 0) {
        if ((ptr->prev->endAddress) + 1 == ptr->startAddress) {
            ptr->prev->endAddress = ptr->endAddress;
            ptr->prev->next = ptr->next;
            if (ptr->next != NULL) {
                ptr->next->prev = ptr->prev;
            }
            
            
            prevfree = ptr;
            ptr = ptr->prev; // Update ptr to the merged node
            // free(prevfree);
            
        }
    }
    if(ptr->next != NULL && ptr->next->type == 0){
        if ((ptr->next->startAddress) == (ptr->endAddress) + 1) {
            temp = ptr->next;
            ptr->endAddress = temp->endAddress;
            ptr->next = temp->next;
            if (temp->next != NULL){
                temp->next->prev = ptr;
            }
            // free(temp);
                        
        }
    }
    // Update the subchain numbers after merging
    subNode *sb = ptr;
    int sno = ptr->schainNo;
    while (sb) {
        sb->schainNo = sno;
        sb = sb->next;
        sno++;
    }
}
// it utilises helper functions to achieve freeing of subnodes
void mems_free(void *ptr){
    subNode *pointer_on_subnode_to_be_freed = findSubNodeFromVirtualAddress(ptr);
    Holemerger(pointer_on_subnode_to_be_freed);
    
}
// it firstly stores all the nodes in an array of type mcNode then uses that array to munmap the nodes
void mems_finish() {
    mcNode *currentMain = first;
    while (currentMain != NULL) {
        subNode *currentSub = currentMain->nextSubChain;
        while (currentSub != NULL) {
            subNode *temp = currentSub;
            currentSub = currentSub->next;
            free(temp);
        }
        mcNode *tempMain = currentMain;
        currentMain = currentMain->next;
        munmap(tempMain->mems_physical_address, tempMain->size);
        free(tempMain);
    }

    // Reset global pointers
    first = NULL;
    last = NULL;
    startptr = NULL;
    endptr = NULL;
    abcd = NULL;
}
