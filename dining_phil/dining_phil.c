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

#define NUM_PHILOSOPHERS 5

#define asm __asm__ __volatile__

char* philo_names[] = {"Plato", "Socrates", "Aristotle", "Confucious", "Russel"};
sem_t forks[NUM_PHILOSOPHERS];
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


void think(int pid) {
  pthread_mutex_lock(&rand_mutex);
  int think_time = (gen_rand_num() % 8) + 2; // think from 2 - 9 seconds
  pthread_mutex_unlock(&rand_mutex);
  printf("%s is thinking for %d seconds...\n", philo_names[pid], think_time);
  sleep(think_time); 
}


/*
 * This function implements the solution. The solution was obtained from the Little Book of Semaphores by Allen B. Downey:
 * http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
 * Section 4.4.4 (Solution #2)
 */
void eat(int pid) {
  int first;
  int second;
  int left = pid;
  int right = (pid + 1) % NUM_PHILOSOPHERS;
  
  pthread_mutex_lock(&rand_mutex);
  int eat_time = (gen_rand_num() % 20) + 1; // eat from 1-20 sec
  pthread_mutex_unlock(&rand_mutex);

  // only philosopher 0 starts with left
  if (pid == 0) {
    first = left;
    second = right;
  } else {
    first = right;
    second = left;
  }

    // Pick up forks and eat
    sem_wait(&forks[first]);
    sem_wait(&forks[second]);
    printf("%s is eating with forks %d and %d for %d seconds\n", philo_names[pid], first, second, eat_time); 

    sleep(eat_time);

    // Put down forks
    sem_post(&forks[first]);
    sem_post(&forks[second]);
    printf("%s has put down forks %d and %d\n", philo_names[pid], first, second);
}


void *philosophize(void* tid) {
  int pid = (int)tid;
  
  while(1) {
    think(pid);
    eat(pid);
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

  // Initialize semaphores for forks
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    sem_init(&forks[i], 0, 1);
  }

  // create an array of threads for philosophers
  pthread_t philosophers[NUM_PHILOSOPHERS];

  // create philosophers
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_create(&(philosophers[i]), NULL, philosophize, (void*)i); 
  }
   
  for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_join(philosophers[i], NULL);
  }
  
  return 0;  
}
