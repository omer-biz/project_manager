#include <ncurses.h>
#include <locale.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "nxjson/nxjson.h"

#include "config.h"
#include "project.h"

int main(int argc, char **argv) {

  // TODO:
  // check for categories
  //  prompt for defaults
  //  if there is no let the user create

  // check for projects
  struct stat st;
  struct Project *prj;
  char *buffer;
  const nx_json *json;
  int fd;

  if (stat(PROJECTS_FILEPATH, &st) != -1) {
    if ((fd = open(PROJECTS_FILEPATH, O_RDONLY)) != -1) {
      buffer = malloc(st.st_size + 1);

      if (read(fd, buffer, st.st_size) != st.st_size) {
        exit(-1); // can't read file
      }
      buffer[st.st_size] = '\0';
      close(fd);

      json = nx_json_parse(buffer, 0);
      if (json == 0) {
        // error
      }

      const nx_json *ptr;
      for (ptr = json->children.first; ptr != NULL; ptr = ptr->next) {
        printf("Name: %s\n", nx_json_get(ptr, "name")->text_value);
        printf("directory: %s\n", nx_json_get(ptr, "directory")->text_value);
        printf("score: %f\n", nx_json_get(ptr, "score")->num.dbl_value);
        printf("last_update: %lu\n", nx_json_get(ptr, "last_update")->num.u_value);
        printf("category_index: %lu\n", nx_json_get(ptr, "category_index")->num.u_value);
      }

      nx_json_free(json);

    } else {
      // can't open file
    }
  } else {
    // can't find file
  }
  //  TODO:
  //  if there is no let the user create


  setlocale(LC_ALL, "");
  initscr();

  endwin();
}
