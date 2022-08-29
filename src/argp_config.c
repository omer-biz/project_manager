#include "argp_config.h"

const char *argp_program_version = "project_manager aplpha-0.0.1";
const char *argp_program_bug_address = "omerabdi@protonmail.com";

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct argp_opts *op = state->input;
  switch (key) {
    case 'l': // list all the projects
      op->list_projects = 1;
      break;
    case 'n': // new project
      op->new_project = 1;
      if (arg != NULL) {
        strncpy(op->new_project_name, arg, PROJECT_NAME_LENGTH);
      }
      break;
    case 'a': // activate project
      op->activate_prj = 1;
      if ( arg != NULL )
        strncpy(op->active_project_name, arg, PROJECT_NAME_LENGTH);
      break;
    case 'x':
      op->create_and_activate = 1;
      if (arg != NULL)
        strncpy(op->new_project_name, arg, PROJECT_NAME_LENGTH);
      break;
    case 't':
      op->activate_tui = 1;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}
