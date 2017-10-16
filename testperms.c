//// test posix perms prototype

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv){
	
	/// check to see if filename was passed...
	if(argc >1){
		struct stat sb;
		FILE * fp;
		/// check execute bit for user
		printf("%s is %s executable for user.\n", argv[1], stat(argv[1], &sb) == 0 && sb.st_mode & S_IXUSR ? "" : "not");
		/// check execute bit for group
		printf("%s is %s executable for group.\n", argv[1], stat(argv[1], &sb) == 0 && sb.st_mode & S_IXGRP ? "" : "not");

		printf("Triggering fopen...");
		fp = fopen("argv[1]", "rw");
	
	
	} else {
		/// arg wasn't passed...try again...
		printf("Proper argument not supplied, please supply a file and try again\n\n");

	
	}




return 0;

}
