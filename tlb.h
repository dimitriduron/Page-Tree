#include <stdio.h>
/*

*/
typedef struct tlb_node{
    unsigned int virtualAddress = -1;
    unsigned int frame = -1;
    struct tlb_node *nextNode;
};