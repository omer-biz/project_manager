#include <stdlib.h>
#include <string.h>

#include "project.h"
#include "config.h"

// public varaibles

size_t project_length = 0;

// private varaibles

// indexable_projects projects capacity
size_t idx_prj_capacity = 50; 

// number of times we increased the capacity
size_t capacity_increase_count = 0;
struct project **indexable_projects; // deleteing a project is going to be a pain

// private functions

void init_indexable_projects() {
  indexable_projects = malloc(idx_prj_capacity * sizeof(struct project *));
}

void update_idx_prj_capacity() {
  idx_prj_capacity = idx_prj_capacity * (++capacity_increase_count + .5);
  indexable_projects = realloc(indexable_projects, idx_prj_capacity);
}

// public functions

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

  // first time running
  if (project_length == 0)
    init_indexable_projects();

  if (idx_prj_capacity < project_length)
    update_idx_prj_capacity();

  indexable_projects[project_length++] = ptr;

  return ptr;
}

struct project *search_prj_by_name(char *project_name, struct project *prj) {
  for (; prj != NULL; prj = prj->next) {
    if (strcmp(prj->name, project_name) == 0) {
      return prj;
    }
  }
  return NULL;
}
