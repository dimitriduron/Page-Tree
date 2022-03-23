#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <math.h>
using namespace std;

/*
PageTable
    
Main struct that will hold all data pertaining to the page table levels
and other important data


*/
typedef struct PageTable{
    int levelCount;
    unordered_map<int, unsigned int> bitmaskArr;
    unordered_map<int, int> shiftArr;
    unordered_map<int, unsigned int> entrycountArr;
};

typedef struct Level{
    int *depth;
    PageTable *pgtable;
    unordered_map<unsigned int, Level> *nextLevel;
};

unsigned int virtualAddressToPageNum(unsigned int, unsigned int, unsigned int);
unsigned int getMask(unsigned int, unsigned int);