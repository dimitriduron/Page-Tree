#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//helper functions
extern bool checkForTracer(string);

int main(int argc, char **argv){
    //variables used throughout the program
    int n = -1;
    int c = -1;
    int marker = -1;
    int totBits = 0;
    int num;
    int lvlNum;
    int level[3] = {0, 0, 0};
    int prev_arg = -1;
    string temp_arg;
    cout << argc << endl;

    // reads the command line and makes sure all inputs are valid, optional and mandatory
    for(int i = 1; i < argc; i++){
        
        temp_arg = argv[i];
        cout << temp_arg << endl;
        
        //mandatory argument conditions
        if(checkForTracer(temp_arg)){
            ifstream f(temp_arg);
            if(!f.good()){
                cout << "Unable to open <<" + temp_arg << ">>" << endl;
                return 0;
            }
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
            totBits += num;
        }
        //optional argument conditions

        //recognize the argument types
        else if(temp_arg.compare("-n")) prev_arg = 1;
        else if(temp_arg.compare("-c")) prev_arg = 2;
        else if(temp_arg.compare("-o")) prev_arg = 3;
        else                            prev_arg = -1;
    }
    if(totBits > 28){
        cout << "Too many bits used in page tables" << endl;
        return 0;
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