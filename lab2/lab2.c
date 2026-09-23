
#include <stdio.h>     //printf, getline, perror
#include <stdlib.h>    //free, exit, exit failure
#include <string.h>    //strtok
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // waitpid
#include <unistd.h>    //fork, execlp

int main() {
  char *linePtr = NULL;
  size_t n = 0;

  while (1) {
    printf("Enter programs to run. \n");
    ssize_t len = getline(&linePtr, &n, stdin);
    if (len == -1) {
      free(linePtr);
      break;
    }

    char *saveptr;
    char *tokenized = strtok_r(linePtr, "\n", &saveptr);

    pid_t pid = fork();

    if (pid < 0) {
      perror("fork failed");
      break;
    }
    // fork error
    if (pid == 0) {
      // child process
      execlp(tokenized, tokenized, NULL);
      // exec failed
      printf("Execution failed");
      exit(EXIT_FAILURE);
    } else {
      // parent process
      int status; // stores the information about how child proecess terminated
      if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
      }
    }
  }
  free(linePtr);
  return 0;
}
