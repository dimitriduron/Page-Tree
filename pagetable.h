#include <iostream>
#include <stdio.h>
using namespace std;

/*
PageTable
    
Main struct that will hold all data pertaining to the page table levels
and other important data


*/
typedef struct PageTable{
    int *levelCount;
    int bitmaskArr[3];
    int shiftArr[3];
    long entrycountArr[3];
};

typedef struct Level{
    int *depth;
    PageTable *pgtable;
    Level *nextLevel;
};

unsigned int virtualAddressToPageNum(unsigned int, unsigned int, unsigned int);