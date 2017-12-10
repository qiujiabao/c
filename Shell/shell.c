
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#define MAXLINE 80 /* The maximum length command */
int main(void)
{
  char *args[MAXLINE/2 + 1]; /* command line with max 40 arguments */
  int should_run = 1; /* flag to determine when to exit program */
  int cocurrent; /* flag to determine if the parent should wait */
  while (should_run) {
    printf("Shell>");
    fflush(stdout);
    char str[41];
    fgets(str, 41, stdin); /* reading input */
    char *token;
    int i = 0;
    cocurrent = 0; /* default: parent should wait */
    token = strtok(str, " \n");
    while (token != NULL) {
      if (token[strlen(token)-1] == '\n') token[strlen(token)-1] = '\0';
      /* Since fgets() reads the "return", it should be deleted. */
      if (strcmp(token, "exit") == 0) should_run = 0;
      if (strcmp(token, "&") == 0) cocurrent = 1;
      else args[i] = token;
      i++;
      token = strtok(NULL, " ");
    }
    int pid = fork();
    if (pid < 0) {
      fprintf(stderr, "Fork failed.");
    }
    else if (pid == 0) {
        if (should_run == 1) {
          if (execvp(args[0], args) < 0) perror("Error");
        }
    }
    else {
      if (cocurrent == 0) { wait(NULL); }
    }
  }
  return 0;
}
