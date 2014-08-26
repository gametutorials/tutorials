#ifndef _CFMOD_H
#define _CFMOD_H

// Here we include the fmod.h file that holds the Fmod prototypes
#include "fmod.h"

// This class allows you to initialize, load and play a MIDI or MOD song
class CFmod 
{
public:

	// Typically constructor
	CFmod();

	// This will clean up memory and close the sound system
	~CFmod();

	// This initilizes the FMod sound system AND loads the song you pass in
	bool Init(char *strName);

	// This will play the current song stored in the m_pModule (Must be loaded first)
	void PlaySong();

	// This plays a new song
	void PlayNewSong(char *szNewSong);

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
	char *GetFileName()	{ return m_strName;					}

	// This returns the current order of the song (Like a measure)
	int GetOrder()	{	return FMUSIC_GetOrder(m_pModule);	}

	// This returns the current row of the song (Like a beat)
	int GetRow()	{	return FMUSIC_GetRow(m_pModule);	}


private:

	// This holds the data for the current song loaded (Like a handle)
	FMUSIC_MODULE *m_pModule;

	// This holds the name of the current song loaded
	char m_strName[255];

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
// Nothing new was added to this file.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//
