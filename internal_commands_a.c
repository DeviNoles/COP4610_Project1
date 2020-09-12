#include "proj1.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void internal_cd(execution_list *node) {
  if (node->command_and_args->next == NULL) {
    chdir(getenv("HOME"));
    return;
  } else if (strcmp(node->command_and_args->next->value, "~") == 0) {
    chdir(getenv("HOME"));
    return;
  }
  char *temp = node->command_and_args->next->value;
  printf("CHANGING DIRECTORIES: %s\n", temp);
  if (chdir(temp) == -1) {
    printf("%s is not a directory.\n", temp);
  }
}

void internal_echo(execution_list *node) {
  // printf("CALLING INTERNAL ECHO\n");
  string_list *current = node->command_and_args->next;
  int i = 0;
  int fd = node->stdout_pipe[1];

  while (current) {
    if (i++ > 0) {
      dprintf(fd, " ");
    }
    dprintf(fd, "%s", current->value);
    current = current->next;
  }
  dprintf(fd, "\n");
}

void internal_exit(execution_list *node) {
  // TODO: Wait for jobs

  // If the user provided an exit code, use that. Otherwise, default to 0.
  int exit_code = 0;
  if (node->command_and_args->next) {
    exit_code = atoi(node->command_and_args->next->value);
  }
  exit(exit_code);

  // wait for global list to exit
  // pid_t waitpid(pidlist, int *status, int options);
  // Option is 0 since I check it later
  /*might need this
   if (WIFSIGNALED(status)){
       printf("Error\n");
   }
   else if (WEXITSTATUS(status)){
       printf("Exited Normally\n");
   }
  //To Here and see the difference*/
}
