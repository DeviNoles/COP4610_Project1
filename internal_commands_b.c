#include "proj1.h"

void internal_jobs(execution_list *node) {
  execution_list *current = background_jobs;
  int fd = node->stdout_pipe[1];
  while (current) {
    if (!current->has_completed) {
      dprintf(fd, "[%d] [%ld] ", current->job_id, (long)current->pid);
      dprint_full_command(fd, current);
      dprintf(fd, "\n");
    }
    current = current->next;
  }
}
