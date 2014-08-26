//**********************************************************************************//
//																					//
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -					//
//																					//
//		$Author:		Ben Humphrey	digiben@gametutorials.com					//
//																					//
//		$Program:		Structures													//
//																					//
//		$Description:	Using structures in a simple, clear way.					//
//																					//
//**********************************************************************************//


// This tutorial shows us how to use structures to group data together so we can use
// a single variable to represent a bunch of data.  Let's go through our tutorial below 
// and see how to define and use structures.


#include <stdio.h>										// include our standard header file for printf(), etc..

														// Below is a "typedef" structure.  It's called a "typedef" because it means we are defining a type.
														// The type we are defining is the type "MONEY".  The "_MONEY" is what the compiler will call the structure.
														// By putting a typedef in front of the structure, it's like using a #define.  it's just a macro, or alias.
														// The compiler doesn't use the alias when debugging, or complaining about something. (yah it's confusing).
														// Like, the compiler will say something like "struct _MONEY not referenced" or "struct _MONEY incompatible types".
														// Think of structures like a tool box.  All the variables that are in the structure, like the "Dollar" and "Cents" are tools.
														// Would you want to carry all of your tools in your hands rather than in a nice tool box?  Or course not.  Same concept.
														// We use structures if we are going to be passing around a lot of variables.  In this program, we wouldn't really
														// use a structure because we don't really do much with the change, but I wanted to make a simple program that uses
														// Structures so you could see how to access them, and the syntax.  Most people only use structures if they are going
														// To be passing the information into a function, or a lot of functions.
														// So, here we define an alias of the structure "_MONEY".  The "_" in front of "MONEY" doesn't need to be there, 
														// But most people do it that way.  It just needs to be different from the alias, "MONEY", at the bottom of the typedef.
typedef struct _MONEY {									// Create a structure called "_MONEY"
	int		Dollars;									// We want a variable to hold the Dollars, so we choose an integer.   (ex, 1, 2 333, 1200000) Whole number
	double	Cents;										// Then we want a variable to hold the Cents, so we choose a double.  (ex, .00003, .02, etc)  Floating point number (decimal number)
} MONEY;												// Here is our alias, "MONEY".  We now have a variable of our own to use. "MONEY" now means "struct _MONEY"

														// I create two of the same types of structures to show the difference.

struct TEMP {											// Here is just a regular structure, without the typedef.
	int		Dollars;									// We want a variable to hold the Dollars, so we choose an integer.   (ex, 1, 2 333, 1200000) Whole number
	double	Cents;										// Then we want a variable to hold the Cents, so we choose a double.  (ex, .00003, .02, etc)  Floating point number (decimal number)
};														// Since there is no alias, like in a typedef structure, we just put a semicolon at the end to show we are done, and we don't need an alias.

void main()
{
	MONEY Change;										// Now we create a variable of type "MONEY".  We will call it "Change".
	struct TEMP Change2;								// See the difference?  If we don't use a typedef, we have to put "struct" in front of the variable.
														// That is why most people use typedefs, they just look cleaner, that's the difference.

	Change.Cents = 0.02;								// We give our first structure 2 cents.  We put a "." after the structure name to access the variables inside.
	Change.Dollars = 2;									// Let's give the first structure 2 dollars.  If you have visual studio 6 or so, if you type a period after a structure,
														// It will bring up a box that shows you all the variables inside the structure.  This is called "Intellisense".  It's a lifesaver.

														// Now the first structure holds 2 dollars and 2 cents right?

	Change2.Cents = 0.04;								// Give 4 cents to the second structure.  Remember, we use "." to access the variables inside.
	Change2.Dollars = 4;								// Let's give the second structure 4 dollars.
														
														// Print out the 2 structures to show their contents.														
	printf("The change comes out to %d dollars and %.2f cents.\n", Change.Dollars, Change.Cents);
	printf("The change comes out to %d dollars and %.2f cents.\n", Change2.Dollars, Change2.Cents);

														// You'll notice I use a ".2" in "%.2f"  ...  The .2 makes it so it doesn't print out like this: "0.02000000"
														// It restricts the decimals to 2, or what ever number you want.  Take it out and then run and compile the program.
														// See what it does without the ".2", If you don't remember, %f is for float/double variables.
														// It prints out what variable is associated with it, like %d (integer) is associated with Change.Dollars, and
														// %.2f is associated with Change.Cents, and the same for the second structure.  If %d is first, then it's associated with
														// The first variable passed into printf().  Quick reference:  %d is for integers, %f is for decimal numbers, and %s is for strings.
														// There is more, like binary and hexadecimal representations, but I have never used them except for assignments in school.
}

// QUICK OVERVIEW

// By now you know how to create and access structures right?
// BUT, you might still be confused when to use them.  You will understand
// When your programs get big and you don't want to use a thousand global variables.

// An example: if we have a player structure, we would want to keep track of their:
// Health, Mana, Armor, Weapons, Inventory, Stats of any kind... etc..

// Well, what if we want to pass all that stuff into a function?  If we don't have a structure
// It would look like this:

// void InitPlayer(int Health, int Mana, ARMOR Armor, WEAPON Weapons, INV Inv, etc..);

// Now who wants to type all that in every time?  You would have to remember what all the variable names were...

// If we use a structure, say a PLAYER structure like this:

// typedef struct _PLAYER {
// {
//		int Health;
//		int Mana;
//		ARMOR Armor
//		WEAPON Weapon;
//		INV Inv;
//		etc...;
// } PLAYER;

// We could just say:

// void InitPlayer(PLAYER Player);

// Now we just need to pass in the player structure that we would create at the beginning of the game.
// PLAYER Player;

// Also, a structure can hold a structure, like ARMOR, WEAPON, INV, etc...  It can be infinite.

// Of course, we probably would make the first parameter a pointer if we wanted to change the "Player" variable, 
// but don't worry about that now.  Hopefully you have a better understanding of structures and their uses.

// © 2000-2005 GameTutorials
