#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj1.h"

char *expand_argument(char *token) {
  if (token[0] == '$') {
    // Check if the variable exists. If not, return an empty string.
    char *value = getenv(&token[1]);
    if (!value) {
      char* data = malloc(1);
      data[0] = '\0';
      return data;
    } else {
      return strdup(value);
    }
  } else if (token[0] == '~') {
    int length = strlen(getenv("HOME")) + strlen(token) + 1;
    char *fullPath = (char *)malloc(length);
    strcpy(fullPath, getenv("HOME"));
    strcat(fullPath, "/");
    if (strlen(token) > 1)
      strcat(fullPath, &token[1]);
    fullPath[length - 1] = 0;
    return fullPath;
  }

  else {
    char delim[] = ":";
    char *pathString =
        (char *)malloc(strlen(getenv("PATH")) + strlen(token) + 1);
    strcpy(pathString, getenv("PATH"));
    char *ptr = strtok(pathString, delim);

    while (ptr != NULL) {
      ptr = strtok(NULL, delim);
    }

    return token;
  }
}
