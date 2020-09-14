#include "proj1.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

extern execution_list *background_jobs;

void setup_pipes(execution_list *current_node, execution_list *last_node,
                 int *term_fds);

int is_internal_command(const char *command) {
  return (!strcmp(command, "cd") || !strcmp(command, "echo") ||
          !strcmp(command, "jobs") || !strcmp(command, "exit"));
}

void execute_list_node(execution_list *current_node, execution_list *last_node,
                       char *PATH, int *term_fds) {
  // Do work depending on which the nodes are
  if (current_node->type == EXEC_LIST_PROCESS) {
    string_list *current = current_node->command_and_args;
    // count the length of the string_list
    int len = 0;
    while (current) {
      len++;
      current = current->next;
    }

    // If the command is empty, just skip.
    if (len == 0)
      return;
    if (!current_node->command_and_args->value)
      return;
    if (strlen(current_node->command_and_args->value) == 0)
      return;

    char **argv = (char **)malloc(len * sizeof(char *));
    // index for argv
    int index = 0;
    current = current_node->command_and_args;
    while (current) {
      argv[index++] = current->value;
      current = current->next;
    }

    // at this point, argv contains the necessary process stuff
    // If lookup_executable returns NULL, report an error instead
    // of starting a process.

    // Setup all pipes before doing work
    // Either do an internal command, or go external

    if (is_internal_command(argv[0])) {
      execution_list *next = current_node->next;
      current_node->pid = 0;
      // Increment total jobs
      total_jobs++;

      if (next) {
        if (next->type == EXEC_LIST_PROCESS) {
          // close(current_node->stdout_pipe[0]);
          // close(current_node->stdout_pipe[1]);
        } else if (next->type == EXEC_LIST_FILE) {
          if (!next->is_inverted_redirect) {
            int exists = access(current_node->next->filename, F_OK) != -1;
            int fd = open(next->filename, O_RDWR | O_CREAT | O_TRUNC);
            if (!exists) {
              chmod(current_node->next->filename, 0664);
            }
            dup2(fd, current_node->stdout_pipe[1]);
            close(fd);
          } else {
            int fd = open(current_node->next->filename, O_RDONLY);
            dup2(fd, current_node->stdout_pipe[0]);
            close(fd);
          }
        }
      } else {
        // current_node->stdout_pipe[1] = STDOUT_FILENO;
        dup2(STDOUT_FILENO, current_node->stdout_pipe[1]);
      }

      // if (!strcmp(argv[0], "echo")) {
      //   current_node->stdout_pipe[1] = STDOUT_FILENO;
      // }

      execute_internal_command(argv[0], current_node);
      // close(current_node->stdout_pipe[0]);

      if (next) {
        close(current_node->stdout_pipe[1]);
      }
    } else {
      // If the command has a /, treat it like a file.
      char *exec_path = NULL;
      if (strstr(argv[0], "/") != NULL) {
        if (access(argv[0], F_OK) != -1) {
          exec_path = argv[0];
        } else {
          printf("Command does not exist.\n");
          return;
        }
      }

      // If it is a file that exists, don't use path.
      else {
        exec_path = lookup_executable(argv[0], PATH);
        if (!exec_path) {
          printf("Command does not exist.\n");
          return;
        }
      }

      if (exec_path) {
        // Increment total jobs
        total_jobs++;

        pid_t child_pid = fork();
        if (child_pid == 0) {
          // we are in the child process
          setup_pipes(current_node, last_node, term_fds);

          // Execute external commands
          execv(exec_path, argv);
        } else {
          // We are in the parent process
          current_node->pid = child_pid;

          // Wait for the process to exit, if it is not being piped into
          // anything, or it is not a background process.
          if (!current_node->is_background) {
            int status;
            waitpid(child_pid, &status, 0);

            if (current_node->next &&
                current_node->next->type == EXEC_LIST_PROCESS) {
              close(current_node->stdout_pipe[1]);
            }
          }
        }
      }
    }
  }
}

void setup_pipes(execution_list *current_node, execution_list *last_node,
                 int *term_fds) {
  // If we need to do any forwarding, then we should do it now.
  // If there is no next node to forward to, then stdout should
  // print to the terminal.
  //
  // Like wise, if there is no previous node, then stdin should be
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
      execution_list *file_node = current_node->next;
      while (file_node && file_node->type == EXEC_LIST_FILE) {
        // If it's inverted, then use it as stdin
        if (!file_node->is_inverted_redirect) {
          // Check if the file existed, so that if it did not, we give it 664.
          int exists = access(file_node->filename, F_OK) != -1;
          int fd = open(file_node->filename, O_RDWR | O_CREAT | O_TRUNC);
          if (!exists) {
            chmod(file_node->filename, 0664);
          }
          dup2(fd, STDOUT_FILENO);
          close(fd);
        } else {
          int fd = open(file_node->filename, O_RDONLY);
          dup2(fd, STDIN_FILENO);
          close(fd);
        }
        file_node = file_node->next;
      }

      // If a process comes after this, then we need to do two things:
      // 1. Setup pipes, of course.
      // 2. Change the ->next to the process in question.
      // Both can be knocked out easily.
      if (file_node && file_node->type == EXEC_LIST_PROCESS) {
        current_node->next = file_node;
        file_node->prev = current_node;
      }
    }
  } else {
    // If there is no next node, then pipe to the terminal.
    // fprintf(stderr, "out\n");
    // close(term_fds[0]);
    // dup2(term_fds[1], STDOUT_FILENO);
    // close(term_fds[1]);
  }
}
