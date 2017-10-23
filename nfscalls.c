//// simulating NFS calls on a file name provided in argv[1]
//
//
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <sys/file.h>


int main (int argc, char **argv){
	
	struct flock fl;
	fl.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	fl.l_start  = 0;        /* Offset from l_whence         */
	fl.l_len    = 0;        /* length, 0 = to EOF           */
	fl.l_pid    = getpid(); /* our PID                      */
	int result, fd;
	// set up timestamp stuff
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo= localtime ( &rawtime );
	const char *filename = argv[1];
	struct stat statbuf;
	FILE *user;
	FILE *writefile;
	char *command = "whoami";
	struct dirent  *dp;
	struct passwd  *pwd;
	struct group   *grp;
	struct tm      *tm;
	char            datestring[256];
	user = popen(command, "r");
	char uname[10];
	
	// open file descriptor
	fd = open(filename, O_RDWR), 0;

	// get username of user running queries...
	fgets(uname, sizeof(uname) -2, user); 
	
	// stat file
	result = stat(filename, &statbuf);

	/// access call
	result=access(filename, W_OK);
	if(result == 0 ){
		printf("%s has write access to %s\n", uname, filename);
	}else{
		printf("%s does not have write access to %s\n", uname, filename);
	
	}
	result=access(filename, R_OK);
	if(result == 0 ){
		printf("%s has read access to %s\n", uname, filename);
	}else{
		printf("%s does not have read access to %s\n", uname, filename);
	
	}



	/// file locking 
	//
	//set type as read lock (not exclusive) and request lock
	fl.l_type = F_RDLCK;
	fcntl(fd, F_SETLKW, &fl);
	/// request unlock of file
	fl.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &fl);
	/// set lock type to write lock (exclusive lock) and request lock
	fl.l_type = F_WRLCK;
	fcntl(fd, F_SETLKW, &fl);
	/// open file for appending.....
	writefile = fopen(argv[1], "a+");
	/// write new timestamp to file... 
	fprintf(writefile, "%s %s", "Write time: ", asctime(timeinfo) );
	/// close file...
	fclose(writefile);
	/// request unlock of file...
	fl.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &fl);





return 0;
}
