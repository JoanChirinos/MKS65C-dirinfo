#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

const char FILE_D_TYPE = 8;
const char DIRECTORY_D_TYPE = 4;
extern int errno;

void printHelper(int fileMode, int b, int c, char p) {
    if ((fileMode & b) && (fileMode & c))
    printf("%c",p);
    else
    printf("-");
}

void printPermissions(int fileMode) {
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

void giveMeSize(long int temp) {
    const long int tera = 1099511627776;
    const long int giga = 1073741824;
    const long int mega = 1048576;
    const long int kilo = 1024;

    if (temp >= giga) {
        printf("%4ld G", temp / giga);
    } else if (temp >= mega) {
        printf("%4ld M", temp / mega);
    } else if (temp >= kilo) {
        printf("%4ld K", temp / kilo);
    } else {
        printf("%4ld B", temp);
    }
}

void giveMeUser(struct stat * temp) {
    // number of links!
    printf("%3lu ", temp -> st_nlink);

    // user
    // printf("%3u ", file_stats->st_uid);
    struct passwd* user_info = getpwuid(temp -> st_uid);
    printf("%s ", user_info -> pw_name);

    // group
    //printf("%3u ", file_stats->st_gid);
    struct group* group_info = getgrgid(temp -> st_gid);
    printf("%s", group_info -> gr_name);

}

void giveMeTime(struct stat * temp) {
    char arr[100];
    strftime(arr, 100, "%b %d %R", localtime(&(temp -> st_atime)));
    printf(" %s ", arr);
}

int giveMeDirSize(char * path) {
    DIR * mydir = opendir(path);
    struct dirent * file;
    struct stat * stats = calloc(sizeof(struct stat), 1);
    long int size = 0;
    while((file = readdir(mydir)) != NULL) {
        stat(file -> d_name, stats);
        if(file -> d_type == DIRECTORY_D_TYPE && strcmp(file -> d_name, ".") != 0  && strcmp(file -> d_name, "..") != 0 && strcmp(file -> d_name, ".git") != 0) {
            size += giveMeDirSize(file -> d_name);
        } else if (file -> d_type == FILE_D_TYPE)  {
            size += stats -> st_size;
        }
    }
    return size;
}

// driver function
void printAll(char * path) {
    DIR *mydir = opendir(path);
    struct dirent * file;
    struct stat * stats = calloc(sizeof(struct stat), 1);
    long int total = 0;
    printf("\ntotal size of cwd:");
    giveMeSize(giveMeDirSize(path));
    printf("\n");

    // print out current directory first
    while((file = readdir(mydir)) != NULL) {
        stat(file -> d_name, stats);
        if (file -> d_type == DIRECTORY_D_TYPE && strcmp(file -> d_name, ".") == 0) {
            printf("d");
            printPermissions(stats -> st_mode);
            giveMeUser(stats);
            total += giveMeDirSize(path);
            printf("   4 K");
            giveMeTime(stats);
            printf(" %s\n", file -> d_name);
        }
    }

    mydir = opendir(path);

    // print out parent directory info
    while((file = readdir(mydir)) != NULL) {
        if(file -> d_type == DIRECTORY_D_TYPE && strcmp(file -> d_name, "..") == 0) {
            printf("d");
            printPermissions(stats -> st_mode);
            giveMeUser(stats);
            total += 4096;
            printf("   4 K");
            giveMeTime(stats);
            printf(" %s\n", file -> d_name);
        }
    }

    mydir = opendir(path);

    // print out file info
    while((file = readdir(mydir)) != NULL) {
        stat(file -> d_name, stats);
        if ((file -> d_type == DIRECTORY_D_TYPE) && (strcmp(file -> d_name, ".") != 0) && (strcmp(file -> d_name, "..") != 0)) {
            printf("d");
            printPermissions(stats -> st_mode);
            giveMeUser(stats);
            total += stats -> st_size;
            giveMeSize(stats -> st_size);
            giveMeTime(stats);

            printf(" %s\n", file -> d_name);
        }
        if (file -> d_type == FILE_D_TYPE) {
            printf("-");
            printPermissions(stats -> st_mode);
            giveMeUser(stats);
            giveMeSize(stats -> st_size);
            giveMeTime(stats);

            printf(" %s\n", file -> d_name);
        }
    }
    printf("\ntotal size(including subdirectories):");
    giveMeSize(total);
    printf("\n");
    closedir(mydir);
}

int main(int argc, char* argv[]) {
  FILE * straw;
  int errnum;
  char path[256];
  if (argc == 1) {
    printf("Path to directory: ");
    scanf("%s", path);
  }
  else if (argc == 2) {
    strcpy(path, argv[1]);
  }
  straw = fopen(path, "rb");
  if (straw == NULL) {
    errnum = errno;
    printf("Error opening file: %s\n", strerror(errnum));
  }
  else {
    printAll(path);
    return 0;
  }
}
