//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Functions										 //
//																		 //
//		$Description:	Work with functions to create a menu.			 //
//																		 //
//***********************************************************************//


// What we do in this tutorial is create a menu for the user to select options from.
// You know how on a calculator there are things like square root, cosine, sine, etc...?
// Those are called functions.  Those functions allow us to do things with just calling
// one function and not having to actually do the math for the square root, cosine, etc.
// Functions in programming are the same.  We create a function that does something, and
// then we can call that function by just 1 line of code, not actually copying and pasting
// the code each time we want to perform that function.  This makes code smaller, more readable
// and less frustrating :).  In this tutorial we create a menu function that draws the menu options.
// Some functions take parameters and some return values.  For instance, square root takes a number
// to square, then it returns the result of the square root right? : (double value = sqrt(49);)
// The "value" is the return value, and the 49 is the parameter that is passed into the function.
// This is an example of using a function; however, in this tutorial we won't use a input parameter.
// When programming large projects, we have many different .cpp files.  When we create a function
// we want other .cpp files to be able to use that function that might perhaps be in another
// .cpp file.  To do this, we create "function prototypes".  These are just one line declarations
// of the functions.  We then put these "function prototypes" in .h files (header files).  In
// this way other .cpp files can all include the .h (header) file and the compiler can recognize
// the function by it's prototype.  It's really to help out the compiler... since we are nice guys/gals :)
// We will go into greater detail later. Below is our function prototype for our DrawMenu function.


// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>							// We must include this file to use certain functions like printf(), etc..

// The C++ language has a data type named "bool" (boolean) and the C language does not.
// A boolean is a variable that holds a value, true, or false.  (true is 1, false is 0)
// They can be convient, so we will define the equivalent C++ variable below to use in
// this C program.
typedef int bool; // New boolean data type
const int true = 1; // TRUE
const int false = 0; // FALSE

void DrawMenu();							// This declares our function name and what it returns.
// This declaration would usually			// This let's the compiler know we have a function, and what parameters we need to pass in.
// Go in another file, say, functions.h		// This is like declaring a variable.  We initialize our variable at the beginning, 
											// then usually assign it something later.
											// In our case, our function doesn't need to return anything so we put a "void" , meaning nothing.
											// The next tutorial will deal more with functions and returning things from them.
											// When declaring a function, we must put a semicolon after it.

void DrawMenu()								// This is the part where we assign the function to do something.
{											// We don't use a semicolon because we are not finished with the function, we are starting it with "{" and ending it with "}".
	printf("\t\t ****************Game Menu**************** \n");
	printf("\t\t *                                       * \n");	// This is our game menu, comprised of printf()'s :)
	printf("\t\t *   1) New Game                         * \n");	// Beautiful isn't it?  hehe.
	printf("\t\t *   2) Load Game                        * \n");	// You can obviously find more creative ways to display a menu than this.
	printf("\t\t *   3) Save Game                        * \n");	// Just somehow we have to let the user know their options.
	printf("\t\t *   4) Inventory                        * \n");	// You'll notice the "\t".  This is like "\n", but it means TAB.  It's just so you
	printf("\t\t *   5) Options                          * \n");	// Don't have to press space bar 5 times... I am just expanding your C vocabulary :)
	printf("\t\t *   6) Quit                             * \n");
	printf("\t\t *                                       * \n");
	printf("\t\t ***************************************** \n");	
}																	// This is the end of the function.
																	// If you remember the macro you did with PRINT_NUM(), this is very similar.

void main()
{
	bool StillPlaying=true;					// Define our boolean flag to true.  In this case, let's make a boolean and set it to true, meaning we
	int choice=0;							// are "StillPlaying" the game.  When Quit is chosen, we will set it to false, ending the game loop.

											// Let's create a while loop to continue until they Press '6' for quit.
	while(StillPlaying)						// This says, do everything below until StillPlaying == false.  Remember, a statement doesn't happen if 0 is the result from the expression.
	{
		DrawMenu();							// All we need to do is call our function, like so.
											// Since there are no parameters to pass in, we just close the parenthesis (); with a semicolon.															
											// Now comes the part where we need to be able to choose from the menu.
	
		printf("Choose from the menu: ");	// Prompt the user

		scanf("%d", &choice);				// Get the users choice, (remember the & makes it so we can change "choice".  Think of it as a security thing.  It's permission to mess with choice.) 
												
											// This is our "switch" statement.  Just a quick review:
											// What it does is take "choice" (or whatever is passed in), and does a bunch of if/else statements on it.
											// it's pretty much the same things as if/else, but looks cleaner.
		switch(choice)						// "case 1:"  is the same thing as saying, "if(choice == 1)"  You see?  A lot cleaner.
		{
			case 1: printf("You chose a New Game!\n");		// if (choice == 1)
					break;					// break - breaks out of the switch statement. 
			case 2: printf("You chose to Load a Game!\n");	// if (choice == 2)
					break;									// Print the choice and break;
			case 3: printf("You chose to Save a Game!\n");	// if (choice == 3)
					break;									// Print the choice and break;
			case 4: printf("You chose your Inventory!\n");	// if (choice == 4)
					break;									// Print the choice and break;
			case 5: printf("You chose Options!\n");			// if (choice == 5)
					break;									// Print the choice and break;
			case 6: StillPlaying = false;					// if (choice == 6)
					printf("Game Over Already?\n");			// StillPlaying is set to false.  This means, when the code goes back up to the top, while(StillPlaying) will NOT be true and fail.
					break;									// A weird thing about switch statements, you don't need brackets {} for more than one line.
		}													// You just need a break;
	}	// while (StillPlaying)								// It's a good idea to label what the closing bracket is coming from.
															// When your code gets huge, it's kinda annoying to go back and match up all the brackets to find the statement.
															
}    	// void main()										// Once the user hits 6, the while loop quits and ends our program.
														
															// And that's it!  Here is a menu.  Sure it's the simplest menu ever, but it's a start :)
															// The more important thing here is the use of a function.

// © 2000-2005 GameTutorials


