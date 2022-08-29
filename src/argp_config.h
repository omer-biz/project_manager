#ifndef __ARGP_CONFIG__
#define __ARGP_CONFIG__

#include <argp.h>
#include <string.h>

#include "config.h"

static char doc[] = "A simple program to track of my personal projects";
static const struct argp_option options[] = {
  { "list", 'l', 0, OPTION_ARG_OPTIONAL, "List all the available projects" }, 
  { "new", 'n', "project_name", OPTION_ARG_OPTIONAL, "Create a new project in the current directory,"
                                        " if no name is provided the current directory's name will be used" }, 
  { "activate", 'a', "project_name", 0, "Activate a project i.e. cd into the directory and start a shell" },
  { "create_and_activate", 'x', "project_name", OPTION_ARG_OPTIONAL, "Creates and activates a project in the current directory" },
  { "tui", 't', 0, OPTION_ARG_OPTIONAL, "Activate the tui interface." },
  { 0 }
};

error_t parse_opt(int key, char *arg, struct argp_state *state);
static const char args_doc[] = "";
static struct argp argp = { options, parse_opt, args_doc, doc };

struct argp_opts {
  int list_projects;
  int new_project;
  int activate_prj;
  int create_and_activate;
  int activate_tui;
  char active_project_name[PROJECT_NAME_LENGTH];
  char new_project_name[PROJECT_NAME_LENGTH];
};

#endif
