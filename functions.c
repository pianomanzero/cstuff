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


int main(){

	calculateTriangularNumber(10);
	calculateTriangularNumber(20);
	calculateTriangularNumber(50);

	gcd(150, 35);
	gcd(126, 405);
	gcd(83, 240);



	return 0;



}
