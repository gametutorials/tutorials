// Done by TheTutor

/* 
	What is dynamic memory???  Dynamic Memory is "memory that is given to your program for it
	it to exclusively use at runtime".

	Let's say you wanted to read in a text file and store it in a char[] -- Well, you know 
	for every piece of text (letters, punctuation, spaces, etc.) in the text file you're 
	going to need an entry in your char[] -- It's impossible to know how big the text file is
	going to be unless you want to write a program for every text file that ever exists and 
	that text file can't change.  This is obviously nuts!

	So we'll decide at runtime how large the file is and make the char[] big enough at that
	time.  When we say "runtime", we mean "while running the program" or "on the fly".
*/

#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	// Creates a "file stream" to the text file "TheTextFile.txt"
	ifstream fileIn("TheTextFile.txt"); 

	// Error check -- If "TheTextFile.txt" doesn't exist (or isn't this directory)
	//				  fileIn will equal NULL -- So we check that fileIn DOES NOT equal NULL.
	if(!fileIn)
		return EXIT_FAILURE; // Tells the OS the program encountered an error

	int count = 0; // Let's go through the file and count all the characters in it
	char dummyChar; // A place to store a character from the file

	while(!fileIn.eof()) // Keep going until the we've reached the END-OF-THE-FILE
	{
		fileIn >> dummyChar; // Read in the file character by character
		count++; // We've read yet another character
	}

	// The \" Means <Print out the " symbol> -- Since normally the " is used to enclose text.
	// We have to do this special character sequence to actually print it out.
	cout << "There are " << count << " characters in \"TheTextFile.txt\"" << endl;

	// If you look at the text file you'll notice "This is the sample text file."
	// is made up of 24 letters.  So that means that the EOF character is included
	// in our count but NOT the "space" between words.

	// Well lets make a char[] (read char array) big enough to hold all the letters
	char *letters = new char[count];	/*	What we've just done is made an array
											of char that is of size "count" in
											our case 25 -- Which happens to be the perfect
											size because we'll use the slot set aside for
											the EOF character as the slot for the NULL
											terminator (remember those?) */
												
										/*	Notice the key word "new" -- This can be used
											to make an array of variables OR just one
											variable such as:
											char *one_char = new char; */

	// Now lets fill the array.  First lets move the file pointer back to the beginning of the file
	fileIn.clear();
	fileIn.seekg(NULL,ios::beg);

	// We reset count so that we can use it as an index into letters
	count = 0;

	while(!fileIn.eof())
	{
		fileIn >> letters[count];
		count++; // Move to next index
	}

	// Make sure that "letters" ends with a NULL character
	letters[count - 1] = NULL;		// Remember if we make an array like this:
									// char an_array[25], then the last index
									// into that array is 24.

	// Lets see what we did.  We pass in the letters array, which will display all the characters in it.
	cout << endl << endl << letters << endl;

	// Well it looks a little squished without the spaces but that wasn't the point!

	// Before we quit the program we must free the memory (well, if want to consider ourselves
	//													   good programmers, we must free the memory)
	delete[] letters;				/*  Here is the basic "notation" for freeing 
									    dynamically allocated memory:
										If it was created like this:  int *an_int = new int;
										to free it you call: delete an_int;
										But if an array of int was created like this:
										int *an_intArray = new int[22];
										To free all the memory associated with *an_intArray
										you have to call: delete[] an_intArray;
										If you just do delete an_intArray, you will only
										free the first member of an_intArray. */

	fileIn.close(); // Be sure to close the file we opened 
		return EXIT_SUCCESS; // Program was successful

} // end of main()

// The dynamics of it all...

/*
	There is no denying it, dynamic memory (or rather the misuse of dynamic memory) WILL cause you some
	grief in the form of nasty programming bugs.  So wield extra caution when creating/freeing dynamic 
	memory.

	Questions, comments, and scuttle-bug can be typed directly into the forums at www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/