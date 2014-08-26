#ifndef _CFMOD_H
#define _CFMOD_H

// Include file for using C++ string object
#include <string>
using namespace std;

// Here we include the fmod.h file that holds the Fmod prototypes
#include "fmod.h"

// This file should be included along with fmod.h for you applications.
// I decided to make a class out of the Fmod library so I wouldn't have
// to worry about freeing memory and all the code that I don't care about.
// I just want to load a MP3 or a WAV, I don't care about the underlying stuff.

// This class allows you to initialize, load and play a MP3 or WAV sound
class CFmod {

public:

	// Typical constructor
	CFmod();

	// This will clean up memory and close the sound system
	~CFmod();

	// This inits your CFmod class MUST BE CALLED before any Fmod functions
	bool Init(char*);

	// This will play the current song stored in the m_pSound (Must be loaded first)
	void PlaySong();

	// This will free the memory stored in m_pSound (Song go bye bye)
	void FreeSound();

	// This will close the FMod Sound System
	void FreeSoundSystem();

	//////////////////// Data Access Functions //////////////////// 

	// This returns the pointer to our song module
	FSOUND_SAMPLE *GetSound() {	return m_pSound;					}

	// This will return a string of characters for the name of the song.
	// Some songs might not have the song information stored, so it
	// will be NULL.  
	char *GetSongName()	{ return FSOUND_Sample_GetName(m_pSound);	}

	// This will return a string of characters for the name of the file loaded
	const char *GetFileName() { return m_strName.c_str();			}

private:

	// This holds the data for the current song loaded (Like a handle)
	FSOUND_SAMPLE *m_pSound;

	// This holds the name of the current song loaded
	string m_strName;

	// This holds the sound channel that is returned from FSOUND_PlaySound()
	// Since we pick any free channel, we need to know which one the sound was assigned.
	int m_soundChannel;

	// This will display a error message for whatever error may occur
	void DisplayError();

	// This takes a file name (a mp3 or wav file) and loads it into the m_pSound
	bool LoadSong(char *strName);
};


#endif


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
