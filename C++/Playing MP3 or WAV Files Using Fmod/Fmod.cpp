//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Fmod Player										 //
//																		 //
//		$Description:	Demonstrates how to play .wav or .mp3 music files//
//																		 //
//***********************************************************************//

#include <iostream>								// Include the standard input output stream
#include "CFmod.h"								// Include our header file for Fmod prototypes
using namespace std;							// Include the standard namespace


///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is if you want to create a global variable and not init the sound system
/////
///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CFmod::CFmod()
{
	// Clear the file name
	m_strName.clear();

	// Set the current song module to NULL
	m_pSound = NULL;
}


///////////////////////////////// ~CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will clean up memory and close the sound system
/////
///////////////////////////////// ~CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CFmod::~CFmod()
{
	// Free the sound pointer that loaded our song if it's valid
	FreeSound();

	// Now free the whole sound system
	FreeSoundSystem();
}


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This inits the sound system and should be called before any Fmod functions
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

	// Return the result from when we attempt to load the song
	return LoadSong(strName);
}


///////////////////////////////// LOAD SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a file name (a wav or mp3 file) and loads it into the m_pSound
/////
///////////////////////////////// LOAD SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFmod::LoadSong(char *strName)
{
	// ==========================================================================================
	// LOAD SONG
	// ==========================================================================================

	// If strName is invalid, can't load song
	if(!strName)
		return false;

	// Let the user know the song is being loaded
	cout << "Loading the song...\n\n";

	// Everything should be up and running with the sound system, so let's load a song.
	// To load a song we just pass in the file name to FSOUND_Sample_Load().
	// This returns a FSOUND_SAMPLE pointer.  We use this pointer to query and play the song.

	// Load the desired song and store the pointer to the sound.
	m_pSound = FSOUND_Sample_Load(FSOUND_FREE, strName, 0, 0);

	// Make sure we got a valid sound pointer
	if (!m_pSound )
	{
		// Here we call our error function since we couldn't get a valid pointer
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
/////	This plays the current sound that is loaded on any free channel
/////
///////////////////////////////// PLAY SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

// This will play the current song stored in the m_pSound (Must be loaded first)
void CFmod::PlaySong()
{
	// ==========================================================================================
	// PLAY SONG
	// ==========================================================================================

	// We should have a pointer to a sound that will allow us to play the song.
	// All we need to do is pass in the module to FSOUND_PlaySound().  We pass
	// in SOUND_FREE to say we don't care which channel it's on, just pick any
	// free channel.  The next parameter is the pointer to our sound.

	// Play our song that we loaded, then get the sound channel it chose to assign it too.
	m_soundChannel = FSOUND_PlaySound(FSOUND_FREE, m_pSound);				

}


///////////////////////////// DISPLAY ERROR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will display an error, determined by the FMod library
/////
///////////////////////////// DISPLAY ERROR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::DisplayError()
{
	// Print the error message to the screen
	cout << FMOD_ErrorString(FSOUND_GetError()) << endl;
}


///////////////////////////////// FREE SOUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will free the memory stored in m_pSound (Song go bye bye)
/////
///////////////////////////////// FREE SOUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::FreeSound()
{
	// Free the sound in the current soundChannel
	FSOUND_StopSound(m_soundChannel);
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
// This tutorial shows how to load and play MP3 and WAV files.  This is was
// an exciting tutorial to write because it does so much for you!  The Fmod library
// is perfect for games and demos.
// 
// * What Can I Play With This? * (WAV, MP3)
// 
// Though this tutorial only plays .mp3's, it will play a .wav with the same code,
// there is no need to change anything except the actual file name.  Cool huh?
//
// * Files Included *
//
// The files included are necessary for compiling and running the application.
// You do not need the FMODVC.LIB or FMOD.H to run it of course, but to compile
// it you will.  You will ABSOLUTELY need FMOD.DLL to run the application.
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
// I love this library and I hope you will too!!  Let us know what you do with it.
//
// I would like to thank the team at www.Fmod.org for the library and also the sample
// source code.  The code in this tutorial was primarily from the sample distributed
// in the FMod library.
//
// I would also like to thank NIB at freemp3snib.homestead.com for the music!
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
//



