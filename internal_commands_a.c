#include "proj1.h"

void internal_cd(execution_list *node) {}

void internal_echo(execution_list *node) {
  printf("CALLING INTERNAL EHCO\n");
  int firstNum = 0;
  while(node->command_and_args!="null"){
    if(firstNum==1){
      printf("%s", node->command_and_args);
    }
    firstNum=1;
    node->command_and_args = node->command_and_args->next;
  }
}

void internal_exit(execution_list *node) {}
