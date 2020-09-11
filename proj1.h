#ifndef PROJ1_H
#define PROJ1_H
#include <stdio.h>
#include <unistd.h>

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

typedef struct _string_list {
  char *value;
  struct _string_list *next;
} string_list;

string_list *create_string_list();
void string_list_add(string_list *list, char *str);
void print_string_list(string_list *list);

typedef struct _execution_list {
  string_list *command_and_args;
  char *filename;
  int fds[2];
  enum { EXEC_LIST_FILE, EXEC_LIST_PROCESS } type;
  pid_t pid;
  struct _execution_list *next;
} execution_list;

execution_list *build_execution_list(char **expanded_tokens, int size);
void print_execution_list(execution_list *exec_list);
void free_execution_list(execution_list *exec_list);

void execute_list_node(execution_list *current_node, execution_list *last_node, char* PATH);

char *lookup_executable(char *command, char* PATH);

void pipe_stdout_into_stdin(pid_t from, pid_t to);

void pipe_file_into_stdin(const char* filename, pid_t pid);

#endif
