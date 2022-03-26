#include <stdio.h>
/*
This struct is meant to be the nodes within the TLB linked list
Only holds onto the important information such as an address and the frame associated with it.
*/
typedef struct tlb_node{
    bool active = false;
    unsigned int virtualAddress;
    unsigned int frame;
    struct tlb_node *nextNode;
};

// struct helper functions
tlb_node* createCache(int);