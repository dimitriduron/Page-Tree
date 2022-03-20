#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//helper functions
extern bool checkForTracer(string);
extern bool isNumber(string);
extern int outputCheck(string);
extern bool isNegative(string);

int main(int argc, char **argv){
    //variables used throughout the program
    int n = -1;
    int c = -1;
    int o = -1;
    int level[3] = {0, 0, 0};

    int marker = -1;
    int totBits = 0;
    int num;
    int lvlNum;
    int prev_arg = -1;
    string temp_arg;

    // reads the command line and makes sure all inputs are valid, optional and mandatory
    for(int i = 1; i < argc; i++){
        
        temp_arg = argv[i];
        //cout << temp_arg << endl;
        
        //mandatory argument conditions
        if(checkForTracer(temp_arg)){
            ifstream f(temp_arg);
            if(!f.good()){
                cout << "Unable to open <<" + temp_arg << ">>" << endl;
                return 0;
            }
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
            if(lvlNum == 2) marker = -1;
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

    cout << c << endl;
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
    else if(s.compare("summary") == 0)          return 6;
    else                                        return 0;
}

/*
input: check for string to be negative, expected to be a number
output: yes if the first char is -, otherwise false
*/
bool isNegative(string s){
    return s[0] == '-';
}