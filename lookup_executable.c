#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "proj1.h"

// If there is a tilde in a path, you must expand that first
// Maybe give a warning if the program is not a file (i.e. it's a
// directory)
// Should we resolve symlinks?
char *lookup_executable(char *command, char *PATH) {
  char *path = strdup(PATH);
  if (!path) {
    return command;
  }
  // Split the PATH by :
  char *saveptr = path;
  char *directory_path;

  // while (directory_path) {
  for (char *directory_path = strtok_r(path, ":", &saveptr);
       directory_path != NULL; directory_path = strtok_r(NULL, ":", &saveptr)) {
    // List the directory to try to find a file.
    DIR *dir;
    dir = opendir(directory_path);

    if (!dir) {
      // directory_path = strtok_r(path, ":", &saveptr);
      continue; // Dir does not exist
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      // Read file info, make sure it is a file, and then return
      // the absolute path.
      // printf("Entry: %s\n", entry->d_name);

      if (!strcmp(entry->d_name, command)) {
        // Execute files or symlinks
        if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
          // It's a file, return it.
          // Use realpath to find the absolute path of the directory.
          // Then, concat it with the command to get the absolute file path.
          char realpath_buf[PATH_MAX];
          char *abs = realpath(directory_path, realpath_buf);
          if (abs) {
            // Concat
            int length_of_abs = strlen(abs);
            int length = length_of_abs + 1 + strlen(entry->d_name);
            char *abs_exec_path = (char *)malloc(length);
            strcpy(abs_exec_path, abs);
            strcpy(&abs_exec_path[length_of_abs + 1], entry->d_name);
            abs_exec_path[length_of_abs] = '/';
            abs_exec_path[length] = '\0';
            // printf("FINAL: %s\n", abs_exec_path);
            return abs_exec_path;
          } else {
            return NULL;
          }
        } else {
          fprintf(stderr, "%s: not a file\n", command);
          return NULL;
        }
      }
    }
  }

  // If we didn't find it, just return NULL.
  return NULL;
}
