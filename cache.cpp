#include<iostream>
#include <cmath>
#include <stdlib.h>
#include <bits/stdc++.h>
#include "cache.h"

//pointer to the cache block
typedef	struct cacheBlock* cache_block;

//converting hexadeciomal string to binary string
string hextoBinary(string hex){
    string binary;
    for(int i=0;i<hex.length();i++){
        if(i>1){
            switch(hex[i]){
                case '0':
                    binary += "0000";
                    break;
                case '1':
                    binary += "0001";
                    break;
                case '2':
                    binary += "0010";
                    break;
                case '3':
                    binary += "0011";
                    break;
                case '4':
                    binary += "0100";
                    break;
                case '5':
                    binary += "0101";
                    break;
                case '6':
                    binary += "0110";
                    break;
                case '7':
                    binary += "0111";
                    break;
                case '8':
                    binary += "1000";
                    break;
                case '9':
                    binary += "1001";
                    break;
                case 'a':
                    binary += "1010";
                    break;
                case 'b':
                    binary += "1011";
                    break;
                case 'c':
                    binary += "1100";
                    break;
                case 'd':
                    binary += "1101";
                    break;
                case 'e':
                    binary += "1110";
                    break;
                case 'f':
                    binary += "1111";
                    break;
                default:
                    break;
            }
        }
    }
    return binary;
}

//converting the Decimal number to binary string
string DecimalToBinary(int n) {
    string binaryNumber;
    int num=n;
    int i = 0;
    while (n > 0) {
        binaryNumber += std::to_string(n%2);
        n = n / 2;
        i++;
    }
    reverse(binaryNumber.begin(),binaryNumber.end());
    return binaryNumber;
}

//converting the binary to decimal number
int binarytoDec(string binaryString){
    int value = 0;
    int indexCounter = 0;
    for(int i=(int)binaryString.length()-1;i>=0;i--){
        if(binaryString[i]=='1'){
            value += pow(2, indexCounter);
        }
        indexCounter++;
    }
    return value;
}

//for tree based LRU
//On a cache hit this updates the upper bits
//Returns true in case of cache hit
bool Check(vector<vector<int>> arr,int setIndex,int tag,int ways)
{
    bool flag=false;
    for (long int i = ways; i <2*ways; ++i)
    {
        if(arr[setIndex][i]==tag)
        {
            flag=true;
            while(i!=1)
            {
                arr[setIndex][i/2]=1-(i%2);
                i=i/2;
            }
            break;
        }
    }
    return flag;
}

//for tree based LRu
//it may be 0 or the tag if its eviction then tag is returned else 0
int treeLRU(vector<vector<int>> arr,int setIndex,int tag,int ways)
{
    long int i=1;
    while(i<ways)
    {
        if(arr[setIndex][i]==0)
        {
            arr[setIndex][i]=1;
            i=2*i;
        }
        else if(arr[setIndex][i]==1)
        {
            arr[setIndex][i]=0;
            i=2*i+1;
        }
    }
    int a=arr[setIndex][i];
    arr[setIndex][i]=tag;
    return a;
}

//constructor
Cache::Cache(int Cachesize, int Blocksize, int associativity, int Replacementpolicy){
    CacheSize = Cachesize;
    BlockSize = Blocksize;
    Associativity = associativity;
    ReplacementPolicy = Replacementpolicy;
    int totalBlocks = CacheSize/BlockSize;
    ways = totalBlocks;
    //total number of blocks and ways according to cache and block size
    if(Associativity == 0){
        totalSets = 1;
        //fully Associative cahce has only one set
    }else if(Associativity == 1){
        totalSets = totalBlocks;
        ways = 1;
        //the number of ways for direct mapped is 1
    }else{
        totalSets = totalBlocks/Associativity;
        ways = Associativity;
        // the ways in set associative depends on Associativity value here
        //which is greater than 2

    }
    indexSize = (int)log2(totalSets);
    //log of total set is the binary digits required to show the index number
    tagSize = 32-((int)log2(Blocksize)+indexSize);
    //removing the block off set and index remaining will be the tag
    sets = new cache_block[totalSets];
    //sets stores the array lo linked list
    for(int k=0; k < totalSets; k++){
        cache_block setHead = nullptr;
        for(int j=0; j < ways; j++){
            auto newCacheBlock = new cacheBlock;
            //new cache block is created int the linked list
            newCacheBlock->validBit = 0;
            newCacheBlock->dirtyBit = 0;
            newCacheBlock->pointer = setHead;
            setHead= newCacheBlock;
        }
        sets[k] = setHead;
    }
    //for tree based LRUwe need an extra array to store
    if(ReplacementPolicy == 2){
        array = vector<vector<int>>(totalSets, vector<int>(2*ways));
    }
}

//access method of cache class
void Cache::accesses(string address, char ch) {
    totalAccesses++;
    //Accessed
    string binaryAddress = hextoBinary(address);
    //binary address of the memory address
    string tagElement, tempSetIndex;
    int setIndex = 0;
    for(int i=0;i<tagSize;i++){
        tagElement += binaryAddress[i];
    }
    for(int i=0;i<indexSize;i++){
        tempSetIndex += binaryAddress[tagSize + i];
    }
    //divided into the tag and setindex
    setIndex = binarytoDec(tempSetIndex);
    //decimal number of setIndex
    bool read = false;
    bool write = false;
    //set read and write to false
    if(ch == 'r'){
        read = true;
        //if read is instruction
        //inc the read accesses
        readAccesses++;
    }else{
        write = true;
        //if write is the instruction then write accessess are increased
        writeAccesses++;
    }
    bool foundInAll = false;
    bool capacityAndConflicts = false;
    for(auto & element : allElements){
        if(element == (tagElement+tempSetIndex)){
            foundInAll = true;
        }
    }
    //checking for compulsory misses
    if(!foundInAll){
        capacityAndConflicts = true;
        compulsoryMisses++;
        allElements.push_back(tagElement + tempSetIndex);
    }
    bool check = false;
    if(ReplacementPolicy == 2){
        check = Check(array, setIndex, binarytoDec(tagElement), ways);
    }
    bool found = false;
    cache_block temp = sets[setIndex];
    cache_block prev = temp;
    if(!check) {
    	//checking for the tag in the setIndex given
    	//transversing throught the linked list to find the tag
        while (temp != nullptr) {
            if (temp->tag == tagElement) {
                if (write) {
                    temp->dirtyBit = 1;
                    //if found and write is the instruction then dirty bit is one
                }
                found = true;
                //making the found block to the first in the linked list
                if (ReplacementPolicy == 1) {
                    if (prev == temp) {
                        sets[setIndex] = temp;
                        break;
                    }
                    prev->pointer = temp->pointer;
                    temp->pointer = sets[setIndex];
                    sets[setIndex] = temp;
                    break;
                }
            }
            prev = temp;
            temp = temp->pointer;
        }
    }
    if(!found){
    	//if the element is not found
        if(read) {
        	//if the instruction is read then read miss
            readMisses++;
        }else {
        	//if the instruction is write then write miss
            writeMisses++;
        }
        //either read or write it is a cache miss
        cacheMisses++;
        if(!capacityAndConflicts){
            if(Associativity == 1){
            	//As the Associativity has only 1 way so the capcity misses occurs
                capacityMisses++;
            }else {
            	//conflict misses are not for direct mapped
                conflictMisses++;
            }
        }
        //create the new cache block to insert into the linked list
        auto temp1 = new cacheBlock;
        temp1->tag = tagElement;
        temp1->dirtyBit = 0;
        if(write){
            temp1->dirtyBit = 1;
            //if it is write we make the dirty bit as 1
        }
        temp1->validBit = 1;
        if(ReplacementPolicy == 0) {
        	//for Random
        	//we select a random way in the set store or replace from that way
            temp = sets[setIndex];
            prev = temp;
            int random = rand()%ways;
            for(int i=0;i<random;i++){
                prev = temp;
                temp = temp->pointer;
            }
            //if the removed block has dirtybit 1 then it is evicted
            if(temp->dirtyBit == 1){
                dirtyBlocksEvicted++;
            }
            //remove the old cache block from the linked list
            temp1->pointer = temp->pointer;
            prev->pointer = temp1;
        }
        else if(ReplacementPolicy == 1) {
            //LRU
            //for LRU the new block is stored at start
            //and the end block is evicted
            temp1->pointer = sets[setIndex];
            sets[setIndex] = temp1;
            temp = sets[setIndex];
            prev = temp;
            while(temp->pointer != nullptr){
                prev = temp;
                temp = temp->pointer;
            }
            //if the dirty bit is 1 for evict bit
            //then it means that dirtyBlock is evict
            if(temp->dirtyBit == 1){
                dirtyBlocksEvicted++;
            }
            //last but one is pointed to nullptr as it will be the new last
            prev->pointer = nullptr;
        }else if(ReplacementPolicy == 2) {
            int a = treeLRU(array, setIndex, binarytoDec(tagElement), ways);
            temp1->tag = DecimalToBinary(a);
            temp = sets[setIndex];
            prev = temp;
            while(temp->pointer != nullptr){
                if(temp->tag == tagElement){
                    if(temp->dirtyBit == 1){
                        dirtyBlocksEvicted++;
                    }
                    temp1->pointer = temp->pointer;
                    prev->pointer = temp1;
                }
                prev = temp;
                temp = temp->pointer;
            }
        }
    }
}

void Cache::printStats(string fileName){
    fstream outputFile;
    outputFile.open(fileName, ios::out);
    //opeing the file and printing the stats to the file
    outputFile <<  "total Accesses : " << totalAccesses << endl;
    outputFile <<  "read Accesses : " << readAccesses << endl;
    outputFile <<  "write Accesses : " << writeAccesses << endl;
    outputFile <<  "cache Misses : " << cacheMisses << endl;
    outputFile <<  "compulsory Misses :" << compulsoryMisses << endl;
    outputFile <<  "capacityMisses : " << capacityMisses  << endl;
    outputFile <<  "conflict Misses : " << conflictMisses << endl;
    outputFile <<  "read Misses : " << readMisses << endl;
    outputFile <<  "write misses : " << writeMisses << endl;
    outputFile <<  "dirty blocks evicted : " << dirtyBlocksEvicted << endl;
    outputFile.close();
}
