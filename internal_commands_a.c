#include "proj1.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void internal_cd(execution_list *node) {
  // Throw an error if there is more than one argument.
  string_list *cur = node->command_and_args->next;
  if (cur && cur->next) {
    fprintf(stderr, "cd takes only one argument at max.\n");
    return;
  }

  // cd $HOME if zero arguments.
  if (node->command_and_args->next == NULL) {
    chdir(getenv("HOME"));
    return;
  } else if (strcmp(node->command_and_args->next->value, "~") == 0) {
    // Support ~
    chdir(getenv("HOME"));
    return;
  }

  char *path = node->command_and_args->next->value;
  struct stat st;
  int result = stat(path, &st);

  // Check if it exists.
  if (result == -1 && errno == ENOENT) {
    fprintf(stderr, "%s: no such file or directory\n", path);
    return;
  }

  // If it's not a directory, we can't cd.
  if (!S_ISDIR(st.st_mode)) {
    fprintf(stderr, "%s is not a directory.\n", path);
    return;
  }

  if (chdir(path) == -1) {
    fprintf(stderr, "Failed to chdir() into %s\n", path);
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
  // Wait for jobs
  execution_list *job = background_jobs;
  while (job) {
    int status;
    if (!job->has_completed && job->pid != 0) {
      waitpid(job->pid, &status, 0);
    }
    job = job->next;
  }

  printf("Commands executed: %d\n", total_jobs);

  // Release all job memory
  free_execution_list(background_jobs);

  // If the user provided an exit code, use that. Otherwise, default to 0.
  int exit_code = 0;
  if (node->command_and_args->next) {
    exit_code = atoi(node->command_and_args->next->value);
  }
  exit(exit_code);
}
