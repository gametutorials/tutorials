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

// This is a compiler directive that includes libraries (For Visual Studio).
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need this library to compile this program.
#pragma comment(lib, "fmodvc.lib")

#include <stdio.h>							// Here we include the standard include file
#include <stdlib.h>							// This is included so we can use exit()
#include <conio.h>							// This is used so we can use _kbhit() and getch()
#include "fmod.h"							// This is the fmod header file that we need to use fmod

// What is Fmod?  What let's see what www.fmod.org says about it:

// "FMOD is the fastest, most powerful and easiest to use sound system on Windows 
// and Linux and WinCE there is. Supporting 3d sound, midi, mods, mp3, ogg vorbis, 
// wma, recording, geometry processing, cd playback, mmx, internet streaming, 
// dsp effects, spectrum analysis, user created samples and streams, synchronization 
// support, DMO FX, EAX 1&2, A3D 3.0, C/C++/VB/Delphi/MASM and MORE you cant go wrong!"

// Isn't that amazing??  And it's free!  Basically, what we at GameTutorials.com would
// use it for is DEMOS DEMOS DEMOS and GAMES GAMES GAMES!  The DarkSide demo uses this
// for it's music.  I thought it would be a good idea to get a GameTutorials.com demo
// so you could see the simplest way to play a song. *Note* - This tutorial only plays
// MOD and MIDI formats.  Other function calls are necessary for the other formats.

// This is the song name that we are going to load.  It doesn't need to have
// .mod as the extension, but it seemed fitting.
#define SONG_NAME "techno.mod"

int main()									// Beginning of our program
{
	FMUSIC_MODULE *pModule;

	// ==========================================================================================
	// CHECK DLL VERSION
	// ==========================================================================================

	// First we want to make sure the DLL that we are using is the correct one we need.
	// Below we check if the one we are using is less than the one we need.

	// Make sure we are using the correct DLL version
	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		// Display the error message that shows we need the correct version
		printf("Error : You are using the wrong DLL version!  You should be using FMOD %.02f\n", FMOD_VERSION);

		// Quit the program
		exit(1);
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
		// If we couldn't initialize the sound system we print out the error
		printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));

		// Quit the program
		exit(1);
	}

	// ==========================================================================================
	// LOAD SONG
	// ==========================================================================================

	// Everything should be up and running with the sound system, so let's load a song.
	// To load a song we just pass in the file name to FMUSIC_LoadSong().
	// This returns a FMUSIC_MODULE pointer.  We use this pointer to query and play the song.

	// Load the desired song and store the pointer to that module.
	pModule = FMUSIC_LoadSong(SONG_NAME);

	// Make sure we got a valid module pointer
	if (!pModule)
	{
		// Print the error message to the screen
		printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));

		// Quit the program
		exit(1);
	}

	// ==========================================================================================
	// PLAY SONG
	// ==========================================================================================

	// We should have a pointer to a module that will allow us to play the song.
	// All we need to do is pass in the module to FMUSIC_PlaySong().

	// Play our song that we loaded
	FMUSIC_PlaySong(pModule);

	// Here we post a message that tells how to quit the program
	printf("Press any key to quit\n");
	printf("=========================================================================\n");

	// The song should be playing right now, so we can start querying information about it.
	// Let's get the name of the song first.  We do this by passing in the pointer pModule
	// to FMUSIC_GetName().  This will return the name as a char string.

	// Display the name of the file being played
	printf("Playing %s...\n", FMUSIC_GetName(pModule));

	// Now that the song is playing, we want to get some information about it.
	// Below we print out the order, the row, the amount of channels being played and
	// the CPU usage.  Let me explain a bit about these.  First off, the order can be
	// associated with a measure of a midi file.  So think of order as the measure.
	// What is a measure?  If you have ever looked at sheet music you will see vertical
	// lines going across the horizontal lines, but spaced somewhat far apart.  Inside
	// each measure there are notes.  The amount of notes depend on the measure count.
	// Say, a 4/4 measure would have 4 beats per measure, so 4 quarter notes.  So,
	// moving on, the next one is a row.  Think of a row as the beats, or notes...
	// Some songs have 127 (like this one) rows per order and some have 63, and some
	// have others.  You can use this information for doing cool effects, like... if
	// the order is 4 and the row is 120, you could make your cube spin the opposite way.
	// It makes for cool demo's, like on the DarkSide demo on the internet.
	// The channels that are playing are the amount of instruments being used in the song
	// currently.  Some songs use a lot, some songs might use 4 or 5.  If you are playing
	// a MIDI song verses a MOD song it will not display the order and row.  Finally, the
	// CPU Usage is quite obvious... what percent of the CPU is being used by the song.

	// Loop until we hit the keyboard
	while (!_kbhit())
	{
		// Print out the details of the current song being played.
		// Notice the "\r", it will redraw over itself each time.
		printf("order: %d, row: %d channels playing: %d cpu usage: %.02f%%     \r", 
				FMUSIC_GetOrder(pModule),		// This returns the order the mod is on
				FMUSIC_GetRow(pModule),			// This returns the current row of the order
				FSOUND_GetChannelsPlaying(),	// This returns the amount of channels being played
				FSOUND_GetCPUUsage());			// This returns the CPU usage for this song
	} 

	// We get here if we hit a key on the keyboard, so that must mean we are done.
	// Now we need to do the clean up.  We just call FMUSIC_FreeSong() and FSOUND_Close().
		
	// Wait for a key to be pressed before we quit
	_getch();

	// Go the the next line
	printf("\n");

	// Free the current module
	FMUSIC_FreeSong(pModule);

	// Close and turn off the FMOD Sound System.
	FSOUND_Close();

	// Return a success!
    return 0;
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
// There is so much that can be done with the FMod Sound System, but I have no idea
// how to do it all.  Check out our MP3 tutorial as well.
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
// © 2000-2005 GameTutorials

