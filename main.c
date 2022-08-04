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
#include <libgen.h>

#include "nxjson/nxjson.h"

#include "config.h"
#include "project.h"
#include "argp_config.h"

int load_projects_from_file(const char *filepath, struct project **prj, struct project **lprj);
int save_to_file(struct project *first);
int new_project_in_pwd(struct project **last, char *project_name);
void list_all_projects(struct project *first);

int main(int argc, char **argv) {
  
  struct argp_opts opts = { 0 };
  argp_parse(&argp, argc, argv, 0, 0, &opts);


  // check for projects
  struct project *first_project = NULL;
  struct project *last_project = NULL;
  int ret;
  ret = load_projects_from_file(PROJECTS_FILEPATH, &first_project, &last_project);

  if (opts.list_projects == 1) {
    list_all_projects(first_project);
    exit(0);
  }

  if (opts.new_project == 1) {
    new_project_in_pwd(&last_project, opts.new_project_name);
  }

  save_to_file(first_project);
}

void list_all_projects(struct project *first) {
  for (; first != NULL; first = first->next) {
    printf(
      "name: \"%s\"\n"
      "directory: \"%s\"\n"
      /* "score: %.2f\n" */
      "start_date: %lu\n\n",
      /* "last_update: %lu\n" */
      /* "category_index: %u\n" */
      /* "time_spent: %lu\n" */
      /* "progress: %u\n\n\n", */
      first->name,
      first->directory,
      /* first->score, */
      first->start_date
      /* first->last_update, */
      /* first->category_index, */
      /* first->time_spent, */
      /* first->progress */
    );
  }
}

int save_to_file(struct project *first) {
  struct project *ptr;
  char *buffer;
  FILE *conf_file;
  const char *fmt = "  {\n"
    "    \"name\": \"%s\",\n"
    "    \"directory\": \"%s\",\n"
    "    \"score\": %.2f,\n"
    "    \"start_date\": %llu,\n"
    "    \"last_update\": %llu,\n"
    "    \"category_index\": %u,\n"
    "    \"time_spent\": %llu,\n"
    "    \"progress\": %u\n"
    "  }";

  buffer = malloc(CHUNK_SIZE);
  if (buffer == NULL) {
    fprintf(stderr, "can't allocate memory");
    exit(-5);
  }

  conf_file = fopen(PROJECTS_FILEPATH, "w+");
  if (conf_file == NULL) {
    fprintf(stderr, "can't open file to save projects");
    exit(-6);
  }

  fwrite("[\n", 2, 1, conf_file);
  for (ptr = first; ptr != NULL; ptr = ptr->next) {
    snprintf(buffer, CHUNK_SIZE - 3, fmt, // possible overflow here
        ptr->name,
        ptr->directory,
        ptr->score,
        ptr->start_date,
        ptr->category_index,
        ptr->time_spent,
        ptr->last_update,
        ptr->progress
    );
    if (ptr->next != NULL)
      strcat(buffer, ",\n");
    fwrite(buffer, strlen(buffer), 1, conf_file);
  }
  fwrite("\n]\n", 3, 1, conf_file);
  fclose(conf_file);

  return 0;
}

int load_projects_from_file(const char *filepath, struct project **prj, struct project **lprj) {
  struct stat st;
  char *buffer;
  const nx_json *json;
  int fd;

  if (stat(filepath, &st) != -1) {
    if ((fd = open(filepath, O_RDONLY)) != -1) {
      buffer = malloc(st.st_size + 1);

      if (buffer == NULL) {
        fprintf(stderr, "can't allocate memory");
        exit(-7);
      }

      if (read(fd, buffer, st.st_size) != st.st_size) {
        fprintf(stderr, "can't read file");
        exit(-4);
      }
      buffer[st.st_size] = '\0';
      close(fd);

      json = nx_json_parse(buffer, 0);
      if (json == 0) {
        fprintf(stderr, "can't parse file");
        exit(-2);
      }

      // empty array
      if (json->children.first == NULL) {
        return -7; 
      }

      CREATE_AND_LINK_PROJECT(json->children.first, NULL, *prj);
      const nx_json *ptr;
      struct project *prnt, *chld;
      for (ptr = json->children.first->next, prnt = *prj;
          ptr != NULL; ptr = ptr->next, prnt = chld) {

        CREATE_AND_LINK_PROJECT(ptr, prnt, chld);
      }

      *lprj = chld;

      nx_json_free(json);
      free(buffer);

    } else {
      fprintf(stderr, "can't open file");
      exit(-3);
    }
  } else {
    fprintf(stderr, "can't find file");
    return -1;
  }

  return 0;
}

int new_project_in_pwd(struct project **lprj, char *project_name) {
  struct project *prj;
  char *buffer;
  char *dir_name;

  prj = malloc(sizeof(struct project));
  memset(prj, 0, sizeof(struct project));

  // get the current directory
  if ((buffer = getcwd(NULL, 0)) != NULL) {
    strncpy(prj->directory, buffer, MAX_FILENAME - 1);
    free(buffer);
    buffer = NULL;
  } else if ((buffer = getenv("PWD")) != NULL) {
    strncpy(prj->directory, buffer, MAX_FILENAME - 1);
  } else {
    printf("Can't set the directory");
    exit(-1);
  }

  if (project_name != NULL && strlen(project_name) != 0) {
    strncpy(prj->name, project_name, PROJECT_NAME_LENGTH);
  } else {
    // the directory name is the project name, if no name is given
    dir_name = basename(prj->directory);
    strncpy(prj->name, dir_name, PROJECT_NAME_LENGTH);
  }

  prj->start_date = time(NULL);

  // make lprj(last pointer) point to the newly created project
  (*lprj)->next = prj;
  *lprj = prj;

  return 0;
}
