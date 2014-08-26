//**********************************************************************************//
//																					//
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -					//
//																					//
//		$Author:		Ben Humphrey	digiben@gametutorials.com					//
//																					//
//		$Program:		ReadingFiles											    //
//																					//
//		$Description:	This gives several ways to read from a file				    //
//																					//
//**********************************************************************************//

#include <iostream>										// Include our standard header
#include <string>										// Include this to use strings
#include <fstream>										// Include this to use file streams
using namespace std;									// Set our namespace to standard

// To practice reading data from files we will read from a game character's player file.
// If you are making an application that holds students' information, or a game that
// holds player information, this tutorial is necessary to know how to access that data.
// If you are a database programmer you wouldn't read data like this, but it's a good
// start before we jump into more difficult subjects.  The file we will read in this
// tutorial is stats.txt, which stores the player's name, health and gold data.

int main()												// A standard and plain main (hey! that rhymed!) :)
{														// The start of our program
	ifstream fin;										// Here we create an ifstream object and we will call it "fin", like "cin"
	string strLine = "";								// Lets create a string to hold a line of text from the file
	string strWord = "";								// This will hold a word of text
	string strName = "";								// This will hold the player's name
	int health=0, gold = 0;								// We will read in the player's health and gold from a file
														
	fin.open("Stats.txt");								// To open a file to read we call a function from our ifstream object called open().
														// An ifstream stands for "Input File Stream", meaning we are creating a stream of
														// data that we will be using to read data from an input file (our file we open).
														// A "stream" is just a technical way of saying a way to read and/or write data.
														// Think of all those martial arts classes you took, "Be like water, be like a stream..." :)
														// We just tell the function "open()" the file name we want to open.  It must be
														// in the same directory as the program if not specified (i.e., "c:\stats.txt")
														// The "." after "fin" allows us to access some functions within an object/class.
														// This is an important feature in OOP (Object Oriented Programming).  Instead of
														// Having functions scattered all over the place, we create objects (classes) that
														// store variables and functions associated with the object's functionality.  So in
														// a ifstream object we have functions for dealing with files like close(), open(), etc...
														// Later you will see how to create classes yourself.
													
														
	if(fin.fail())										// But before we start reading .. we need to check IF there is even a file there....
	{													// We do that by calling a function from "fin" called fail().  It tells us if the file was opened or not.
		cout << "ERROR: Could not find Stats.txt!\n";	// Now, Print out an error message.  This is very important, especially when getting into huge projects.
														// There are so many times where I have spent wasted time trying to find out why my program was crashing.
		return 1;										// Return is like a 'break' but it's for functions, not loops.  You notice our main function, "int main()" , returns an int.
	}													// This is reserved for error codes from the program.  If there are zero errors, than you return a zero to signify "no errors".
														// Operating systems, such as Windows has certain codes you could return to tell the OS the error that occured in your program.  For
														// our needs, we'll just return 1 to mean "we encountered at least one problem."
														
	cout << endl;										// Just to space things out when we go down one line

														// Now we should start reading from the file since we found the file and opened it.
														// Here we just do a while loop to get every word in the file.
	while(fin >> strWord)								// "fin" works just like "cin", it will read 1 word at a time.  The loop will continue until we reach the end of the file.
		cout << strWord;								// Here we print out the word "fin" just read in.  This results in printing the whole document to the screen (but not very neat...)
														// After it reads each word, it moves the file cursor/pointer to the next word until it gets to the end (EOF - End of File).

														// At the end of every file the computer puts a symbol to show it's the end of the file.
														// EOF is #define'd to be -1 -- But what is really important is that when you see EOF
														// you think End-Of-File.  When fin encounters that value, it returns 0 to the while loop, telling it to stop.

	cout << endl << endl;										// Just to space things out when we go to the next line
														
														// Once the input file stream reaches the end of the file, it set's a flag, EOF to TRUE.
	fin.clear();										// We clear the flag inside of "fin" that holds EOF.  If we don't, a boolean will tell us that we are still at the EOF.
	fin.seekg(NULL, ios::beg);							// We use this to go to the beginning of the file.  "ios" means input-output-stream.  "ios" is a class/object.  The "beg" is 
														// a constant value that means "beginning".  To access constant values in a class/object in C++, you use ::. Don't worry about it now.
														// Anyway, we use the seekg() function to set fin's file reading cursor position back to the beginning.  As we read through
														// a file, the ifstream object (fin) keeps track of where it is in the file.  We want to reset that position since
														// we want to start reading from the beginning of the file again to practice reading in files in a different way.
														
														// This is another way to read from a file.  getline() gets a whole line of text at a time.														
														// getline needs to pass in (the input file stream, and the string to store what it gets).
														// If we get to the end of the file, getline() returns a 0 to stop the while loop.
	while(getline(fin, strLine))						// This gets a line of the text file at a time and stores it in the string "strLine".
		cout << strLine << endl;						// Next we print out line by line as we get the line in the file.
	
	cout << endl;										// Just to space things out when we go to the next line
														// Let's do one last example on reading in data from files with ifstreams.  
														// This next way stores the data correctly for the player.
	fin.clear();										// We clear the flag inside of "fin" that holds EOF.
	fin.seekg(NULL, ios::beg);							// Set the file cursor position back to the beginning like we did before.
														// This is a tricky thing below..
														// Remember up above how we read in a word at a time and it moved the file pointer to the next word??
														// Well, below we are doing just that.  We read PAST the first word, then read in the next word.
	fin >> strWord >> strName;							// Here we read past the word "Player: ", then we read in the players name and it stores it in "strName".
	fin >> strWord >> health;							// Here we read past the word "Health: ", then we read in the players health and it stores it in "health".
	fin >> strWord >> gold;								// Here we read past the word "Gold: ", then we read in the players gold and it stores it in "gold".	
														// We can do this since we know the exact format of our player file.  We just skip the first word of each line.
														// Notice that the first word in the file includes the ":" character.  "fin" will read in characters until it hits a space.

	cout << "The Player's name is:   " << strName << endl;	// Here we print out the Player's name that we retrieved from the file.
	cout << "The Player's health is: " << health << endl;	// Now we print out the Health of the player that we got from the "Health: 50" part of the file.
	cout << "The Player's gold is:   " << gold << endl;		// Then we print out the amount of gold the player has, which we retrieved from the line "Gold: 20" in the file "stats.txt".

														// close() closes the file for us. (very important - don't forget this)
	fin.close();										// We must always close the file after we are done with it.  It frees memory and make's it so we can access it again.

	return 0;											// Return with zero problems
}														// End of our program

// Now that we know how to read in data from a file, we can then use
// that data in our program that we want to create.  For instance, you
// might want to read in all the students in a class and then check to see
// which students have a failing grade, then email them half way through the
// semester and let them know they better improve or fail the class.
// Or, perhaps you want to make a game which has a player.  If you have a save
// option in your game, you are going to want to store the player's data so that
// whenever you start the game again from the save game file, it will have the
// correct stats, position, level, current map, etc, correctly.

// © 2000-2005 GameTutorials
