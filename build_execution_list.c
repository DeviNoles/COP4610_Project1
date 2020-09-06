#include <stdlib.h>
#include <string.h>

#include "proj1.h"

execution_list* build_execution_list(char** expanded_tokens, int size) {
  execution_list* node = (execution_list*)malloc(sizeof(execution_list));
  execution_list* first_node = node;
  node->type = EXEC_LIST_PROCESS;
  node->next = NULL;

  for (int i = 0; i < size; i++) {
    char* token = expanded_tokens[i];

    // If this is a pipe or redirect symbol, build a new node.
    // Otherwise, just add the argument to the current command.
    if (!strcmp(token, "|")) {
      execution_list* new_node =
	  (execution_list*)malloc(sizeof(execution_list));
      new_node->type = EXEC_LIST_PROCESS;
      new_node->next = NULL;
      node->next = new_node;
      node = new_node;
    } else if (!strcmp(token, ">")) {
      execution_list* new_node =
	  (execution_list*)malloc(sizeof(execution_list));
      new_node->type = EXEC_LIST_FILE;
      new_node->next = NULL;
      node->next = new_node;
      node = new_node;
    } else {
      // TODO: string list append
    }
  }

  return first_node;
}

void print_execution_list(execution_list* exec_list) {
  if (!exec_list) return;
  print_execution_list(exec_list->next);
  if (exec_list->type == EXEC_LIST_FILE) {
    printf("Exec list file: %s\n", exec_list->filename);
  } else {
    // TODO: Print string list
    printf("Exec list process\n");
    // TODO: Print full command
  }
}

void free_execution_list(execution_list* exec_list) {
  if (!exec_list) return;
  free_execution_list(exec_list->next);
  // TODO: Actually delete the things allocated here
}
