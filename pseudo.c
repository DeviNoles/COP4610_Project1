typedef struct {
  char** command_and_args;
  char* filename;
  FILE* input, *output;
  enum {
    FILE,
    PROCESS
  } type;
} execution_list;

int main() {
  while (true) {
    print_prompt();
    line = read_stdin_line();
    tokenlist* tokens = parse(line);

    // The first token is the command. Everything else is an argument, zero or more arguments.
    char** expanded_tokens = (char*)(sizeof(char*) * tokens->size);

    for (int i = 0; i < tokens->size; i++) {
      char* expanded = expand_argument(tokens->items[i]); // EXPANDINATOR
      expanded_tokens[i] = expanded;
    }

    // By this point, all paths and variables have been expanded.
    // Next, build a linked list of execution_list nodes.
    // Basically, we want to loop through the tokens, and each group of tokens, separated
    // by > or |, should be associated with a single execution_list node.
    //
    // Once we have the execution_list built, everything else is very easy.
    execution_list* exec_list = build_execution_list(expanded_tokens);

    // Just loop through each one, keeping track of the previous item, to know where stdin
    // and out are coming from.
    //
    execution_list* last_node = NULL;
    while (exec_list) {
      // There are four cases here. Basically, you need to look at last_node to see where
      // input for the current process/file comes from.
      // If the PROCESS is a builtin, we should call the appropriate function here.
      // Otherwise, it's an external command. Look up the program in the $PATH, and then call execve.
      last_node = exec_list;
      exec_list = exec_list->next;
    }
}
