#include <unistd.h>
#include<errno.h>
#include"my_malloc.h"
#include<stdio.h>
#include<pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sbrkLock = PTHREAD_MUTEX_INITIALIZER;
__thread memBlock * multiFreeHead = NULL;

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
	
 

void * addNewBlock(size_t size){
  memBlock * newBlock;
  pthread_mutex_lock(&sbrkLock);
  newBlock = sbrk(sizeof(memBlock));
  void * add = sbrk(size);
  pthread_mutex_unlock(&sbrkLock);
  dataSeg = dataSeg+size;
  newBlock->size = size;
  newBlock->nextFree = NULL;
  newBlock->add = add;
  return newBlock->add;
}


void ts_free_lock(void * ptr){
  pthread_mutex_lock(&lock);/////lockone
  memBlock * toFree = (void*) (ptr - sizeof(memBlock));
  if(toFree->add != ptr){
    pthread_mutex_unlock(&lock);//////lockone
    return;
  }
  toFree->nextFree = NULL;
  if(freeHead == NULL)
    freeHead = toFree;
  else
    freeHead = addSorted(toFree,freeHead);
  merge(freeHead);
  pthread_mutex_unlock(&lock);//////lockone
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

void merge(memBlock * freeHeader){
  if(freeHeader == NULL || freeHeader->nextFree == NULL){
    return;
  }
  memBlock * curr = freeHeader;
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


void * ts_malloc_lock(size_t size){
  pthread_mutex_lock(&lock);
  if(size == 0){
    pthread_mutex_unlock(&lock);
    return NULL;
  }
  void * address;
  address = bf_findSetBlocks(size);
  if(address != NULL){
    pthread_mutex_unlock(&lock);
    return address;
  }else{
    address = addNewBlock(size);
    pthread_mutex_unlock(&lock);
    return address;
  }
}




///////////////////////////////////////////
//////////lockfree version here////////////
///////////////////////////////////////////
void * findSetBlocks_nolock(size_t size){
  if(multiFreeHead == NULL)
    return NULL;
  memBlock * curr = multiFreeHead;
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
    memBlock * remainFree = (void*)(chose->add + size);
    remainFree->size = chose->size - size -sizeof(memBlock);
    remainFree->add = chose->add + size + sizeof(memBlock);
    chose->size = size;
    remainFree->nextFree = chose->nextFree;
    chose->nextFree = remainFree;
  }
  multiFreeHead = deleteFree(chose,multiFreeHead);
  return chose->add;    
}


void * ts_malloc_nolock(size_t size){
  if(size == 0)
    return NULL;
  void * address;
  address = findSetBlocks_nolock(size);
  if(address != NULL){
    return address;
  }else{
    address = addNewBlock(size);
    return address;
  }
}

void ts_free_nolock(void * ptr){
  memBlock * toFree = (void*)(ptr - sizeof(memBlock));
  if(toFree->add != ptr)
    return;
  toFree->nextFree = NULL;
  if(multiFreeHead == NULL)
    multiFreeHead = toFree;
  else
    multiFreeHead = addSorted(toFree,multiFreeHead);
  merge(multiFreeHead);
}




