#ifndef CACHE_H
#define CACHE_H
#include <iostream>
#include <vector>
// Cahce header file
using namespace std;

//sturture of the cahce Block
//Cache block which has tag, validbit, dirtybit and a pointer to next cacheblock
struct cacheBlock{
	string tag;
	int validBit;
	int dirtyBit;
	struct cacheBlock * pointer;
};
typedef	struct cacheBlock* cache_block;


///------------------  Cache Class ---------------------- ///

class Cache{
private:
    int CacheSize, BlockSize, Associativity, ReplacementPolicy;
    int tagSize, indexSize;
    //tag size and the index size of the memory address
    int totalSets,ways;
    //sets and ways of the cahce
    vector<vector<int>> array;//for pseudo tree LRU

    //=== stroing stats === //
    int totalAccesses=0;
    int readAccesses=0;
    int writeAccesses=0;
    int cacheMisses=0;
    int compulsoryMisses=0;
    int conflictMisses=0;
    int capacityMisses = 0;
    int readMisses=0;
    int writeMisses=0;
    int dirtyBlocksEvicted=0;
    
    //storing the all the elements that got into the cache
    //to identify the conflict and compulsory misses and capacity misses
    vector<string> allElements;

    //Stores the all the cahce data in array of linked list
    cache_block * sets;
public:
    // constructor
    Cache(int,int,int,int);
    // accesses of intruction throught this method
    void accesses(string,char);
    //printing the stats of the instruction done
    //to the asked file
    void printStats(string);
};

#endif //CACHE_H