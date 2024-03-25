#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF 200

int main(int argc, char *argv[]) {
  FILE *fp = NULL;
  char buf[MAX_BUF];
  int read_size = 0;

  fp = fopen("/dev/pseudo", "r");

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  if (argc < 2) {
    printf("Error: read size not specified\n");
    return -1;
  }

  read_size = atoi(argv[1]);

  if (read_size > MAX_BUF) {
    printf("Error: read size too large\n");
    return -1;
  }

  if (read_size < 1) {
    printf("Error: read size too small\n");
    return -1;
  }

  fread(buf, 1, read_size, fp);

  buf[read_size] = '\0';

  for (int i = 0; i < read_size; i++) {
    printf("pseudo: %d\n", buf[i]);
  }

  fclose(fp);
  return 0;
}
