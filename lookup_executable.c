#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "proj1.h"

char *lookup_executable(char *command) {
  char *path = getenv("PATH");
  if (!path)
    return command;
	printf("Looup command %s\n", command);

  // Split the PATH by :
  char *directory_path = strtok(path, ":");

  while (directory_path) {
    // List the directory to try to find a file.
    DIR *dir;
    dir = opendir(directory_path);

    if (!dir)
      continue; // Dir does not exist
		printf("In dir %s\n", directory_path);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      // Read file info, make sure it is a file, and then return
      // the absolute path.
			// printf("Entry: %s\n", entry->d_name);

      if (!strcmp(entry->d_name, command)) {
        if (entry->d_type == DT_REG) {
          // It's a file, return it.
          printf("Found command %s as file %s in dir %s\n", command,
                 entry->d_name, directory_path);
          return command;
        }
      }
    }
		printf("Done a\n");

    directory_path = strtok(path, ":");
  }

  // If we didn't find it, just return NULL.
}
