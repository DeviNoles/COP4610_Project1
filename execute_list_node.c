#include <stdlib.h>

#include "proj1.h"

void execute_list_node(execution_list *current_node,
                       execution_list *last_node) {
  // Do work depending on which the nodes are
  if (current_node->type == EXEC_LIST_PROCESS) {
    string_list *current = current_node->command_and_args;
    // count the length of the string_list
    int len = 0;
    while (current) {
      len++;
      current = current->next;
    }
    char **argv = (char **)malloc(len * sizeof(char *));
    // index for argv
    int index = 0;
    current = current_node->command_and_args;
    while (current) {
      argv[index++] = current->value;
      current = current->next;
    }
    // at this point, argv contains the necessary process stuff
    // TODO: actually do execv logic here
    // TODO: lookup path of command when we use execv
    // TODO: If lookup_executable returns NULL, report an error instead
    // of starting a process.
    char *exec_path = lookup_executable(argv[0]);
  }
}
