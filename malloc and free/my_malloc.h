#ifndef __MYMALLOC_H__
#define __MYMALLOC_H__
#include <unistd.h>

struct _memBlock{
  size_t size;
  void * add;
  struct _memBlock * nextFree;
};

typedef struct _memBlock memBlock;

memBlock *  deleteFree(memBlock * toDelete,memBlock * curr);
void * ff_findSetBlocks(size_t size);
void * addNewBlock(size_t size);
void *ff_malloc(size_t size);
void free(void * ptr);
memBlock * addSorted(memBlock * new, memBlock * curr);
void ff_free(void * ptr);
void merge();
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void * bf_malloc(size_t size);
void bf_free(void * ptr);


#endif


