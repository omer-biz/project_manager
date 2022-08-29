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
#include <sys/wait.h>

#include "nxjson/nxjson.h"

#include "config.h"
#include "project.h"
#include "argp_config.h"


int load_projects_from_file(const char *filepath, struct project **prj, struct project **lprj);
int save_to_file(struct project *first);
int new_project_in_pwd(struct project **last, struct project **first, char *project_name);
void list_all_projects(struct project *first);
void activate_project(struct project *prj);
void activate_tui(struct project *prj);

int main(int argc, char **argv) {

  struct argp_opts opts = { 0 };
  argp_parse(&argp, argc, argv, 0, 0, &opts);

  struct project *first_project = NULL;
  struct project *last_project = NULL;
  int ret;
  ret = load_projects_from_file(PROJECTS_FILEPATH, &first_project, &last_project);

  if (opts.activate_tui == 1) {
    activate_tui(first_project);
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

  save_to_file(first_project);
}

void activate_tui(struct project *first) {
  WINDOW *w;
  int i, ch, padding = 10;
  char *project_names[project_length];
  char item[PROJECT_NAME_LENGTH + padding];

  /* printf("project_length: %lu\n", project_length); */
  for (i = 0; i < project_length; ++i) {
    printf("pr: %p, name: %s\n", indexable_projects[i], indexable_projects[i]->name);
  }
  return;
  initscr();
  w = newwin(10, 10, 2, 2);
  box(w, 0, 0);
  for (i = 0; first != NULL; first = first->next, ++i) {
    if (i == 0) wattron(w, A_STANDOUT);
    else wattroff(w, A_STANDOUT);
    sprintf(item, "%-10s", first->name);
    mvwprintw(w, i + 1, 2, "%s", first->name);

    project_names[i] = (char *)malloc(PROJECT_NAME_LENGTH);
    strncpy(project_names[i], first->name, PROJECT_NAME_LENGTH);
  }
  wrefresh(w);
  getch();
  i = 0;
  noecho();
  keypad(w, TRUE);
  curs_set(0);

  /* while((ch = wgetch(w)) != 'q') { */
  /*   mvwprintw(w, i+1, 2, "%s", ) */
  /* } */

  getch();
  delwin(w);
  endwin();
}

void activate_project(struct project *prj) {
  time_t start, end;
  pid_t pid;

  start = time(NULL);
  pid = fork();
  if (pid == 0) {
    chdir(prj->directory);
    execl(getenv("SHELL"), basename(getenv("SHELL")), NULL);
  } else if (pid > 0) {
    wait(0);
    end = time(NULL);
    prj->time_spent += (end - start);
    prj->last_update = time(NULL);
    printf("time spent: %lus\n", prj->time_spent);
  } else {
    fprintf(stderr, "fork failed\n");
    exit(-1);
  }
}

void list_all_projects(struct project *first) {
  for (; first != NULL; first = first->next) {
    printf(
      "name: \"%s\"\n"
      "directory: \"%s\"\n"
      "last_update: %s",
      first->name,
      first->directory,
      ctime(&(first->last_update))
    );

    if (first->next != NULL)
      puts("");
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
      *lprj = *prj;
      const nx_json *ptr = 0;
      struct project *prnt = 0, *chld = 0;
      for (ptr = json->children.first->next, prnt = *prj;
          ptr != NULL; ptr = ptr->next, prnt = chld) {

        CREATE_AND_LINK_PROJECT(ptr, prnt, chld);
      }

      if (chld != NULL)
        *lprj = chld;

      nx_json_free(json);
      free(buffer);

    } else {
      fprintf(stderr, "%s\n", strerror(errno));
      exit(-3);
    }
  } else {
    if ((fd = open(filepath, O_CREAT | O_WRONLY)) != -1) {
      char *empty_json = "[\n\n]";
      write(fd, empty_json, strlen(empty_json));
      return -1;

    } else {
      fprintf(stderr, "%s\n", strerror(errno));
      exit(-1);
    }
  }

  return 0;
}

int new_project_in_pwd(struct project **lprj, struct project **fprj, char *project_name) {
  struct project *prj;
  char *buffer;
  char *dir_name;

  if (access(PROJECT_DIR_ID, F_OK) == 0) {
    fprintf(stderr, "Project exists\n");
    exit(-1);
  }

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
    fprintf(stderr, "Can't set the directory");
    exit(-1);
  }

  if (project_name != NULL && strlen(project_name) != 0) {
    strncpy(prj->name, project_name, PROJECT_NAME_LENGTH);
  } else {
    // the directory name is the project name, if no name is given
    dir_name = basename(prj->directory);
    strncpy(prj->name, dir_name, PROJECT_NAME_LENGTH);
  }

  prj->last_update = prj->start_date = time(NULL);

  // make lprj(last pointer) point to the newly created project
  if (*lprj != NULL) {
    (*lprj)->next = prj;
    *lprj = prj;
  } else { // if the json file is empty
    *fprj = *lprj = prj;
  }

  int fd;
  if ((fd = open(PROJECT_DIR_ID, O_CREAT | O_WRONLY)) != -1) {
    write(fd, prj->name, strlen(prj->name));
  } else {
    fprintf(stderr, "Can't create project identifier in the currentdir\n%s\n",
        strerror(errno));
  }

  return 0;
}
