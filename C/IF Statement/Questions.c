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


// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>							// We must include this file to use certain functions

void main()									// The beginning of our program
{											// This starts the main function
	int age=0;								// Initialize a integer variable for the age

	printf("How old are you? ");			// Print to the screen a questions about the users age.

	scanf("%d", &age);						// Read in the answer after the user hits return.
											// You'll notice the "&" symbol right before the variable.
											// Here comes a confusing part for everyone when starting to program.
											// scanf() is a function that takes 2 parameters.  The first is the format
											// that the user is typing in... either that be a number, a decimal number, or a string,
											// or a single character.  Now, the next parameter is the variable to HOLD whatever the user
											// Types in.  Ok, here comes the confusing part..
											// If you pass a variable into a function, and the function CHANGES the variable in any way,
											// when the function is finished, the variable will NOT change unless the function has
											// the address in memory to the variable.  You give the function the address in memory by
											// putting the "&" symbol in front.  If you didn't, the program would crash after you typed in an age.
											// This is a HUGE subject that will be discussed later, and more fully.
											// Essentially, when anything is passed into a function, you could think of it as the
											// function making it's own local copy.  If you want the function to change what ever you passed in,
											// You have to use &.  If that doesn't make any sense.. don't sweat it.  I will explain it all
											// later when we start using functions and pointers.
												
											
											// NOW!  Get ready for IF and ELSE statements! Muhahaha!
	if(age > 20)							// This is an IF statement.  This is simply saying:
	{										// "  If age is greater than 20 "     
		printf("You're over 20 huh?\n");	// Print this message
	}										// End the if statement


	if(age > 30)							// This checks if the age entered was over 30
		printf("You're over 30!?\n");		// Print this message

											// Now you might have noticed that there was no "{" "}" brackets.
											// Well, you don't HAVE to use brackets if there is only one line
											// after the condition, either it be a while, a for, or an if/else....
											// The fact is that it's also a good habit to get into and makes your
											// Code more readable (which is good!)  You also tend to find yourself
											// adding things inside your statements (for, while, if) so you might as well
											// Just put it there for the future; just a coding preference I guess.

											// If the user typed 35 as their age, the program would print out both messages above.
											// But if they entered something like 25, it would only print the first one.

	if(age < 20) {							// Here is another coding style...  people who put their "{" after the conditional statement.
		printf("You're a young'n!\n");		// This doesn't do anything different, it's just that some people prefer it.
	}										// I've programed for a language that REQUIRES it, but C/C++ is VERY flexible where there are
											// very few REQUIRED syntax.  Actually, you could even do this and it would work:
											// "	if(age < 20) printf("You're a young'n!\n"); "
											// You don't even need a new line OR brackets (if it's only one line after).
											// This is why ";" semicolons are important.  It let's the compiler know to
											// go on to the next statement.  you can do things like this:
											// "	age++; age--; age = age; "
											// This adds 1 to age, subtracts one to age, then assigns age to age...
											// Which does absolutely nothing, but you see my point, you don't need to skip a line, just use semicolons.
											// It all boils down to readable code.  That is what is important.
				
	if(age < 20 && age > 12)				// Here is something new.  The "&&" means "and".
	{										// So the statement is saying, "if ( age is less than 20 AND greater than 12) "
		printf("Being in your teens can tough...\n");
	}										// Printf out the message
											// You will use this type of expression a lot.  There is also :
											// "!=" -> does not equal    and    "||" -> or
											// So instead of checking if both things are true, you can check if one or the other are true
											// by using "||", the OR operator.
											// If you are familiar with the binary operators, "&", and "|" ....
											// These are similar, but different.  "&&" and "||" check conditions between 2 statements.
											// if the user is a teenager, the message would have printed to the screen.
	
	if(age == 100)							// This checks if "age equals 100".  This is different from "age = 100".
		printf("WOW!  What's your secret!?\n");
											// "age == 100" checks to see if the value of age is 100, while "age = 100" assigns the value 100 to the variable age.
	
	if(age > 50)							// Here is another usage of IF statements.
		printf("Life after 50 can be great!\n");
	else									// If age isn't greater than 50, print the other message below.
		printf("You've still got a LONG way to go...\n");

											// This is what we call IF/ELSE statements.
											// "	if( <The statement here is true> )	  "
											// "		<do this>						  "
											// "	else								  "
											// "		<do this instead>                 "

											// You might be wondering by now about the spacing after conditional statements.
											// It all falls back to code readability.  It's not fun to look at un-formatted code.
											// If you ever want to people to help you (which you WILL!) you have to comment and 
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
											
											// It's so much easier to understand.  Even if you only have one line after each.	

}											// This ends the main function (program is done)


// © 2000-2005 GameTutorials
