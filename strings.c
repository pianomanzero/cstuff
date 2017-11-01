// Working with char strings in C
//
//
//

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){

// it's important to remember that in C, strings are merely arrays of chars
	char greeting[] = { 'H','e','l','l','o','\0' };
	// in order to manipulate it like a string we would need to give it a null terminator '\0'
	// otherwise it'll just concatinate with the rest of the 'actual' strings...
	//
	// the above declaration would be the same as
	char greet2[] = "Hello again";

	// note that since we're treating it like a string to begin will, C naturally assigns it a null term...
	// we still need to designate it as a char array, however the functionality in stdio.h handles the rest
	// to add further string capabilities we add the string.h header file
	printf("Greeting: %s\n", greeting);
	printf("Greeting2: %s\n", greet2);

	// you can get a string from stdin with scanf() as well
	// caveat: an input string must be unbroken when assigning this way
	// a space will terminate the input. For example if we entered "some input"
	// the only thing scanf would read would be "some"
	char inputStr[100];
	printf("Input some text: ");
	scanf("%s", inputStr);
	printf("You entered: %s\n", inputStr); 

	// to get around the above limitation in scanf you would need multiple char arrays of appropriate length
	// and capture like this, assuming we would want to capture three words
	// scanf("%s%s%s", s1, s2, s3);
	//

	// the function getchar() can be used to read in a single char from the terminal. when the end of the line
	// is reached the function returns \n which can also be used to get around the above scanf limitation
	// a better way to do so, however, is with the gets() function, which is to read in a single line of text,
	// is not as cumbersome as using getchar().  It takes a single arg, a char array in which the line of text 
	// is to be stored.  it reads up until it encounters a \n char and stores these chars (minus \n) in the array.
	// gets is dangerous and shouldnt be used since it can be used to cause major buffer overruns, so fgets is
	// what we will use instead. care should be exercized as fgets doesn't automatically insert \0 as gets would.
	char newInput[50];
	printf("Enter some more text with spaces: ");
	fgets(newInput, sizeof(newInput), stdin);
	printf("You entered: %s\n", newInput);

	// getline() is also another safe alternative to gets()
	// it also reads from a stream (FILE, stdin, etc)
	// it allocates space for the line dynamically so you end up needing to free it.
	// it does remove the limitation on line length however.
	// getline() also allows you to choos your own single-char line delimiter with detdelim
	char *line = NULL;
	size_t len = 0;
	printf("enter some more text: ");
	getline(&line, &len, stdin);
	printf("You entered: %s\n", line);

	// strings can also be passed in from the command line.
	// argv is a char array and argc is an integer containing the 
	// number of args passed in argv. You can access each like you would 
	// an array.
	if (argc > 1){
	
		// we're going to start at 1 because argv[0] is the name of the program by default
		// in most versions of C
		printf("argc is %i\n", argc);
		for (int c=1; c<argc; ++c){
			printf("'%s' was passed in from the cli using argv[%i]\n", argv[c], c);
	
		}
	}




	return 0;
}
