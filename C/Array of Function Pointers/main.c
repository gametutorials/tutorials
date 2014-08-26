// Done by TheTutor

/*
	This tutorial is all about arrays of functions pointers.  When you can tell people
	you know these, they'll	falsely think you're a genius :)
	
	Really there's nothing tricky or hard about arrays of function pointers what-so-ever.
	So to show you how to "use them" this tutorial is going to create an array of function 
	pointers that display various "menus" in our "pretend game".  Yeah you're going to 
	have to pretend this was a part of a much larger project (like a game) to get the
	full effect :)

	So in menu.h we define our "menu displaying functions" and a VERY SIMPLE player struct
	just so we have something to fill in and play with.

	Then we create our array of function pointers below and use them in main()
	The main() certainly doesn't do anything extravagant it just prints out our various
	"menus".  Now you can't even USE the menus, it JUST displays them.

	So without further ado...
*/

// In Visual Studio 2005 Microsoft added safe versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions
#define _CRT_SECURE_NO_DEPRECATE

#include "menu.h"
#include <malloc.h> 
#include <string.h> // For strcpy()

// Here we are creating our array of function pointers
void (*Menus[])(SPlayer *player) = 
{
	SaveMenu,
	LoadMenu,
	OptionsMenu
};

/*	Okay here we're going to break down the syntax of our above declaration 

	void (*Menus[])(SPlayer *player) =
	  |
	  |_____ The return type FOR ALL functions in the "array of function pointers".


	void (*Menus[])(SPlayer *player) =
			 |
			 |____ The '*' says this is a "function pointer".
				   "Menus" is the arbitrary name of the function (array of functions)
				   that we decided to use.
				   The '[]' says this is "an array of function pointers".  Since we DID 
				   NOT specify a size we could put AS MANY functions we wanted inside the
				   body of this declaration (they'd have to have the same "signature" as the
				   functions already in there).


  void (*Menus[])(SPlayer *player) =
						|
						|____ This is the parameter list common TO ALL functions in the
							  function pointer array.


  Then notice the '=' (equal sign).  This is needed to say "okay we're going to start
  listing the functions to be stored in the array..."

  Now notice how all the functions in the array are between {}'s with the '}' being followed
  by a semi-colon.

  {
	SaveMenu,		// First function in array (So it resides at Menus[0])
	LoadMenu,		// Second function in array (So it resides at Menus[1])
	OptionsMenu  	// Third function in array (So it resides at Menus[2])
  };
  
*/


// Okay now let's utilize our array of function pointers so we can see I'm not just blowing
// a bunch of smoke up your behind :)
int main()
{
	// First we'll make a dummy player
	SPlayer *player = NULL; // player 1

	player = malloc(sizeof(SPlayer)); // Allocate the SPlayer

		// Error Check
		if(!player)
			return 1; // Couldn't allocate memory

	player->level = 50; // He's at level 50 
	strcpy(player->name,"Edge"); // Set the player's name

	// Call the first function in our array of function pointers (i.e. call SaveMenu())
	(*Menus[0])(player); // Note the syntax for calling the function:
						 // (*Menus[0]) is the function name with '0' (zero) corresponding
						// to which function in the array is to be executed.
					   // &player is the argument the function takes.
					  // So that's how you call functions contained in an array of 
					 // function pointers.

	printf("\n\n"); // Skip 2 lines

	// Call the second function in our array of function pointers (i.e. call LoadMenu())
	(*Menus[1])(player);

	printf("\n\n"); // Skip 2 lines

	// Call the third function in our array of function pointers (i.e. call OptionsMenu())
	(*Menus[2])(player);

	printf("\n\n"); // Skip 2 lines

	free(player); // Free up the player

	return 0; // And we're done :)
}

// One last pointer...

/*
	Function pointers can be very useful, but shouldn't be used "just because".
	This "menu example" is actually a practical use of function pointers.  Each menu could
	have an ID associated with it (it's position in the array) and then instead of having to do
	something like this:

	switch(ID)
	{
		case 0:

			MenuFunc_1();
				break;

		case 1:

			MenuFunc_2();
				break;

		// etc...
	}

	You could just do something like this:

	(*Menus[ID])();

	As usual any questions or comments can be posted at www.GameTutorials.com.

*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
