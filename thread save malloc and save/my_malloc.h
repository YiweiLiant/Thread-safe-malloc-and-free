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
void * bf_findSetBlocks(size_t size);
void * addNewBlock(size_t size);
memBlock * addSorted(memBlock * new, memBlock * curr);
void merge();
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void * ts_malloc_lock(size_t size);
void ts_free_lock(void * ptr);

void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void * ptr);



#endif


