// Test program to see if we can read and manipulate the ps page from logfiles
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// building my own readline funcs to simplify matters
// not exactly UNIX philosophy in principle, but simplifying matters is important right now.





int main(int argc, char *argv[]){
		
	// char to hold pwd
	char pwdloc[256];
	char header[256];
	// file stream ptr to hold incoming command output
	// char to hold SRDATA location, passed in through argv
	// char to hold SRDATA output file location
	// char to hold loc of ps file (will need to concat chars together)
	//
	//
	//
	//
	//
	// if argc = 1, no args passed in, assume base dir of logset, 
	// build base dir of logset loc char with pwd
	// check to make sure nodenames has output (nodenames | wc -l > 0)
	// use current bash vers of nodenames to get node names
	// get number of nodes from nodenames | wc -l
	// for num of nodes build char array of paths to each ps file
	// 	cat each on end of output of base dir var
	//
	// for num of elements in ps dir
	// 	prepend node name
	// 	read first line of file to get header
	//	(we need a way to filter on "nfs|rpc|lock|portmap")
	//	grab only needed lines
	//
	//
	// FOr now lets assume argv[1] holds the path to the ps file....
	
	if (argc > 1){

		char const* const psfileloc = argv[1];
		FILE* psfile = fopen(psfileloc, "r");
		// get header of ps file
		fgets(header, sizeof(header),psfile);
		printf("contents of header[]: %s\n", header);

	}
}
