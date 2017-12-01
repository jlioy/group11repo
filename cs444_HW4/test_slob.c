#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Must provide number of chars to allocate\nExiting...\n");
    exit(1);
  }

  int num_chars = atoi(argv[1]);
  int i;
  char* x;

  printf("%ld\n", syscall(359));
  srand((unsigned) num_chars);
  for (i = 0; i < 1000; i++) {
    x = malloc(rand() % 10000);
    free(x);
  }
  double cl1 = syscall(360);
  double total = syscall(359) + cl1
  double eff1 = cl1 / total;

  for (i = 0; i < 1000; i++) {

    x = malloc(rand() % 10000);
    free(x);
  }
  doulble cl2 = syscall(360);
  total = syscall(359) + cl2
  double eff2 = cl2 / total;

  for (i = 0; i < 1000; i++) {

    x = malloc(rand() % 10000);
    free(x);
  }
  double cl3 = syscall(360);
  total = syscall(359) + cl3
  double eff3 = cl3 / total;

  double effa = (cl1 + cl2 + cl3) / 3;

  printf("Free: %ld\n", fr);
  printf("Used: %ld\n", claimed);
  printf("Total: %ld\n", total);
  printf("Efficiency: %lf%\n", efficiency * 100);

  return 0;
}
