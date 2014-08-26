//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		YourName										 //
//																		 //
//		$Description:	Asks and prints your name to the screen			 //
//																		 //
//***********************************************************************//


// In this tutorial we show you how to get input from a user through the
// keyboard.  What we do below is print out a message to the user that asks
// their name, and then we wait for the user to type in their name and hit
// the enter key on the keyboard.  Pretty simple huh?  We also go over what
// a "variable" is.  This basically just stores what the user types in.
// So now let's move on and check the code to see how we grab input.

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

int main()									// The beginning of our program
{
	char szName[128];						// This means we are creating a "string"
											// of "char" (CHARacters).  This is called a variable.
											// A variable could be a "char", "int", "float", "double",
											// or one we create ourselves.  An "int" is for numbers.
											// A "float" is for floating point numbers like "1.2".
											// A "double" is for larger floating point numbers like 3.1462859.
											// Anyway, we now have a variable with 128 characters. 
											// The number is arbitrary (can be however many needed).
											// In other words, we have space to put a word in that's at most 128
											// characters long.  I chose the name "szName" because it is a string.
											// sz<The Name> is a standard for string variables.
											// You can type whatever you want as the variable name,
											// but if you are working with a lot of variables it's
											// easier to decipher what type of variable it is this way.

	printf("What is your name? ");			// Print to the screen "What is your name? "

	scanf("%s", szName);					// This function waits like a prompt.  The program
											// won't go any further until you type something and hit ENTER or <return>.
											// This function takes 2 parameters.  a "," separates parameters.
											// In this case, we passed in:  "%s" and szName.
											// The "%s" means we want to get a string from the user.
											// If we wanted a integer (number) we would use "%d".
											// The second parameter is our variable that we want to store what
											// is typed in by the user.  In this case, the user's name.

	printf("Hello %s!\n", szName);			// Once the user types in their name and hit's return,
											// we will take our variable that holds the name typed in
											// and print it to the screen.  We use the "%s" again to say we
											// want to print a string.  We use the "\n" (new line) character again
											// to skip a line.  After the end " there is the ","
											// to decipher between parameters.  The next parameter we pass in is
											// our variable that stores the name.  What is in the variable will
											// be printed to the screen where ever the "%s" is placed in the previous parameter.
											// We could also do "printf("Hi %s %s %s", szName, szName, szName);"
											// This would printf out, say the name is Ben .. "Hi Ben Ben Ben".
											// The function "printf()" knows how many parameters to read in by how
											// many of "%s" type things you use.  So we used 3, so we need to pass 
											// in 3 other variables.

	return 0;								// Return 0 to say "Zero errors occurred".
}

// The output for this program should be:

// What is your name? <Say we typed in Ben as our name>
// Hello Ben!

// If any of this didn't make sense, refer to the "HelloWorld" tutorial.


// © 2000-2005 GameTutorials
