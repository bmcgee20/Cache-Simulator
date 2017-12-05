/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 */

#define BLOCK_SIZE  64        /* Cache block size (or cache line size) in bytes \\
                                 (must be power of 2). 4 Bytes = 1 Word */
#define WAY_SIZE    8         /* Associativity; 1-way = direct-mapped */
#define CACHE_SIZE  32768     /* Cache capacity in bytes (must be power of 2)*/

#define NUM_BLOCKS  (CACHE_SIZE / BLOCK_SIZE)
#define NUM_SETS    (NUM_BLOCKS / WAY_SIZE)
#define ReplacementType 1  //1 is NRU  2 is PLRU  3 is RR
#define DBG true

/*The data structure of direct-mapped cache*/
struct direct_mapped_cache {
    unsigned valid_field[NUM_BLOCKS];   /* Valid field */
    unsigned dirty_field[NUM_BLOCKS];   /* Dirty field; since we don't distinguish writes and \\
                                           reads in this project yet, this field doesn't really matter */
    uint64_t tag_field[NUM_BLOCKS];     /* Tag field */
    char data_field[NUM_BLOCKS][BLOCK_SIZE];  /* Data field; since we don't really fetch data, \\
                                                 this field doesn't really matter */
    int hits;                          /* Hit count */
    int misses;                        /* Miss count */
};

//to make fully associative just set number of sets to number of blocks
//direct can just be made to 1 set
//n way just set to n-ways
struct set_associative_cache{
    unsigned valid_field[NUM_SETS][NUM_BLOCKS/NUM_SETS];   /* Valid field */
    uint64_t offset_field[NUM_SETS][NUM_BLOCKS/NUM_SETS];     /* Tag field */
    uint64_t tag_field[NUM_SETS][NUM_BLOCKS/NUM_SETS];     /* Tag field */
    int hits;                          /* Hit count */
    int misses;                        /* Miss count */
    //all start as one if accessed turned to zero replace all 1's before zeros
    uint64_t NRU_field[NUM_SETS][NUM_BLOCKS/NUM_SETS];     /* Tag field */

};

struct noder{
	int flag;
	struct noder *left;
	struct noder *right;
	struct noder *parent;
	struct noder *root;
};
typedef struct noder node;

struct tree{
	struct noder *root;
};
typedef struct tree tree;
/*Read the memory traces and convert it to binary*/
uint64_t convert_address(char memory[]);

/*Simulate the direct-mapped cache*/
void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address);
void set_mapped_cache_access(struct set_associative_cache *cache, uint64_t address, int type);

tree *CreateTree();
node *CreateNodes(node *parent, tree *root);

node *FillTree(tree *birch,node *noder, int currentDepth, int maxDepth);
