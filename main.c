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

#include "nxjson/nxjson.h"

#include "config.h"
#include "project.h"

int load_projects_from_file(const char *filepath, struct project **prj);
int save_to_file(struct project *first);

int main(int argc, char **argv) {

  // TODO
  // check for categories
  //  prompt for defaults
  //  if there is no let the user create

  // check for projects
  struct project *first_project = NULL;
  load_projects_from_file(PROJECTS_FILEPATH, &first_project);
  save_to_file(first_project);

  setlocale(LC_ALL, "");
  initscr();

  endwin();
}

int save_to_file(struct project *first) {
  struct project *ptr;
  char *buffer;
  FILE *conf_file;
  const char *fmt = "  {\n"
    "    \"name\": \"%s\",\n"
    "    \"directory\": \"%s\",\n"
    "    \"score\": %.2f,\n"
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
    snprintf(buffer, CHUNK_SIZE - 3, fmt,
        ptr->name,
        ptr->directory,
        ptr->score,
        ptr->last_update,
        ptr->category_index,
        ptr->time_spent,
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

int load_projects_from_file(const char *filepath, struct project **prj) {
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

      CREATE_AND_LINK_PROJECT(json->children.first, NULL, *prj);
      const nx_json *ptr;
      struct project *prnt, *chld;
      for (ptr = json->children.first->next, prnt = *prj;
          ptr != NULL; ptr = ptr->next, prnt = chld) {

        CREATE_AND_LINK_PROJECT(ptr, prnt, chld);
      }

      nx_json_free(json);
      free(buffer);

    } else {
      fprintf(stderr, "can't parse file");
      exit(-3);
    }
  } else {
    fprintf(stderr, "can't find file");
    exit(-1);
  }

  return 0;
}
