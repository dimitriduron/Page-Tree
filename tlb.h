#include <stdio.h>
/*

*/
typedef struct tlb_node{
    unsigned int virtualAddress;
    unsigned int frame;
    struct tlb_node *nextNode;
};

// struct helper functions
tlb_node* createCache(int);