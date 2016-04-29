# myls
CS 481 Lab 5

Lists the contents of directories and files

To run type "./myls [-cd:lfhr] [file ...]" (-d must have additonal arg the block size)

## Supported Options

	
1. -c, –classify: append file type indicators: ’/’ for directories; ’@’ for symbolic links; ’*’ for executables.
2. -d, –disk-usage: specify file size on disk, based on its number of allocated blocks. This option takes
the file system block size (in bytes) as a mandatory parameter.
3. -l, –long-listing: use detailed (long) listing format, printing in order:
   * inode number
   * mode (using the same “drwxrwxrwx” format of the standard ls command.
   * owner (you must convert the uid to the proper user name)
   * group (you must convert the gid to the proper group name)
   * file size (in bytes unless otherwise specified)
   * file modification time
    - if file has been modified in current year, use Month Date hh:mm format, e.g. “Apr 17 05:03”
    - otherwise, use Month Date Year format, e.g. “Apr 15 2013”.
   * – file name (not including any leading directories)
3. -f, –follow-symlinks: follow the targets of symbolic links
4. -h, –human-readable: if long listing format is specified, print file sizes in a human readable format using
B for bytes, KB for kilobytes, MB for megabytes and GB for gigabytes.
5. -r, –recursive: list subdirectories recursively using depth-first, pre-order traversal.


