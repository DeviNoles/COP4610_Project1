#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "proj1.h"

execution_list *background_jobs;
int total_jobs;

int main() {
  // Init background jobs
  int job_count = 0;
  background_jobs = NULL;
  total_jobs = 0;

  // Create a pipe for stdin and stdout.
  int term_fds[2];
  pipe(term_fds);

  // Connect stdin, stdout
  // dup2(STDIN_FILENO, term_fds[0]);
  dup2(STDOUT_FILENO, term_fds[1]);
  close(term_fds[0]);
  close(term_fds[1]);

  while (1) {
    char *PATH = getenv("PATH");
    char *cwd = getcwd(NULL, 0);
    char *machine = getenv("MACHINE");
    if (!machine) {
      machine = getenv("HOSTNAME");
    }
    if (!machine) {
      machine = "(unknown machine)";
    }
    printf("%s@%s: %s > ", getenv("USER"), machine, cwd);

    /* input contains the whole command
     * tokens contains substrings from input split by spaces
     */

    char *input = get_input();
    //	printf("whole input: %s\n", input);

    tokenlist *tokens = get_tokens(input);

    // The first token is the command. Everything else is an argument, zero or
    // more arguments.
    char **expanded_tokens = (char **)malloc(sizeof(char *) * tokens->size);

    for (int i = 0; i < tokens->size; i++) {
      // printf("token %d: (%s)\n", i, tokens->items[i]);
      char *expanded = expand_argument(tokens->items[i]);
      expanded_tokens[i] = expanded;
    }

    // By this point, all paths and variables have been expanded.
    // Next, build a linked list of execution_list nodes.
    // Basically, we want to loop through the tokens, and each group of tokens,
    // separated by > or |, should be associated with a single execution_list
    // node.
    //
    // Once we have the execution_list built, everything else is very easy.
    execution_list *exec_list =
        build_execution_list(expanded_tokens, tokens->size);

    // Just loop through each one, keeping track of the previous item, to know
    // where stdin and out are coming from.
    execution_list *first_node = exec_list;
    execution_list *last_node = NULL;
    while (exec_list && exec_list != last_node) {
      // There are four cases here. Basically, you need to look at last_node to
      // see where input for the current process/file comes from.
      // TODO: If the PROCESS is a builtin, we should call the appropriate
      // function here. Otherwise, it's an external command. Look up the program
      // in the $PATH, and then call execve.
      execute_list_node(exec_list, last_node, PATH, term_fds);

      // Ignore background tasks here
      if (exec_list->type == EXEC_LIST_PROCESS && exec_list->is_background) {
        // Add background
        execution_list *new_node =
            (execution_list *)malloc(sizeof(execution_list));
        *new_node = *exec_list;
        new_node->job_id = (job_count++) + 1;
        new_node->has_completed = 0;

        if (!background_jobs) {
          background_jobs = new_node;
        } else {
          execution_list *cur = background_jobs;
          while (cur->next) {
            cur = cur->next;
          }
          cur->next = new_node;
        }

        // Print background task
        // [Job number] [CMD’s PID]
        printf("[%d] %ld\n", new_node->job_id, (long)exec_list->pid);
      }

      last_node = exec_list;
      exec_list = exec_list->next;
    }

    // Wait for jobs
    // TODO: Be able to print the entire command (i.e. CMD | CMD2)
    execution_list *job = background_jobs;
    while (job) {
      int status;
      if (!job->has_completed && job->pid != 0) {
        int result = waitpid(job->pid, &status, WNOHANG);
        if (result > 0) {
          dprintf(STDOUT_FILENO, "[%d]+ ", job->job_id);
          dprint_full_command(STDOUT_FILENO, job);
          dprintf(STDOUT_FILENO, "\n");
          job->has_completed = 1;
        }
      }
      job = job->next;
    }

    // TODO: Cleanup execution list, etc.
    // If any child is a background node, save the cleanup until later.
    // if (!first_node->is_background) {
    //   free_execution_list(first_node);
    // }
    free(input);
    free_tokens(tokens);
  }

  return 0;
}
