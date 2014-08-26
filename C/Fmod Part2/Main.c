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

  Simple right!?!  Thanks to Fmod, playing sounds/music is a much, much easier task 
  than it ever was before.

************************************************************************/

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
// .mp3 and .wav formats.  Other function calls are necessary for the other formats.

// This is the song name that we are going to load.
// Put between the quotes the name of an .mp3 or .wav file you'd like to play (including extension).
#define SONG_NAME "FILL ME IN"

int main()									// Beginning of our program
{
	FSOUND_SAMPLE *pSound;					// This will hold our sound pointer to the song
	int soundChannel = 0;					// This is the sound channel our song is on

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

	// Let the user know the song is being loaded
	printf("Loading the song...\n\n");

	// Everything should be up and running with the sound system, so let's load a song.
	// To load a song we just pass in the file name to FSOUND_Sample_Load().
	// This returns a FSOUND_SAMPLE pointer.  We use this pointer to query and play the song.

	// Load the desired song and store the pointer to the sound.
	pSound = FSOUND_Sample_Load(FSOUND_FREE,SONG_NAME,0,0);

	// Make sure we got a valid sound pointer
	if (!pSound)
	{
		// Print the error message to the screen
		printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));

		// Quit the program
		exit(1);
	}

	// ==========================================================================================
	// PLAY SONG
	// ==========================================================================================

	// We should have a pointer to a sound that will allow us to play the song.
	// All we need to do is pass in the module to FSOUND_PlaySound().  We pass
	// in SOUND_FREE to say we don't care which channel it's on, just pick any
	// free channel.  The next parameter is the pointer to our sound.

	// Play our song that we loaded, then get the sound channel it chose to assign it too.
	soundChannel = FSOUND_PlaySound(FSOUND_FREE,pSound);				

	// Here we post a message that tells how to quit the program
	printf("Press any key to quit\n");
	printf("=========================================================================\n");

	// Now that the song is playing, we want to get some information about it.
	// Below we print out the amount of channels being played and the CPU usage.  
	// The CPU Usage is displayed in a percentage.  The "channels playing" should
	// only say one, since we only have one sound going.  There are more channels
	// when dealing with MOD and MIDI music because it isn't one constant flow of
	// sound data, it is a mixing of sounds.

	// Loop until we hit the keyboard
	while (!_kbhit())
	{
		// Print out the details of the current song being played.
		// Notice the "\r", it will redraw over itself each time.
		printf("channels playing: %d cpu usage: %.02f%%     \r", 
				FSOUND_GetChannelsPlaying(),	// This returns the amount of channels being played
				FSOUND_GetCPUUsage());			// This returns the CPU usage for this song
	} 

	// We get here if we hit a key on the keyboard, so that must mean we are done.
	// Now we need to do the clean up.  We just call FMUSIC_FreeSong() and FSOUND_Close().
		
	// Wait for a key to be pressed before we quit
	_getch();

	// Go the the next line
	printf("\n");

	// Free the sound in the current soundChannel
	FSOUND_StopSound(soundChannel);

	// Close and turn off the FMod Sound System.
	FSOUND_Close();

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
// This tutorial will play a .wav and .mp3 file with the same code.
// There is no need to change anything except the actual file name.  Cool huh?
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
//
// © 2000-2005 GameTutorials
