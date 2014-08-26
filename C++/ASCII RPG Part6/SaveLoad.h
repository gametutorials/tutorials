#ifndef _SAVELOAD_H
#define _SAVELOAD_H


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is our save game file extension
const char kSaveExtension[] = ".sav";

// These are our prompt dialog box size and position
const int kLoadPromptX = 15;
const int kLoadPromptY = 10;
const int kLoadPromptWidth = 50;
const int kLoadPromptHeight = 4;


// This is our class for saving and loading game files
class CSaveLoad
{
public:

	// This loads a game file
	bool Load(char *szFile);

	// This saves a game file
	bool Save(char *szFile);

	// This prompts the user to either enter a file to save or load, depending on bIsLoading
	void PromptForFile(bool bIsLoading);
};

#endif

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// This file holds the defines and class for loading and saving
// game files.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
