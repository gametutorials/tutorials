/* Done by TheTutor

  Okay -- This tutorial is all about preprocessor directives.  For starters, what the heck
  are a preprocessor directives?  Well basically they are "methods" used to make source files
  (programming projects) easy to change AND compile under different compiler settings.  They
  can make your code MUCH MORE readable, reusable and compatible.  You know you're looking at
  a preprocessor directives when you see "code" that begins with the '#' character.  The 
  number sign (#) MUST be the first NON-WHITE character on a line containing a preprocessor
  directive.

  Preprocessor directives get "dealt with" BEFORE the source file(s) get compiled.  They
  are in fact "preprocessed".

  There are a fair number of preprocessor directives, but this tutorial is going to cover
  the "main ones" (a complete list for you to look up is at the bottom :)

  Now we'll go through some examples...

*/

#include <stdio.h> // The #include (read "pound include") is used to include other
				  // files.  It takes the include file, and treats it as if it's contents
				 // appeared (in the file that is including it) wherever the #include
				// is found.  #include's are used to load "standard header files"
			   // (these would be header files that allow you to call C functions such as
		      // "printf()").  When loading a "standard header file" the file's name
		     // is enclosed between angled brackets (example: <stdio.h>).  #include can 
		    // also be used to include "user defined header files".  These files are
		   // contained between quotes (example: "main.h").

#define NUM 1	// The #define (read "pound define") is used mostly for "substitutions".
				// Before this .c file is compiled, the preprocessor will go through it
				// and replace anywhere it finds "NUM" with "1".

// As you can see directly above, the #define can be used for "integer substitutions".  It 
// can also be used for "char" (text) substitutions.  Again BEFORE this .c file is compiled
// the preprocessor will replace everywhere it sees "TEXT" with "GameTutorials and
// preprocessor directives". 
#define TEXT "GameTutorials and preprocessor directives"

// **Note** above are just TWO EXAMPLES of what #define can be used to replace

int main()
{

	// Notice the substitutions that get made in these print statements:

	// Print out "NUM" to the screen
	printf("%d\n",NUM);

	// Print out "TEXT" to the screen
	printf("%s\n\n",TEXT);


/******************** The #if, #else, and #endif ***********************/
/**********************************************************************/

#if NUM // The #if works just like you'd expect.  BEFORE the .c file is compiled,
		// this condition is evaluated.  If it's true (i.e. non-zero), ALL the code
		// between the #if and #else statement is compiled into the program.  If it is 
		// false (i.e. equals zero) NONE of the code between the #if and #else is compiled
		// into the program (it's like it was erased).

	printf("NUM is non-zero\n");

#else	// If the expression "#if NUM" is false, the code between the #else and 
		// #endif will be compiled.  If the expression "#if NUM" is true, NONE of 
		// the code between the #else and #endif will be compiled.

	printf("NUM is zero\n");

#endif

/*	Each #if you have MUST have a corresponding #endif.  #if statements can be nested.
	So for example you could have something like this:

	#if A_CONDITION

		// Code

		#if ANOTHER_CONDITION

			// More Code

		#endif

  #endif

  One great use for the #if is to use it to quickly "comment out" a large portion of code
  Simply go:

  #if 0

	// Large chunk of code

  #endif

  When you compile, ALL THE CODE between the #if and #endif (in the example above)
  WILL NOT be compiled

*/

	return 0; // Everything went well :)

} // end of main()


/*

  There is at least one more preprocessor directive that gets used a fair deal.  You'll see
  this at the beginning of "user defined header files", it is the #ifndef preprocessor
  directive.
  
  The following "if not defined" (#ifndef) would evaluate to true:
	
  #ifndef 0

  In general, if the identifier (in the above example, the identifier is "0") is NOT DEFINED 
  the condition is true (nonzero).  Otherwise, the condition is false (zero).	
	  
  Let's suppose we made a header file named "main.h".  It's very standard to begin
  the header file like this:

  #ifndef MAIN_H
  #define MAIN_H

  // The header file's contents

  #endif

  The above is done to prevent the "redefinition of multiple header files" throughout a 
  project.  This way you can have many .c files that #include the same header file
  without any conflictions.
  
  Now as promised, here's the list of preprocessor directives:

  #define
  #elif
  #else
  #endif
  #error
  #if
  #ifdef
  #ifndef
  #import
  #include
  #line
  #pragma
  #undef

  Until you become AT LEAST an intermediate programmer, you shouldn't have to worry about
  any of the other preprocessor directives besides the ones we just covered (and even then
  as a beginning programmer you'll probably only see the #include and #define for awhile).

  If you're dying to know more about the preprocessor, you can of course check out MSDN
  (there's a link on GameTutorials).

  As always, direct any questions you may have to the GameTutorials message board.

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/

