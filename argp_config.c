#include "argp_config.h"

const char *argp_program_version = "project_manager 0.0.1";
const char *argp_program_bug_address = "omerabdi@protonmail.com";

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct argp_opts *op = state->input;
  switch (key) {
    case 'l': // list all the projects
      op->list_projects = 1;
      break;
    case 'n': // new project
      op->new_project = 1;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}
