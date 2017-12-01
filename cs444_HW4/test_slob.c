lude <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  double claimed = 0;
  double total = 0;
  if (argc != 2) {
    printf("Must provide seed\nExiting...\n");
    exit(1);
  }

  unsigned int seed = atoi(argv[1]);

  int i;
  char* x;

  srand((unsigned) seed);
  for (i = 0; i < 1000; i++) {
    x = malloc(rand() % 10000);
    free(x);
  }
  claimed = syscall(360);
  total = syscall(359);
  double eff1 = claimed / total;

  for (i = 0; i < 1000; i++) {

    x = malloc(rand() % 10000);
    free(x);
  }
  claimed = syscall(360);
  total = syscall(359);
  double eff2 = claimed / total;

  for (i = 0; i < 1000; i++) {

    x = malloc(rand() % 10000);
    free(x);
  }
  claimed = syscall(360);
  total = syscall(359);
  double eff3 = claimed / total;

  double effa = (eff1 + eff2 + eff3) / 3;
  double fragmentation = 1-effa;
  printf("Fragmentation: %lf%\n", fragmentation);

  return 0;
}

