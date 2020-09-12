#include "proj1.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
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
  execution_list *job = background_jobs;
  while (job) {
    int status;
    waitpid(job->pid, &status, 0);
    job = job->next;
  }

  // Release all job memory
  free_execution_list(background_jobs);

  // If the user provided an exit code, use that. Otherwise, default to 0.
  int exit_code = 0;
  if (node->command_and_args->next) {
    exit_code = atoi(node->command_and_args->next->value);
  }
  exit(exit_code);
}
