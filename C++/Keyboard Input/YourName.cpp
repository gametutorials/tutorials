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


#include <iostream>				// Here we include our standard C++ definitions
#include <string>				// Here we include a new file, that holds "string" definitions  
								// We must have this to use the "string" type
using namespace std;			// Set our namespace to standard (we will always have this line)
												

int main()						// This creates our "main" function, which controls the whole program
{								// The "{" signifies the start of the function
	string strName;				// Here we create something called a "variable".
								// We are going to read in a name from the user, so we need
								// some place in memory to HOLD the name.  If I type in "Ben",
								// I need to store it somewhere to access it later.  Just like when I
								// ask you your name, and you tell it to me, my brain needs to store it some place
								// In my memory so I can remember it and say, "Hello <your name>".  Computers work the same way.
								// If we want to hold a integer, we wouldn't use a "string" variable, we would use
								// an "int" variable.  like, "int mynumber;".  We can call the variable name
								// virtually anything we want, but no spaces and characters like this.. "!<>{} .. etc"
								// We call our string variable, "strName".  It doesn't have to be called that, it could be called "name",
								// but this is a programming convention that you will find.  "str" stands for string.  "sz" is also used by people.
								// So, in order to make our variables readable and have clean code, we label them like this.
								// When your code becomes huge, you can always scroll up to where the variable was created and see what
								// type is was.  For example, an int, double, float, or string, etc.. so we label them accordingly.
								// There are many other types like, "float" and "double" used for numbers like "23.5" that have decimals (floating point numbers).
								// Also, if you want to declare other variable types, like "int" and "float" you do not have to include
								// any special headers (like <string>).  They are already built into the standard C++ language.

								// Now we display a question to the screen:
	cout << "What is your name? ";
								// Notice we don't use an "endl" at the end.  This is because we want the user to
								// type their name on the same line as the question.  You'll see once you run it.
	cin >> strName;				// We use "cin" (console input) to read in input from the keyboard.
								// The syntax is opposite from cout, ">>".  We give "cin" a variable to store the name, "strName".
								// So now, what ever name the user types in, it will be stored and can then be access from the variable "strName".
								// If you get confused with the << and >>,  Just think of it like this:
								// We are pushing text out to the screen with cout so we use << which is going AWAY from cout, (like Pac Man)
								// where, cin >> is reading in data so the >> is going TOWARDS cin.  That's how I see it anyway. :)
								// Notice the semicolon at the end.  All statements inside of our functions must have semicolons after them.
								// Except some few exceptions, which I will explain later.  Just do what I do :)
								// Display a greeting to the screen that says Hello to the user:
	cout << "Hello " << strName << endl;
								// Ok, if I typed in "Ben", it would say, "Hello Ben".
								// We display the string, "Hello " with a space after it.
								// Then we display what ever is stored in the variable "strName".
								// Then we skip down a line, and end with a semicolon.
	
	return 0;					// Return with zero problems
}

// The output for this program should be:

// What is your name? <Say we typed in Ben as our name>
// Hello Ben


// Let's say we want to do some other weird stuff to test the capabilities of cout:
// We want to say, "Hello Ben Ben Ben!"

// How would we do that using cout???

// Like this:   cout << "Hello " << strName << " " << strName << " " << strName << "!" << endl;
// Try it for yourself.  You can do as many "<<" iterations as you want.

// Try reading in a number instead of a name.  Use the type "int" instead of "string"

// If any of this didn't make sense, refer once more back to the first tutorial, "HelloWorld".

// © 2000-2006 GameTutorials
