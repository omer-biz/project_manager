#include <ncurses.h>
#include <locale.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "project.h"
#include "argp_config.h"

int main(int argc, char **argv) {

  struct argp_opts opts = { 0 };
  argp_parse(&argp, argc, argv, 0, 0, &opts);

  setup_projectfile_path();

  struct project *first_project = NULL;
  struct project *last_project = NULL;
  int ret;
  ret = load_projects_from_file(PROJECTS_FILEPATH, &first_project, &last_project);

  if (opts.activate_tui == 1) {
    activate_tui();
  }

  if (opts.list_projects == 1) {
    list_all_projects(first_project);
    exit(0);
  }

  if (opts.new_project == 1) {
    new_project_in_pwd(&last_project, &first_project, opts.new_project_name);
  }
  else if (opts.activate_prj == 1) {
    if (strlen(opts.active_project_name) != 0) {
      struct project *s = search_prj_by_name(opts.active_project_name, first_project);
      if (s == NULL) {
        fprintf(stderr, "Can not find: %s\n", opts.active_project_name);
        exit(-1);
      }
      activate_project(s);
    }
  }
  else if (opts.create_and_activate == 1) {
    new_project_in_pwd(&last_project, &first_project, opts.new_project_name);
    activate_project(last_project);
  }

  activate_tui();

  save_to_file(first_project);
  return ret; 
}
