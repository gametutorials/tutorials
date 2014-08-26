/*	Done by TheTutor

	Okay memory allocation -- This is something you'll do time and time again while
	programming.

	Every time your vid-console of choice says "Loading..." it's first freeing up all the
	memory (RAM) that was previously being used.  Then it's allocating enough space to store
	all the textures,stats,scores,etc that will be used in the next level/scene.

	If you had to load EVERYTHING into memory at once, you'd need like 20 gigs of RAM to play
	a half-way decent game :)

	So for this example we're going to do something simple.

	We're going to ask for a number.  Then we are going to allocate that many ints.
	Once we have our "allocated array of ints", we're going to fill them with 1 through (whatever was entered).
	Lastly, we'll display this "list of allocated ints" to the screen and then
	free up the memory.

	Ahhhh.... I hope that wasn't too techie :)

*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h> // defines EXIT_FAILURE and EXIT_SUCCESS
#include <malloc.h>

int main()
{
	int *numList = NULL; // A pointer where we're going to assign the memory that we create

	int maxNum = 0; // This is to be filled by the "user".  It will tell us how many ints
				    // we need to allocate memory for.

	int count = 0; // This will be used as a "counter"


	// Prompt 'em for a number
	printf("Enter the number of allocations: ");

	scanf("%d",&maxNum); // Read in max number of allocations storing it in "maxNum"

	// MAKE THE MEMORY
	numList = (int*)malloc(maxNum * sizeof(int));

			/* Okay piece by piece:
			
			  The (int*) is a typecast.  This says the type of memory we are allocating is
			  of type int.  malloc() (memory allocation) is the function you will use to 
			  create memory. "maxNum" is the number of ints we want to create.  So for instance,
			  if you wanted to dynamically make an array of 10 ints you'd go:
			  array_of_ints = (int*)malloc(10 * sizeof(int));

			  Now what the sizeof() function is doing is returning the number of bytes that makes
			  up what ever the sizeof() function is passed.  So for example, on Intel 32-Bit processors
			  (you can think of this as Win95 and up) ints are 4 bytes (32 bits) in size.
			  So sizeof(int) = 4.
			  
				Make sense?

				What malloc() is doing is creating the "exact number of bytes" you request
				and then "giving" that memory to whatever you assign it to.  When you malloc()
				memory nothing else (internal or external to your program) can touch it until
				you free it.

			  On with the program :)
			  
			*/

	// It's always good to check if malloc() failed -- On failure, malloc() returns NULL
	if(numList == NULL)
	{
		printf("\nmalloc() failed\n");
			return EXIT_FAILURE;
	}

	// Okay, if we get here malloc() worked!
	// Let's fill our newly created memory
	for(count = 0; count < maxNum; count++)
		numList[count] = count + 1; // The first element in the array will be filled with '1'.
									 // The second element in the array will be filled with '2'.
									 // If you are not sure what an array is, you should check out
									 // the array tutorial :)

	printf("\n\n"); // Two quick blank lines
	
	// Okay, now print 'em to the screen -- No formatting so it could be UGLY 
	for(count = 0; count < maxNum; count++)
		printf("%d ",numList[count]);

	// Last, but certainly not least, we must free the memory
	free(numList);
	numList = NULL; // It's a good idea to set the pointer you used to store the memory
					 // to NULL after you are done with it.  Many times a functions will
					 // check to see if a pointer equals NULL to determine if it has "valid
					 // memory associated with it", but if you just free() the pointer it
					 // will point to "an address in memory" but there WON'T be any "valid
					 // memory" associated with it.

	printf("\n\n"); // Two quick blank lines (cause I felt like it)

	return EXIT_SUCCESS; // Program terminated correctly


} // end of main()

/* BE FOREWARNED:

	You are going to have errors when dealing with dynamic memory.  I'd say
	dynamic memory (memory allocated while the program is running) and memory corruption
	(over stepping an array bounds for example)	are the	most frequent places
	bugs/errors occur in programs.

	So don't get too frustrated :)

	If have questions, post them at www.GameTutorials.com.

*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
