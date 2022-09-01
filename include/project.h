#ifndef PROJECT_H
#define PROJECT_H

#include <time.h>

#include "config.h"

#ifndef CREATE_AND_LINK_PROJECT
#define CREATE_AND_LINK_PROJECT(json, prnt, chld)      \
  chld = create_project(                               \
    nx_json_get(json, "name")->text_value,             \
    nx_json_get(json, "directory")->text_value,        \
    nx_json_get(json, "score")->num.dbl_value,         \
    nx_json_get(json, "start_date")->num.u_value,      \
    nx_json_get(json, "category_index")->num.u_value,  \
    nx_json_get(json, "time_spent")->num.u_value,      \
    nx_json_get(json, "last_update")->num.u_value,     \
    nx_json_get(json, "progress")->num.u_value,        \
    prnt                                               \
  )
#endif

struct project {
  char name[PROJECT_NAME_LENGTH];
  char directory[MAX_FILENAME];
  double score;
  time_t start_date;
  unsigned int category_index;
  time_t time_spent;
  time_t last_update;
  unsigned int progress;
  struct project *next;
};

// list of pointers pointing to the individual projects
extern struct project **indexable_projects;

// number of all the projects on the heap
extern size_t project_length;

struct project *create_project(const char *name, const char *directory, double score,
    time_t start_date, unsigned int category_index, time_t time_spent, time_t last_update,
    unsigned int progress, struct project *parent);

struct project *search_prj_by_name(char *project_name, struct project *first);

#endif // PROJECT_H
