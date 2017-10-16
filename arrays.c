/// working with arrays
//
//
#include <stdio.h>


int main (int argc, char **argv){

	/// various examples of  initializing arrays
	//  followed by an actual program using arrays
	//
	//initialize a five element int array with values
	int counters[5] = { 0, 1, 2, 3, 4 };

	/// initialize a five element char array with values 
	//	- note the single quotes around chars....
	char letters[5] = { 'a', 'b', 'c', 'd', 'e' };

	/// initialize a float array with some values but not all
	//	- note that we can assign elements in any order we like
	float sample_data[500] = { [0] = 100.0, [3] = 300.5, [1] = 500.5, [2] = 678.45 };

	/// we can initialize arrays in this fashion as well
	//	- note that the largest index number specified
	//	sets the size of the array
	//
	float more_data[] = { [0] = 1.0, [49] = 100.20, [99] = 456.34 };


	/// example of programatically assigning values
	//	assign first 5 vals
	int array_values[10] = { 0, 1, 4, 9, 16 };
	int i;
	for ( i = 5; i < 10; ++i){
		array_values[i] = i * i;
	} // end for

	for ( i = 0; i < 10; ++i){
		printf("array_values[%i] = %i\n", i, array_values[i]);
	} // end for


	/// array can be declared and assigned values on the fly....
	//
	char word[] = { 'H', 'e', 'l', 'l', 'o', '!' };

	printf("word[] contains: ");
	/// reuse int i from earlier, since it gets reset to 0....
	//
	for ( i = 0; i < 6; ++i){
		printf("%c", word[i]);	
	} // end for

	printf("\n");

	for (i = 0; i<6; ++i){
		printf("word[%i] = %c\n", i, word[i]);
	} //end for





	return 0;
	

}
