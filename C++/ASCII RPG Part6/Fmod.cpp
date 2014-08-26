//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 6)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include <iostream>								// Include the standard input output stream
#include "CFmod.h"								// Include our header file for Fmod prototypes
using namespace std;							// Include the standard namespace


///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor.  It just sets everything to NULL for starters
/////
///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CFmod::CFmod()
{
	// Clear the file name
	memset(m_strName, 0, sizeof(char));

	// Set the current song module to NULL
	m_pModule = NULL;
}


///////////////////////////////// ~CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will clean up memory and close the sound system
/////
///////////////////////////////// ~CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CFmod::~CFmod()
{
	// Free the module that loaded our song if it's valid
	FreeSoundModule();

	// Now free the whole sound system
	FreeSoundSystem();
}


///////////////////////////////// INITIALIZE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This inits the sound system and loads the files specified.
/////	It should be called before any Fmod functions
/////
///////////////////////////////// INITIALIZE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFmod::Init(char *strName)
{
	// ==========================================================================================
	// CHECK DLL VERSION
	// ==========================================================================================

	// First we want to make sure the DLL that we are using is the correct one we need.
	// Below we check if the one we are using is less than the one we need.

	// Make sure we are using the correct DLL version
	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		// Display the error message that shows we need the correct version
		cout << "Error : You are using the wrong DLL version!  You should be using FMOD " << FMOD_VERSION << endl;
			return false;
	}

	// ==========================================================================================
	// INIT SOUND SYSTEM
	// ==========================================================================================

	// Initialize our FMod Sound System with good quality sound and a global focus
	if (!FSOUND_Init(44100, 32, FSOUND_INIT_GLOBALFOCUS))
	{
		// Here we call our error and quit function since we failed to initialize the sound
		DisplayError();
			return false;
	}

	// Return the result of trying to load the song
	return LoadSong(strName);
}


///////////////////////////////// LOAD SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a file name (a midi or mod file) and loads it into the m_pModule
/////
///////////////////////////////// LOAD SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFmod::LoadSong(char *strName)
{
	// ==========================================================================================
	// LOAD SONG
	// ==========================================================================================

	// Load the desired song and store the pointer to that module.
	m_pModule = FMUSIC_LoadSong(strName);

	// Make sure we got a valid module pointer
	if (!m_pModule )
	{
		// Here we call our error and quit function since we couldn't get a valid pointer
		DisplayError();
			return false;
	}

	// Set the file name to the name passed in
	strcpy(m_strName, strName);

	// Return a success!
	return true;
}


///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will allow you to init the sound system and load a song right away
/////
///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

// This will play the current song stored in the m_pModule (Must be loaded first)
void CFmod::PlaySong()
{
	// ==========================================================================================
	// PLAY SONG
	// ==========================================================================================

	// Play our song that we loaded
	FMUSIC_PlaySong(m_pModule);
}


///////////////////////////////// PLAY NEW SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This plays a new song and frees the memory with the current playing song
/////
///////////////////////////////// PLAY NEW SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::PlayNewSong(char *szNewSong)
{
	// If we are playing a song, let's free it
	if(m_pModule)
		FMUSIC_FreeSong(m_pModule);

	// Load and play a new song
	LoadSong(szNewSong);
	PlaySong();
}


///////////////////////////// DISPLAY ERROR AND QUIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will display an error message and quit the program
/////
///////////////////////////// DISPLAY ERROR AND QUIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::DisplayError()
{
	// Print the error message to the screen
	cout << FMOD_ErrorString(FSOUND_GetError()) << endl;
}


///////////////////////////////// FREE SOUND MODULE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will free the memory stored in m_pModule (Song go bye bye)
/////
///////////////////////////////// FREE SOUND MODULE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::FreeSoundModule()
{
	// Free the current module (This doesn't free the sound system)
	FMUSIC_FreeSong(m_pModule);
}


///////////////////////////////// FREE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will close the FMod Sound System
/////
///////////////////////////////// FREE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::FreeSoundSystem()
{
	// Close and turn off the FMod Sound System.
	FSOUND_Close();
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Nothing new was added to this file.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
// © 2000-2005 GameTutorials
//
