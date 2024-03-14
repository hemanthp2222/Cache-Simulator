#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "cache.h"

using namespace std;


int main(){
	int CacheSize=0, BlockSize=0, Associativity=0, ReplacementPolicy=0;
    string filename;
	fstream inputfile;
	string file;
    //new.txt contains cahce size and block size with Associativity to be used and the Replacement Policy
    //and at last contains the file name of the text file containing the memory traces
	inputfile.open("new.txt", ios::in);
    inputfile >> CacheSize >> BlockSize >> Associativity >> ReplacementPolicy >> filename;
    inputfile.close();
    //initialising the cahce using the constructor of class
    //with the cachesize , block size, associativity and replacement Policy
	Cache A(CacheSize, BlockSize, Associativity, ReplacementPolicy);
    fstream memoryFile;
    //the file name is used to get the memory traces that needs to be implemented with instruction
    memoryFile.open(filename, ios::in);
    string instruction;
    //vector list to store the hexadecimal address and the instruction
    vector<string> inst(2,"");
    while(getline(memoryFile, instruction)){
        //both are stored in a line
        stringstream ss(instruction);
        string word;
        inst.clear();
        //clearing the instructions of previous
        while(ss >> word){
            //storing the instruction dividesd by the spaces into vector
            inst.push_back(word);
        }
        //accesses is the method to access the cache sets and blocks
        //accordingly with the instruction of read or write
        A.accesses(inst[0], inst[1][0]);
    }
    memoryFile.close();
    //will be creating a output file named output.txt to store the stats
    //will be using the output.txt if already exists
    A.printStats("output.txt");
    //printing the stats to output.txt
	return 0;
}


//CS19B051 NUTHAN NAIK
//CS19B066 HEMANTH