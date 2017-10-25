#ifndef MT_H
#define MT_H

unsigned long genrand_int32(void);
void init_by_array(unsigned long init_key[], int key_length);
void init_genrand(unsigned long s);
#endif 
