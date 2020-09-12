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
  string_list *current = node->command_and_args;
  if (node->command_and_args->next) {
    print_string_list(node->command_and_args->next);
    printf("\n");
  }
}

void internal_exit(execution_list *node) {}
