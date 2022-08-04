#include <stdlib.h>
#include <string.h>

#include "project.h"
#include "config.h"

struct project *create_project(const char *name, const char *directory, 
    double score, time_t start_date, unsigned int category_index, time_t time_spent,
    time_t last_update, unsigned int progress, struct project *parent) {

  struct project *ptr;
  ptr = malloc(sizeof(struct project));
  /* memset(ptr, 0, sizeof(struct project)); */

  strncpy(ptr->name, name, PROJECT_NAME_LENGTH);
  strncpy(ptr->directory, directory, MAX_FILENAME);
  ptr->score = score;
  ptr->start_date = start_date;
  ptr->category_index = category_index;
  ptr->time_spent = time_spent;
  ptr->last_update = last_update;
  ptr->progress = progress;

  if (parent != NULL) 
      parent->next = ptr;
  
  ptr->next = NULL;

  return ptr;
}
