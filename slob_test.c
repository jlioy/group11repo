#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifndef __NR_memory_size
#define __NR_memory_size 359
#endif

#ifndef __NR_memory_used
#define __NR_memory_used 360
#endif

int main() {
        char * buffer0, buffer1, buffer2, buffer3, buffer4, buffer5, buffer6, buffer7;

        buffer0 = (char*) malloc (9000000);
        buffer1 = (char*) malloc (345555);
        buffer2 = (char*) malloc (1234);
        buffer3 = (char*) malloc (144445554);
        buffer4 = (char*) malloc (55630);
        buffer5 = (char*) malloc (10064666665);
        buffer6 = (char*) malloc (105555555550);
        buffer7 = (char*) malloc (1000000000);

        double psize = (double)syscall(__NR_memory_size);
        printf("Size: %f\n",psize);
        double pused = (double)syscall(__NR_memory_used);
        printf("Used: %f\n",pused);

        double total = psize + pused;
        double eff = used / total;

        printf("Efficiency: %f\n", eff);
        return 0;
}
