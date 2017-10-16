//// working with switch statements
//   by using operators (example, 3 + 4) etc
//
#include <stdio.h>

int main(){

	
	float val1, val2;
	char opp;
	puts("Please enter your expression: \n");
	scanf("%f %c %f", &val1, &opp, &val2);

	switch(opp){
	
		case '+':
			printf("%.2f\n", val1 + val2);
			break;
		case '-':
			printf("%.2f\n", val1 - val2);
			break;
		case '/':
			if (val1==0 || val2==0){
				printf("You can't divide by zero, yo!\n");
				printf("What're ya tryin' ta do, break the universe!?\nSheeesh....\n\n\n");
			} else {
				printf("%.2f\n", val1 / val2);
			}
			break;
		case '*':
			printf("%.2f\n", val1 * val2);
			break;
		default:
			printf("Not sure what ya mean there, pardner....\n\n\n");
	
	} //end switch


	return 0;

}
