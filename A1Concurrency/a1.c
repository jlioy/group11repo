#include <stdio.h>
#include <pthread.h>

#include "mt.h"

/*
 * This is an item that goes in the buffer
 */
struct item {
  int number; // randomly generated number
  int wait_time; // randomly generated number of seconds (between 2 and 9)
};

struct consumer {
  int id;
  int number; // takes on the value of item's number
};

struct producer {
  int id;
  int wait_time; // between 3 and 7 seconds
};

/*
 * Generates a random number with rdrand x86 asm, if supported.
 * Otherwise uses the Mersenne Twister
 *
 * This code was adapted from Kevin McGrath's class website at:
 * http://web.engr.oregonstate.edu/cgi-bin/cgiwrap/dmcgrath/classes/17F/cs444/index.cgi?examples=1
 */
int gen_rand_num() {
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  unsigned int random;
  char vendor[13];
  
  eax = 0x01;

  __asm__ __volatile__(
                       "cpuid;"
                       : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                       : "a"(eax)
                       );
  
  if(ecx & 0x40000000){
    printf("I support rdrand\n");
    // TODO: use rdrand
  } else{
    printf("I do not support rdrand\n");
    random = genrand_int32();
  }

  if (random < 0 ) {
    random = random * -1;
  }
     
  return random;
}

int main() {
  unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};
  unsigned long length = 4;
  init_by_array(init, length);  

  unsigned int x = 0;
  printf("%d\n", x);
  x = gen_rand_num();
  printf("%d\n", x);

  return 0;  
}
