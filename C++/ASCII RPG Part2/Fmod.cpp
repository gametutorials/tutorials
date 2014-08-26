//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 2)								 //
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

	// Now that we know we have the right DLL, we need to initialize the FMod Sound System.
	// We want our mix rate to be to be 44100mhz (Good CD Quality), and we pass in 32 for
	// the max software channels, then FSOUND_INIT_GLOBALFOCUS to show we want it to play
	// no matter what window is in focus.
	
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

	// Everything should be up and running with the sound system, so let's load a song.
	// To load a song we just pass in the file name to FMUSIC_LoadSong().
	// This returns a FMUSIC_MODULE pointer.  We use this pointer to query and play the song.

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

	// We should have a pointer to a module that will allow us to play the song.
	// All we need to do is pass in the module to FMUSIC_PlaySong().

	// Play our song that we loaded
	FMUSIC_PlaySong(m_pModule);
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
// We added a new function to our CFmod class, PlayNewSong().  This frees up
// the memory with the current playing song and starts a new one.
//
// All the credit of this Fmod Library goes to "FireLight Multimedia" because 
// they made they library, I just made a neat client.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
// © 2000-2005 GameTutorials
//
