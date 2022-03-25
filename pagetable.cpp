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

/*
Input:  Level pointer and the index to create the new Level at, the pageNumber
Output: Void return, but create a new page at the specific index

Description:    This a nice helper function to simplify the confusing code and creates a new page
        in a simple way.
*/
void createPage(Level *rootLevel, unsigned int pageNumber){
    Level *newLevel;
    newLevel = new Level;
    rootLevel->nextLevel[pageNumber] = newLevel;
    newLevel->depth = rootLevel->depth+1;
    newLevel->pgtable = rootLevel->pgtable;
}

/*
Input:  Pagetable pointer and virtual address
Output: the frame value

Description:    Will go through the page table, each level, and set up the path within the tree
        for the virtual address to exist in. When the frame already exists, the pageHit counter will increment.
*/
int insertAddress(PageTable *table, unsigned int virtualAddress){
    unsigned int mask;
    unsigned int pageNum;
    struct Level *currentLevel;
    currentLevel = table->rootLevelPtr;

    for(int i = 0; i < table->levelCount-1; i++){
        mask = table->bitmaskArr[i];
        pageNum = virtualAddressToPageNum(virtualAddress, mask, table->shiftArr[i]);
        table->pages[i] = pageNum;
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
    table->pages[table->levelCount-1] = pageNum;
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

/*
Input:  The amount of bits that the pages take up within the virtual address, and the virtual address
        (virtual address isnt necessarilly used literaly as this function is used to generally get the offset)
Output: Only the offset of the virtual address inputted

Description:    It does what it says.
*/
unsigned int getOffset(unsigned int pageBits, unsigned int virtualAddress){
    unsigned int mask = getMask(31-pageBits, 0);
    unsigned int offset;
    offset = virtualAddress & mask;
    return offset;
}

/*
Input:  lengt of page bits, virtual address input, frame value of address
Output: The entire physical address that is stored, including frame and offset

Description:    Does what it says.
*/
unsigned int getFrameAddr(unsigned int pageBits, unsigned int virtualAddress, unsigned int frame){
    unsigned int frameAddr;

    frameAddr = frame;
    frameAddr = frameAddr << 32-pageBits;
    frameAddr = frameAddr | getOffset(pageBits, virtualAddress);

    return frameAddr;
}

/*
Input:  The length of bits used, the virtual address
Output: The address of only the page bits section, excludes offset portion

Description:    Transforms the virtual address to exclude the offset section.
*/
unsigned int getPageBits(unsigned int pageBits, unsigned int virtualAddress){
    return virtualAddress & getMask(31, pageBits);
}

/*
Description:    Edited version from output helper file, since it was throwing errors
            when I adjusted some of the parameters to work for my map method.
*/
void report_pages(int levels, unordered_map<uint32_t, uint32_t> pages, uint32_t frame) {
  /* output pages */
  for (int idx=0; idx < levels; idx++)
    printf("%X ", pages[idx]);
  /* output frame */
  printf("-> %X\n", frame);

  fflush(stdout);
}

/*
Input:  table pointer, the virtual address, total amount of bits pages use
Output: Returns true if the frame is stored within TLB, otherwise false

Description:    Utilizes the linked list data structure to easily iterate through and check data.
        This function only iterates to check for existence, no editing is done in this.
*/
bool checkTLB(PageTable* table, unsigned int address, unsigned int totBits){
    tlb_node* addressPtr;
    addressPtr = table->tlbPtr;
    while(addressPtr->nextNode != NULL && addressPtr->virtualAddress != 0){
        if(getPageBits(totBits, address) == getPageBits(totBits, addressPtr->virtualAddress)){
            return true;
        }
        addressPtr = addressPtr->nextNode;
    }
    if(getPageBits(totBits, address) == getPageBits(totBits, addressPtr->virtualAddress)){
        return true;
    }
    return false;
}

/*
Input:  table pointer, an address to add, the length of bits for pages, a frame to assign if applicable
Output: 

Description:    
*/
int adjustTLB(PageTable* table, unsigned int address, unsigned int totBits, unsigned int frameVal, int tlbMaxSize){

    tlb_node* tlbPtr;
    tlb_node* prevPtr;
    tlbPtr = table->tlbPtr;
    prevPtr = NULL;

    // nothing is in the linked list yet so inital node is created here
    if(table->tlbSize == 0){
        tlbPtr->nextNode = NULL;
        tlbPtr->frame = frameVal;
        tlbPtr->virtualAddress = address;
        table->tlbSize++;
        return frameVal;
    }

    while(tlbPtr->nextNode != NULL){
        if(getPageBits(totBits, address) == getPageBits(totBits, tlbPtr->virtualAddress)){
            break;
        }
        prevPtr = tlbPtr;
        tlbPtr = tlbPtr->nextNode;
    }

    // if the address hasnt been inputted and the cache isnt at the max size yet
    if(table->tlbSize < tlbMaxSize){
        tlbPtr->virtualAddress = address;
        tlbPtr->frame = frameVal;
        tlbPtr->nextNode = table->tlbPtr;
        table->tlbPtr = tlbPtr;
        table->tlbSize++;
    }
    // the nodes will readjust and move the tlbPtr to the front of the Linked List
    if(prevPtr != NULL){
        prevPtr->nextNode = tlbPtr->nextNode;
        tlbPtr->nextNode = table->tlbPtr;
        table->tlbPtr = tlbPtr;
    }
    // return the frame at the front of the linked list
    return table->tlbPtr->frame;
}