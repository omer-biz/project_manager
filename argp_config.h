#ifndef __ARGP_CONFIG__
#define __ARGP_CONFIG__

#include <argp.h>

static char doc[] = "A simple program to track of my personal projects";
static const struct argp_option options[] = {
  { "list", 'l', 0, OPTION_ARG_OPTIONAL, "List all the available projects" }, 
  { "new", 'n', 0, OPTION_ARG_OPTIONAL, "Create a new project in the current directory" }, 
  { 0 }
};

error_t parse_opt(int key, char *arg, struct argp_state *state);
static const char args_doc[] = "[FILENAME]";
static struct argp argp = { options, parse_opt, args_doc, doc };

struct argp_opts {
  int list_projects;
  int new_project;
};

#endif
