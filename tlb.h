#include <stdio.h>
/*

*/
typedef struct tlb_node{
    unsigned int virtualAddress = 0;
    unsigned int frame = NULL;
    struct tlb_node *nextNode;
};

// struct helper functions
tlb_node* createCache(int);