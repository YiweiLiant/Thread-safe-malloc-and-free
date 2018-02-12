#include <unistd.h>
#include<errno.h>
#include"my_malloc.h"
#include<stdio.h>
memBlock * blocksEnd = NULL;
memBlock * blocksHead = NULL;

unsigned long get_data_segment_size(){
  unsigned long ans = 0;
  memBlock * curr = blocksEnd;
  while(curr != NULL){
    ans = ans + curr->size;
    curr = curr->pre;
  }
  return ans;
}

unsigned long get_data_segment_free_space_size(){
  unsigned long ans = 0;
  memBlock * curr = blocksEnd;
  while(curr != NULL){
    if(curr->isUse == UNUSED){
      ans = ans + curr->size;
    }
    curr = curr->pre;
  }
  return ans;
}


void addBlock(size_t size,void * address,memBlock * pos){
  if(size == 0)
    return;
  memBlock * newBlock;
  newBlock = sbrk(sizeof(memBlock));
  if(errno == ENOMEM){
    //   fprintf(stderr,"fail to sbrk\n");
    return;
  }
  newBlock->size = size;
  newBlock->start = address;
  newBlock->isUse = UNUSED;
  newBlock->next = pos->next;
  newBlock->pre = pos;
  pos->next = newBlock;
  if(newBlock->next == NULL){
    blocksEnd = newBlock;
  }else{
    newBlock->next->pre = newBlock;
  }
  mergeBlock();
}

void * findSetBlocks(size_t size,int fitMethod){
  memBlock * curr = blocksHead;
  memBlock * chose = NULL;
  while(curr != NULL){
    if(curr->isUse == UNUSED && curr->size >= size){
      if(fitMethod == FIRST_FIT){
	chose = curr;
	break;
      }else if(fitMethod == BEST_FIT){
	if(chose == NULL || chose->size > curr->size)
	  chose = curr;
      }
    }
    curr = curr->next;
  }
  if(chose == NULL)
    return NULL;
  size_t remain = chose->size - size;
  addBlock(remain,chose->start + remain,chose);
  chose->size = size;
  chose->isUse = USED;
  return chose->start;
}

void mergeBlock(){
  memBlock * curr = blocksEnd;
  while(curr != NULL && curr->pre != NULL){
    if(curr->isUse == UNUSED && curr->pre->isUse == UNUSED){
      curr->pre->size = curr->size + curr->pre->size;
      curr->pre->isUse = UNUSED;
      curr->pre->next = curr->next;
      if(curr->next != NULL){
	curr->next->pre = curr->pre;
      }else{
	blocksEnd = curr->pre;
      }
    }
    curr = curr->pre;
  }
}

void addBlockEnd(size_t size,void *address){
  memBlock * newBlock;
  newBlock = sbrk(sizeof(memBlock));
  if(errno == ENOMEM){
    //fprintf(stderr,"fail to sbrk\n");
    return;
  }
  newBlock->start = address;
  newBlock->size = size;
  newBlock->isUse = USED;
  newBlock->pre = blocksEnd;
  newBlock->next = NULL;
  if(blocksEnd != NULL)
    blocksEnd->next = newBlock;
  else
    blocksHead = newBlock;
  blocksEnd = newBlock;
}



void *allMalloc(size_t size,int fitMethod){
  if(size == 0)
    return NULL;
  void * address;
  address = findSetBlocks(size,fitMethod);
  if(address != NULL){
    return address;
  }
  address = sbrk(size);
  if(errno == ENOMEM){
    // fprinf(stderr,"fail to sbrk\n");
    return NULL;
  }
  addBlockEnd(size,address);
  return address;
}

int FreeinBlocks(void * ptr){
  if(ptr == NULL)
    return 0;
  memBlock * curr = blocksEnd;
  while(curr!= NULL){
    if(curr->start == ptr){
      curr->isUse = 0;
      return 1;
    }
    curr = curr->pre;
  }
  return 0;
}

void * ff_malloc(size_t size){
  return allMalloc(size,FIRST_FIT);
}
void free(void * ptr){
  if(FreeinBlocks(ptr) == 0)
    return;
  mergeBlock();
}

void ff_free(void * ptr){
  free(ptr);
}

void * bf_malloc(size_t size){
  return allMalloc(size,BEST_FIT);
}

void bf_free(void * ptr){
  free(ptr);
}

