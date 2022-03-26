#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//included header files
#include "tracereader.h"
#include "output_mode_helpers.h"
#include "pagetable.h"


//helper functions
extern bool checkForTracer(string);
extern bool isNumber(string);
extern int outputCheck(string);
extern bool isNegative(string);

int main(int argc, char **argv){
    //variables used throughout the program
    int n = -1;
    int c = 0;
    int o = 0;
    unordered_map<int, int> level;

    int marker = -1;
    int totBits = 0;
    int num;
    int lvlNum;
    int prev_arg = -1;
    string temp_arg;
    int filename_index;

    //******COMMAND LINE READING******//
    // reads the command line and makes sure all inputs are valid, optional and mandatory
    for(int i = 1; i < argc; i++){
        
        temp_arg = argv[i];
        
        //mandatory argument conditions
        if(checkForTracer(temp_arg)){
            ifstream trace_file(temp_arg);
            if(!trace_file.good()){
                cout << "Unable to open <<" + temp_arg << ">>" << endl;
                return 0;
            }
            filename_index = i;
            trace_file.close();
            //all arguments following the .tr file are used to specify the page levels and their bit size
            marker = i;
        }
        else if(marker != -1){
            //num holds the amount of bits on the page
            num = stoi(temp_arg);
            lvlNum = i-marker-1;
            if(num < 1){
                //
                cout << "Level " << lvlNum << " page table must be at least 1 bit" << endl;
                return 0;
            }
            level[lvlNum] = num;
            totBits += num;
            
            //check for the next argv still being a number or within argc range
            if(argc > i+1){
                temp_arg = argv[i+1];
                if(!isNumber(temp_arg)) marker = -1;
            }
        }

        //optional argument conditions
        else if(temp_arg.compare("-n") == 0) prev_arg = 1;
        else if(temp_arg.compare("-c") == 0){
            
            prev_arg = 2;
            c = 0;
        }
        else if(temp_arg.compare("-o") == 0){
            prev_arg = 3;
            o = 6;
        }
        else if(isNumber(temp_arg) || isNumber(temp_arg.substr(1))){

            if(isNegative(temp_arg))    num = -stoi(temp_arg.substr(1));
            else                        num = stoi(temp_arg);

            if(prev_arg == 1)       n = num;
            else if(prev_arg == 2){
                c = num;
                if(c < 0){
                    cout << "Cache capacity must be a number, greater than or equal to 0" << endl;
                    return 0;
                }
            }
            prev_arg = -1;
        }
        else if(outputCheck(temp_arg) > 0){
            o = outputCheck(temp_arg);
            prev_arg = -1;
        }
    }
    if(totBits > 28){
        cout << "Too many bits used in page tables" << endl;
        return 0;
    }


    //*******MAIN LOGIC*********//
    //initialize the pagetable struct
    struct PageTable *pgtable;
    pgtable = new PageTable;
    struct Level *tempLevel;
    tempLevel = new Level;
    tempLevel->depth = 0;
    tempLevel->pgtable = pgtable;
    pgtable->rootLevelPtr = tempLevel;
    
    //initialize the TLB if -c is checked
    if(c)   pgtable->tlbPtr = createCache(c);
    else    pgtable->tlbPtr = NULL;

    pgtable->levelCount = lvlNum+1;
    //31 represents the 32nd bit of the hex values/addresses
    int temp_num = 31;
    
    // fills in data for PageTable struct
    for(int i = 0; i <= lvlNum; i++){
        
        pgtable->entrycountArr[i] = pow(2, level[i]);
        pgtable->shiftArr[i] = temp_num-level[i]+1;
        pgtable->bitmaskArr[i] = getMask(temp_num, pgtable->shiftArr[i]);

        temp_num -= level[i];
    }
    
    // read in trace file
    p2AddrTr mtrace;
    unsigned int vAddr;

    FILE *testFile;
    testFile = fopen(argv[filename_index], "r");
    unsigned int addressCount = 0;
    unsigned int cacheHits = 0;
    unsigned int pageHits = 0;
    unsigned int tempFrame = 0;
    unsigned int frameNum = 0;
    unsigned int offset;
    unsigned int frameAddr;
    bool found;

    //***PROCESS ADDRESSES***//
    while(!feof(testFile) && (n > 0 || n == -1)){
        if(NextAddress(testFile, &mtrace)){
            vAddr = mtrace.addr;
            addressCount++;

            //cout << hex << vAddr << endl;
            //******TLB SECTION*******//
            if(c){
                found = checkTLB(pgtable, vAddr, totBits);
                if(found){
                    cacheHits++;
                    // we use 0 for frame parameter because we wont encounter the case where the node doesnt exist
                    frameNum = adjustTLB(pgtable, vAddr, totBits, 0);
                }
            }
            if(!c || !found){
                tempFrame = frameNum;
                frameNum = insertAddress(pgtable, vAddr);
                if(c){
                    adjustTLB(pgtable, vAddr, totBits, frameNum);
                }
            }

            // check for inactive TLB or if TLB hasnt found anything
            // frameNum = insertAddress(pgtable, vAddr);
            
            
            // when -o is set to offset, we need to print the offset of each 
            if(o == 5){
                hexnum(getOffset(totBits, vAddr));
            }
            else if(o == 2){
                frameAddr = getFrameAddr(totBits, vAddr, frameNum);
                report_virtual2physical(vAddr, frameAddr);
            }
            else if(o == 4){
                report_pages(pgtable->levelCount, pgtable->pages, frameNum);
            }
            else if(o == 3){
                frameAddr = getFrameAddr(totBits, vAddr, frameNum);
                report_v2pUsingTLB_PTwalk(vAddr, frameAddr, found, (tempFrame == frameNum));
            }
            found = false;
        }
        if(n > 0) n--;
    }


    //***FINAL OUTPUT SECTION***//
    //basic summary output, command line default output
    if(o == 0){
        //parameters: pagesize, cachehits, pagetablehits, addresses, frames_used, bytes
        report_summary(pow(2, 32-totBits), cacheHits, pgtable->pageHits, addressCount, pgtable->frameNum, 1);
    }
    //bitmask situation
    else if(o == 1){
        report_bitmasks(pgtable->levelCount, pgtable->bitmaskArr);
    }
    
    
    return 0;
}



/*
Input: Takes in a string, expected to be from the command line
Output: true if the file ends with .tr, false otherwise
*/
bool checkForTracer(string input){
    if(input.length() > 3){
        if(input[input.length()-1] == 'r' && input[input.length()-2] == 't'){
            return true;
        }
    }
    return false;
}

/*
Input: takes in a string input to scan each char for being a number
Output: True if the entire string is a number, false otherwise
*/
bool isNumber(string s){
    string::const_iterator it = s.begin();
    while(it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

/*
Input: takes in a string, which is expected to be from the command line
Output: returns an int from 1-6 if valid string, otherwise returns 0
*/
int outputCheck(string s){
    if(s.compare("bitmasks") == 0)              return 1;
    else if(s.compare("virtual2physical") == 0) return 2;
    else if(s.compare("v2p_tlb_pt") == 0)       return 3;
    else if(s.compare("vpn2pfn") == 0)          return 4;
    else if(s.compare("offset") == 0)           return 5;
    else if(s.compare("summary") == 0)          return 0;
    return 0;
}

/*
input: check for string to be negative, expected to be a number
output: yes if the first char is -, otherwise false
*/
bool isNegative(string s){
    return s[0] == '-';
}