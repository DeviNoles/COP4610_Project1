#ifndef PROJ1_H
#define PROJ1_H
#include <stdio.h>

typedef struct {
  int size;
  char **items;
} tokenlist;

char *get_input(void);
tokenlist *get_tokens(char *input);

tokenlist *new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);
char *expand_argument(char *token);


typedef struct _execution_list {
  char** command_and_args;
  char* filename;
  FILE* input, *output;
  enum {
    EXEC_LIST_FILE,
    EXEC_LIST_PROCESS
  } type;
  struct _execution_list* next;
} execution_list;

execution_list* build_execution_list(char** expanded_tokens, int size);

#endif
