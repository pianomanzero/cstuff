/// working with scanf and inputs
//
//

#include <stdio.h>


int main(){


	int number;
	char text[20];
	printf("We're going to work with inputs.\n\n");
	printf("please enter an integer:\n");
	scanf("%i", &number);
	printf("You entered %i\n\n", number);
	
	printf("Now lets try entering some text:\n");
	scanf("%s", text);
	printf("You entered: %s\n\n\n", text);

	return 0;




}
