#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <nxjson.h>
#include <termbox.h>

#include "project.h"
#include "tui.h"
#include "config.h"

// public varaibles

size_t project_length = 0;

// private varaibles

// indexable_projects projects capacity
size_t idx_prj_capacity = 50; 

// number of times we increased the capacity
size_t capacity_increase_count = 0;
struct project **indexable_projects; // deleteing a project is going to be a pain

// private functions

void init_indexable_projects() {
  indexable_projects = malloc(idx_prj_capacity * sizeof(struct project *));
}

void update_idx_prj_capacity() {
  idx_prj_capacity = idx_prj_capacity * (++capacity_increase_count + .125);
  indexable_projects = realloc(indexable_projects, idx_prj_capacity);
}

// public functions

void setup_projectfile_path() {
  FILE *fd;
  const char empty_json[] = "[\n\n]";
  fprintf(stderr, "path: %s\n", PROJECTS_FILEPATH_PATH);

  if (access(PROJECTS_FILEPATH_PATH, F_OK) != 0) {
    if (mkdir(PROJECTS_FILEPATH_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0) {
      fd = fopen(PROJECTS_FILEPATH, "w+");
      fwrite(empty_json, strlen(empty_json), 0, fd);
      fclose(fd);
    } else {
      fprintf(stderr, "Can't create %s error: %s\n", PROJECTS_FILEPATH_PATH, strerror(errno));
      exit(-1);
    }
  }
}

struct project *create_project(const char *name, const char *directory, 
    double score, time_t start_date, unsigned int category_index, time_t time_spent,
    time_t last_update, unsigned int progress, struct project *parent) {

  struct project *ptr;
  ptr = malloc(sizeof(struct project));

  strncpy(ptr->name, name, PROJECT_NAME_LENGTH);
  strncpy(ptr->directory, directory, MAX_FILENAME);
  ptr->score = score;
  ptr->start_date = start_date;
  ptr->category_index = category_index;
  ptr->time_spent = time_spent;
  ptr->last_update = last_update;
  ptr->progress = progress;

  if (parent != NULL) 
      parent->next = ptr;
  
  ptr->next = NULL;

  // first time running
  if (project_length == 0)
    init_indexable_projects();

  if (idx_prj_capacity < project_length)
    update_idx_prj_capacity();

  indexable_projects[project_length++] = ptr;

  return ptr;
}

struct project *search_prj_by_name(char *project_name, struct project *prj) {
  for (; prj != NULL; prj = prj->next) {
    if (strcmp(prj->name, project_name) == 0) {
      return prj;
    }
  }
  return NULL;
}

void activate_tui() {
  struct tb_event ev;
  int y = 0;
  int ret;

  ret = tb_init();
  if (ret) {
    fprintf(stderr, "tb_init() failed with error code %d\n", ret);
    exit(ret);
  }

  tb_hide_cursor();
  draw_screen();
  tb_present();
  tui_state = 1;
  while (tui_state) {
    tb_poll_event(&ev) ;
    switch(ev.type) {
      case TB_EVENT_RESIZE:
        draw_screen();
        break;
      case TB_EVENT_KEY:
        handle_key(ev);
        draw_screen();
        break;
    }
  }
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
      fprintf(stderr, "f:%s\n", strerror(errno));
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
  if ((fd = open(PROJECT_DIR_ID, O_CREAT | O_RDWR)) != -1) {
    write(fd, prj->name, strlen(prj->name));
  } else {
    fprintf(stderr, "Can't create project identifier in the currentdir\n%s\n",
        strerror(errno));
  }

  return 0;
}
