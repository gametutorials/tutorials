//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Strings											 //
//																		 //
//		$Description:	Ask/Answers questions with strings.				 //
//																		 //
//***********************************************************************//


// In more programs you will want to use strings.  This tutorial shows you
// how to use them.  Basically, a string is just an array of characters to
// form words and phrases, but instead of creating a standard array of chars,
// we use the STL (Standard Template Library) that comes with C++ to use the
// "string" class.  This class is easier to use and more intuitive.  Look
// below for our tutorial on using the string variable.

#include <iostream>							// This line of code is necessary to include all the C++ standard functions\classes definitions
#include <string>							// This allows us to use the variable type, "string"
using namespace std;						// Set our namespace to standard (don't stress on this right now)

int main()									// This creates our function main()
{											// This starts the main function
	string strAnswer = "";					// We create a variable called "strAnswer" of type "string"
											// This variable will hold what the user types in.  If we
											// Are getting words and sentences, we want to use a "string".
											// A string is an array of characters (just a bunch of characters, one after the other).
											// Look at the C equivalent for this tutorial.  C++ is a lot easier in this case.
											// But behind the scenes, a string has a char [] (array of characters) too, but we don't see it.
											// The "str" stands for "string".  We want to label variables like this so we know what type they are.
											// All strings are terminated with a '\0'.  We will discuss that later though.  
											// Once again, we want our variables to be understandable to us and other who
											// are reading our code, so we label them sometimes, (depending on the variable type) with
											// a prefix that lets us know what type of variable it is.  "int, float and double" usually doesn't have them.
											// We initialize the string with (= "").  This basically sets the string to 0 characters.
											// We could say:		string strAnswer = "Programming"; 
											// In this case, the string would now hold the word "Programming".
											// Or:		string strAnswer = "I love Programming!"
											// BUT!  We don't want it to hold anything, so we set it to "" which means no characters.
											// A string always has double quotes around it.  That is how the compiler differentiates between code and text.

	cout << "Do you like programming yet? ";// Pose a questions to the screen

	cin >> strAnswer;						// Read in what the user types in (This waits until they hit <enter>).
											// Next we use if and else statements to test what the user typed in.
											// Since the '=' operator assigns a value to a variable, we need to use two '=' operators
											// to test if something is EQUAL to something else, like below.
											// That whole if statement is saying:  if (strAnswer equals yes or strAnswer equals Yes).
											// The '||' characters (which are called pipes, shift-\ ) means OR in code.
											// It's called a "logical or".  Refer to our conditional statements tutorial for more info.
											// So, below we are creating a sentence just like in spoken language.
											// We need to check the 2 cases in case they use a capital letter.  We could also check against "YES".
											// You can do do as many "||" 's as you want.  I recommend using parenthesis around each check though.
	if( ( strAnswer == "yes" ) || ( strAnswer == "Yes" ))
	{										// If the user typed in "yes" or "Yes" , print this:
		cout << "Good for you!  It's fun isn't it?" << endl;	
	}
	else									// Else, if the user didn't type in "yes" or "Yes",
	{										// print these words of encouragement  :)
		cout << "Don't worry, if it seems frustrating now, it will become more clear." << endl;
	}								
											// Let's do the exact process, except in a different way
	cout << "Do you like programming yet? " << endl;

	cin >> strAnswer;						// Get the answer again
											
											// Below is a second way of checking input from the user.
											// What this does is check the first character that the user typed in.
											// This way might enable the user to be less formal.  Let me explain...
											// The user can type "yah sure!" or "yup!" or "yah, what ever..."
											// and it would still be interpreted as a yes.  You see?
											// Before you can fully understand how this works, let me explain how strings work.
											// Think of the word "string".  Think of how people use the word...
											// "There was a string of people lined up to get their tickets."
											// Think of it like this, "A string of characters".  A character being a letter.
											// All a string is, is just a bunch of lined up characters right?  Strings are words.
											// Since these strings are lined up, you could almost put them in slots right?
											// Well, that is exactly how the computer handles strings, just slots that hold the letters.
											// This is called an "Array".  You can have an array of numbers too!
											// Let's take the word "Hello"  In a string, this is set up like this:
											// "H" is the ZERO'th slot, "e" is the FIRST slot, "l" is the SECOND slot, "l" is the THIRD slot, and "o" is the FOUTH.
											// Yes, the slot starts at zero... this can be TREMENDOUSLY confusing.
											// BUT! We are forgetting one last slot. . .  ALL strings have a final slot
											// which is  '\0'.  Your are thinking... WHAT?  what the heck is that?
											// This is called a NULL terminator.  That is why they say, a NULL terminated string.
											// This thing lets the compiler know the string's end.  Refer to the C tutorial instead
											// of the C++ tutorial to further get an explanation.  The C tutorial shows you what's going
											// on behind the scenes of a "string".  C++ doesn't need to allocate a number of characters to store,
											// It does it dynamically (on the fly).  That is the nice thing about C++.  We will pick this topic up later.

											// Below, we are just checking the first character of the string.
											// Since the string is an array of characters, we need to use array notation.
											// To access a certain character in the string, we use the brackets [].  Inside the
											// brackets we need to give it the slot number to access.  [0] is the first character,
											// which just might be 'y' or 'Y'.  We use single quotes to delimit a character, and double quotes from strings.
											// We could not say, 'Hello', we need double quotes for 2 or more characters together.
											// So, in English, this line is saying:  if (the first character in strAnswer is a lower case y or a uppercase Y)
	if( ( strAnswer[0] == 'y' ) || ( strAnswer[0] == 'Y' ) )
	{										// If they typed a 'y' or 'Y' first.. print the message
		cout << "Good for you!  It's fun isn't it?" << endl;
	}
	else									// Else if they didn't type a 'y' or 'Y' first.. encourage them :)
	{									
		cout << "Don't worry, if it seems frustrating now, it will become more clear." << endl;
	}
											// FURTHER EXPLANATION:  Here is what's happening if they typed in "yes":
											// strAnswer[0] == 'y' , strAnswer[1] == 'e', strAnswer[2] == 's'
											// The zero slot is 'y', the 1 slot is 'e' , the 2nd slot is 's'.
											// Remember the slots start at zero, so all we have to do if check if the zero slot equals a 'y' or 'Y'.
											// If this doesn't make sense to you, think of any word and map it out.  Figure out what letter goes in what slot.

											// BTW, this is the notation to access an array;
											//      <variableName>[index]  i.e.  szName[3] == '\0'  if szName is "Ben"
											
	return 0;								// Return with zero problems											
}											// This ends the main function (program is done)


// © 2000-2005 GameTutorials
