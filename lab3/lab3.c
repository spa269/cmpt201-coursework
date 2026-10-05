#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// store line(after 5-> 0)
/// total number = line added so far(have to change the value of total -> pass by ptr)
void addToHistory(char *history[], int *total, char *line) {
  int index = *total % 5;
  free(history[index]);
  history[index] = line;
  (*total)++;
}
char *readLine() {
  char *buffer = NULL;
  size_t capacity = 0;

  printf("Enter input: ");
  ssize_t length = getline(&buffer, &capacity, stdin);
  if (length == -1) {
    free(buffer);
    return NULL;
  }
  if (length > 0 && buffer[length - 1] == '\n') {
    buffer[length - 1] = '\0';
  } // remove \n(enter),
  return buffer;
}

// print the stored lines, doesn't have to change the value of total(pass by value)
void printHistory(char *history[], int total) {
  int start;
  if (total > 5) {
    start = total - 5;
  } // more than 5 lines --> print recent 5
  else {
    start = 0;
  } // less than 5 lines --> print from the start
  for (int i = start; i < total; i++) {
    printf("%s\n", history[i % 5]);
  }
}

int main() {
  char *history[5] = {NULL};
  int total = 0;
  char *line;
  while (1) {
    char *line = readLine();
    if (line == NULL) {
      break;
    }
    addToHistory(history, &total, line);
    if (strcmp(line, "print") == 0) {
      printHistory(history, total);
    }
  }
  printf("\n");
  // free memory(getline() has malloc inside so have to free it!)
  for (int i = 0; i < 5; i++) {
    free(history[i]);
  }
  return 0;
}
