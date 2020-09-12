#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "proj1.h"

int main() {
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
    char* cwd = getcwd(NULL, 0);
    printf("%s@%s: %s > ", getenv("USER"), getenv("MACHINE"), cwd);

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
    execution_list *last_node = NULL;
    pid_t last_pid; // TODO: Set to NULL PID
    while (exec_list) {
      // There are four cases here. Basically, you need to look at last_node to
      // see where input for the current process/file comes from.
      // TODO: If the PROCESS is a builtin, we should call the appropriate
      // function here. Otherwise, it's an external command. Look up the program
      // in the $PATH, and then call execve.
      execute_list_node(exec_list, last_node, PATH, term_fds);

      // Track the final PID.
      if (exec_list->type == EXEC_LIST_PROCESS) {
        // TODO: Ignore background tasks here
        last_pid = exec_list->pid;
      }

      last_node = exec_list;
      exec_list = exec_list->next;
    }

    // TODO: Make it so that exec_list always has a PID, and then we call
    // wait(pid)
    pid_t temp;
    do {
      temp = wait(NULL);
      if (temp != last_pid)
        break;
    } while (temp != last_pid);

    // TODO: Cleanup execution list, etc.
    free_execution_list(exec_list);
    free(input);
    free_tokens(tokens);
  }

  return 0;
}
