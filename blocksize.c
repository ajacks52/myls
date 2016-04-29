#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>


int main()
{
  struct stat fi;
  stat("a.out", &fi);
  printf("%d %lld \n", fi.st_blksize, fi.st_size);
  return 0;
}
