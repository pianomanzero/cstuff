//// access call tests on a file name provided in argv[1]
//
//
//
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char **argv){


	int result;
	const char *filename = argv[1];
	result=access(filename, W_OK);

	printf("result is %i\n\n", result);

return 0;
}
