#include "pagetable.h"
/*
Input:  2 hex values, the first being the virtual address and the second being the mask hex
        1 value to shift by that amount of bits
Output: a hex value to represent the page number

Description:    Given a virtual address, get the page number for that virtual address. This is done
        by anding the virtual address with the mask, then shifted til the right most active bit is
        to the farthest right bit location.
*/
unsigned int virtualAddressToPageNum(unsigned int virtualAddress, unsigned int mask, unsigned int shift){
    return (virtualAddress & mask) >> shift;
}

/*
Input:  2 numbers, one being the left most bit and the right being the right most bit to mask
Output: return a hex value where bits between left and right are all 1, other bits are 0

Description:    To get the mask, we will want to make sure the amount of bits being considered is correct,
        which is an inclusive range of bits. When shifting over the bits to the correct placement, we have
        to compensate for this inclusive portion by subtracting 1 from the shift.
*/
unsigned int getMask(unsigned int left, unsigned int right){
    unsigned int mask = 0x0;
    unsigned int bitAmount = left-right+1;

    for(int i = 0; i < bitAmount; i++){
        mask++;
        mask = mask << 1;
    }
    mask = mask << right-1;
    return mask;
}

void insertPage(PageTable *table, unsigned int virtualAddress, unsigned int frame){
    unsigned int mask;
    for(int currentLevel = 0; currentLevel < table->levelCount; currentLevel++){
        mask = table->bitmaskArr[currentLevel];
        

    }
}

void insertAddress(PageTable *table, unsigned int virtualAddress, unsigned int frame){

}