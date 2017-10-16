#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <fcntl.h>

int main (int argc, char **argv){

	//check argv for filename arg and operation arg
	if(argc > 2){
		FILE *fp;
		int fd = 0;	
		struct stat st;
		fp=fopen(argv[1], argv[2]);
		fd=fileno(fp);
		fstat(fd, &st);

	
	} else {
		
		printf("Please specify a file and operation\n\n");
		printf("Usage:\n");
		printf("fstat_test <filename> <operation>");

	
	}
	


return 0;
}
