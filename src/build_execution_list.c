#include <stdlib.h>
#include <string.h>

#include "proj1.h"

execution_list *build_execution_list(char **expanded_tokens, int size) {
  execution_list *node = (execution_list *)malloc(sizeof(execution_list));
  execution_list *first_node = node;
  node->prev = NULL;
  node->type = EXEC_LIST_PROCESS;
  node->next = NULL;
  node->is_background = 0;
  node->command_and_args = create_string_list();
  pipe(node->stdout_pipe);

  for (int i = 0; i < size; i++) {
    char *token = expanded_tokens[i];

    // If this is a pipe or redirect symbol, build a new node.
    // Otherwise, just add the argument to the current command.
    if (!strcmp(token, "|")) {
      execution_list *new_node =
          (execution_list *)malloc(sizeof(execution_list));
      new_node->type = EXEC_LIST_PROCESS;
      new_node->prev = node;
      new_node->next = NULL;
      new_node->command_and_args = create_string_list();
      new_node->is_background = 0;
      node->next = new_node;
      pipe(new_node->stdout_pipe);
      node = new_node;
    } else if (!strcmp(token, ">")) {
      execution_list *new_node =
          (execution_list *)malloc(sizeof(execution_list));
      new_node->type = EXEC_LIST_FILE;
      new_node->prev = node;
      new_node->next = NULL;
      new_node->is_inverted_redirect = 0;
      node->next = new_node;
      node = new_node;
    } else if (!strcmp(token, "<")) {
      execution_list *new_node =
          (execution_list *)malloc(sizeof(execution_list));
      new_node->type = EXEC_LIST_FILE;
      new_node->prev = node;
      new_node->next = NULL;
      new_node->is_inverted_redirect = 1;
      node->next = new_node;
      node = new_node;
    } else if (!strcmp(token, "&")) {
      // Mark the current process as background
      // In order to do this, backtrack until we find a process node.
      execution_list *current = node;
      while (current && current->type != EXEC_LIST_PROCESS) {
        current = current->prev;
      }
      if (current) {
        current->is_background = 1;
      }
    } else {
      // If the current node is a file, then set the filename.
      // Otherwise, add the argument to the command list.
      if (node->type == EXEC_LIST_FILE) {
        node->filename = strdup(token);
      } else {
        string_list_add(node->command_and_args, token);
      }
    }
  }

  return first_node;
}

void print_execution_list(execution_list *exec_list) {
  if (!exec_list)
    return;
  if (exec_list->type == EXEC_LIST_FILE) {
    printf("Exec list file: %s\n", exec_list->filename);
  } else {
    printf("Exec list process: ");
    print_string_list(exec_list->command_and_args);
    printf("\n");
  }
  print_execution_list(exec_list->next);
}

void free_execution_list(execution_list *exec_list) {
  if (!exec_list)
    return;
  free_execution_list(exec_list->next);
  if (exec_list->type == EXEC_LIST_PROCESS) {
    close(exec_list->stdout_pipe[0]);
    close(exec_list->stdout_pipe[1]);
    free_string_list(exec_list->command_and_args);
  } else {
    free(exec_list->filename);
  }
  free(exec_list);
}

void dprint_full_command(int fd, execution_list *node) {
  // First, find the first process in the chain.
  execution_list *root = node;
  while (root->prev) {
    root = root->prev;
  }

  // Next, print each node.
  int i = 0;
  while (root) {
    if (root->type == EXEC_LIST_PROCESS) {
      if (i++ > 0) {
        dprintf(fd, " | ");
      }
      dprint_string_list(fd, root->command_and_args);
    } else {
      if (i++ > 0) {
        if (root->is_inverted_redirect) {
          dprintf(fd, " < ");
        } else {
          dprintf(fd, " > ");
        }
      }

      dprintf(fd, "%s", root->filename);
    }
    root = root->next;
  }
}
