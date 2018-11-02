#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char FILE_D_TYPE = 8;
const char DIRECTORY_D_TYPE = 4;

long long int size_of_dir(char* path) {
  long long int total_size = 0;

  DIR* current_directory = opendir(path);

  struct dirent* current_file;
  struct stat* current_file_stats = calloc(sizeof(struct stat), 1);
  char whole_path[512];

  while ((current_file = readdir(current_directory)) != NULL) {

    // if current file is a directory
    if (current_file->d_type == DIRECTORY_D_TYPE) {

      // skip self and parent
      if (!strcmp(current_file->d_name,".") || !strcmp(current_file->d_name,"..")) {
        continue;
      }

      // cat dir path to whole_path
      strcpy(whole_path, "");
      strcat(whole_path, path);
      strcat(whole_path, "/");
      strcat(whole_path, current_file->d_name);

      // run this fxn on new directory
      total_size += size_of_dir(whole_path);
    }

    // if current file is a file
    if (current_file->d_type == FILE_D_TYPE) {
      // resets and copies path to whole_path + filename
      strcpy(whole_path, "");
      strcat(whole_path, path);
      strcat(whole_path, "/");
      strcat(whole_path, current_file->d_name);

      // gets stats of current FILE
      stat(whole_path, current_file_stats);

      // adds current file size to total_size
      total_size += current_file_stats->st_size;
    }
  }
  free(current_file_stats);
  return total_size;
}

void print_info(struct dirent* file) {
  // get file stats
  struct stat* file_stats = calloc(sizeof(struct stat), 1);
  stat(file->d_name, file_stats);

  // mode
  char* readable_mode = calloc(sizeof(char), 12);
  // converts octal mode to what we see in ls -l
  strmode(file_stats->st_mode, readable_mode);
  // printing mode
  printf("%s ", readable_mode);
  // freeing so I don't forget later
  free(readable_mode);

  // number of links!
  printf("%3d ", file_stats->st_nlink);

  // user
  printf("%3u ", file_stats->st_uid);

  // group
  printf("%3u ", file_stats->st_gid);

  if (!strcmp(file->d_name, "..")) {
    printf("   ? B\t");
  }
  else {
    long long int size;
    if (file->d_type == DIRECTORY_D_TYPE) {
        size = size_of_dir(file->d_name);
    }
    if (file->d_type == FILE_D_TYPE) {
      size = file_stats->st_size;
    }
    if (size >= 1000000000) {printf("%4lld G\t", size / 1000000000);}
    else if (size >= 1000000) {printf("%4lld M\t", size / 1000000);}
    else if (size >= 1000) {printf("%4lld K\t", size / 1000);}
    else printf("%4lld B\t", size);
  }

  // time
  char temp[100];
  strftime(temp, 100, "%b %d %R", localtime(&(file_stats -> st_atime)));
  printf("%12s ", temp);

  printf("%s\n", file->d_name);
}

int main() {
  // DIRECTORY SIZE IS SIZE OF CONTENTS (in a recursive fashion), NOT THE ACTUAL DIRECTORY FILE
  // long long int dir_size = size_of_dir(".");
  // printf("total size of /. : %lld B\n", dir_size);
  // print_file_info("./mydir.c");

  DIR* dir = opendir(".");
  struct dirent* current_file;
  while ((current_file = readdir(dir)) != NULL) {
    print_info(current_file);
  }

  return 0;
}