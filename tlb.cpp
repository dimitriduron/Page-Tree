#include "tlb.h"
/*
Creates and intializes a node
*/
tlb_node* createNode(){
    tlb_node* node;
    node = new tlb_node;
    node->nextNode = NULL;
    return node;
}
/*
Sets up the cache to have n amount of nodes with empty values
*/
tlb_node* createCache(int num){
    tlb_node* rootNode;
    rootNode = new tlb_node;
    tlb_node* tempNode;
    tempNode = rootNode;
    for(int i = 0; i < num-1; i++){
        tempNode->nextNode = createNode();
        tempNode = tempNode->nextNode;
    }
    return rootNode;
}