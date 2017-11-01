/// working with structures
//
// 
// structures (structs) are another tool for grouping stuff together, similar to arrays.
// structs are a way of grouping sets of logically related variables together

#include <stdio.h>



int main(int argc, char *argv[]){

	int nextMonth;
	// lets define a struct called date to hold the month, day, and year
	struct date {

		int month;
		int day;
		int year;
	
	};  
	
	// note: a semicolon is required at the end of struct definitions

	struct time {
	
		int hour;
		int minutes;
		int seconds;
	};

	//  the parts of the struct are called 'members'.
	//  in a sense defining a struct defines a new 'type' whithin C
	//  in that variables can subsequently be delcare do be of type <struct_name>
	struct date today, tomorrow;
	// note that we can declare structs in the same line like other vars of the same
	// data type.
	// Unlike other datatypes and arrays, struct members are accessed with special syntax
	today.day = 25;
	today.year = 2017;

	// structs can also be initialized like arrays, as long as one knows the order of the members
	struct time this_time = {3, 30, 55 }; 
	// this can also follow the following format
	struct time that_time = { .hour = 4, .minutes = 15, .seconds = 33 };

	if (today.month == 12){
	
		nextMonth = 1;
	
	}

	if (today.month == 1 && today.day == 1){
	
		printf("HAPPY NEW YEAR!!!\n");
	
	}


	// using structs in expressions
	int century = today.year / 100 + 1;

	tomorrow.month = today.month;
	tomorrow.day = today.day +1;
	tomorrow.year = today.year;

	// a powerful use of structs is combining them with arrays
	struct date birthdays[15];
	// we've just defined the birthdays array to contain 15 elements of 
	// type struct date.  Referencing a struct element in the array is simple
	// and follows both the natural array notation and struct notation
	birthdays[1].month = 8;
	birthdays[1].day = 8;
	birthdays[1].year = 1979;

	// to pass the entire time structure contained in birthdays[1] to a function 
	// the array element is specified (commented since we haven't written this function)
	// checkDate(birthdays[1]);
	

	// initialization of arrays containing structs is similar to multidimensional arrays
	struct time runtTime[5] = { {12,0,0}, {12,10,1}, {13, 15, 79} };
	// the inner braces are optional, but makes reading easier.
	// single elements can be initialized on their own as well with 
	// similar syntax
	// note also that using similar notation with [n].member in the above context
	// you can initialize specific members of the struct at array element n
	
	// structures are also flexible enough to be able to contain  other structures.
	// You can define a struct that itself contains other structs as one or more of its members, 
	// or you can define structures that contain entire arrays (this is specifically useful when
	// working with structs which contain members of char type).
	





	

	return 0;
}

