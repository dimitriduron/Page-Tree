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
    unsigned int mask = 0;
    unsigned int bitAmount = left-right+1;

    for(int i = 0; i < bitAmount; i++){
        mask++;
        mask = mask << 1;
    }
    // the if is for the case that Im getting a mask for the offset bits
    if(right == 0) return mask >> 1;
    mask = mask << right-1;
    return mask;
}

void createPage(Level *rootLevel, unsigned int pageNumber){
    Level *newLevel;
    newLevel = new Level;
    rootLevel->nextLevel[pageNumber] = newLevel;
    newLevel->depth = rootLevel->depth+1;
    newLevel->pgtable = rootLevel->pgtable;
}

int insertAddress(PageTable *table, unsigned int virtualAddress){
    unsigned int mask;
    unsigned int pageNum;
    struct Level *currentLevel;
    currentLevel = table->rootLevelPtr;

    for(int i = 0; i < table->levelCount-1; i++){
        mask = table->bitmaskArr[i];
        pageNum = virtualAddressToPageNum(virtualAddress, mask, table->shiftArr[i]);

        //returns true if the next level doesnt exist
        if(currentLevel->nextLevel.find(pageNum) == currentLevel->nextLevel.end()){
            createPage(currentLevel, pageNum);
            //cout << "page inserted at " << hex << pageNum << dec << endl;
        }
        
        currentLevel = currentLevel->nextLevel[pageNum];
    }
    
    //lowest level we should be accessing, we need to check for frameMap existence now
    mask = table->bitmaskArr[table->levelCount-1];
    pageNum = virtualAddressToPageNum(virtualAddress, mask, table->shiftArr[table->levelCount-1]);
    //frame doesnt exist, so we dump frame in here and increment
    if(currentLevel->frameMap.find(pageNum) == currentLevel->frameMap.end()){
        currentLevel->frameMap[pageNum] = table->frameNum;
        table->frameNum++;
    }
    else{
        table->pageHits++;
    }
    return currentLevel->frameMap[pageNum];
}

unsigned int getOffset(unsigned int pageBits, unsigned int virtualAddress){
    unsigned int mask = getMask(31-pageBits, 0);
    unsigned int offset;
    offset = virtualAddress & mask;
    return offset;
}

unsigned int getFrameAddr(unsigned int pageBits, unsigned int virtualAddress, unsigned int frame){
    unsigned int frameAddr;

    frameAddr = frame;
    frameAddr = frameAddr << 32-pageBits;
    frameAddr = frameAddr | getOffset(pageBits, virtualAddress);

    return frameAddr;
}