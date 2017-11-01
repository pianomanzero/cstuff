// tutorial on executing system commands and reading their output
// primarily useful on *NIX machines, may behave differently on other OSes


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// my own little custom jobby to make running system commands easier
		// (note, args like cat <file> or anything taking params etc 
		// must be enclosed like so: "cat <file>"
		// this enables them to be treated as a single string arg
void runcmd(char *cmd, char rtrn[]){

	// filestream to hold what we read from command execution
	FILE *cmdStream;
	// local char array to hold stuff we read from filestream
	char oput[255];
	// open pipe to command stream
	cmdStream = popen(cmd, "r");
	// use fgets to get contents of filestream and assign to oput
	fgets(oput, sizeof(oput), cmdStream);
	// use strcpy to copy oput to array passed in
	strcpy(rtrn, oput);

	

}


int main(int argc, char *argv[]){
	
	// where our command will eventually live
	char *command = NULL;
	// char to hold output of command for later use
	char cmdOutput[255];
	// make sure we've passed in args
	if (argc > 1){
	
		// set command to CLI arg 
		// (note, args like cat <file> or anything taking params etc 
		// must be enclosed like so: "cat <file>"
		// this enables them to be treated as a single string arg
		command = argv[1];
		// run runcmd function
		runcmd(command, cmdOutput);

		printf("The output of %s is: %s\n", command, cmdOutput);
		
	
	}else{


		// if no command provided, use whoami
		command = "whoami";
		runcmd(command, cmdOutput);	
	
		printf("The output of whoami is: %s\n", cmdOutput);
	}



	return 0;
}


