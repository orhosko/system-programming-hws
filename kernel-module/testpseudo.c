#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 200

int main(int argc, char *argv[]) {
  FILE *fp = NULL;
  char buf[MAX_BUF];
  int read_size = 0;

  if (argv[1][0] == 'w') {
    fp = fopen("/dev/pseudo", "w");
  } else {
    printf("Error: invalid mode\n");
  }

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  if (argc < 3) {
    printf("Error: text is not given\n");
    return -1;
  }

  if (strlen(argv[2]) > MAX_BUF) {
    printf("Error: text too large\n");
    return -1;
  }

  fwrite(argv[2], 1, strlen(argv[2]), fp);

  fclose(fp);

  fp = fopen("/dev/pseudo", "r");

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  read_size = strlen(argv[2]);

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
    printf(": %d\n", buf[i]);
  }

  fclose(fp);
  return 0;
}
