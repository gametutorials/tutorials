//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Questions										 //
//																		 //
//		$Description:	Asks/Answers questions using if/else statements. //
//																		 //
//***********************************************************************//


// This tutorial shows you how to use "if statements" and "else statements"
// to control what happens in your program.  In this example, we have the user
// input their age, and then depending on how old they are, we print out a
// specialized message.  You will use if and else statements in pretty much
// every program you will ever create, especially if you allow the user to
// do things and interact with your program.  Look below to see how we use them.


#include <iostream>							// This line of code is necessary to include all the C++ standard functions\classes definitions
using namespace std;						// Set our namespace to standard (don't stress on this right now)

int main()									// This creates our function main()
{											// This starts the main function
	int age=0;								// Initialize a integer variable for the age

	cout << "How old are you? ";			// Print to the screen a questions about the users age.

	cin >> age;								// Read in the answer after the user hits return.
											
											// NOW!  Get ready for IF and ELSE statements! Muhahaha!
	if(age > 20)							// This is an IF statement.  This is simply saying:
	{										// "If age is greater than 20", then...    
											// Print this message
		cout << "You're over 20 huh?" << endl;
	}										// End the if statement


	if(age > 30)							// This checks if the age entered was over 30,
		cout << "You're over 30!?" << endl;	// Print this message

											// Now you might have noticed that there was no "{" "}" brackets.
											// Well, you don't HAVE to use brackets if there is only one line
											// after the condition, either it be a while, a for, or an if/else....
											// The fact is that it's a good habit to get into and makes your
											// code more readable (which is good!)  You also tend to find yourself
											// adding things inside your statements (for, while, if), so you might as 
											// well just put it there for the future, just a coding preference I guess.

											// If the user typed 35 as their age, the program would print out both messages above.
											// But if they entered something like 25, it would only print the first one.

	if(age < 20) {							// Here is another coding style...  people who put their "{" after the conditional statement.
		cout << "You're a young'n!" << endl;// This doesn't do anything different, it's just that some people prefer it.
	}										// There are some languages that REQUIRE it, but C/C++ is VERY flexible where there are
											// very few REQUIRED syntax.  Actually, you could even do this and it would work:
											// "if(age < 20) cout << "You're a young'n!" << endl;"
											// You don't even need a new line OR brackets (if it's only one line after).
											// This is why ";" semicolons are important.  It let's the compiler know to
											// go on to the next statement.  you can do things like this:
											// "age++; age--; age = age;"
											// This adds 1 to age, subtracts one to age, then assigns age to age...
											// which does absolutely nothing, but you see my point.  
											// You don't need to skip a line, just use semicolons.
											// it all boils down to readable code.  That is what is important.
				
	if(age < 20 && age > 12)				// Here is something new.  The "&&" means "and".
	{										// So the statement is saying, "if ( age is less than 20 AND greater than 12)"
		cout << "Being in your teens can be tough..." << endl;
	}										// cout the message
											// You will use this type of expression a lot.  There is also :
											// "!=" -> does not equal    and    "||" -> or
											// So instead of checking if both things are true, you can check if one or the other are true
											// by using "||"   the OR operator.
											// If you are familiar with the binary operators, "&", and "|" ....
											// These are similar, but different.  "&&" and "||" check conditions between 2 statements.
											// if the user is a teenager, the message would have printed to the screen.
	
	if(age == 100)							// This checks if "age equals 100".  This is different from "age = 100"
		cout << "WOW!  What's your secret!?" << endl;
											// "age == 100" checks to see if the value of age is 100, while "age = 100" assigns the value 100 to the variable age.
	
	if(age > 50)							// Here is another usage of IF statements.
		cout << "Life after 50 can be great!" << endl;
	else									// If age isn't greater than 50, print the other message below
		cout << "You've still got a LONG way to go..." << endl;
											// Look below for an if/else statement explanation

	return 0;								// Return with zero problems
}											// This ends the main function (program is done)

// This is what we call IF/ELSE statements.
// "	if( <The statement here is true> )	  "
// "		<do this>						  "
// "	else								  "
// "		<do this instead>                 "

// You might be wondering by now about the spacing after conditional statements.
// It all falls back to code readability.  It's not fun to look at un-formatted code.
// If you ever want people to help you (which you WILL!) you have to comment and 
// indent your code, or else they won't even try.
// Remember, you CAN and probably should use { } 's.

//		if (etc) 
//		{
//			<do this>
//		}
//		else
//		{
//			<do this>
//		}

// It's so much easier to understand.  Even if you only have one line after each statement.

// © 2000-2005 GameTutorials
