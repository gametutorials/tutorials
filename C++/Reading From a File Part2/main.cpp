// Done by TheTutor

/*  If case you haven't noticed by now, there are lots of ways to 
	do the "same thing" when programming.  This tutorial is going to 
	go over "reading from a file" once again.

	So, to read in from a file (our file will be a .txt file) there's 
	a few basic steps:

	1)  Create an ifstream (input file stream).  You use an ifstream to read in data from a file.

    2)  Open the file.  Use the "ifstream" to open the file thus allowing the possibility of
		reading data from it.

	3)  Read the data.

	4)  Close the ifstream.  When we don't need the "ifstream" anymore, be sure we close it
		and free up the memory associated with it.

	So with that, let's get reading in some files...
*/

#include <iostream> // Include this so we can use "cout"
#include <string>   // Include this so we can declare and use a "string" variable
#include <fstream>  // We include this header file so we can declare and use a variable of
				    // type "ifstream".  You will need to #include <fstream> when you want
				    // to read or write to a file.

using namespace std; // We're using the "standard namespace"

// Our main function (i.e. the CORE of our program)
int main()
{
	ifstream fileIn("GT.txt");  // So what is this doing?  Well this is combining steps
								// 1 and 2 from above.  First we are creating a variable
							   // of type "ifstream" (we named it "fileIn")  Then at the 
							  // same time (using a constructor which you'll get to when
							 // you get to classes :) we open the file we want our "ifstream"
						    // to read from.  The file is named "GT.txt" (it's a text file).

	// Error Check
	if(!fileIn)
		return 1; // If we could NOT open the file (for whatever reason) we quit the
				 // program immediately.  Notice the syntax:  !fileIn
				// This returns TRUE if fileIn doesn't have a valid file 
			   // associated with it.

	// If we get here, we've opened "GT.txt" and we're ready to read from it

	string strBuff; // Here we're creating the string "strBuff".  We will use "strBuff"
				   // (meant to be short for "string buffer") to read in from the file we 
				  // just opened each word (such as, "hippopotamus") of the file until we 
				 // reach the end of the file.

	// Using the while loop, we're going to do step 3 from above.
	// This says "while we have NOT reached the end of the file, keep looping".
	while(!fileIn.eof())
	{
		fileIn >> strBuff; // Here we read in a word
		cout << strBuff; // Now we're simply regurgitating the word we read in to the screen

		cout << " "; // **NOTE** We have to put a space between the words.  When we 
					// create an ifstream and read in a "string" the way we are, all white 
				   // space will get skipped over.  So for example, say our text file looked
				  // like this:

				// My name is                  TheTutor

			    // After the first file is read, strBuff will equal "My"
			    // After the second file is read, strBuff will equal "name"
			    // After the third file is read, strBuff will equal "is"
			    // After the fourth file is read, strBuff will equal "TheTutor"
			   
			    // Notice that all the white space (spaces or tabs) are skipped.
	}

	fileIn.close(); // Lastly, we do step 4 from above and close the file (we're done with it)
		return 0;
}

// Are you reading into this...

/*
	File reading can be a tad tricky.  Especially when you get into parsing specific data or 
	custom file formats.  This tutorial should give you a good first step into understanding
	how to read in a file

	Questions, comments, suggestions, or other useful banter should be proudly posted on 
	the forums at www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
