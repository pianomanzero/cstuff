/// working with if statements
//
//
#include <stdio.h>

int main (){

	int guess;
	int secret = 50;

	printf("Lets play a game, I'll think of a number and you'll guess the number\nI'll tell you if it's higher or lower\n\n");
       	printf("Please enter an integer:\n");
	scanf("%i",&guess);
	
	if (guess < secret){
	
		printf("%i is less than %i\n\n", guess, secret);
	
	} //end if

	if (guess > secret){
	
		printf("%i is greater than %i\n\n", guess, secret);
	} // end if

	if (guess == secret){
	
		printf("Congratulations!!! %i was the secret number!!! \n\n", secret);
	} // end if


}
