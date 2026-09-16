#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
  char *linePtr = NULL;
  size_t n = 0;

  printf("Please enter some text : ");
  ssize_t len = getline(&linePtr, &n, stdin);
  // error handling if getline() failed
  if (len < 0) {
    perror("getline failed!");
    free(linePtr); // free the line variable allcated
    exit(EXIT_FAILURE);
  }

  char *saveptr = NULL;
  char *ret = strtok_r(linePtr, " ", &saveptr);
  printf("Tokens : ");
  while ((ret = strtok_r(NULL, " ", &saveptr))) {
    printf("%s\n", ret);
  }

  free(linePtr);
  return 0;
}

// char *saveptr;
// char *ret = strtok_r(buff, " ", &saveptr);
