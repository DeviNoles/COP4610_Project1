#include <stdlib.h>
#include <string.h>

#include "proj1.h"

string_list *create_string_list() {
  string_list *list = (string_list *)malloc(sizeof(string_list));
  list->value = NULL;
  list->next = NULL;
  return list;
}

void string_list_add(string_list *list, char *str) {
  if (!list)
    return;
  if (!list->value) {
    list->value = strdup(str);
  } else {
    if (!list->next) {
      list->next = create_string_list();
    }
    string_list_add(list->next, str);
  }
}

void print_string_list(string_list *list) {
  if (!list || !list->value)
    return;
  printf("%s", list->value);
  if (list->next) {
    printf(" ");
    print_string_list(list->next);
  }
}
