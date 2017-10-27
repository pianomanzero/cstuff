//// tutorial for demonstrating doing stuff with a file name provided in argv[1]
//	Some demos may only be visible with a separate diagnostic tool output 
//	such as pcap or an strace, etc
//
//	items demonstrated:
//	- file locking
//	- file reading/writing
//	- reading output of a command (technically a file stream pipe with popen)
//	- reading file attributes (read/write access, etc)
//	- access call against file
//
//
//
//
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
#include <utime.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <sys/file.h>


int main (int argc, char **argv){
	
	/// for locking we need to set up our fcntl flock struct
	//	we'll need this further down when we demonstrate 
	//	file locking for read/write, etc.
	//	see man fcntl for more
	//
	struct flock fl;
	fl.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	fl.l_start  = 0;        /* Offset from l_whence         */
	fl.l_len    = 0;        /* length, 0 = to EOF           */
	fl.l_pid    = getpid(); /* our PID                      */

	// here's our file descriptor, we need this for referring to the file....
	int result, fd, fd2;
	// set up timestamp stuff so we can write to the file
	time_t rawtime;
	time_t mtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo= localtime ( &rawtime );
	// set up constant for holding filename of file we passed in from the command line
	const char *filename = argv[1];
	// set up const for new filename newfile.txt
	const char *filename2 = "newfile.txt";
	// establish buffer for holding stats
	struct stat statbuf;
	// establish struct for utimebuf buffer
	struct utimbuf new_times;
	// establish file pointers to read from, write to, etc
	// in the case of user we need to establish a file stream or "pipe" so that 
	// we can read from the stream that would normally return to stdio
	// when we run "whoami"
	FILE *user;
	FILE *writefile;
	// establish another file pointer for our new file we'll create within the program
	FILE *writefile2;
	// we'll be using the "whoami" command to read the identity of the user running the program
	char *command = "whoami";

	struct dirent  *dp;
	struct passwd  *pwd;
	struct group   *grp;
	struct tm      *tm;
	//char array for date stuff
	char            datestring[256];
	// assing the output of popen ("pipe open") to user, the "r" is opening the stream for reading...
	user = popen(command, "r");
	// we need a char array to hold the results of the above, and to trim off the \n that comes back
	char uname[10];
	
	// open file descriptor
	fd = open(filename, O_RDWR), 0;

	// get username of user running queries using fgets, reading user as if it were a file stream
	// we need the "sizeof - 2" instead of the normal "sizeof - 1" normally used with an array to 
	// determine size because the last element of this particular array of chars is the \n char
	// that messes up our outputs later on 
	fgets(uname, sizeof(uname) -2, user); 
	
	// stat file to gather attrs and load into statbuf struct
	result = stat(filename, &statbuf);

	/// access call on file for user running program, can be seein in pcaps if using remote filesystem
	//  remember, however, when using NFS for example, that attributes can be cached so an NFS3_ACCESS call
	//  may not be seen after one is initially made, would go straight to GETATTR call, etc
	result=access(filename, W_OK);
	if(result == 0 ){
		/// if our current user has write access to our file...
		printf("%s has write access to %s\n", uname, filename);
	}else{
		// should be triggered if user has anything other than write access to our file...
		printf("%s does not have write access to %s\n", uname, filename);
	
	}
	result=access(filename, R_OK);
	if(result == 0 ){
		// if our user has read access to our file
		printf("%s has read access to %s\n", uname, filename);
	}else{
		// should be triggered if user has anything other than read access to our file...
		printf("%s does not have read access to %s\n", uname, filename);
	
	}

	// in reference to above checks, user can have write but not read access, vice versa, etc


	/// demonstration of using fcntl for file locking. 
	//	flock will lock locally but will not function as expected over NFS, etc, 
	//	thus fcntl is used to illustrate non exclusive read locks and 
	//	exclusive write locks
	//	see above for usage and reference to our flock struct "fl"
	//
	//set type as read lock (not exclusive) and request lock
	//	setting the fl.l_type member of our struct to "F_RDLCK"
	fl.l_type = F_RDLCK;
	// fcntl is then used to actually request the lock
	// 	remember, fd is our int file descriptor we created earlier
	// 	F_SETLKW is the action performed, fl references the lock type in our struct
	// 	F_SETLKW is used instead of F_SETLK because F_SETLKW will req
	// 	lock and allow for NLM_BLOCKED, will wait for lock instead of
	// 	failing
	fcntl(fd, F_SETLKW, &fl);
	/// request unlock of file
	fl.l_type = F_UNLCK;
	/// since we're just requesting an unlock of the file we were using
	//  we can use F_SETLK instead of F_SETLOCKW
	fcntl(fd, F_SETLK, &fl);
	/// set lock type to write lock (exclusive lock) and request lock
	fl.l_type = F_WRLCK;
	// fcntl is then used to actually request the lock
	// 	remember, fd is our int file descriptor we created earlier
	// 	F_SETLKW is the action performed, fl references the lock type in our struct
	// 	F_SETLKW is used instead of F_SETLK because F_SETLKW will req
	// 	lock and allow for NLM_BLOCKED, will wait for lock instead of
	// 	failing
	fcntl(fd, F_SETLKW, &fl);
	/// open file for appending using the "writefile" file pointer established earlier
	writefile = fopen(argv[1], "a+");
	/// use fprintf to write new timestamp to file, 
	//  writefile is our file pointer, timeinfo is our struct holding our time data
	fprintf(writefile, "%s %s %s %s %s", "Write time: ", asctime(timeinfo), "written by ", uname, "\n");
	/// close file...
	fclose(writefile);
	/// request unlock of file...
	fl.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &fl);

	/// open writefile2 for writing, 
	//	if file named newfile.txt already exists, will overwrite
	//	if it does not exist, will create
	writefile2 = fopen(filename2, "w");
	fprintf(writefile2, "%s %s %s %s %s", "Write time: ", asctime(timeinfo), "written by ", uname, "\n");
	
	// close writefile after writing timestamp
	fclose(writefile2);


	// illustrate setting attributes such as mtime..
	//
	// stat filename and load into statbuf buffer struct
	result = stat(filename, &statbuf);

	// set mtime as seconds since epoch
	mtime = statbuf.st_mtime;

	// keep atime unchanged
	new_times.actime = statbuf.st_atime;

	// set mtime to current time
	new_times.modtime = time(NULL);

	//use utime to update times on filename2
	utime(filename, &new_times);

	
	



return 0;
}
