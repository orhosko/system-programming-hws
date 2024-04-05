#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define MAX_BUF 200

// TODO implement append_test
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

int llseek_test(char *offset) {
  FILE *fp = fopen("/dev/pseudo", "r");
  char buf[MAX_BUF];
  int offs = 0;

  if (offset != NULL) {
    offs = atoi(offset);
  }

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  printf("SEEK_SET: %d\n", offs);

  fseek(fp, offs, SEEK_SET);

  int read = fread(buf, 5, 1, fp);

  printf("%d\n", read);

  for (int i = 0; i < 5; i++) {
    printf("buff: %d(%c)\n", buf[i], buf[i]);
  }

  printf("\n");

  ////////////////////////////////////////////////////////////

  printf("SEEK_CUR: %d\n", offs);

  fseek(fp, offs, SEEK_CUR);

  read = fread(buf, 5, 1, fp);

  printf("%d\n", read);

  for (int i = 0; i < 5; i++) {
    printf("buff: %d(%c)\n", buf[i], buf[i]);
  }

  printf("\n");

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

int shift_test(char *offset) {
  FILE *fp = fopen("/dev/pseudo", "r");
  char buf[MAX_BUF];
  int offs = 0;

  if (offset != NULL) {
    offs = atoi(offset);
  }

  if (fp == NULL) {
    printf("Error opening file\n");
    return -1;
  }

  printf("Offset: %d\n", offs);

  ioctl(fileno(fp), 0, offs);

  fclose(fp);

  read_test("50");

  return 0;
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Usage: %s [r|w|a|l] [size|text|text|offset]\n", argv[0]);
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

  case 'l':
    printf("llseek test\n");
    llseek_test(argv[2]);
    break;

  case 's':
    printf("Shift test\n");
    shift_test(argv[2]);
    break;

  default:
    printf("Error: invalid mode\n");
    return -1;
  }

  return 0;
}
