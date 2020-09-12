#include "proj1.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void internal_cd(execution_list *node) {
    if (node->command_and_args->next == NULL) {
        chdir(getenv("HOME"));
        return;
    }
    else if (strcmp(node->command_and_args->next->value,"~") == 0) {
        chdir(getenv("HOME"));
        return;
    }
    char* temp = node->command_and_args->next->value;
    printf("CHANGING DIRECTORIES: %s\n",temp);
    if (chdir(temp) == -1) {
       printf("%s is not a directory.\n",temp);
    }
}

void internal_echo(execution_list *node) {
  printf("CALLING INTERNAL EHCO\n");
  int firstNum = 0;
  while(node->command_and_args->next != NULL){
    if(firstNum == 1){
      printf("%s", node->command_and_args);
    }
    firstNum=1;
    node->command_and_args = node->command_and_args->next;
  }
}
 
void internal_exit(execution_list *node) {}
