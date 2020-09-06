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

