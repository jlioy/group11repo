/*
 * Concurrency 2: The Dining Philosophers Problem
 * CS 444 Operating Systems 2
 * Fall Term 2017
 * Group 11: Brian Wiltse and Joshua Lioy
 *
 * Solution for the Dining Philosopher problem. 
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <arpa/inet.h>

#include "mt.h"

#define NUM_PROC 5

#define asm __asm__ __volatile__

sem_t resource_mutex;
sem_t block_mutex;
pthread_mutex_t rand_mutex;
int active = 0; 
int waiting = 0;
int must_wait = 0;


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


void *use_res(void* tid) {
  int pid = (int)tid;
  
  while(1) {
    printf("%d working as planned\n", pid);
    break;
    
  }
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Missing seed argument. Exiting...\n");
    exit(1);
  }
  
  unsigned long seed = htonl(atoi(argv[1]));
  init_genrand(seed);
  pthread_mutex_init(&rand_mutex, NULL);
  sem_init(&resource_mutex, 0, 1);
  sem_init(&block_mutex, 0, 0); 
  
  pthread_t processes[NUM_PROC];
 
 // create processes
  for (int i = 0; i < NUM_PROC; i++) {
    pthread_create(&(processes[i]), NULL, use_res, (void*)i); 
  }
   
  for(int i = 0; i < NUM_PROC; i++) {
    pthread_join(processes[i], NULL);
  }
  
  return 0;  
}
