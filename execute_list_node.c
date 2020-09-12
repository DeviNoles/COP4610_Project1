#include "proj1.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void setup_pipes(execution_list *current_node, execution_list *last_node,
                 int *term_fds);

int is_internal_command(const char *command) {
  return (!strcmp(command, "cd") || !strcmp(command, "echo") ||
          !strcmp(command, "jobs") || !strcmp(command, "exit"));
}

void execute_list_node(execution_list *current_node, execution_list *last_node,
                       char *PATH, int *term_fds) {
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

    // Setup all pipes before doing work
    // Either do an internal command, or go external

    if (is_internal_command(argv[0])) {
      // setup_pipes(current_node, last_node, term_fds);
      execution_list *next = current_node->next;
      if (next) {
        if (next->type == EXEC_LIST_PROCESS) {
          // dup2(current_node->stdout_pipe[1], STDOUT_FILENO);
          // close(current_node->stdout_pipe[0]);
          // close(current_node->stdout_pipe[1]);
          // Write to stdout
        } else if (next->type == EXEC_LIST_FILE) {
          int fd = open(next->filename, O_WRONLY | O_CREAT);
          dup2(fd, current_node->stdout_pipe[1]);
          close(fd);
        }
      } else {
        current_node->stdout_pipe[1] = STDOUT_FILENO;
      }

      close(current_node->stdout_pipe[0]);
      execute_internal_command(argv[0], current_node);

      if (next) {
        close(current_node->stdout_pipe[1]);
      }
    } else {
      // If it is a file that exists, don't use path.
      char *exec_path = lookup_executable(argv[0], PATH);
      if (!exec_path) {
        if (access(argv[0], F_OK) != -1) {
          exec_path = argv[0];
        } else {
          printf("Command does not exist.\n");
          return;
        }
      }

      if (exec_path) {
        pid_t child_pid = fork();
        if (child_pid == 0) {
          // we are in the child process
          setup_pipes(current_node, last_node, term_fds);

          // Execute external commands
          execv(exec_path, argv);
        }
      }
    }
  }
}

void setup_pipes(execution_list *current_node, execution_list *last_node,
                 int *term_fds) {
  // If we need to do any forwarding, then we should do it now.
  // TODO: If there is no next node to forward to, then stdout should
  // print to the terminal.
  //
  // TODO: Like wise, if there is no previous node, then stdin should be
  // stdin from the terminal itself.

  if (last_node) {
    if (last_node->type == EXEC_LIST_PROCESS) {
      // Read from previous stdout
      dup2(last_node->stdout_pipe[0], STDIN_FILENO);
      close(last_node->stdout_pipe[0]);
      close(last_node->stdout_pipe[1]);
    } else if (last_node->type == EXEC_LIST_FILE) {
      int fd = open(last_node->filename, O_RDONLY);
      dup2(fd, STDIN_FILENO);
      close(fd);
    }
  }

  if (current_node->next) {
    if (current_node->next->type == EXEC_LIST_PROCESS) {
      dup2(current_node->stdout_pipe[1], STDOUT_FILENO);
      close(current_node->stdout_pipe[0]);
      close(current_node->stdout_pipe[1]);
      // Write to stdout
    } else if (current_node->next->type == EXEC_LIST_FILE) {
      int fd = open(current_node->next->filename, O_WRONLY | O_CREAT);
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
  } else {
    // If there is no next node, then pipe to the terminal.
    // fprintf(stderr, "out\n");
    // close(term_fds[0]);
    // dup2(term_fds[1], STDOUT_FILENO);
    // close(term_fds[1]);
  }
}
