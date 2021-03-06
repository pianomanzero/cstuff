// Generate fibonacci numbers using variable length arrays
//

#include <stdio.h>

int main(){

	int i, numFibs;

	printf("How many Fibonacci numbers do you want (1 - 75)? ");
	scanf("%i", &numFibs);

	if(numFibs <1 || numFibs > 75){
		printf("Bad number, out of range!\n\n");
		return 1;
	}

	unsigned long long int Fibonacci[numFibs];
	Fibonacci[0] = 0;   /// by definition
	Fibonacci[1] = 1;   /// ditto

	for ( i = 2; i < numFibs; ++i ){
		Fibonacci[i] = Fibonacci[i-2] + Fibonacci[i-1];
	}

	for ( i = 0; i < numFibs; ++i){
		printf("%llu ", Fibonacci[i]);
	}

	printf("\n");
	



	return 0;
}
