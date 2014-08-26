// Done by TheTutor

/*	Okay this tutorial is all about searching for a file (specifically a text file)
	The user will have to specify the directory they want to search in AND the name
	of the text file (must end with .txt) -- If we find the .txt file specified
	we'll print it out the screen, otherwise the program crashes horribly,
	(just kidding) it prints out "File specified does not exist."

	So without further ado...
*/

#include <io.h> // This contains the struct _finddata_t
#include <direct.h> // This contains the function _chdir()
#include <fstream> // We include this so we can use "ifstream"
#include <iostream> // We include this so we can use "cout" and "cin"
#include <string> // We include this so we can use "string"

using namespace std; // We'll be using the "standard namespace"

// This function will print the contents of "fileName" (this will be the full path name)
void PrintFile(const char *fileName);

int main()
{
	string fileName; // This will hold our file name
	string dirPath; // This holds the directory path
	
	// Prompt user for a directory
	cout << "Please enter the directory you'd like to search in: ";

	// Read in the directory
	cin >> dirPath;

		// Error Check -- If we can't change to the directory the user typed in
		//				  it's an invalid directory, no need to go on
		if(_chdir(dirPath.c_str()) == -1)
		{
			/** Side Note:  _chdir() changes the current working directory to 
						    the directory passed in (if possible) returns -1 on error
							(meaning couldn't change to specified directory) **/

			cout << "Directory that was entered is invalid.  Program terminating!"
				 << endl;

			return EXIT_FAILURE; // Mission NOT accomplished
		}

	// Prompt user for a file name
	cout << endl << "Type in the text file (.txt) you want to open." << endl
		 << "You must include the \".txt\":  ";

	// Read in file name
	cin >> fileName;

		/** Error check -- Make sure the file ended with .txt **/
		
		// If the file name is ONLY 4 characters long or less, it obviously
		// can't be a file name ending it ".txt"
		if(fileName.length() < 5)
		{
			/** Side note:  string::length() returns the length of the string, if we had 
						    a string h that equaled "Hello", h.length() would equal 5 **/						    

			cout << endl << "The file was supposed to end with \".txt\"" << endl
				 << "You didn't follow directions.  Program terminating!" << endl;

			return EXIT_FAILURE; // Mission NOT accomplished
		}
		
		// Next if the string DOES NOT end in ".txt" they typed in an illegal file name

		// I'll break down the arguments to the string::compare() method
		// fileName.length() - 4 -- This is the position in the string you want to begin
		//							 to compare from, fileName.length() will return the 
		//							 length of the string then we're subtracting 4 from that.
		// 4 -- Number of characters we want to compare
		// ".txt" -- The characters we want to compare to
		// If this function succeeds (i.e. the two "strings" are identical) the return value is zero
		// otherwise it's some number (either positive or negative depending on which string was "greater")
		if(fileName.compare(fileName.length() - 4, 4, ".txt"))
		{
			cout << endl << "The file was supposed to end with \".txt\"" << endl
				 << "You didn't follow directions.  Program terminating!" << endl;

			return EXIT_FAILURE; // Mission NOT accomplished
		}

	// Okay if we get here they typed in a legal file name.  First thing we'll do is
	// make sure that dirPath's last character is the '\'
		
		/** Side Note:  strlen() returns the length (number of characters in a char array)
						NOT including the NULL terminating character **/

	if(dirPath[dirPath.length() - 1] != '\\')
		dirPath[dirPath.length()] = '\\';  // We're not actually appending two '\\' characters
							              // Just like when you print out a '\' you need 
										  // two together to "mean" just one '\'.

	// We will use this structure to locate the file the user specified
	_finddata_t fileData = {0};

	// Append the file name to dirPath
	dirPath.append(fileName);

	// _findfirst() fills "fileData" with information about the file "dirPath" if 
	// that file indeed exists.  If it DOES NOT exist the return value will be -1.
	if(_findfirst(dirPath.c_str(),&fileData) != -1)
	{
		/** Side Note:  Now in this program we don't use the "fileData" at all, but 
						the information it gets filled (assuming the call to _findfirst() 
						succeeds) is stuff like:  file attributes, size of file in bytes, etc **/

		// Print the file to the screen
		PrintFile(dirPath.c_str());

		_fcloseall(); // Close all streams that may have been opened (specifically by the
					 // call to _findfirst() ).
	}
	else
		cout << "File specified does not exists." << endl << endl;


	return EXIT_SUCCESS; // We're done!

} // end of main()

// This prints the specified file to the screen
void PrintFile(const char *fileName)
{
	ifstream fileIn;

	// Open the file
	fileIn.open(fileName);

		// Error Check -- Always want to make sure you REALLY opened the file :)
		if(!fileIn)
		{
			cout << "Something really bad happened" << endl;
				return;
		}

	// While we HAVE NOT reached the end of the file
	while(fileIn.eof() == false)
	{
		char a_char = {0}; // This represents one character

		// We get a character from the file
		fileIn.get(a_char);

		// If it's the "return character" -- We'll return
		if(a_char == '\n')
			cout << endl;
		else
			cout << a_char; // Otherwise we just print it to the screen
	}

	fileIn.close(); // Close the file

} // end of void PrintFile(const char *fileName)

// **CAUTION**
// cin >> dirPath and cin >> fileName will stop reading in characters as soon as 
// they find a white space (" ").  So if you have directories or file names with 
// spaces in them, this tutorial won't work as you expect.

// If you need more help, head to the message board at www.GameTutorials.com

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
