/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Directory/file support header
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/


#ifndef DIR_H
#define DIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "const.h"
#include "string.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char name[STRLEN];    // directory name
  struct dirent **LIST; // files in directory
  int N;                // number of files in directory
  char **files;         // filtered files
  char **paths;         // filtered paths
  int n;                // number of filtered files
} dir_t;

bool fileexist(char *fname);
int findfile(char *dir_path, char *pattern, char *filter, char fname[], int size);
int countfile(char *dir_path, char *pattern);
int createdir(char *dir_path);
void extension(char* path, char extension[], int size);
void extension2(char* path, char extension[], int size);
void basename_without_ext(char* path, char basename[], int size);
void basename_with_ext(char* path, char basename[], int size);
void directoryname(char* path, char dirname[], int size);

#ifdef __cplusplus
}
#endif

#endif

