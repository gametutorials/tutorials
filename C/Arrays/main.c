// Done by TheTutor

/*	What is an array???  An array can be thought of a collection of variables
	into one variable. For example:

	We could represent the name Sue like this:

	char letter1 = 'S';
	char letter2 = 'u';
	char letter3 = 'e';

	Then by printing out each letter we'd have "Sue".
	
	printf("%c%c%c\n",letter1,letter2,letter3);

	But boy would that suck!

	So instead we can use a character array:

	char name2[4] = "Sue";

	printf("%s\n",name2);

	////////////////////////////////////////////

	Let's break down name --

	name[0] = 'S'
	name[1] = 'u'
	name[2] = 'e'
	name[3] = NULL		Every char array MUST end with a NULL character .
						Not sure what a NULL character is???  Simply think of it as
						a way of saying "This character sequence stops here".

	Every "chunk" of an array is called an element of the array.  "name[0]" is an element of
	array "name".  "name[1]" is an element of name and "name[2]" is an element of array name.

	Notice how the first element of name has an index of 0.  An index is nothing more than a
	position in an array.  For example:

	name[] = "Carlos"		By not specifying a number of elements, this declaration will make
							name the appropriate length.  One element for each letter and a
							NULL terminating character.  Thus the total length of name is 7
							elements.  We'll prove that in the coding part.

	'r' is at index 2 of array name.  Remember an array's first index is always zero.

	Okay that, for the most part, is what an array is.  Now lets do some coding.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // Contains the function definition of strlen()

#define SHOW_ANSWERS 1 // To see the "answers" (i.e. what's printed to the screen),
					  // simply change this to 1.  However, I HIGHLY suggest looking at 
					 // the program and writing down on paper what you think will be printed
				    // out first, it's a great way to learn :)

int main()
{
	char name[] = "Carlos"; // arrays don't have to be of just type char.  They can be 
							// int, double, user defined types, etc.

	int numbers[] = {0,1,2,3,4,5,6,7,8,9};  // int, double, and all other data types are 
											// different from char in that they DO NOT
											// have a NULL terminating character.  Notice
											// the slightly different declaration.

	// First let's prove that the length of name is 7 elements
	// The strlen() function returns the length of a char string.
	printf("name[] length = %d\n\n\n",strlen(name));

	// If you ran this program you noticed it printed out 6. 
	// BUT I THOUGHT YOU SAID THE LENGTH OF name WAS 7 ELEMENTS?!?!?!

	// Change the above code so the first line reads like this:
	// char name[6] = "Carlos";

		/* Carlos has 6 letters in it so this should work right???
		   You'll find that leads to undefined results, it could not compile or it
		   could say the strlen of "Carlos" is 62 (or some other number, etc.).
		   It's totally OS dependent.  The reason it flips out is that by changing it
		   to char name[6] = "Carlos", there are only 6 elements in the array.
		   There is no room for the NULL terminating character that MUST always be there
		   for char arrays.  So take a mental note:  strlen() returns the length of
		   a string (char array) minus the NULL terminating character.
		*/

	 
	//	Remember set SHOW_ANSWERS to 1 (up above) to see what's get printed out
	//	but you should do it on paper first :)

	if(SHOW_ANSWERS)
	{
		printf("%s\n",name);
		printf("%c\n\n",name[3]);
	}

	name[3] = name[2];

	if(SHOW_ANSWERS)
		printf("%s\n\n",name);

	name[5] = 't';

	if(SHOW_ANSWERS)
		printf("%s\n\n",name);

	name[3] = '\0'; // '\0' is the equivalent to NULL

	if(SHOW_ANSWERS)
		printf("%s\n\n",name);

	// Okay now let's print out a value from the int array
	printf("numbers[2] = %d\n",numbers[2]);
	
	if(SHOW_ANSWERS)
		printf("%d\n",numbers); // What do you think this does???
								// Arrays (On a low-level computer basis) are sequential
								// chunks of memory of certain lengths.  This statement will
								// print out the address in memory where the first element of 
								// number resides.

	return EXIT_SUCCESS; // EXIT_SUCCESS is a predefined number that tells the Operating System
						 // this program did not encounter any problems while running.
						 // **NOTE** It's defined to be 0 so you may see "return 0;" in it's place.

}

// Well I hope this somewhat clears up what is meant by an array.
// If your still having trouble, feel free to post a message at www.GameTutorials.com.

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
