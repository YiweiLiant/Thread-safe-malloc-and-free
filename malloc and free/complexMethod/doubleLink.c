#include <unistd.h>
#include<errno.h>
#include"my_malloc.h"
#include<stdio.h>
memBlock * blocksHead = NULL;
memBlock * freeHead = NULL;
memBlock * blocksEnd = NULL;
memBlock * freeEnd = NULL;

unsigned long get_data_segment_size(){
  unsigned long ans = 0;
  memBlock * curr = blocksHead;
  while(curr != NULL){
    ans = ans + curr->size;
    curr = curr->next;
  }
  return ans;
}

unsigned long get_data_segment_free_space_size(){
  unsigned long ans = 0;
  memBlock * curr = freeHead;
  while(curr != NULL){
    ans = ans + curr->size;
    curr = curr->nextFree;
  }
  return ans;
}

void * addNewBlock(size_t size){
  memBlock * newBlock = sbrk(sizeof(memBlock));
  newBlock->start = sbrk(size);
  newBlock->isUse = USED;
  newBlock->size = size;
  newBlock->nextFree = NULL;
  newBlock->preFree = NULL;
  newBlock->pre = blocksEnd;
  newBlock->next = NULL;   //line 38 add NULL
  blocksEnd = newBlock;
  if(blocksHead == NULL)
    blocksHead = blocksEnd;
  else
    blocksEnd->pre->next = blocksEnd;
  return blocksEnd->start;
}

void addNewFree(memBlock * pos,size_t used){
  memBlock * new = pos->start + used;
  new->size = pos->size - used - sizeof(memBlock);
  new->start = pos->start + used + sizeof(memBlock);
  new->isUse = UNUSED;
  new->next = pos->next;
  new->pre = pos;
  pos->next = new;
  if(new->next != NULL)
    new->next->pre = new;
  else
    blocksEnd = new;
  new->nextFree = pos->nextFree;
  new->preFree = pos;
  pos->nextFree = new;
  if(new->nextFree != NULL)
    new->nextFree->preFree = new;
  else
    freeEnd = new;
}

void deleteFree(memBlock * toDelete){
  toDelete->isUse = USED;
  if(toDelete->preFree != NULL)
    toDelete->preFree->nextFree = toDelete->nextFree;
  else
    freeHead = toDelete->nextFree;
  if(toDelete->nextFree != NULL)
    toDelete->nextFree->preFree = toDelete->preFree;
  else
    freeEnd = toDelete->preFree;
}

void * ff_findInFree(size_t size){
  if(freeHead == NULL)
    return NULL;
  memBlock * curr = freeHead;
  memBlock * chose = NULL;
  while(curr != NULL){
    if(curr->size >= size){
      chose = curr;
      break;
    }
    curr = curr->nextFree;
  }
  if(chose == NULL)
    return NULL;
  if(chose->size - size > sizeof(memBlock)){
    addNewFree(chose,size);
  }
  chose->size = size;
  chose->isUse = USED;
  deleteFree(chose);
}


void * ff_malloc(size_t size){
  if(size == 0)
    return;
  void * address = ff_findInFree(size);
  if(address != NULL)
    return address;
  address = addNewBlock(size);
  return address;
}

void ff_free(void * ptr){
  memBlock * toFree = ptr - sizeof(memBlock);
  if(toFree->start != ptr)
    return;
  toFree->isUse = UNUSED;
  toFree->nextFree = NULL;
  toFree->preFree = freeEnd;
  freeEnd = toFree;
  if(freeHead == NULL)
    freeHead = freeEnd;
  else
    freeEnd->preFree->nextFree = freeEnd;
  merge();
  merge();
}

void merge(){
  memBlock * curr = freeHead;
  while(curr != NULL && curr->next != NULL){
    if(curr->isUse == UNUSED && curr->next->isUse == UNUSED){
      curr->size = curr->size + curr->next->size;// + sizeof(memBlock);  //line 120 change sizeof
      curr->next = curr->next->next;
      if(curr->next != NULL){      
	curr->next->pre = curr;
      }
      else{
	blocksEnd = curr;
      }
      if(curr->nextFree != NULL)
	curr->nextFree = curr->nextFree->nextFree;
      if(curr->nextFree != NULL)
	curr->nextFree->pre = curr;
      else
	freeEnd = curr;
    }
    curr = curr->nextFree; 
  }
}
