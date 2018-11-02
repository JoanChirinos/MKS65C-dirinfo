#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//
// void print_contents(char* dir) {
//   DIR* d;
//   d = opendir(dir);
//   struct dirent* entry = readdir(d);
//   while (entry) {
//     int file_name = open(entry->d_name);
//     char* type = (entry -> d_type == 8) ? "File: " : "Directory: ";
//     printf("%s |", type);
//     if (!strcmp(file_name, "File: ")) {
//       struct stat * file_stats = calloc(sizeof(struct stat), 1);
//       stat(file_name, file_stats);
//       printf("%ld\n", file_stats->st_size);
//     }
//     printf("%d\n", file_name);
//     entry = readdir(d);
//   }
//   closedir(d);
// }

void printHelper(int fileMode, int b, int c, char p) {
  if ((fileMode & b) && (fileMode & c))
  printf("%c",p);
  else
  printf("-");
}

void printPermissions(int fileMode) {
  printf("-");
  /* owner permissions */
  printHelper(fileMode, S_IRUSR, S_IREAD, 'r');
  printHelper(fileMode, S_IWUSR, S_IWRITE, 'w');
  printHelper(fileMode, S_IXUSR, S_IEXEC, 'x');
  /* group permissions */
  printHelper(fileMode, S_IRGRP, S_IREAD, 'r');
  printHelper(fileMode, S_IWGRP, S_IWRITE, 'w');
  printHelper(fileMode, S_IXGRP, S_IEXEC, 'x');
  /* other user permissions */
  printHelper(fileMode, S_IROTH, S_IREAD, 'r');
  printHelper(fileMode, S_IWOTH, S_IWRITE, 'w');
  printHelper(fileMode, S_IXOTH, S_IEXEC, 'x');
}

void statP(char * str) {
  struct stat * pter = malloc(sizeof(struct stat)); // allocate some memory
  stat(str, pter); // record metadata into pter
  // printing file info:
  long int size = pter -> st_size;
  printPermissions(pter -> st_mode);
  // printf("mode(permissions): (%3o) ", pter -> st_mode&0777);
  if(size < 1000)
  printf(" %ldB ", size);
  else if (size < 1000000)
  printf(" %ld.%ldKB ", size/1000, size%1000);
  else if (size < 1000000000)
  printf(" %ld.%ldMB ", size/1000000, size%1000000);
  else
  printf(" %ld.%ldGB ", size/1000000000, size%1000000000);
  char temp[100];
  strftime(temp, 100, "%b %d %R", localtime(&(pter -> st_atime)));
  printf("%s ", temp);
  printf("%s\n", str);
  free(pter);
}

int main() {
  char desired_dir[100] = "temp_dir";
  DIR *mydir = opendir(desired_dir);;
  struct dirent *myfile;
  struct stat *mystat = calloc(sizeof(struct stat), 1);
  long int total;
  char buf[512];

  printf("\nStatistics for %s: \n", desired_dir);
  while((myfile = readdir(mydir)) != NULL ) {
    stat(myfile ->d_name, mystat);
    total += mystat -> st_size;
  }

  // formatting for bytes
  if(total < 1000)
  printf("Total: %ldB\n", total);
  else if (total < 1000000)
  printf("Totall: %ld.%ldKB\n", total/1000, total%1000);
  else if (total < 1000000000)
  printf("Total: %ld.%ldMB\n", total/1000000, total%1000000);
  else
  printf("Total: %ld.%ldGB\n", total/1000000000, total%1000000000);

  mydir = opendir(desired_dir);
  myfile = readdir(mydir);
  // printing dirs
  printf("\ndirectories: \n");
  while((myfile = readdir(mydir)) != NULL)
  {
    if(myfile -> d_type == 4) {
      printf("  %s\n", myfile -> d_name);
    }
  }
  // printing files
  printf("\nfiles: \n");
  mydir = opendir("./");
  while((myfile = readdir(mydir)) != NULL) {
    if(myfile -> d_type == 8) {
      char filename[50] = "./";
      strcat(filename, myfile -> d_name);
      // printf("current name: %s\n", filename);
      statP(filename);
      // printf("  %s\n", myfile -> d_name);
    }
  }

  closedir(mydir);
}
