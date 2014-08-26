//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		HelloWorld										 //
//																		 //
//		$Description:	Print "Hello World" to the screen				 //
//																		 //
//***********************************************************************//


// This is the first tutorial that teaches you how to begin programming in
// C.  C is a language you use to program, and the predessesor to C++;
// the most powerful and robust language ever created for programming.  This
// is the language you would use to make the huge games and applications you
// see in the stores.  Please read our beginner FAQ before viewing the tutorials.
// Also, if you have our video tutorials, please watch those first so you know
// how to use Visual Studio.

// What you are reading now is comments.  When you put a "//" in front of the
// line of code the compiler ignores it so you can type pretty much whatever
// you want.  How our tutorials work is you read the comments usually at the top,
// then go through the code and read the comments above and alongside the code,
// then usually you have something else to sum up what you just learned at the
// bottom of the file.  If there is more than one .c file (code file), then
// you always want to start with Main.c.  Now, let's go through and learn
// how to create our first program.  Read the green comments below to see what
// everything does.  All we do in this application is just print "Hello World!"
// to the screen (a black and white console window resembling DOS).


#include <stdio.h>				// This includes a header file called "stdio.h".
								// If we did not include this file we would not be able to call
								// The function "printf" down below.

								// The line below means we are calling a function that returns
								// nothing, AKA, "void".  We MUST have a function called "main()"
								// in order to run a program.  This basically lets the compiler
								// know we are ready to begin our program.

void main()
{								// The "{" signifies the start of the function

	printf("Hello World\n");	// We called another function called "printf()".  This prints
								// the message in quotes out to the screen.
								// This is one of first functions anyone uses when beginning to program.
								// The "(" ")" after the function name signify a begin and end
								// to passing in parameters.  The parameter we passed in was a string
								// filled with "Hello World".  You'll notice the "\n" right before
								// the last quote.  This tells the cursor to go to the next line on the screen.
								// So if we have "Hello World\n\n" , this would go down 2 lines.
								// The ";" at the end of the "printf()" function let's the compiler
								// know we are done with that line of code.
}								// The "}" signifies the end of the function


// Ahhh.... I feel good about this... :)
// To run this with Visual Studio C++ , Hit Control-F5.
// If you don't already have a project open, it will prompt you
// You should see 

// Hello World
// Press any key to continue . . .		<-- Visual Studio puts this at the end to show the program is finished

// © 2000-2005 GameTutorials
