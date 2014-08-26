//**********************************************************************************//
//																					//
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -					//
//																					//
//		$Author:		Ben Humphrey	digiben@gametutorials.com					//
//																					//
//		$Program:		WritingFiles											    //
//																					//
//		$Description:	This shows you how to write to a file					    //
//																					//
//**********************************************************************************//


// In this tutorial we show how to write data to a file.  This is very useful for saving
// data that needs to be loaded in.  For example, if we are making a game we need to
// save the player's data and such so we know the player's last position, health and
// level.  Also, when writing a text editor we need to save the text to a file.
// We show you in this tutorial how to save a player's name, health and gold to a file.
// Once you learn how to read in data you can then read this data back in and use it.
// Take a look below to read our tutorial on writing files.

#include <iostream>										// Include our standard header
#include <string>										// Include this to use strings
#include <fstream>										// Include this to use file streams
using namespace std;									// Set our namespace to standard;

int main()												// A standard and plain main (hey, still rhymes!) :)
{														// Start of our program
	ofstream fout;										// Here we create an ofstream and we will call it "fout", like "cout".
														// "ofstream" stands for "output file stream".  That means we are sending data to a file.
	string szLine ="";									// Lets create a string to hold a line of text from the file
	string szWord = "";									// This will hold a word of text
	string szName = "Adol";								// This holds the players name
	int health=100, gold = 75;							// We holds the players health and gold
														
	fout.open("Stats.txt");								// We call a function from our ofstream object called open().
														// We just tell the function open() the file name we want to open or create.
														// If the file doesn't exist, it will create it for us once we start writing to it.
														
														// Below, we use fout just like cout.  Instead of writing to the screen though, it writes to our open file.
	fout << "Player: " << szName << endl;				// This prints "Player: Adol" to our file
	fout << "Health: " << health << endl;				// This prints "Health: 100" to our file
	fout << "Gold: "   << gold   << endl;				// This prints "Gold: 75" to our file

														// close() closes the file for us. (very important)
	fout.close();										// We must always close the file after we are done with it.  It frees memory and make's it so we can access it again.
	return 0;											// Return with zero problems
}														// End of our program


//////////////////////////// Quick notes //////////////////////////
//
//	So, now that you know how to write things to a file,
//  Do you think that you could write a save- and load-game function?
//  
//  Of course, you would not want to hard code your data.  You would
//  most likely have the player enter in their name in the beginning,
//  then you save that off, and start with a default health and gold.
//  You should be able to take this and apply it to a simple save game
//  function.  Now that you know how to read in data and save data, you
//  can make a save game file, maybe called <playerName>.sav (IE. Adol.sav).
//  Once you save the data, it's simple to read in.  
//
//	As you can see, this is really simple.  If you know cout, you know ofstream.
//  I call my variable fout (file Output) because it is so much like cout (console Output).
//
//  © 2000-2005 GameTutorials
