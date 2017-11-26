/*
 * Concurrency 3 Problem 2 
 * CS 444 Operating Systems 2
 * Fall Term 2017
 * Group 11: Brian Wiltse and Joshua Lioy
 *
 * Solution for the search-insert-delete concurrency problem.
 *
 * The solution to this problem was adapted from the solution 
 * to the serch-insert-delete problem given in 
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

#define MAX_PROC 5
#define asm __asm__ __volatile__

struct node {
  int data;
  struct node* next;
};

struct Lightswitch {
  int counter;
  sem_t mutex;
};

struct node* list_head = NULL;
struct Lightswitch search_switch;
struct Lightswitch insert_switch;
sem_t insert_mutex;
sem_t no_searcher;
sem_t no_inserter;
pthread_mutex_t rand_mutex;


/*
 * ----------------------- LIGHTSWITCH FUNCTIONS ----------------------
 *  The lightswitch is also described in The Little Book of Semaphores 
 *  (cited above).
 */

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

/*
 * ----------------------- RANDOMIZING FUNCTIONS ----------------------
 */

/*
 * Generates a random number with rdrand x86 asm, if supported.
 * Otherwise uses the Mersenne Twister
 *
 * This code was adapted from Kevin McGrath's OSU Operating Systems 2
 * class website at:
 * http://web.engr.oregonstate.edu/cgi-bin/cgiwrap/dmcgrath/classes/17F/cs444/index.cgi?examples=1
 */
int gen_rand_num() {
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  int random;
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

void rand_sleep() {
  int random;
  pthread_mutex_lock(&rand_mutex);
  random = gen_rand_num();
  pthread_mutex_unlock(&rand_mutex);
  sleep(random % 6 + 1);
}

int get_rand_data() {
  int data;
  data = gen_rand_num() % 50;
  return data;
}

/*
 * ----------------------- LINKED LIST FUNCTIONS ----------------------
 */

void append_to_list(int data){
  struct node* current = list_head;
  struct node* new_node = (struct node*)malloc(sizeof(struct node));
  new_node->data = data;
  new_node->next = NULL;
  
  if (current == NULL) {
    list_head = new_node;    
  } else {
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_node;
  }
}

void print_list() {
  struct node* current = list_head;
  printf("List: ");
  while (current != NULL) {
    printf("%d->", current->data);
    current = current->next;
  }
  printf("NULL\n");
}

int find_in_list(int data) {
  int pos = -1;
  int count = 0;
  struct node* current = list_head;
  
  while(current != NULL) {
    if (current->data == data) {
      pos = count;
      break;
    }
    count += 1;
    current = current->next;
  }
  return pos; 
}

int get_list_size() {
  int count = 0;
  struct node* current = list_head;
  while (current != 0) {
    count++;
    current = current->next;
  }
  return count;
}

void remove_node(int pos) {
  struct node* temp = list_head;
  struct node* next;
  if (pos == 0) {
    list_head = list_head->next;
    free(temp);
  } else {
    for (int i = 0; i < pos - 1; i++) {
      temp = temp->next;
    }
    printf("removing node number %d: %d\n", pos, temp->next->data);
    next = temp->next->next;
    free(temp->next);
    temp->next = next;
  }

}

/*
 * ----------------------- SEARCH, INSERT, DELETE FUNCTIONS ----------------------
 */

void *searcher(void* tid) {
  int pid = (int)tid;
  int rand_data;
  int pos;
  while(1) {
    pthread_mutex_lock(&rand_mutex);
    rand_data = get_rand_data(); 
    pthread_mutex_unlock(&rand_mutex);

    switch_lock(&search_switch, &no_searcher);
    pos = find_in_list(rand_data);
    if (pos >= 0) {
      printf("Searcher %d found %d at position %d\n", pid, rand_data, pos);
    } else{
      printf("Searcher %d did not find %d in the list\n", pid, rand_data);
    }
    switch_unlock(&search_switch, &no_searcher);
    rand_sleep();
  }
}

void *inserter(void* tid) {
  int pid = (int)tid;
  while(1) {
    int data;
    struct node* current = list_head;
    pthread_mutex_lock(&rand_mutex);
    data = get_rand_data();
    pthread_mutex_unlock(&rand_mutex); 
    
    switch_lock(&insert_switch, &no_inserter);
    sem_wait(&insert_mutex);

    append_to_list(data);
    printf("inserter %d appending %d\n", pid, data);
  
    sem_post(&insert_mutex);
    switch_unlock(&insert_switch, &no_inserter);
    rand_sleep();
  }
}


void *deleter( void* tid) {
  int pid = (int)tid;
  int random;
  int list_size;
  int node_num;
  while(1) {
    pthread_mutex_lock(&rand_mutex);
    random = gen_rand_num();
    pthread_mutex_unlock(&rand_mutex);
  
    sem_wait(&no_searcher); 
    sem_wait(&no_inserter);
  
    print_list();
    list_size = get_list_size();
    if (list_size > 0) {
      node_num = random % list_size;
      printf("deleter %d deleting from position %d\n", pid, node_num);
      remove_node(node_num);
      print_list(); // Print here since we know 
                    // that nothing else is acting on the list
    } else {
      printf("Nothing for deleter %d to delete!\n", pid);
    }
    sem_post(&no_searcher); 
    sem_post(&no_inserter);
    rand_sleep();
  }
}

int main(int argc, char* argv[]) {
  int num_s;
  int num_i;
  int num_d;
  
  if (argc != 5) {
    printf("Usage: %s <numSearchers> <numInserters> <numDeleters> <Seed> Exiting...\n", argv[0]);
    exit(1);
  }

  num_s = atoi(argv[1]);
  num_i = atoi(argv[2]);
  num_d = atoi(argv[3]);
  
  if (num_s > MAX_PROC || num_i > MAX_PROC || num_d > MAX_PROC) {
    printf("Limit of %d for each type\nExiting...", MAX_PROC);
    exit(1);
  }
    
  unsigned long seed = htonl(atoi(argv[4]));
  init_genrand(seed);
  
  pthread_mutex_init(&rand_mutex, NULL);
  sem_init(&insert_mutex, 0, 1);
  sem_init(&no_searcher, 0, 1);
  sem_init(&no_inserter, 0, 1);
  
  init_switch(&search_switch);
  init_switch(&insert_switch);
 
  pthread_t searchers[MAX_PROC];
  pthread_t inserters[MAX_PROC];
  pthread_t deleters[MAX_PROC];

  for (int i = 0; i < num_s; i++) {
    pthread_create(&(searchers[i]), NULL, searcher, (void*)i); 
  }
  for (int i = 0; i < num_i; i++) {
    pthread_create(&(inserters[i]), NULL, inserter, (void*)i); 
  } 
  for (int i = 0; i < num_d; i++) {
    pthread_create(&(deleters[i]), NULL, deleter, (void*)i); 
  }
  
  for(int i = 0; i < num_s; i++) {
    pthread_join(searchers[i], NULL);
  } 
  for(int i = 0; i < num_i; i++) {
    pthread_join(inserters[i], NULL);
  }
  for(int i = 0; i < num_d; i++) {
    pthread_join(deleters[i], NULL);
  }
  
  return 0;  
}
