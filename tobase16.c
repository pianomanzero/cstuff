//// convert a positive int to base16
//	- another example of using arrays...
//	- example of converting input array in argv to long int
//

#include <stdio.h>
#include <stdlib.h>   /// required for atoi() and atof()

int main (int argc, char *argv[]){

	 ///check argc to make sure args passed
	if ( argc >1 ){
	
	


	char * ptr;  /// strtol() requires ptr strtl(char, ptr, base)
	long ret;
	ret = strtol(argv[1], &ptr, 10);	
	const char baseDigits[16] = {'0', '1', '2','3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	int convertedNumber[64];
	long int numberToConvert = ret;



	int nextDigit, base = 16, index = 0;


	
	/// convert to the indicated base
	//
	do {
		convertedNumber[index] = numberToConvert % base;
		++index;
		numberToConvert = numberToConvert / base; 
	} while ( numberToConvert != 0 );

	printf("Converted number = ");

	for (--index; index >=0; --index) {
		nextDigit = convertedNumber[index];
		printf("%c", baseDigits[nextDigit]);
	} //end for

	printf("\n");
	return 0;

	} else {
		printf("\nIllegal usage, please supply an integer\n\n");
		printf("usage: tobase16 <integer>\n\n");
	return 0;
	
	}


} 
