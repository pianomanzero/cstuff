// an example showing how getopt is typically used to work with command line options and flags.
// normally a switch statement is used to dispatch on the return value from getopt. 
// Typically each case just sets a variable that is used later in th eprogram
// a second loop is used to process the remaining non-option arguments
//



#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main (int argc, char **argv)
{
 int aflag = 0;
   int bflag = 0;
     char *cvalue = NULL;
       int index;
 int c;

   opterr = 0;

     while ((c = getopt (argc, argv, "abc:")) != -1)
         switch (c)
       {
     case 'a':
             aflag = 1;
             break;
           case 'b':
             bflag = 1;
             break;
           case 'c':
             cvalue = optarg;
             break;
           case '?':
             if (optopt == 'c')
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
             else if (isprint (optopt))
               fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             else
               fprintf (stderr,
                  "Unknown option character `\\x%x'.\n",
                     optopt);
             return 1;
           default:
             abort ();
           }

       printf ("aflag = %d, bflag = %d, cvalue = %s\n",
         aflag, bflag, cvalue);

 for (index = optind; index < argc; index++)
     printf ("Non-option argument %s\n", argv[index]);
   return 0;
}
