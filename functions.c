/// working with functions tutorial
//

#include <stdio.h>


// here's a function
//
void calculateTriangularNumber (int n){

	int i, triangularNumber = 0;

	for ( i = 1; i <= n; ++i){
		triangularNumber += i;
	}

	printf ("triangular number %i is %i\n", n, triangularNumber);

} //end calculateTriangularNumber


// here's another function, a revision of our Greatest Common Divisor program
//
void gcd (  int u, int v ){

	int temp;

	printf("The gcd of %i and %i is ", u, v);

	while (v != 0){
		temp = u %v;
		u = v;
		v = temp;		
	}

	printf("%i\n", u); 

} //end gcd

// here's another function which demonstrates passing/returning a float
float absVal(float x){

	if ( x < 0){
		x = -x;
	}

	return x;

} // end absVal


// we'll use the following function to show how functions can call other functions...
//

float squareRoot(float x){

	const float epsilon = .00001;
	float guess = 1.0;
	//    calling absVal from further up
	while (absVal(guess*guess - x) >= epsilon){
		guess = ( x / guess + guess )/2.0;
	}

	return guess;

} // end squareRoot


// you can pass entire arrays to a function too, not just single elements of an array as we do in main()
// passing entire arrays require some extra handling
// to pass an array, all that is needed is to pass the array name
// The following declaration for minimum() defnies the function as returning
// a value of type int and taking as an arg an array with the number of elements 
// being determined by the next arg, nelms
int minimum(int values[], int nelms){
	
	int minValue, i;
	minValue = values[0];
	for (i = 1; i<nelms; ++i){
		if (values[i] < minValue ){
			minValue = values[i];
		}

	}

	return minValue;

} //end minimum


void asort(int a[], int n){

	int i, j, temp;
	for (i = 0; i<n-1; ++i){
		for(j = i+1; j < n; ++j){
			if ( a[i] > a[j] ){
				temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}


} // end asort



int main(){

	calculateTriangularNumber(10);
	calculateTriangularNumber(20);
	calculateTriangularNumber(50);

	gcd(150, 35);
	gcd(126, 405);
	gcd(83, 240);


	float f1 = -15.5, f2 = 20.0, f3 = -5.0;
	int i1 = -716;
	float result;

	result = absVal(f1);
	printf("result = %.2f\n", result);
	printf("f1 = %.2f\n", f1);
	
	result = absVal(f2);
	printf("result = %.2f\n", result);
	printf("f2 = %.2f\n", f2);

	// typecast an int to a float and pass in
	result = absVal( (float) i1 );
	printf("result = %.2f\n", result);
	printf("i1 = %i\n", i1);

	printf("\nNow we'll pass -6.0/4 to absVal inside the printf call itself\n");
	printf("%.2f\n", absVal(-6.0)/4);
	
	printf("squareRoot(2.0) = %f\n", squareRoot(2.0));
	printf("squareRoot(144.0) = %f\n", squareRoot(144.0));
	printf("squareRoot(17.5) = %f\n", squareRoot(17.5));


	printf("\nNow we'll test our minimum() function which takes the entire array as an argument\n");
	int scores[10], i, minScore;
	int numScores;
	// this bit is like a prototype (see function prototypes in C++ for more information)
	int minimum(int values[], int nelms);

	printf("How many scores would you like to enter? ");
	scanf("%i", &numScores);

	printf("Enter %i scores: \n", numScores);
	for (i = 0; i<numScores; ++i){
	
		scanf("%i", &scores[i]);
		
	} // endfor

	minScore = minimum(scores, numScores);
	printf("\nMinimum score is %i\n", minScore);

	printf("Now lets sort the scores[] array in ascending order...\n\n");
	asort(scores,numScores);

	for (i = 0; i<numScores; ++i){
	
		printf("scores[%i] = %i\n", i, scores[i]);
	
	}

	return 0;



}
