#include "../libs/tree.h"
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char filename[PATH_MAX];
  int files = 0;
  int directories = 0;

  signal(SIGINT, handler);

  if (getcwd(filename, sizeof(filename)) == NULL) {
    perror("Getting current working directory");
  }

  if (argc > 1) {
    printf("%s\n", argv[1]);

    if (dir_not_empty(argv[1]) == 0) {
      printf("|\n");
    }

    read_cwd_contents(argv[1], 3, &files, &directories);
  } else {
    printf(".\n");

    if (dir_not_empty(filename) == 0) {
      printf("|\n");
    }

    read_cwd_contents(filename, 3, &files, &directories);
  }

  printf("\nDirectories: %d\n", directories);
  printf("Files: %d\n", directories);

  return EXIT_SUCCESS;
}

void read_cwd_contents(char cwd[PATH_MAX], int spaces, int *files,
                       int *directories) {
  struct stat fi;
  DIR *dp = opendir(cwd);

  if (dp == NULL) {
    fprintf(stderr, "Error opening cwd %s\nWith error: %s", cwd,
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct dirent *de;
  char path[PATH_MAX];

  while ((de = readdir(dp)) != NULL) {
    if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 &&
        de->d_name[0] != '.') {
      snprintf(path, sizeof(path), "%s/%s", cwd, de->d_name);
      stat(path, &fi);

      printf("|");
      for (int i = 0; i < spaces; i++) {
        printf("-");
      }

      if (S_ISREG(fi.st_mode)) {
        printf(" 📄 %s\n", de->d_name);
        (*files)++;
      } else if (S_ISDIR(fi.st_mode)) {
        printf(" 📁 %s\n", de->d_name);
        read_cwd_contents(path, spaces + 3, files, directories);
        (*directories)++;
      }
    }
  }

  closedir(dp);
}

void handler(int sig) {
  char c;
  signal(sig, SIG_IGN);
  fflush(stdin);

  printf("\nDamn, that hurts,\n"
         "Are you sure you want to leave the program [y/n] ");

  while (c != 'y' && c != 'Y' && c != 'n' && c != 'N') {
    c = getchar();
    if (c == 'y' || c == 'Y') {
      exit(0);
    } else if (c == 'n' || c == 'N') {
      signal(SIGINT, handler);
      getchar();
    } else {
      printf("\nHey buddy you can only choose yes or no, try again ");

      /* Outerwise the lines if formatted as:
      // while ((c = getchar()) != '\n' && c != EOF)
           ;
      */
      // clang-format off

      int ch;
      while ((ch = getchar()) != '\n' && ch != EOF);
      // clang-format on
      signal(SIGINT, handler);
    }
  }
}

int dir_not_empty(const char *path) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    return -1;
  }

  struct dirent *entry;
  int is_empty = 0;

  entry = readdir(dir);

  if (entry != NULL) {
    is_empty = 1;
  }

  closedir(dir);
  return is_empty;
}
