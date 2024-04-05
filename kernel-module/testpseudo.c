#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 200

int append_test(char *text) {
  FILE *fp = fopen("/dev/pseudo", "w+");

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  fwrite("Hello, world!", 1, 13, fp);

  if (strlen(text) > MAX_BUF) {
    printf("Error: text too large\n");
    return -1;
  }

  if (text != NULL) {
    fwrite(text, 1, strlen(text), fp);
  }

  fclose(fp);
  return 0;
}

int write_test(char *text) {
  FILE *fp = fopen("/dev/pseudo", "w");

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  fwrite("Hello, world!", 1, 13, fp);

  if (strlen(text) > MAX_BUF) {
    printf("Error: text too large\n");
    return -1;
  }

  if (text != NULL) {
    fwrite(text, 1, strlen(text), fp);
  }

  fclose(fp);
  return 0;
}

int read_test(char *size) {
  FILE *fp = fopen("/dev/pseudo", "r");
  char buf[MAX_BUF];
  int read_size = 0;

  if (size != NULL) {
    read_size = atoi(size);
  }

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  printf("%d\n", read_size);

  int read = fread(buf, read_size, 1, fp);

  printf("%d\n", read);

  buf[read] = '\0';

  for (int i = 0; i < read_size; i++) {
    printf("buff: %d(%c)\n", buf[i], buf[i]);
  }

  printf("\n");

  fclose(fp);
  return 0;
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Usage: %s [r|w] [size|text]\n", argv[0]);
    return -1;
  }

  switch (argv[1][0]) {
  case 'r':
    printf("Read test\n");
    read_test(argv[2]);
    break;

  case 'w':
    printf("Write test\n");
    write_test(argv[2]);
    break;

  case 'a':
    printf("Append test\n");
    append_test(argv[2]);
    break;

  default:
    printf("Error: invalid mode\n");
    return -1;
  }

  return 0;
}
