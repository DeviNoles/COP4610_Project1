#include "proj1.h"

void internal_jobs(execution_list *node) {
  execution_list *current = background_jobs;
  int fd = node->stdout_pipe[1];
  while (current) {
    if (!current->has_completed) {
      dprintf(fd, "[%d] [%ld] ", current->job_id, (long)current->pid);
      print_string_list(current->command_and_args);
    }
    current = current->next;
  }
  dprintf(fd, "\n");
}
