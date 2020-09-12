#include "proj1.h"

void internal_jobs(execution_list *node) {
  node = background_jobs;
while (node) {
  printf("[%d] %d ", node->job_id, (long)node->pid);
  print_string_list(node->command_and_args->next);
  node = node->next;
}
}
