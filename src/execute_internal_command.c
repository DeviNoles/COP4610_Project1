#include "proj1.h"
#include <string.h>

void execute_internal_command(const char *command, execution_list *node) {
  if (!strcmp(command, "cd")) {
    internal_cd(node);
  } else if (!strcmp(command, "echo")) {
    internal_echo(node);
  } else if (!strcmp(command, "jobs")) {
    internal_jobs(node);
  } else if (!strcmp(command, "exit")) {
    internal_exit(node);
  }
}
