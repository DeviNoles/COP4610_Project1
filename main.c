#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj1.h"

int main() {
  while (1) {
    printf("%s@%s: %s > ", getenv("USER"), getenv("MACHINE"), getenv("PWD"));

    /* input contains the whole command
     * tokens contains substrings from input split by spaces
     */

    char *input = get_input();
    //	printf("whole input: %s\n", input);

    tokenlist *tokens = get_tokens(input);
    char *fullCommand = "";
    for (int i = 0; i < tokens->size; i++) {
      printf("token %d: (%s)\n", i, tokens->items[i]);
      expand_argument(tokens->items[i]);
    }

    free(input);
    free_tokens(tokens);
  }

  return 0;
}

char *expand_argument(char *token) {
  printf("CALLING EXPAND ARGUMENT");
  if (token[0] == '$') {
    token++;
    char *pathString =
        (char *)malloc(strlen(getenv(token)) + strlen(token) + 1);
    strcpy(pathString, getenv(token));
    printf("%s\n", pathString);
    printf("COMMAND : %s\n", getenv(token));
  } else if (token[0] == '~') {
    char *fullPath = (char *)malloc(strlen(getenv("HOME")) + strlen(token) + 1);
    strcpy(fullPath, getenv("HOME"));
    strcat(fullPath, "/");
    strcat(fullPath, token);
    printf("%s\n", fullPath);
    return fullPath;
    free(fullPath);
  }
  /* -arguments
          else if(token[0]=='-'){
                  char *tempHolder = (char *) malloc(1 + strlen(fullCommand) +
     strlen(token) ); char *holder = (char *) malloc(1 + strlen(fullCommand));
                  strcpy(holder, fullCommand);
                  strcpy(tempHolder, holder);

                  strcat(tempHolder, token);
                  fullCommand = tempHolder;
                  printf("FULL COMMAND");
          }*/

  else {
    char delim[] = ":";
    char *pathString =
        (char *)malloc(strlen(getenv("PATH")) + strlen(token) + 1);
    strcpy(pathString, getenv("PATH"));
    printf("%s\n", pathString);
    char *ptr = strtok(pathString, delim);

    while (ptr != NULL) {
      printf("'%s'\n", ptr);
      ptr = strtok(NULL, delim);
    }

    // free(tempHolder);
    return token;
  }
}
