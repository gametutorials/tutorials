/* Done by TheTutor

  This tutorial is all about preprocessor directives.  For starters, what the heck
  are a preprocessor directives?  Well, basically they are "methods" used to make source files
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

#include <iostream> // The #include (read "pound include") is used to include other
				   // files.  It takes the include file, and treats it as if it's contents
				  // appeared in the file that is including it wherever the #include
				 // is found.  #include's are used to load "standard header files"
				// such as the header files that allow you to call C++ functions/methods
			   // such as "cout".  When loading a standard header file the file's name
			  // is enclosed between angled brackets (example: <iostream>)  #include can 
			 // also be used to include "user defined header files".  These files are
			// contained between quotes (example: "main.h")

#define NUM 1	// The #define (read "pound define") is used mostly for "substitutions".
				// Before this .cpp file is compiled, the preprocessor will go through it
				// and replace anywhere it finds "NUM" with "1"

// As you can see directly above, the #define can be used for "integer substitutions".  It 
// can also be used for text substitutions.  Again BEFORE this .cpp file is compiled
// the preprocessor will replace everywhere it sees "TEXT" with "GameTutorials and
// preprocessor directives" 
#define TEXT "GameTutorials and preprocessor directives"

// **NOTE** above are just two examples of what #define can be used to replace.  It can 
// be used to replace just about anything.

using namespace std; // We're using the "standard namespace" -- This namespace holds
					// iostream's standard functions/methods (such as "cout" and "cin")

int main()
{

	// Notice the substitutions that get made in these print statements

	// Print out "NUM" to the screen
	cout << NUM << endl;

	// Print out "TEXT" to the screen
	cout << TEXT << endl << endl;


/******************** The #if, #else, and #endif ***********************/
/**********************************************************************/

#if NUM // The #if works just like you'd expect -- BEFORE the .cpp file is compiled,
		// this condition is evaluated.  If it's true (i.e. non-zero), ALL the code
		// between the #if and #else statement is compiled into the program.  If it is 
		// false (i.e. equals zero) NONE of the code between the #if and #else is compiled
		// into the program as if it was erased.

	cout << "NUM is non-zero" << endl;

#else	// If the expression "#if NUM" is false, the code between the #else and 
		// the #endif will be compiled.  If the expression "#if NUM" is true NONE of the code
		// between the #else and #endif will be compiled

	cout << "NUM is zero" << endl;

#endif

/*	For each #if you have MUST have a corresponding #endif.  #if statements can be nested.
	So for example you could have something like this:

	#if A_CONDITION

		// Code

		#if ANOTHER_CONDITION

			// More Code

		#endif

  #endif

  One great use for the #if is to use it to quickly get rid of large portions of code.

  #if 0

	// Large chunk of code

  #endif

  When you compile, all the code between the #if and #endif (in the example above)
  will NOT be compiled.
*/

	return EXIT_SUCCESS; // Everything went well :)

} // end of main()


/*
  There is at least one more preprocessor directive that gets used a fair deal.  You'll see
  this at the beginning of "user defined header files", it is the #ifndef preprocessor
  directive.
  
  The following "if not defined" (#ifndef) would evaluate to true
	
  #ifndef 0

  In general, if the identifier (in the above example, the identifier is "0") is NOT DEFINED 
  the condition is true (nonzero).  Otherwise, the condition is false (zero).	
	  
  Lets suppose we made a header file named "main.h".  It's very standard to begin
  the header file like this:

  #ifndef MAIN_H
  #define MAIN_H

  // The header file's contents

  #endif

  The above is done to prevent header conflicts by including the same header file more than
  once throughout a project.  This way you can have many .cpp files that #include the same header file
  without any conflictions.
  
  Now as promised, here's a list of preprocessor directives:

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

  Until you become at least an intermediate programmer, you shouldn't have to worry about
  any of the other preprocessor directives besides the ones we just covered and even then
  as a beginning programmer you'll probably only see the #include and #define for awhile.

  If you're dying to know more about the preprocessor, you should check check out MSDN or 
  post a question to the GameTutorials message board.
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/

