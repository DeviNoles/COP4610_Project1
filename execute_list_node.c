#include "proj1.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_list_node(execution_list *current_node, execution_list *last_node,
                       char *PATH) {
  // Do work depending on which the nodes are
  if (current_node->type == EXEC_LIST_PROCESS || 1) {
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
    char *exec_path = lookup_executable(argv[0], PATH);
    if (!exec_path) {
      printf("Command does not exist.\n");
    } else {
      pid_t child_pid = fork();
      if (child_pid == 0) {
        // we are in the child process
        execv(exec_path, argv);
        printf("bruh moment.\n");
      } else {
        // We are in the parent process
        pid_t temp;
        current_node->pid = child_pid;
        if (last_node) {
          if (last_node->type == EXEC_LIST_PROCESS) {
            pipe_stdout_into_stdin(last_node->pid, child_pid);
          } else if (last_node->type == EXEC_LIST_FILE) {
            pipe_file_into_stdin(last_node->filename, child_pid);
          }
        }
        do {
          temp = wait(NULL);
          if (temp != child_pid)
            break;
        } while (temp != child_pid);
        printf("It's died.\n");
      }
    }
  }
}
