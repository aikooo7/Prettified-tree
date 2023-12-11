#include <linux/limits.h>
#include <sys/stat.h>

void read_cwd_contents(char cwd[PATH_MAX], int spaces, int *files,
                       int *directories);

void handler(int sig);

int dir_not_empty(const char *path);
