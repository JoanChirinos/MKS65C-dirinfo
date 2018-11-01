#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void print_contents(char* dir) {
  DIR* d;
  d = opendir(dir);
  struct dirent* entry = readdir(d);
  while (entry) {
    int file_name = open(entry->d_name);
    char* type = (entry->d_type == 8) ? "File: " : "Directory: ";
    printf("%s |", type);
    if (!strcmp(file_name, "File: ")) {
      struct stat file_stats = calloc(sizeof(struct stat), 1);
      stat(file_name, file_stats);
      printf("%d\n", file_stats->st_size);
    }
    printf("%s\n", file_name);
    entry = readdir(d);
  }
  closedir(d);
}

int main() {

  print_contents(".");

}
