/*
*  CS 481 Lab 4 "My ls"
*  Adam Mitchell
*
*/

#include "header.h"

bool DEBUG = true;
flags_t *flags;

int LIST_WAS_FULL_OF_CRAP = 0;

int main(int argc, char **argv) {

  Dllist tmp;
  char *printed_file_name;

  flags = talloc(flags_t, 1);
  /* set the appropriate flags in the flags struct and adds file names to Dllist */
  parse_input_opt(argc, argv);

  if (DEBUG) {
    printf( "{ ");
    dll_traverse(tmp, flags->file_names_list) {
      file_name_t *file_name_struct = (file_name_t *) jval_v(dll_val(tmp));
      char *file_name = strdup(file_name_struct->file_name);
      printf( ANSI_COLOR_RED "%s " ANSI_COLOR_RESET,file_name );
    }
    printf("}\n\n" );
  }


  // scrub files make sure they all exist.
  // dll_traverse(tmp, flags->file_names_list) {
  //   file_name_t *file_name_struct = (file_name_t *) jval_v(dll_val(tmp));
  //   char *file_name = strdup(file_name_struct->file_name);
  //
  //   // delete bad files from list.
  //   if( access( file_name, F_OK ) == -1 ) {
  //     // file doesn't exist
  //     fprintf(stderr, "myls: %s: No such file or directory\n", file_name);
  //     dll_delete_node(tmp);
  //     LIST_WAS_FULL_OF_CRAP = 1;
  //   }
  // }

  // if list is empty and we delete at least one bad file exit the program.
  if (dll_empty(flags->file_names_list) && LIST_WAS_FULL_OF_CRAP) {
    exit(EXIT_FAILURE);
  }

  // traverse and print ls
  traverse ();
  free(flags);
}


void traverse () {

  Dllist tmp;
  struct stat file_stat;

  // list is empty and was not full of crap
  if (dll_empty(flags->file_names_list) && !LIST_WAS_FULL_OF_CRAP) {
    ls_dir(".");
  }

  // loop over all the files in the list
  // int file_index = 0;

  dll_traverse(tmp, flags->file_names_list) {
    file_name_t *file_name_struct = (file_name_t *) jval_v(dll_val(tmp));
    char *file_name = strdup(file_name_struct->file_name);

    /* do an initial stat to get the files info */
    lstat(file_name,&file_stat);


    if (S_ISDIR(file_stat.st_mode)) {
      /* not a dir so it must be a file or symlink */
      printf("\n");
      ls_dir(file_name);
    }
    else if (!S_ISLNK(file_stat.st_mode)) {
      /* not a symlink or a dir be a file */
      /* check if the -l flag is set if so print in formatted form */
      if (flags->l) print_formatted (&file_stat);
      /* if the file name */
      print_name_with_classification(&file_stat, file_name);
    }
    else {
      /* must be a symlink  */

      /* get_real_name_if_necessary returns null unless file is a symlink and -f is on */
      char *temp_name = get_real_name_if_necessary(&file_stat, file_name);
      if (temp_name != NULL) {
        file_name = temp_name;
        /* redo stat because file is symlink */
        lstat(temp_name,&file_stat);
      }

      if (S_ISDIR(file_stat.st_mode)) {
        /* not a dir so it must be a file or symlink */
        printf("\n");
        ls_dir(file_name);
      }
      else {
        /* not a symlink or a dir be a file */
        /* check if the -l flag is set if so print in formatted form */
        if (flags->l) print_formatted (&file_stat);
        /* if the file name */
        print_name_with_classification(&file_stat, file_name);
      }
    }
    // file_index++;
  }
}

/**
* Prints an entire directory
* if the -r flag is on it will recurssively print directies from
* the root of where it was called.
*/
void ls_dir(const char *name) {

  DIR *dir;
  struct dirent *file_dir;
  struct stat file_stat;
  Dllist recursive_file_list = new_dllist();
  Dllist tmp;

  dir = opendir(name);
  if (!(dir))
  return;
  if (!(file_dir = readdir(dir)))
  return;

  if (flags->r && strcmp(name, "..")){
    printf("%s\n", name);
  }

  do {
    char *file_name = file_dir->d_name;

    char *temp_file_name;
    asprintf(&temp_file_name,"%s/%s", name, file_name);

    if(lstat(temp_file_name, &file_stat) < 0) {
      fprintf(stderr, "stat failed\n" );
      return;
    }

    if (flags->r) {
      if (file_dir->d_type == DT_DIR && strcmp(file_name, ".") && strcmp(file_name, "..")) {
        // recursive is turned on add all dirs to Dllist
        dll_append(recursive_file_list, new_jval_v(temp_file_name));
      }
    }

    /* check if the -l flag is set if so print in formatted form */
    if (flags->l) print_formatted (&file_stat);
    /* if the file name */
    print_name_with_classification(&file_stat, file_name);

  } while (file_dir = readdir(dir));

  closedir(dir);

  if (DEBUG && !dll_empty(recursive_file_list)) {
    printf( "{ ");
    dll_traverse(tmp, recursive_file_list)
    {
      char *dir_name = strdup((char *) jval_v(dll_val(tmp)));
      printf( ANSI_COLOR_RED "%s, " ANSI_COLOR_RESET, dir_name);
    }
    printf("}\n\n" );
  }

  if (flags->r && !dll_empty(recursive_file_list)) {
    dll_traverse(tmp, recursive_file_list)
    {
      printf("\n");
      char *dir_name = strdup((char *) jval_v(dll_val(tmp)));
      ls_dir(dir_name);
    }
  }
}

void print_name_with_classification (struct stat *file_stat, char *old_name) {

  char *printed_file_name;
  if (flags->c) {
    printed_file_name = malloc(strlen(old_name)+2);
    strcpy (printed_file_name,old_name);
    if (S_ISDIR(file_stat->st_mode)) {
      strcat(printed_file_name,"/");
    }
    else if (S_ISLNK(file_stat->st_mode)) {
      strcat(printed_file_name,"@");
    }
    else if (file_stat->st_mode & S_IXUSR) {
      strcat(printed_file_name,"*");
    }
  }
  else {
    printed_file_name = malloc(strlen(old_name)+1);
    strcpy (printed_file_name,old_name);
  }
  /* print the file name */
  printf("%s\n", printed_file_name);
}

char * get_real_name_if_necessary (struct stat *file_stat, char *name) {
  if (flags->f && S_ISLNK(file_stat->st_mode)) {

    char buf[1024];
    int len;
    if ((len = readlink(name, buf, sizeof(buf))) != -1)
    buf[len] = '\0';
    name = strdup(buf);

    return name;

  }
  return NULL;
}

void print_formatted (struct stat *file_stat) {

  struct group *grp;
  struct passwd *pwd;

  grp = getgrgid(file_stat->st_gid);
  pwd = getpwuid(file_stat->st_uid);

  char date_buffer[15];
  char cyear[10];
  char fyear[10];

  time_t t = time(NULL);

  strftime(cyear, 10, "%Y", localtime(&t));
  strftime(fyear, 10, "%Y", localtime(&(file_stat->st_mtime)));

  if (!strcmp(cyear, fyear))
  strftime(date_buffer, 20, "%b %d %H:%M", localtime(&(file_stat->st_mtime)));
  else
  strftime(date_buffer, 20, "%b %d %Y", localtime(&(file_stat->st_mtime)));

  printf( "%8d ", file_stat->st_ino);
  printf( (S_ISDIR(file_stat->st_mode)) ? "d" : "-");
  printf( (file_stat->st_mode & S_IRUSR) ? "r" : "-");
  printf( (file_stat->st_mode & S_IWUSR) ? "w" : "-");
  printf( (file_stat->st_mode & S_IXUSR) ? "x" : "-");
  printf( (file_stat->st_mode & S_IRGRP) ? "r" : "-");
  printf( (file_stat->st_mode & S_IWGRP) ? "w" : "-");
  printf( (file_stat->st_mode & S_IXGRP) ? "x" : "-");
  printf( (file_stat->st_mode & S_IROTH) ? "r" : "-");
  printf( (file_stat->st_mode & S_IWOTH) ? "w" : "-");
  printf( (file_stat->st_mode & S_IXOTH) ? "x" : "-");
  printf(" %-8s", pwd->pw_name);
  printf(" %s", grp->gr_name);

  if (flags->d) {
    long num_bits = (file_stat->st_size / flags->d_arg  + ((file_stat->st_size % flags->d_arg) ? 1 : 0)) * flags->d_arg;
    if (flags->h) printf("%7s", bytesToReadable (num_bits));
    else printf("%7d ", num_bits);

  }
  else {
    if (flags->h) printf("%7s", bytesToReadable (file_stat->st_size));
    else printf("%8d", file_stat->st_size);
  }

  printf(" %s ", date_buffer);
}


void print_usage() {
  printf("usage: myls [-cd:lfhr] [file ...] (-d must have additonal arg the block size)\n");
}

int parse_input_opt (int argc, char **argv) {

  flags->c = 0;
  flags->d = 0;
  flags->l = 0;
  flags->f = 0;
  flags->h = 0;
  flags->r = 0;
  flags->file_names_list = new_dllist();

  int c;
  while (1)
  {
    static struct option long_options[] = {
      {"classify",        no_argument,   0,  'c' },
      {"disk-usage",      required_argument,   0,  'd' },
      {"long-listing",    no_argument,   0,  'l' },
      {"follow-symlinks", no_argument,   0,  'f' },
      {"human-readable",  no_argument,   0,  'h' },
      {"recursive",       no_argument,   0,  'r' },
      {0,                 0,             0,  0   }
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "cd:lfhr",
    long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
    break;

    switch (c){
      case 'c' :{
        flags->c = 1;
        break;
      }
      case 'd' :{
        flags->d = 1;
        flags->d_arg = atoi(optarg);
        if (flags->d_arg == 0) {
          print_usage();
          exit(EXIT_FAILURE);
        }
        break;
      }
      case 'l' :{
        flags->l = 1;
        break;
      }
      case 'f' :{
        flags->f = 1;
        break;
      }
      case 'h' :{
        flags->h = 1;
        break;
      }
      case 'r' :{
        flags->r = 1;
        break;
      }
      case '?': {
        print_usage();
        exit(EXIT_FAILURE);
        break;
      }
    }
  }

  /* any remaining command line arguments (not options). */
  if (optind < argc)
  {
    while (optind < argc) {
      file_name_t *file_name_struct = talloc(file_name_t, 1);
      file_name_struct->file_name = strdup(argv[optind]);
      dll_append(flags->file_names_list, new_jval_v(file_name_struct));
      optind++;  // Skip to the next argument
    }
  }
  return 0;
}


char * bytesToReadable (long bytes) {

  char *str = malloc(6*sizeof(char));
  int first = 0;
  char size[3];
  double f = 0;

  if (bytes > 1073741824) {
    first = (bytes / 1073741824.0);
    f = round((bytes / 1073741824.0) * 100.0)/100.0;
    size[0] = 'G';
    size[1] = 'B';
  }
  else if (bytes > 1048576) {
    first = (bytes / 1048576.0);
    f = round((bytes / 1048576.0) * 100.0)/100.0;
    size[0] = 'M';
    size[1] = 'B';
  }
  else if (bytes > 1024) {
    first = (bytes / 1024.0);
    f = round((bytes / 1024.0) * 100.0)/100.0;
    size[0] = 'K';
    size[1] = 'B';
  }
  else {
    size[0] = 'B';
    size[1] = '\0';
    sprintf(str, "%d%s",bytes, size);
    return str;
  }
  size[2] = '\0';

  if (first > 9) sprintf(str, "%d%s",first, size);
  else sprintf(str, "%.1f%s", f, size);

  return str;
}
