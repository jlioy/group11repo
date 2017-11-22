/*
 * Concurrency 3 Problem 1
 * CS 444 Operating Systems 2
 * Fall Term 2017
 * Group 11: Brian Wiltse and Joshua Lioy
 *
 * Solution for the resource problem.
 *
 * The solution to this problem is adapted from "Design Patterns for Semaphores"
 * By Kenneth A. Reek
 * Accessed online on 22 Nov 2017 at 
 * https://pdfs.semanticscholar.org/93af/99143f8123032fbcc805656d63617a2268ab.pdf
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
  int random;
 
  while (1) {
    pthread_mutex_lock(&rand_mutex);
    random = gen_rand_num() % 5 + 2;
    pthread_mutex_unlock(&rand_mutex);

    // Allow one thread at a time to see what resources are available
    sem_wait(&resource_mutex);  
    if (must_wait) {
      waiting++;
      printf("Thread %d is waiting\n", pid);
      // Let the next thread see if resources are available
      sem_post(&resource_mutex);
      // Wait until resource is available
      sem_wait(&block_mutex); 
      waiting--;
    }

    printf("Thread %d is active for %d seconds\n", pid, random);
    active++;
    must_wait = (active == 3); // if 3 are using resource, we must wait

    if (waiting > 0 && !must_wait) {
      // Threads are waiting and the resource is now available
      // Release the lock on the resourcee
      sem_post(&block_mutex); 
    } else {
      // Either no threads are waiting or the resource is inaccessible.
      // Let the next thread through.
      sem_post(&resource_mutex);
    }

    sleep(random);
    // Allow one thread at a time to check if resource is now available
    sem_wait(&resource_mutex);
    printf("Thread %d is no longer active\n", pid);
    --active;
    
    // Threads no longer have to wait if active goes to 0.
    if (active == 0){
      must_wait = 0;
    }

    if (waiting > 0 && !must_wait) {
      // Threads are waiting and the resource is now available.
      // Release the lock on the resource.
      sem_post(&block_mutex); 
    } else {
      // Either no threads are waiting or the resource is inaccessible.
      // Let the next thread through.
      sem_post(&resource_mutex);
    }

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
