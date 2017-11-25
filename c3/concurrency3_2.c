/*
 * Concurrency 3 Problem 2 
 * CS 444 Operating Systems 2
 * Fall Term 2017
 * Group 11: Brian Wiltse and Joshua Lioy
 *
 * Solution for the search-insert-delete concurrency problem.
 *
 * The solution to this problem was adapted from the solution given in 
 * The Little Book of Semaphores
 * By Allen B. Downey
 * Accessed online on 24 Nov 2017 at 
 * http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf 
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <arpa/inet.h>

#include "mt.h"

#define NUM_I 5
#define NUM_S 3
#define NUM_D 2

#define asm __asm__ __volatile__

struct LinkedList {
  int data;
  struct LinkedList* next;
};

struct Lightswitch {
  int counter;
  sem_t mutex;
};

struct LinkedList list;
struct Lightswitch search_switch;
struct Lightswitch insert_switch;
sem_t insert_mutex;
sem_t no_searcher;
sem_t no_inserter;
pthread_mutex_t rand_mutex;

/*
 * Generates a random number with rdrand x86 asm, if supported.
 * Otherwise uses the Mersenne Twister
 *
 * This code was adapted from Kevin McGrath's OSU Operating Systems 2
 * class website at:
 * http://web.engr.oregonstate.edu/cgi-bin/cgiwrap/dmcgrath/classes/17F/cs444/index.cgi?examples=1
 */
unsigned long gen_rand_num() {
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  unsigned long random;
  char vendor[13];
  
  eax = 0x01;

  __asm__ __volatile__(
                       "cpuid;"
                       : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                       : "a"(eax)
                       );
  
  if(ecx & 0x40000000){
    // printf("I support rdrand\n");
    asm("rdrand %0" : "=r" (random));
  } else{
    // printf("I do not support rdrand\n");
    random = genrand_int32();
  }

  if (random < 0 ) {
    random = random * -1;
  }
  return random;
}

void *searcher(void* tid) {
  int pid = (int)tid;
  printf("Searcher %d ready\n", pid); 
}

void *inserter(void* tid) {
  int pid = (int)tid;
  printf("inserter %d ready\n", pid); 

}

void *deleter( void* tid) {
  int pid = (int)tid;
  printf("deleter %d ready\n", pid); 

}

void init_switch(struct Lightswitch* lightswitch) {
  lightswitch->counter = 0;
  sem_init(&lightswitch->mutex, 0, 1);
}

void switch_lock(struct Lightswitch* lightswitch, sem_t* semaphore) {
  sem_wait(&lightswitch->mutex);
  lightswitch->counter++;
  if (lightswitch->counter == 1){
    sem_wait(semaphore);
  }
  sem_post(&lightswitch->mutex);
}

void switch_unlock(struct Lightswitch* lightswitch, sem_t* semaphore) {
  sem_wait(&lightswitch->mutex);
  lightswitch->counter--;
  if (lightswitch->counter == 0) {
    sem_post(semaphore);
  }
  sem_post(&lightswitch->mutex);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Missing seed argument. Exiting...\n");
    exit(1);
  }
  
  unsigned long seed = htonl(atoi(argv[1]));
  init_genrand(seed);
  pthread_mutex_init(&rand_mutex, NULL);
  sem_init(&insert_mutex, 0, 1);
  sem_init(&no_searcher, 0, 1);
  sem_init(&no_inserter, 0, 1);
  
  init_switch(&search_switch);
  init_switch(&insert_switch);
 
  pthread_t searchers[NUM_S];
  pthread_t inserters[NUM_I];
  pthread_t deleters[NUM_D];

  // create searcher
  for (int i = 0; i < NUM_S; i++) {
    pthread_create(&(searchers[i]), NULL, searcher, (void*)i); 
  }
  
  // create inserters
  for (int i = 0; i < NUM_I; i++) {
    pthread_create(&(inserters[i]), NULL, inserter, (void*)i); 
  }
  
  // create deleters
  for (int i = 0; i < NUM_D; i++) {
    pthread_create(&(deleters[i]), NULL, deleter, (void*)i); 
  }
  
  
  for(int i = 0; i < NUM_S; i++) {
    pthread_join(searchers[i], NULL);
  }
  
  for(int i = 0; i < NUM_I; i++) {
    pthread_join(inserters[i], NULL);
  }
  
  for(int i = 0; i < NUM_D; i++) {
    pthread_join(deleters[i], NULL);
  }
  
  return 0;  
}
