// creating a c version of poke shell function
//
// 	contains a good example of char array concat 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cpoke(const char addy[]){	

	// these need to be terminated with null char \0.  argv is already term by \0
	char *command = "ping \0";
	// char *addy = a[1];	
	char *options = " -c 1 | grep -E -o '[0-9]+ received' | cut -f1 -d' '\0";
	int nums = sizeof(command) + sizeof(&addy) + sizeof(options);
	char fullcommand[nums];
	FILE *theCmdF;	
	// concat char arrays into fullcommand array 
	// (stack overflow will occur if fullcommand[n] not > total chars)
	strcpy(fullcommand, command);
	strcat(fullcommand, addy);
	strcat(fullcommand, options);
	char result[5];

	// open filestream pipe using file stream pointer and fullcommand char array
	theCmdF = popen(fullcommand, "r");

	fgets(result, sizeof(result) -1, theCmdF);

	if (atoi(result) == 0){
	
		return 1;
	}else if (atoi(result) == 1){
		return 0;
	}


} // end cpoke()

// diagnostic stuff
//	printf("Checking command: %s\n", command);
//	printf("Checking addy: %s\n", addy);
//	printf("Checking options: %s\n", options);
//	printf("\nChecking fullcommand: %s\n\n", fullcommand);
//	printf("\nChecking result: %s\n", result);

int main(int argc, char *argv[]){

	int canReach = cpoke(argv[1]);

	if (canReach == 0){
	
		printf("Looks like we can reach %s\n", argv[1]);

	} else {
	
		printf("something went wrong...\n");
	
	}

	return 0;
}

