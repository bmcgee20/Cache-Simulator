/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "cachesim.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <direct> <trace file name>\n", argv[0]);
        return 1;
    }

#ifdef DBG
    printf("BLOCK SIZE = %d Bytes\n", BLOCK_SIZE);
    printf("%d-WAY\n", WAY_SIZE);
    printf("CACHE SIZE = %d Bytes\n", CACHE_SIZE);
    printf("NUMBER OF BLOCKS = %d\n", NUM_BLOCKS);
    printf("NUMBER OF SETS = %d\n", NUM_SETS);
    printf("\n");
#endif

    char* trace_file_name = argv[2];
    struct direct_mapped_cache d_cache;
    struct set_associative_cache s_cache;
    char mem_request[20];
    uint64_t address;
    FILE *fp;


    /* Initialization */
    for (int i=0; i<NUM_BLOCKS; i++) {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    for(int j=0; j<NUM_SETS; j++){
    	for(int i=0; i<NUM_BLOCKS; i++){
    		s_cache.valid_field[j][i] = 0;
    		s_cache.tag_field[j][i] = 0;
    		s_cache.offset_field[j][i]= 0;
    		s_cache.NRU_field[j][i] = 1;
    	}
    }

    /* Opening the memory trace file */
    fp = fopen(trace_file_name, "r");

    /*
     * DIRECT MAPPED CACHE FUNCTIONALITY
     */
    if (strncmp(argv[1], "direct", 6)==0) { /* Simulating direct-mapped cache */
        /* Read the memory request address and access the cache */
        while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            direct_mapped_cache_access(&d_cache, address);
        }

        /*Print out the results*/
        printf("\n==================================\n");
        printf("Cache type:    Direct-Mapped Cache\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", d_cache.hits);
        printf("Cache Misses:  %d\n", d_cache.misses);

        //PRINT OUT HIT AND MISS RATES
        if(d_cache.hits ==0){
    		printf("Cache Miss Rate: 1.000\n");
    		printf("Cache Hit Rate:  0.000\n");
        }
        else if(d_cache.misses==0){
    		printf("Cache Miss Rate: 0.000\n");
    		printf("Cache Hit Rate:  1.000\n");
        }
        else{
    		float missChance =((float) d_cache.misses/(d_cache.hits+d_cache.misses));
    		float hitChance = 1 - missChance;
    		printf("Hit Rate:  %.3f\n", hitChance);
    		printf("Miss Rate: %.3f\n", missChance);
        }
        printf("\n");
    }
    /*
     * SET ASSOCIATIVE FUNCTIONALITY
     */
    else if(strncmp(argv[1],"set",3)==0){
    	while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            set_mapped_cache_access(&s_cache, address);
    	}
        //PRINT OUT HIT AND MISS RATES
        printf("\n===============================================\n");
        printf("Cache type:    Set-Associative-Mapped Cache\n");
        printf("=================================================\n");
        printf("Cache Hits:    %d\n", s_cache.hits);
        printf("Cache Misses:  %d\n", s_cache.misses);
        if(s_cache.hits ==0){
    		printf("Cache Miss Rate: 1.000\n");
    		printf("Cache Hit Rate:  0.000\n");
        }
        else if(s_cache.misses==0){
    		printf("Cache Miss Rate: 0.000\n");
    		printf("Cache Hit Rate:  1.000\n");
        }
        else{
    		float missChance =((float) s_cache.misses/(s_cache.hits+s_cache.misses));
    		float hitChance = 1 - missChance;
    		printf("Hit Rate:  %.3f\n", hitChance);
    		printf("Miss Rate: %.3f\n", missChance);
        }
    }
    /*
     * FULLY ASSOCIATIVE MAPPING
     *
     */
    else if(strncmp(argv[1],"fully",5)==0){
    	while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
			#undef WAY_SIZE
			#define WAY_SIZE  NUM_BLOCKS
			#undef NUM_SETS
			#define NUM_SETS  (NUM_BLOCKS / WAY_SIZE)
            set_mapped_cache_access(&s_cache, address);
    	}
        //PRINT OUT HIT AND MISS RATES
        printf("\n===============================================\n");
        printf("Cache type:    Fully-Associative Cache\n");
        printf("=================================================\n");
        printf("Cache Hits:    %d\n", s_cache.hits);
        printf("Cache Misses:  %d\n", s_cache.misses);
        if(s_cache.hits ==0){
    		printf("Cache Miss Rate: 1.000\n");
    		printf("Cache Hit Rate:  0.000\n");
        }
        else if(s_cache.misses==0){
    		printf("Cache Miss Rate: 0.000\n");
    		printf("Cache Hit Rate:  1.000\n");
        }
        else{
    		float missChance =((float) s_cache.misses/(s_cache.hits+s_cache.misses));
    		float hitChance = 1 - missChance;
    		printf("Hit Rate:  %.3f\n", hitChance);
    		printf("Miss Rate: %.3f\n", missChance);
        }
    }

    fclose(fp);

    return 0;
}

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n') {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0') {
            binary = (binary*16) + (memory_addr[i] - '0');
        } else {
            if(memory_addr[i] == 'a' || memory_addr[i] == 'A') {
                binary = (binary*16) + 10;
            }
            if(memory_addr[i] == 'b' || memory_addr[i] == 'B') {
                binary = (binary*16) + 11;
            }
            if(memory_addr[i] == 'c' || memory_addr[i] == 'C') {
                binary = (binary*16) + 12;
            }
            if(memory_addr[i] == 'd' || memory_addr[i] == 'D') {
                binary = (binary*16) + 13;
            }
            if(memory_addr[i] == 'e' || memory_addr[i] == 'E') {
                binary = (binary*16) + 14;
            }
            if(memory_addr[i] == 'f' || memory_addr[i] == 'F') {
                binary = (binary*16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    //memory addr is the 0000af40 stuff
    //binary is the convertion to like 44864
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}

void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address)
{
	//this is a bitwise shift
	//right bitwise shift the amount of log2 blocksize
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);
    uint64_t index = block_addr % NUM_BLOCKS;
    //another right bit shift to the log2 num blocks
    uint64_t tag = block_addr >> (unsigned)log2(NUM_BLOCKS);

#ifdef DBG
    printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
#endif
    //calculate if it hits or not and print if it does
    if (cache->valid_field[index] && cache->tag_field[index] == tag) { /* Cache hit */
        cache->hits += 1;
#ifdef DBG
        printf("Hit!\n");
#endif
    } else {
        /* Cache miss */
        cache->misses += 1;
#ifdef DBG
        printf("Miss!\n");
#endif

        if (cache->valid_field[index] && cache->dirty_field[index]) {
            /* Write the cache block back to memory */
        }
        cache->tag_field[index] = tag;
        cache->valid_field[index] = 1;
        cache->dirty_field[index] = 0;
    }
}

void set_mapped_cache_access(struct set_associative_cache *cache, uint64_t address)
{
    //size of each part
    	//index is size 2^n where 2^n is amount of sets
    	//offset is the 2^a where 2^a is the size of block in bytes
    	//tag is 32-n-a
	int setIndexSize = log2(NUM_SETS);
	int offsetSize = log2(BLOCK_SIZE);
	int tagSize = 32 - (setIndexSize + offsetSize);
	//printf("\n index size %d offset  %d  tag %d\n",setIndexSize,offsetSize,tagSize);

	//push all bits out except the tag
	uint64_t tagNum = (address>>(32-tagSize));
    uint64_t blockOffset  = address%BLOCK_SIZE;
    uint64_t blockAddress = address/BLOCK_SIZE;
    uint64_t SetIndex = blockAddress%NUM_SETS;

#ifdef DBG
    printf("Address: %d\n",address);
    printf("Tag: %d\n",tagNum);
    printf("Set: %d\n",SetIndex);
    printf("block: %d\n",blockAddress);
    #endif
    printf("\nNumber of sets: %d\n", NUM_SETS);

    //calculate if it hits or not and print if it does
    int j;
    int i;
    int hit = 0;
    //look in that set and check if any are the same tags
    for(i=0; i<=(NUM_BLOCKS/NUM_SETS); i++){
    	if(cache->tag_field[SetIndex][i]== tagNum && cache->valid_field[SetIndex][i]==1){
    		printf("Hit!\n");
    		cache->hits +=1;
    		hit=1;
    		break;
    	}

    }
    //we did not find a match for it last time
    if(hit==0){
    	cache->misses+=1;
    	printf("Miss!\n");
      //now use NRU to place it somewhere
    	//Used to signal if all NRU are 1 or not
    	int didReplace = 0;
        for(i=0; i<=(NUM_BLOCKS/NUM_SETS); i++){
        	if(cache->NRU_field[SetIndex][i]== 0){
        		//can replace here so do so
       			didReplace = 1;
       			cache->tag_field[SetIndex][i] = tagNum;
       			cache->valid_field[SetIndex][i] = 1;
       			break;
       		}
        }
        //so no blocks has 0 NRU
        if(didReplace==0){
        	//clear them all to 0
            for(i=0; i<=(NUM_BLOCKS/NUM_SETS); i++){
            	cache->NRU_field[SetIndex][i]=0;
            }
        }
    }
}

