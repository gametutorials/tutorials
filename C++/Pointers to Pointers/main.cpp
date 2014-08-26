/* Done by TheTutor

	All right, what this tutorial is going to focus on is "pointers to pointers", also
	called "double pointers".  Double pointers are really not too difficult of a concept,
	but the code can look intimidating.

	A pointer, remember, is nothing more than "a variable which represents a specific address
	in memory".  So a double pointer would be "a variable which represents a specific address
	in memory that represents another address in memory".

	Of course this chain of "pointers" can continue.  You could have "a pointer to a 
	pointer to a pointer" if you wanted, but for the most part "double pointers" is as
	complex as it gets :)

	To demonstrate a use of pointers to pointers, we're going to make a 2-Dimensional array
	at runtime.  This program will prompt you for the dimensions, create the 2D array, fill
	it with numbers, and print it to the screen

*/

#include <iostream>
using namespace std;

int main()
{
	// We're creating a 2D array that is going to represent a Matrix 
	// so first thing to do is ask how many rows and how many columns are going to
	// be in the matrix.
	int row = 0; // Number of rows
	int col = 0; // Number of columns

	// Get the number of rows
	cout << "Enter the number of rows (at least one, less than 10) for the matrix: ";
	cin >> row;

	// Bounds check the number of rows to make sure we don't go below or above our max/min
	if((row <= 0) || (row >= 10))
	{
		cout << "You don't follow instructions to well" << endl;
		return EXIT_FAILURE;
	}

	// Get the number of columns
	cout << "Enter the number of columns (at least one, less than 10) for the matrix: ";
	cin >> col;

	// Bounds check the number of columns
	if((col <= 0) || (col >= 10))
	{
		cout << "You don't follow instructions to well" << endl;
		return EXIT_FAILURE;
	}

	// Begin creation of the "matrix".
	// Here we're declaring a double pointer and allocating memory
	// for "row" number of "integer (int) pointers".  So when we're done, in memory we 
	// will have a chunk of memory that points to chunks of memory capable of holding ints.
	int **matrix = new int*[row];
	
	// Let say we entered 3 for row, in memory it will look something like this:
	/*
		[0] int*	We now have an array of three int* (pointers to int)
		[1] int*
		[2] int*
	*/

	// Now for all the "chunks of memory capable of holding ints" -- We're going to 
	// allocate space for an array on ints "col" large.
	for(int i = 0; i < row; i++)
		matrix[i] = new int[col];
		
	// Again, we'll assume that we entered 3 for column, in memory it will look
	// something like this:
	/*
		[0]->[0][1][2]	Each int* in our array from above, now points to 
		[1]->[0][1][2]	an array of three integers.
		[2]->[0][1][2]
	*/

	// So what we have is a "row by col" array of ints.

	// Now we're going to walk this matrix (accessing it just as if we would of 
	// created in like this: int matrix[row][col];) and fill it with numbers from 
	// 0 to (col - 1).
	for(int i = 0; i < row; i++)
		for(int j = 0; j < col; j++)
			matrix[i][j] = j;

	// Now print out the contents of the matrix to the screen (so it looks 
	// something like a matrix).
	for(int i = 0; i < row; i++)
	{
		cout << endl;
		
		for(int j = 0; j < col; j++)
			cout << matrix[i][j];
	}

	// Extra line for good looks :)
	cout << endl;


	// Now we must free the matrix.  First we'll free all the "col" number of 
	// "integer arrays" we allocated.  Notice the syntax for freeing 
	// multidimensional arrays.  matrix[] holds an array inside each index.
	// Remember that we delete arrays by putting [] after the delete keyword.
	// We need to do that for each index of the matrix multidimensional array.
	for(int i = 0; i < row; i++)
		delete[] matrix[i];

	// Then we'll free the "array of int*" we allocated
	delete[] matrix;

	return EXIT_SUCCESS; // It was a good little program
}

// What's the point of all this???

/*
	If you want to be a good C++ programmer, you have to fully grasp pointers.  It's as simple
	as that.  Pointers tend to be the most difficult C++ concept for people to grasp, so don't 
	get frustrated if you haven't quite got it yet.  Like anything, practice makes perfect.
	Feel free to ask any questions at the forums on www.GameTutorials.com.
*/ 

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
