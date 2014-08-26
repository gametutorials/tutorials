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

/*************************** SPECIAL NOTE *******************************

  As is, this tutorial WILL NOT WORK.  You need to fill in the name of a .mp3 or .wav
  file you'd like to play.  A few lines down you'll see this line of code:
	
	  #define SONG_NAME "FILL ME IN"

  So lets say you wanted to play hardrock.mp3.  You only have to do two easy steps to 
  get this all to work.
  
	1)  Change that line to read:
	
			#define SONG_NAME "hardrock.mp3"

	2)  Recompile the program

  Simple right!?!  Thanks to Fmod playing sounds/music is a much, much easier task 
  than it ever was before.

************************************************************************/


// If you haven't heard of FMOD, it is a library that allows you to play
// many different sound and music files easily.  The library was developed
// by Firelight Multimedia and you can find it at www.FMOD.org.  We include
// the library with this tutorial, so you don't need to download it unless
// there is an update.  In this tutorial we play a .wav and .mp3 music file.  Read
// below for more information on FMOD.


// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need this library to compile this program.
#pragma comment(lib, "fmodvc.lib")

#include <iostream>							// Here we include the standard include file
#include <conio.h>							// This is used so we can use _kbhit() and getch()
#include "CFmod.h"							// This is the fmod classheader file that we need to use fmod
using namespace std;						// Use the standard name space

// What is Fmod?  What let's see what www.fmod.org says about it:

// "FMOD is the fastest, most powerful and easiest to use sound system on Windows 
// and Linux and WinCE there is. Supporting 3d sound, midi, mods, mp3, ogg vorbis, 
// wma, recording, geometry processing, cd playback, mmx, internet streaming, 
// dsp effects, spectrum analysis, user created samples and streams, synchronization 
// support, DMO FX, EAX 1&2, A3D 3.0, C/C++/VB/Delphi/MASM and MORE you cant go wrong!"

// Isn't that amazing??  And it's free!  Basically, what we at GameTutorials.com would
// use it for is DEMOS DEMOS DEMOS and GAMES GAMES GAMES!  *Note* - This tutorial only plays
// .mp3 and .wav formats.  Other function calls are necessary for the other formats.

// This is the song name that we are going to load
// You have to put between the quotes the name of an .mp3 or .wav file you'd like to play
#define SONG_NAME "FILL ME IN"

int main()									// Beginning of our program
{	
	CFmod mySong;							// Create our class

	if(!mySong.Init(SONG_NAME))				// Init our CFmod class AND load the song
		exit(1);						    // If an error is occurred, we immediately
											// exit the program

	// We not have the Fmod Sound System initialized and we also have the song loaded.
	// All we need to do now is play the song.

	// Now we will play the current song loaded (SONG_NAME)
	mySong.PlaySong();

	// Here we post a message that tells how to quit the program
	cout << "Press any key to quit" << endl;
	cout << "=========================================================================\n";

	// Now that the song is playing, we want to get some information about it.
	// Below we print out the amount of channels being played and the CPU usage.  
	// The CPU Usage is displayed in a percentage.  The "channels playing" should
	// only say one, since we only have one sound going.  There are more channels
	// when dealing with MOD and MIDI music because it isn't one constant flow of
	// sound data, it is a mixing of data.

	// Loop until we hit the keyboard
	while (!_kbhit())
	{
		// Print out the details of the current song being played.
		// Notice the "\r", it will redraw over itself each time.
		cout << " channels playing: " << FSOUND_GetChannelsPlaying() // This returns the amount of channels being played
			 << " cpu usage: "<< FSOUND_GetCPUUsage()	// This returns the CPU usage we are taking up
			 << "%     \r";				
	} 

	// We get here if we hit a key on the keyboard, so that must mean we are done.
	// Now we need to do the clean up.  We don't need to clean up because the
	// DeConstructor does it for us.
		
	// Wait for a key to be pressed before we quit
	_getch();

	// Go the the next line
	cout << endl;

	// Return a success!
    return 0;
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
// I would like to thank the team at www.Fmod.org for the library and also the sample
// source code.  The code in this tutorial was primarily from the sample distributed
// in the FMod library.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
//

