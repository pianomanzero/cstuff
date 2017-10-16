/////// fopen, using files as input
//
// this is incomplete, needs finishing

#include <stdio.h>
#include <time.h>
#include <sys/stat>
#include <sys/types>

int main(char argc, char **argv){

	// set up timestamp stuff
	time_t rawtime;
	struct tm * timeinfo;

	time( &rawtime );
	timeinfo= localtime ( &rawtime );


	// create a file pointer for our test_log file
	FILE *logfile;
	
	// open test_log for writing
	logfile=fopen(argv[1], "a+");
	fprintf(logfile, "%s %s", "Write time: ", asctime(timeinfo) );

	//close our test_log file poitner
	fclose(logfile);


	return 0;


}
