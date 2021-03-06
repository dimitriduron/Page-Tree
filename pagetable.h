#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <math.h>
#include "tlb.h"
using namespace std;

/*
PageTable
    
Main struct that will hold all data pertaining to the page table levels
and other important data

levelCount will be a relatively small int, 0-levelCount will be a page to iterate to within the address
bitmaskArr will have the mask for a specific index
shiftArr will be what to shift to from the right to get to the right side of the address
entryCountArr holds 2^bit input because the page can hold that many address

*/
typedef struct PageTable{
    int levelCount;
    unsigned int totSize = 0;
    int TLBsize = 0;
    unsigned int frameNum = 0;
    unsigned int pageHits = 0;
    unordered_map<int, unsigned int> bitmaskArr;
    unordered_map<int, int> shiftArr;
    unordered_map<int, unsigned int> entrycountArr;
    unordered_map<unsigned int, unsigned int> pages;
    struct Level *rootLevelPtr;
    struct tlb_node* tlbPtr;
};

/*
Level is meant to represent a page within the page table, the map to nextLevel points to child Levels
and frameMap is for the bottom/leaf Level to store the frame value into within the tree
*/
typedef struct Level{
    int depth;
    struct PageTable *pgtable;
    unordered_map<unsigned int, Level*> nextLevel;
    unordered_map<unsigned int, unsigned int> frameMap;
};

// a lot of helper functions
unsigned int virtualAddressToPageNum(unsigned int, unsigned int, unsigned int);
unsigned int getMask(unsigned int, unsigned int);
unsigned int getOffset(unsigned int, unsigned int);
unsigned int getFrameAddr(unsigned int, unsigned int, unsigned int);
unsigned int getPageBits(unsigned int, unsigned int);
void report_bitmasks(int, unordered_map<int, unsigned int>);
void report_pages(int levels, unordered_map<uint32_t, uint32_t> pages, uint32_t frame);
void createPage(Level*, unsigned int, PageTable*);
int insertAddress(PageTable*, unsigned int);
bool checkTLB(PageTable*, unsigned int, unsigned int);
unsigned int adjustTLB(PageTable*, unsigned int, unsigned int, unsigned int);