#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#include "mt.h"

#define MAX_SIZE 32

/*
 * This is an item that goes in the buffer
 */
struct item {
  unsigned long id; // randomly generated number
  int wait_time; // randomly generated number of seconds (between 2 and 9)
};

struct buffer {
  struct item items[MAX_SIZE];
  int size;
};

/*
 * Initialize shared buffer
 */
struct buffer buffer = {
  .size = 0
};

/* Initialize semaphores. 
 * Consumer will block when sem_full is 0 (nothing to consume)
 * Producer will block when sem_empty is 0 (buffer is full)
 * mutex will allow only one producer or consumer access to the buffer
 */
sem_t sem_full;
sem_t sem_empty;
pthread_mutex_t mutex;

/*
 * Generates a random number with rdrand x86 asm, if supported.
 * Otherwise uses the Mersenne Twister
 *
 * This code was adapted from Kevin McGrath's class website at:
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
    // TODO: use rdrand
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


void produce(int num_items) {
  int p_wait_time;
  struct item item;
  for(int i = 0; i < num_items; i++) {
    p_wait_time = (gen_rand_num() % 4) + 3;
    item.id = gen_rand_num();
    item.wait_time = (gen_rand_num() % 7) + 2;
    
    sleep(p_wait_time);
    sem_wait(&sem_empty);
    pthread_mutex_lock(&mutex);
    buffer.items[buffer.size++] = item; 
    printf("Producer waited %d seconds, then placed item %lu in buffer\n",
             p_wait_time, item.id);
    print_buffer();
    pthread_mutex_unlock(&mutex);
    sem_post(&sem_full);
  }
   
}

void *consume(void *tid) {
  int cid = (int)tid;
  struct item curr_item;
  sem_wait(&sem_full);
  pthread_mutex_lock(&mutex);
    curr_item = buffer.items[--buffer.size];
  pthread_mutex_unlock(&mutex);
  sem_post(&sem_empty);
  sleep(curr_item.wait_time);
  printf("Consumer %d waited %d seconds, then consumed item %lu\n", 
          cid, curr_item.wait_time, curr_item.id);
     
}

int main(int argc, char* argv[]) {
  pthread_mutex_init(&mutex, NULL);
  sem_init(&sem_full, 0, 0);  
  sem_init(&sem_empty, 0, MAX_SIZE);
  unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};
  unsigned long length = 4;
  int num_items = atoi(argv[1]); 
  init_by_array(init, length); 

  // create an array of threads for consumers
  pthread_t* consumers = malloc(sizeof(pthread_t) * num_items);;
 
  // create consumers
  for (int i = 0; i < num_items; i++) {
    pthread_create(&(consumers[i]), NULL, consume, (void*)i); 
  }

  // create producer
  produce(num_items);
   
  for(int i = 0; i < num_items; i++) {
    pthread_join(consumers[i], NULL);
  }

  return 0;  
}
