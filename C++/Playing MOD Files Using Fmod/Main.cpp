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


// If you haven't heard of FMOD, it is a library that allows you to play
// many different sound and music files easily.  The library was developed
// by Firelight Multimedia and you can find it at www.FMOD.org.  We include
// the library with this tutorial, so you don't need to download it unless
// there is an update.  In this tutorial we play a .mod music file.  Read
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
// MOD and MIDI formats.  Other function calls are necessary for the other formats.

// This is the song name that we are going to load.  It doesn't need to have
// .mod as the extension, but it seemed fitting.
#define SONG_NAME "techno.mod"

int main()									// Beginning of our program
{	
	CFmod mySong;							// Create an instance of our CFmod class

	// Attempt to initialze our class AND load the song.  If we fail to do so, quit the program
	if(!mySong.Init(SONG_NAME))
		exit(1);

	// We now have the Fmod Sound System initialized and we also have the song loaded.
	// All we need to do now is play the song.

	// Now we will play the current song loaded (SONG_NAME)
	mySong.PlaySong();

	// Here we post a message that tells how to quit the program
	cout << "Press any key to quit" << endl;
	cout << "=========================================================================\n";

	// The song should be playing right now, so we can start querying information about it.
	// Let's get the name of the song first.  We do this by calling our data access function:
	// GetSongName().  This will return the name as a char string.

	// Display the name of the file being played
	cout << "Playing " << mySong.GetSongName() << "...\n";

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
	// the order = 4 and the row is 120, you could make your cube spin the opposite way.
	// It makes for cool demo's and graphical programs that need matching to music.
	// The channels that are playing are the amount of instruments being used in the song
	// currently.  Some songs use a lot, some songs might use 4 or 5.  If you are playing
	// a MIDI song verses a MOD song it will not display the order and row.  Finally, the
	// CPU Usage is quite obvious... what percent of the CPU is being used by the song.

	// Loop until we hit the keyboard
	while (!_kbhit())
	{
		// Print out the details of the current song being played.
		// Notice the "\r", it will redraw over itself each time.
		cout << "order: "     << mySong.GetOrder()		// This returns the order the mod is on
			 << " row: "      << mySong.GetRow()		// This returns the current row of the order
			 << " channels playing: " << FSOUND_GetChannelsPlaying() // This returns the amount of channels being played
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
// say, if the order is 8 and the row is 36, scale the screen, or start moving in.
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
//
//

