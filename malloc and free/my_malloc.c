#include <unistd.h>
#include<errno.h>
#include"my_malloc.h"
#include<stdio.h>
memBlock * freeHead = NULL;
unsigned long dataSeg = 0;

unsigned long get_data_segment_size(){
  return dataSeg;
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

memBlock * deleteFree(memBlock * toDelete,memBlock * curr){
  if(curr == NULL)
    return NULL;
  if(curr == toDelete){
    memBlock * rest = curr->nextFree;
    return rest;
  }
  curr->nextFree = deleteFree(toDelete,curr->nextFree);
  return curr;
}


void * bf_findSetBlocks(size_t size){
  if(freeHead == NULL)
    return NULL;
  memBlock * curr = freeHead;
  memBlock * chose = NULL;
  while(curr != NULL){
    if(curr->size >= size){
      if(chose == NULL || chose->size > curr->size)
	chose = curr;
    }
    curr = curr->nextFree;
  }
  if(chose == NULL)
    return NULL;
  if(chose->size - size > sizeof(memBlock)){
    memBlock * remainFree = (void*) (chose->add + size);
    remainFree->size = chose->size - size - sizeof(memBlock);
    remainFree->add = chose->add + size + sizeof(memBlock);
    chose->size = size;
    remainFree->nextFree = chose->nextFree;
    chose->nextFree = remainFree;
  }
  freeHead = deleteFree(chose,freeHead);
  return chose->add;
}
	

void * ff_findSetBlocks(size_t size){
  if(freeHead == NULL)
    return NULL;
  memBlock * curr = freeHead;
  while(curr != NULL){
    if(curr->size >= size){
      if(curr->size - size > sizeof(memBlock)){
	memBlock * remainFree = (void*) (curr->add + size);
	remainFree->size = curr->size - size - sizeof(memBlock);
	remainFree->add = curr->add + size + sizeof(memBlock);
	curr->size = size;
	remainFree->nextFree = curr->nextFree;
	curr->nextFree = remainFree;
      }
      freeHead = deleteFree(curr,freeHead);
      return curr->add;
    }
    curr = curr->nextFree;
  }
  return NULL;
}	
 

void * addNewBlock(size_t size){
  memBlock * newBlock;
  newBlock = sbrk(sizeof(memBlock));
  newBlock->size = size;
  newBlock->nextFree = NULL;
  void * add = sbrk(size);
  newBlock->add = add;
  dataSeg = dataSeg + size;
  return newBlock->add;
}


void * ff_malloc(size_t size){
  if(size == 0)
    return NULL;
  void * address;
  address = ff_findSetBlocks(size);
  if(address != NULL){
    return address;
  }else{
    address = addNewBlock(size);
    return address;
  }
}


void Allfree(void * ptr){
  memBlock * toFree = (void*) (ptr - sizeof(memBlock));
  if(toFree->add != ptr)
    return;
  toFree->nextFree = NULL;
  if(freeHead == NULL)
    freeHead = toFree;
  else
    freeHead = addSorted(toFree,freeHead);
  merge();
}

memBlock * addSorted(memBlock * new,memBlock * curr){
  if(curr == NULL || new->add < curr->add){
    if(curr == NULL){
      new->nextFree = NULL;
    }
    new->nextFree = curr;
    return new;
  }
  curr->nextFree = addSorted(new,curr->nextFree);
  return curr;
}

void merge(){
  if(freeHead == NULL || freeHead->nextFree == NULL){
    return;
  }
  memBlock * curr = freeHead;
  while(curr != NULL && curr->nextFree != NULL){
      int ifMerge = 0;
      if(curr == NULL)
	break;
      if(curr->add + curr->size == curr->nextFree){
	curr->size = curr->size + curr->nextFree->size + sizeof(memBlock);
	curr->nextFree = curr->nextFree->nextFree;
	ifMerge = 1;
      }
      if (ifMerge == 0){
	curr = curr->nextFree;
      }
  }
}


void ff_free(void * ptr){
  Allfree(ptr);
}

void * bf_malloc(size_t size){
  if(size == 0)
    return NULL;
  void * address;
  address = bf_findSetBlocks(size);
  if(address != NULL){
    return address;
  }else{
    address = addNewBlock(size);
    return address;
  }
}



void bf_free(void * ptr){
  Allfree(ptr);
}

