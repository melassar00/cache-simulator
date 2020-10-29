/*
    Muhamad Elassar
    Project 1 - Cache Simulator
    11/4/2019
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct cacheline {
    bool validbit;
    bool dirtybit;
    uint32_t data; 
    unsigned int used;  // count for LRU
} cacheline;

double reads = 0;
double writes = 0;
double writebacks = 0;
double readHits = 0;
int writeHits = 0;
double readMisses = 0;
double writeMisses = 0;
int cacheSize = 0;    
int associativity = 0;
int blocksize = 64;   
int set = 0;          
int words = 0;        
int writePolicy = 0;
int replacementPolicy = 0;
unsigned int usedcount = 0;

void printInfo();
void hitcheck(int32_t address, cacheline ** cache, bool write);
int calculateIndex();
void simulateCache(char * filename, cacheline ** cache);

int main(int argc, char ** argv) {                              
	cacheline ** cache;

    cacheSize = atoi(argv[1]);
    associativity = atoi(argv[2]);
    replacementPolicy = atoi(argv[3]);
    writePolicy = atoi(argv[4]);

    set = cacheSize/associativity/blocksize;
    words = blocksize / 4; //4 bytes per word

    cache = (cacheline **) malloc(sizeof(cacheline*) * set);
    int i;
    for(i = 0; i < set; i++) {
        cache[i] = (cacheline*) malloc(sizeof(cacheline) * associativity);
    }
    
    int j, k;
	for(i = 0; i < set; i++) {
        for(j = 0; j < associativity; j ++) {
            for (k = 0; k < words; k++) {
                cache[i][j].data = 0x0;
                cache[i][j].validbit = false;
                cache[i][j].dirtybit = false;
                cache[i][j].used = UINT32_MAX;
            }
        }
	}
    
    simulateCache(argv[5], cache);
    
    printInfo();

    return 0;
}

void simulateCache(char * filename, cacheline ** cache) {
    FILE *inputfile;
    inputfile = fopen(filename, "r");
    
    if (inputfile == NULL) {
        printf("Error: %s file is not found", filename);
        exit(1);
    }
    
    
    char * cacheline = NULL;
    size_t len = 0;
    
    while ((getline(&cacheline, &len, inputfile)) != EOF) {
        bool writemode = true;
        if (cacheline[0] == 'R') {
            writemode = false;
        } else if (cacheline[0] == 'W') {
            writemode = true;
        } else {
            printf("Wrong input : %s", cacheline);
            exit(1);
        }
    
        int32_t addr = (int32_t) strtol(cacheline + 2, NULL, 0);
        
        hitcheck(addr, cache, writemode);
    }
}

void hitcheck(int32_t address, cacheline ** cache, bool write) {
    if (write) 
        writes++;
    else 
        reads++;
    
    int index = (address) >> 6;
    unsigned int mask = 0;
    for (unsigned i=0; i < calculateIndex(); i++) {
        mask |= 1 << i;
    }
    index = index & mask;
    
    unsigned int mask2 = 0;
    for (unsigned i=0; i < 6; i++) {
        mask2 |= 1 << i;
    }
    
    int i;
    bool hit = false;
    for (i=0; i<associativity; i++) {
        if (cache[index][i].data == (address&(~mask2)) && cache[index][i].validbit) {
            // Handle Hit
            hit = true;
            cache[index][i].used = usedcount++;
            
            if (write) {
                writeHits++;
                cache[index][i].dirtybit = true;
            }
            else 
                readHits++;
            break;
        }
    }
    if (!hit) {
        // Handle Miss
        int LRUindex = 0;
        bool evict = false;
        unsigned int LRUcount = UINT32_MAX;
        for (i=0; i<associativity; i++) {
            if (!cache[index][i].validbit) {
                LRUindex = i;
                evict = false;
                break;
            }
            
            if (cache[index][i].used < LRUcount) {
                LRUindex = i;
                LRUcount = cache[index][i].used;
                evict = true;
            }
        }
        if (evict && (cache[index][LRUindex].dirtybit == true)) { 
            writebacks++;
        }
        
        cache[index][LRUindex].validbit = true;
        cache[index][LRUindex].data = (address&(~mask2));
        cache[index][LRUindex].used = usedcount++;
        if (write){
            writeMisses++;
            cache[index][LRUindex].dirtybit = true;
        }
        else {
            readMisses++;
            cache[index][LRUindex].dirtybit = false;
        }
    }
}

int calculateIndex() {
    int setNum = cacheSize/associativity/blocksize;
    int index = 0;
    while (setNum >>= 1)
        index++;
    return index;
}

void printInfo() {
    double missratio = (readMisses + writeMisses) / (reads + writes);
    printf("%lf\n", missratio);

    if(writePolicy == 0)
        printf("%lf\n", writes);
    else if(writePolicy == 1)
        printf("%lf\n", writebacks);

    printf("%lf\n", readMisses);
}
