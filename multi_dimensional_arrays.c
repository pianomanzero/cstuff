/// multidimensional array tutorial
//
//
//

#include <stdio.h>


int main(int argc, char *argv[]){

	/// declare two dimensional array with 4 rows, 5 cols
	int matrix[4][5];

	/// 2d arrays can be initialized as follows...
	//  note that commas are required after each set of inner braces
	//  inner braces are optional but make reading easier
	//  like all arrays, all elements need not be initialized all at once
	
	int M[4][5] = {
			{10, 5, 3, 17, 82 },
			{ 9, 0, 0, 8, 7 },
			{ 10, 33, 2, 1, 1 }, 
			{ 100, 2, 22, 13, 5 }
	};  /// dont forget your ; here....


	/// print values...
	//
	printf("M[2][2] = %i\n", M[2][2]);



	return 0;
}
