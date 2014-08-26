/* Done by TheTutor

	All right, what this tutorial is going to focus on is "pointers to pointers", also
	called "double pointers".  Double pointers are really not too difficult of a concept,
	but the code can look intimidating.

	A pointer remember, is nothing more than "a variable which represents a specific address
	in memory".  So a double pointer would be "a variable which represents a specific address
	in memory that represents another address in memory".

	Of course this chain of "pointers" can continue.  You could have "a pointer to a 
	pointer to a pointer" if you wanted, but for the most part "double pointers" is as 
	"complex" as it gets :)

	To demonstrate a use of pointers to pointers, we're going to make a 2-Dimensional array
	at runtime.  This program will prompt you for the dimensions, create the 2D array, fill
	it with numbers, and print it to the screen.
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h> // So we can use printf(), scanf()
#include <stdlib.h> // Defines EXIT_FAILURE and EXIT_SUCCESS

int main()
{
	// We're creating a 2D array that is going to represent a "Matrix",
	// so the first thing to do is ask how many rows and how many columns are going to
	// be in the matrix.
	int row = 0; // Number of rows
	int col = 0; // Number of columns

	int i, j; // "Counter variables" for our for loops

	int **matrix = NULL; // The variable that will hold our matrix (pointer to a pointer)

	// Get the number of rows
	printf("Enter the number of rows (at least one, less than 10) for the matrix: ");
	scanf("%d",&row);

	// Bounds check the number of rows
	if((row <= 0) || (row >= 10))
	{
		printf("You don't follow instructions to well\n");
			return EXIT_FAILURE;
	}

	// Get the number of columns
	printf("Enter the number of columns (at least one, less than 10) for the matrix: ");
	scanf("%d",&col);

	// Bounds check the number of columns
	if((col <= 0) || (col >= 10))
	{
		printf("You don't follow instructions to well\n");
			return EXIT_FAILURE;
	}

	// Let's create the "matrix":
	// First what we do is create memory for "row" number of "int*" (a chunk of memory
	// that points to chunks of memory capable of holding ints).
	matrix = (int**)malloc(sizeof(int*) * row);

	// Now, for all the "chunks of memory capable of holding ints" we're going to 
	// allocate space for an array on ints "col" large.
	for(i = 0; i < row; i++)
		matrix[i] = (int*)malloc(sizeof(int) * col);

	// What we have is a "row by col" array of ints.

	// Now we're going to walk this matrix (accessing it just as if we would of 
	// created in like this: int matrix[row][col];), then fill it with numbers from 
	// 0 to (col - 1).
	for(i = 0; i < row; i++)
		for(j = 0; j < col; j++)
			matrix[i][j] = j;

	// Now print out the contents of the matrix to the screen (so it looks something 
	// like a matrix).
	for(i = 0; i < row; i++)
	{
		printf("\n");
		
		for(j = 0; j < col; j++)
			printf("%d",matrix[i][j]);
	}

	// Extra line for good looks :)
	printf("\n");


	// Now we must free the matrix.  First we're free all the "col" number of 
	// "integer (int) arrays" we allocated.  Notice how we loop for "row" times
	// because we made "row" number of "col" sized int arrays.
	for(i = 0; i < row; i++)
		free(matrix[i]);

	// Then we'll free the "array of int*" (array of int arrays) we allocated
	free(matrix);

	return EXIT_SUCCESS; // It was a good little program
}

// Pointers are VERY difficult subject to fully understand and digest.  Feel
// free to ask any questions you may have on malloc() or pointers at:  www.GameTutorials.com

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
