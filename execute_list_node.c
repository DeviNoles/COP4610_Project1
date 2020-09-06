#include "proj1.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_list_node(execution_list *current_node,
                       execution_list *last_node, char* PATH) {
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
        printf("TESTTTTTT\n");
        current_node->fds[0] = STDIN_FILENO;
        current_node->fds[1] = STDOUT_FILENO;
        pipe(current_node->fds);
        if (last_node && last_node->type == EXEC_LIST_PROCESS) {
            dup2(last_node->fds[0], STDIN_FILENO);
            close(last_node->fds[0]); // we have a copy already, so close it
            close(last_node->fds[1]); // not using this end
            dup2(current_node->fds[1], STDOUT_FILENO);
            close(current_node->fds[0]); // not using this end
            close(current_node->fds[1]); // we have a copy already, so close it
        }
        printf("barack obunga\n");
        execv(exec_path, argv);
        printf("bruh moment.\n");
      } else {
        pid_t temp;
        do {
            printf("POOPY\n");
          temp = wait(NULL);
          if (temp != child_pid)
            break;
        } while (temp != child_pid);
        printf("It's died.\n");
      }
    }
  }
  else {
      printf("aGOOGOO GAGA\n");
  }
}
