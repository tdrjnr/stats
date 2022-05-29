#include <stdio.h>

#ifndef _NO_PROTO
int main(int argc, char **argv)
#else
int main(argc, argv)
int argc;
char **argv;
#endif
{
  char input[100];

  while(fgets(input, 100, stdin)) {
    fputs(input, stdout);
    fflush(stdout);
  }
}
