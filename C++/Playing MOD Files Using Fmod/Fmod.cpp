//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Fmod Player										 //
//																		 //
//		$Description:	Demonstrates how to play cool MOD music files	 //
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
	m_strName.clear();

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


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This inits the sound system and loads the files specified.
/////	It should be called before any Fmod functions
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

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
	m_strName = strName;

	// Return a success!
	return true;
}


///////////////////////////////// PLAY SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This plays the song that we loaded into our Fmod system
/////
///////////////////////////////// PLAY SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

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


///////////////////////////// DISPLAY ERROR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will display an error message from the Fmod system
/////
///////////////////////////// DISPLAY ERROR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

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
// This tutorial shows how to load most types of MOD or MIDI files.  This is was
// an exciting tutorial to write because it does so much for you!  The Fmod library
// is perfect for games and demos.
// 
// * What Can I Play With This? * (MIDI, MOD)
// 
// You can play most, if not all MOD formats like .mod, .s3m, .xm, etc....  The file
// extension isn't important because the FMod library checks the actual file type.
// You can call it what ever you want.  This also plays MIDI files.  You don't need
// any new code, just change the file name to a MIDI and it will work!  Cool huh?!!?
//
// Once again, using mods for demos is cool because you can check the position in the
// song, then trigger a cool effect depending where it is.  For instance, you could
// say, if the order is 8 and the row is 36, Scale the screen, or start moving in.
// There is so much that can be done with the FMod Sound System.
//
// * Files Included *
//
// You will ABSOLUTELY need FMOD.DLL to run the application.
// For those of you who haven't worked with DLL's before, a DLL is a Dynamic
// Linked Library that access functions on the fly, in run time.  VB uses them
// for their applications.  It is different than a static link library (.lib)
// because the .LIB's code is compiled into the .EXE.  DLL's are great if you
// have multiple applications uses the same code and don't want to bloat your
// files.  You can either stick the DLL in your system directory or in the same
// directory as the application being run.  Once again, the DLL is a MUST have
// to run your application.  I might add also, that the fmod.h isn't the standard
// fmod.h that you get with the downloadable zip from www.fmod.org.  I added
// the fmod_errors.h file to the end of it because it was a small amount of code
// to leave in a header file.  It's better to just have 1 header file in this case.
//
// I love this library and I hope you will too!!
//
// I would like to thank the team at www.Fmod.org for the library and also the sample
// source code.  The code in this tutorial was primarily from the sample distributed
// in the FMod library.  I created the class from the underlying code.  I hope you
// find much use for it.
//
// I hope this helps you make wonderful applications!  All the credit goes to
// "FireLight Multimedia" because they made they library, I just made a neat client.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
// © 2000-2005 GameTutorials
//
