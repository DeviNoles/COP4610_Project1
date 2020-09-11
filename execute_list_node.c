#include "proj1.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
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
    // TODO: If lookup_executable returns NULL, report an error instead
    // of starting a process.

    // cd, echo, jobs, exit
    if (!strcmp(argv[0], "cd")) {
      execute_internal_command("cd", current_node);
    } else if (!strcmp(argv[0], "echo")) {
      execute_internal_command("echo", current_node);
    } else if (!strcmp(argv[0], "jobs")) {
      execute_internal_command("jobs", current_node);
    } else if (!strcmp(argv[0], "exit")) {
      execute_internal_command("exit", current_node);
    } else {
      // Execute external commands
      char *exec_path = lookup_executable(argv[0], PATH);
      if (!exec_path) {
        printf("Command does not exist.\n");
      } else {
        pid_t child_pid = fork();
        if (child_pid == 0) {
          // we are in the child process

          // Create a pipe for this process.
          pipe(current_node->fds);

          // Connect stdin and stdout to the created pipe.
          // dup2(current_node->fds[0], STDIN_FILENO);
          // dup2(current_node->fds[1], STDOUT_FILENO);
          // dup2(STIN_FILENO, current_node->fds[0]);
          // dup2(STDOUT_FILENO, current_node->fds[1]);
          // close(current_node->fds[0]);
          // close(current_node->fds[1]);

          // If we need to do any forwarding, then we should do it now.
          // TODO: If there is no next node to forward to, then stdout should
          // print to the terminal.
          //
          // TODO: Like wise, if there is no previous node, then stdin should be
          // stdin from the terminal itself.

          if (last_node) {
            if (last_node->type == EXEC_LIST_PROCESS) {
              // int result = dup2(last_node->fds[1], current_node->fds[0]);
              // close(last_node->fds[0]);
              // printf("dup2 returned %d\n", result);
            } else if (last_node->type == EXEC_LIST_FILE) {
              // int fd = open(last_node->filename, O_RDONLY);
              // dup2(fd, current_node->fds[1]);
              // close(fd);
            }
          }
          execv(exec_path, argv);
          printf("bruh moment.\n");
        } else {
          // We are in the parent process
          // pid_t temp;
          // current_node->pid = child_pid;
          // do {
          //   temp = wait(NULL);
          //   if (temp != child_pid)
          //     break;
          // } while (temp != child_pid);
          // printf("It's died.\n");
        }
      }
    }
  }
}
