#ifndef __MYMALLOC_H__
#define __MYMALLOC_H__
#include <unistd.h>
#define USED 1
#define UNUSED 0
#define FIRST_FIT 0
#define BEST_FIT 1

struct _memBlock{
  void * start;
  size_t size;
  struct _memBlock * pre;
  struct _memBlock * next;
  int isUse;
};
typedef struct _memBlock memBlock;

void *allMalloc(size_t size,int fitMethod);
void *ff_malloc(size_t size);
void ff_free(void * ptr);
void * findSetBlocks(size_t size,int fitMethod);
void addBlockEnd(size_t size,void * address);
void addBlock(size_t size,void * address,memBlock * pos);
void mergeBlock();
int FreeinBlocks(void * ptr);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
//void * findBlocks(memBlock * blocks,size_t size);
//void putBlocks(memBlock ** blocks,size_t size,void * address);




void * bf_malloc(size_t size);
void bf_free(void * ptr);


#endif


