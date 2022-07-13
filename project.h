#ifndef PROJECT_H
#define PROJECT_H

#include <time.h>

#include "config.h"

struct project {
  char name[PROJECT_NAME_LENGTH];
  char directory[MAX_FILENAME];
  double score;
  time_t last_update;
  unsigned int category_index;
  struct project *next;
};


struct category {
  char name[MAX_CATEGORY_NAME];
  struct category *next;
};

#endif // PROJECT_H
