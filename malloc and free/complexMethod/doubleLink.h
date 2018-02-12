si#ifndef __MYMALLOC_H__
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
  struct _memBlock * nextFree;
  struct _memBlock * preFree;
  int isUse;
};
typedef struct _memBlock memBlock;

void deleteFree(memBlock * toDelete);
void addNewFree(memBlock * pos,size_t used);
unsigned long get_data_segment_free_space_size();
unsigned long get_data_segment_size();
void * ff_findInFree(size_t size);
void * addNewBlock(size_t size);
void merge();
void * ff_malloc(size_t size);
void * bf_malloc(size_t size);
void bf_free(void * ptr);
void ff_free(void * ptr);


#endif


