#include "tlb.h"

tlb_node* createNode(){
    tlb_node* node;
    node = new tlb_node;
    node->nextNode = NULL;
    return node;
}

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