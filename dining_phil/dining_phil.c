/*
 * Concurrency 1: The Producer-Consumer Problem
 * CS 444 Operating Systems 2
 * Fall Term 2017
 * Group 11: Brian Wiltse and Joshua Lioy
 *
 * Solution for the Producer-Consumer problem with a finite buffer. 
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <arpa/inet.h>

#include "mt.h"

#define NUM_PHILOSOPHERS 5

#define asm __asm__ __volatile__



sem_t forks[5];
sem_t footman;
pthread_mutex_t mutex;
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

void print_buffer() {
  printf("buffer size: %d\n", buffer.size);
  for(int i = 0; i < buffer.size; i++) {
    printf("%lu | ", buffer.items[i]);  
  }
  printf("\n");
}

void *philosophize(void* tid) {
   
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Missing seed argument. Exiting...\n");
    exit(1);
  }
  
  unsigned long seed = htonl(atoi(argv[1]));
  init_genrand(seed);
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&rand_mutex, NULL);
  sem_init(&footman, 0, 4); // only 4 philosophers allowed at table at once  
  for (int i = 0; i < NUM_PHILOSOPHERS; i++);
    sem_init(&forks[i], 0, 1);

  // create an array of threads for consumers
  pthread_t consumers[NUM_CONS];
  pthread_t producers[NUM_PRODS];
 
  // create consumers
  for (int i = 0; i < NUM_CONS; i++) {
    pthread_create(&(consumers[i]), NULL, consume, (void*)i); 
  }

  // create producers
  for (int i = 0; i < NUM_PRODS; i++) {
    pthread_create(&(producers[i]), NULL, produce, (void*)i); 
  }
   
  for(int i = 0; i < NUM_CONS; i++) {
    pthread_join(consumers[i], NULL);
  }
  
  for(int i = 0; i < NUM_PRODS; i++) {
    pthread_join(producers[i], NULL);
  }

  return 0;  
}
