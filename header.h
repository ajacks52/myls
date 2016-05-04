/*
*  CS 481 Lab 4 "My ls"
*  Adam Mitchell
*
*/

#ifndef	_DATATYPES_H_
#define	_DATATYPES_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <math.h>
#include <getopt.h>
#include "dllist.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))

typedef struct {
  int times;
  char *file_name;
} file_name_t;

typedef struct {
    int c;
    int d;
    int l;
    int f;
    int h;
    int r;
    int d_arg;
    Dllist file_names_list;
} flags_t;

int parse_input_opt (int argc, char **argv);
void print_usage();
void print_ls(const char *name);
void print_name_with_classification (struct stat *file_stat, char *old_name);
void print_formatted (struct stat *file_stat);
char * bytesToReadable (long bytes);
char * get_real_name_if_necessary (struct stat *file_stat, char *name);

#endif
