#ifndef _CFMOD_H
#define _CFMOD_H

// Here we include the fmod.h file that holds the Fmod prototypes
#include "fmod.h"

// Include the string header file so we can use "string"
#include <string>
using namespace std;

// This file should be included along with fmod.h for you applications.
// I decided to make a class out of the Fmod library so I wouldn't have
// to worry about freeing memory and all the code that I don't care about.
// I just want to load a MIDI or a MOD, I don't care about the underlying stuff.

// This class allows you to initialize, load and play a MIDI or MOD song
class CFmod {

public:

	// Typically constructor
	CFmod();

	// This will clean up memory and close the sound system
	~CFmod();

	// This initilizes the FMod sound system AND loads the song you pass in
	bool Init(char *strName);

	// This will play the current song stored in the m_pModule (Must be loaded first)
	void PlaySong();

	// This will free the memory stored in m_pModule (Song go bye bye)
	void FreeSoundModule();

	// This will close the FMod Sound System
	void FreeSoundSystem();

	//////////////////// Data Access Functions //////////////////// 

	// This returns the pointer to our song module
	FMUSIC_MODULE *GetModule() {	return m_pModule;		}

	// This will return a string of characters for the name of the song
	char *GetSongName()	{ return FMUSIC_GetName(m_pModule);	}

	// This will return a string of characters for the name of the file loaded
	const char *GetFileName()	{ return m_strName.c_str();	}

	// This returns the current order of the song (Like a measure)
	int GetOrder()	{	return FMUSIC_GetOrder(m_pModule);	}

	// This returns the current row of the song (Like a beat)
	int GetRow()	{	return FMUSIC_GetRow(m_pModule);	}


private:

	// This holds the data for the current song loaded (Like a handle)
	FMUSIC_MODULE *m_pModule;

	// This holds the name of the current song loaded
	string m_strName;

	// This takes a file name (a midi or mod file) and loads it into the m_pModule
	bool LoadSong(char *strName);

	// This will display a error message (provided by FMod library)
	void DisplayError();
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows how to load most types of MOD or MIDI files.  This was
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
// say, if the order is 8 and the row is 36, scale the screen, or start moving in.
// There is so much that can be done with the FMod Sound System.
//
// This file includes the class prototype you need and should be included in your projects.
// I converted this code from the C Fmod tutorial at GameTutorials.com to a more usable
// format, a beautiful class :)  
// 
// I hope this helps you make wonderful applications!  All the credit goes to
// "FireLight Multimedia" because they made they library, I just made a neat client.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
//

