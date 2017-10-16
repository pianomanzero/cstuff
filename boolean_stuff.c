/// working with boolean variables
//	generate a table of prime numbers
//
#include <stdio.h>

int main(){


	int p, d;
	_Bool isPrime;

	for (p = 2; p < 50; ++p){
		isPrime=1;
		for(d = 2; d < p; ++d){
			if( p % d == 0 )
				isPrime = 0;
		} // end inner for

		if ( isPrime ){
			printf("%i ", p);
		} /// end inner if
	
	
	
	} ///end for

	printf("\n");
	return 0;



}
