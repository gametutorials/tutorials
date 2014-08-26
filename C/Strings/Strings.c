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

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>						// You might as well start using these 3 headers as a standard from now on
#include <stdio.h>							// We must include this file to use certain functions like printf(), etc..
#include <string.h>							// We need to include this when using the function strcmp() (compares a string)

void main()									// The beginning of our program
{											// This starts the main function
	char szAnswer[128] = {0};				// You've seen these before!  This is declaring a string.
											// If we did not have the [128] at the end it would just be a character.
											// We wouldn't need the [128] if we were just asking 'y' or 'n' questions.
											// But since we want the user to type in more than one character, we put
											// the [<amount of characters>] after the name of the variable.  Once
											// again, a variable is a name that points to a place in memory.  Instead of 
											// calling everything 0X034F683C, it's easier to use names we understand :)
					 						// Like "szAnswer" should tell you that this variable is a string of characters
											// that will be holding an answer.  Code can be cryptic sometimes, but the
											// more readable the better, especially if other people will be reading it.
											// Even if other people aren't reading it, you sometimes have to come back and 
											// Look at it yourself, and trust me, you DO forget what you were doing, and if you
											// don't comment your code and use readable variable names, you WILL forget.  It sucks.
											// By the way, this is called commenting code.  using "//" to explain things in the code.
											// Of course you would be insane if you commented as much as I am doing here,
											// but I am trying to be as CLEAR as possible.  ONE more thing about this line is, there is an " = {0}".
											// When you have a string, this is the way of initializing it.  If we didn't, it would look like this:
											// "            "IIIIIIIIIIIIIIIIIIIIIIIIIIIII8be"    "    if you printed it out.  (basically 128 characters of junk)
											// This is the address of the variable in memory.
						
	printf("Do you like programming yet? ");// Pose a questions to answer

	scanf("%s", szAnswer);					// Since we are reading in a string, we must use "%s".  Pass in szAnswer to hold the answer.
											// Below is a tricky statement to look at.
											// strcmp() stands for "string compare".  This function compares 2 strings.
											// If you are just testing to see if the strings are equal, it doesn't matter what order
											// you pass them in.  I usually pass in the variable in this case first.
											// The strcmp() function passed back a "0" (zero) if the strings are identical.
											// When I say passes back, I mean that the function returns a number.
											// This might take a lot of explaining... let's see...
											// The code "strcmp(szAnswer, "yes") == 0" calls the function strcmp()
											// and does some internal stuff to check every character in each string to make sure it's identical.
											// If the check finds that they ARE identical, the function strcmp() returns a 0.
											// This means, that the code is literally replaced and the code now looks like this:
											// "0 == 0"  or, "<what ever number was returned> = 0".  It's almost magically replaced.
											// strcmp() returns a negative or positive number if the strings aren't identical, depending
											// on if the left string is greater or less than the right string passed in.
											// But, in most cases, you only test if it equals 0. So moving on.. you'll notice my use of "(", ")"
											// "( strcmp(szAnswer, "yes") == 0)" .. I put one more set of parenthesis around the 
											// "strcmp(szAnswer, "yes") == 0" because we are making a second check in the same "IF" statement.
											// You don't NEED to, but once again, it improves readability.  If you are unfamiliar with how 
											// parenthesis work, here is an example:
											// " ( Do everything inside of here first) / (and divide the result by the result of this statement) " 
											// Better said:   (5 * 2) / 10 = 1  essentially 10 / 10 is what is comes out to, which equals 1.
											// Like I said, you don't need the parenthesis, because the computer knows order of operations,
											// but it is always more readable to the HUMAN who doesn't.  If ever in doubt, use parenthesis.
											// SO!  anyway, we check to see if the user typed in "yes", all lowercase.
											// Then on the other side of the IF statement we check if the user typed "Yes", with the "Y" uppercase.
											// Using the "||" double OR operator, only one of those need to be true in order for the IF statement
											// to execute the code inside of it.
	if( ( strcmp(szAnswer, "yes") == 0) || ( strcmp(szAnswer, "Yes") == 0) )
	{										// If the user typed in "yes" or "Yes" , print this:
		printf("Good for you!  It's fun isn't it?\n");	
	}
	else									// Else, if the user didn't type in "yes" or "Yes"
	{										// Print these words of encouragement  :)
		printf("Don't worry, if it seems frustrating now, it will become more clear.\n");
	}

	printf("Do you like programming yet? ");// Let's do the exact process, except in a different way.

	rewind( stdin );						// Because we are using scanf() - which isn't the best function...
											// We need to CLEAR the buffer (a place that holds something) that holds
											// the keyboard data, in other words, don't remember what the user typed in before.
											// This rewind() function does that.  "stdin" stands for "standard input".
											// Standard input - The buffer that holds what the user has typed in using the keyboard.
	scanf("%s", szAnswer);					// Get the answer again.
											
											// The second way checking input from the user is this way.
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
											// Remember, this is called an "Array".  You can have an array of numbers too!
											// Let's take the word "Hello".  In a string, this is set up like this:
											// "H" is the ZERO'th slot, "e" is the FIRST slot, "l" is the SECOND slot, "l" is the THIRD slot, and "o" is the FOUTH.
											// Yes, the slot starts at zero... this can be TREMENDOUSLY confusing.
											// BUT! We are forgetting one last slot. . .  ALL strings have a final slot
											// which is  '\0'.  This is called a NULL terminator.  That is why they say, a NULL terminated string.
											// This thing lets the compiler know the string's end.  Let's try and explain it a bit more.
											// We create a string of 128 characters right?  At the beginning?  SO!
											// If you want to print out something to the screen, say what someone typed in...
											// If we didn't have a NULL terminator at the end of the string, it would look like this:
											// (Say you entered "yes") "Your answer was yes                      ."
											// Do you see what I mean?  How would it know to stop?
											// We want it to look like this: "Your answer was yes."  We don't want the extra baggage :)\
											// By the way, I chose [128] for the amount of characters because it just seems like a decent number.
											// Just in case you want to type something more than "yes" or "no"...  realistically we
											// would only need like [4]. or maybe even 2 if we are just checking the first character they type in.
	if( szAnswer[0] == 'y' || szAnswer[0] == 'Y' )
	{										// If they typed a 'y' or 'Y' first.. print the message
		printf("Good for you!  It's fun isn't it?\n");
	}
	else									// Else if they didn't type a y or Y first..
	{										// Encourage them :)
		printf("Don't worry, if it seems frustrating now, it will become more clear.\n");
	}
											// FURTHER EXPLANATION:  Here is what's happening if they typed in "yes":
											// szAnswer[0] == 'y' , szAnswer[1] == 'e', szAnswer[2] == 's'
											// The zero slot is 'y', the 1 slot is 'e', the 2 slot is 's'.
											// Remember the slots start at zero, so all we have to do if check if the zero slot equals a 'y' or 'Y'.
											// If this doesn't make sense to you, think of any word and map it out.  Figure out what letter goes in what slot.

											// BTW, this is the notation to access an array;
											// "		<variableName>[index]  i.e.  szName[4] == '\0'  if szName is "Ben"   "
											// You CANNOT! do a check like this:  if(szName == "Ben").  This doesn't work.
											// If szName is an array, that is the same thing as saying if(szName[0] == "Ben")
											// A regular variable is a one dimensional array, so it's default is <name>[0].
											// num[0] = 2;  is the same thing as num = 2;
											// you just don't need to use the [0] because it's understood that's what you meant.
											// Yes, this can be very confusing, but like ANY language you learn, there are things like this.  Take Japanese :)

}											// This ends the main function (program is done)

// © 2000-2005 GameTutorials
