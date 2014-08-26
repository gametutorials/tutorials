// Done by TheTutor

/*	What is an array???  An array can be thought of a collection of variables
	into one variable. For example:

	We could represent the name Sue like this

	char letter1 = 'S';
	char letter2 = 'u';
	char letter3 = 'e';

	Then by printing out each letter we'd have "Sue".

	cout << letter1 << letter2 << letter3 << endl;     SIDE NOTE:  endl is equivalent to "\n"

	But boy would that suck!  So instead we can use a character array.

	char name[4] = "Sue";

	cout << name << endl;

	////////////////////////////////////////////

	Let's break down name --

	name[0] = 'S'
	name[1] = 'u'
	name[2] = 'e'
	name[3] = NULL		Every char array MUST end with a NULL character. 
						Not sure what a NULL character is???  Simply think of as a way of
						saying "This character sequence stops here".  Also '\0' acts as 
						NULL for character arrays.  (name[3] = '\0';  - That works too).

	Every "chunk" of an array is called an element of the array.  name[0] is an element of
	array name, name[1] is an element of name and name[2] is an element of array name.

	Notice how the first element of name has an index of 0.  An index is nothing more that a
	position in an array.  Example:

	name[] = "Carlos"		By not specifying a number of elements, this declaration will make
							name the appropriate length.  One element for each letter and a
							NULL terminating character.  Thus the total length of name is 7
							elements.  We'll prove that in the coding part.

	'r' is at index 2 of array name -- Remember an array's first index is always zero.

	Okay -- that for the most part is what an array is now lets do some coding --

  */

#include <iostream>
using namespace std;

int main()
{
	char name[] = "Carlos"; // arrays don't have to be just chars -- they can be 
							// int, double, user defined types, etc.

	// First lets prove that the length of name is 7 elements
	// The strlen() function returns the length of a char string
	cout << "name[] length = " << strlen(name) << endl << endl << endl;

	// If you ran this program you noticed it printed out 6 -- 
	// BUT I THOUGHT YOU SAID THE LENGTH OF name WAS 7 ELEMENTS?!?!?!

	// Change the above code so the first line reads like this:
	// char name[6] = "Carlos";

		/* Carlos has 6 letters in it so this should work right???
		   You'll find that this won't even compile -- the reason is name only has 6
		   elements.  There is no room for the NULL terminating character that MUST always be
		   there for char arrays -- So take a mental note:  strlen() returns the length of a
		   string (char array) minus the NULL terminating character */

	/*	All right, below lets do a couple different examples with the character array 
		I suggest guessing the result of each operation, then uncommenting (remove the // from)
		the cout and see if you were right
	*/

	//cout << name << endl;
	//cout << name[2] << endl << endl;

	name[3] = name[2];

	//cout << name << endl << endl;

	name[5] = 't';

	//cout << name << endl << endl;

	name[3] = NULL;

	//cout << name << endl << endl;


	// Okay lets make an array of type int

	int numbers[] = {0,1,2,3,4,5,6,7,8,9};  // int, double, and all other data types are 
											// different from char in that they DO NOT
											// have a NULL terminating character -- Notice
											// the slightly different declaration.

	cout << "numbers[2] = " << numbers[2] << endl;
	
	//cout << numbers << endl; //-- What do you think this does???
							 // Arrays (On a low-level computer basis) are sequential
							 // chunks of memory of certain lengths -- This statement will
							 // print out the address in memory where the first element of 
							 // number resides.

	// Well I hope this somewhat clears up what is meant by an array --
	// If your still having trouble please visit the forums at www.GameTutorials.com.


	return EXIT_SUCCESS; // EXIT_SUCCESS is a predefined number that tells the Operating System
						 // that this program did not encounter any problems while running.

}

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
