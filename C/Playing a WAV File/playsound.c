//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		PlaySound										 //
//																		 //
//		$Description:	Shows how to play a wav file					 //
//																		 //
//***********************************************************************//


// This tutorial shows you how to place a .Wav sound file.  Programs can be
// boring without sound, especially games, so let's show you how to do this.
// The function we will use is PlaySound().  You need to include a special
// library to your code to use this function.  This library is called Winmm.Lib.
// Now let's go through our tutorial and see how to add sound code.


#include <windows.h>			// This needs to be included for PlaySound()
#include <stdio.h>				// This line of code is necessary to include all the C standard functions

#pragma comment(lib,"winmm.lib") // This adds the static library "winmm.lib" to the project

void main()						// This creates our function main()
{								// The beginning of our program
	int i = 0;					// This will be used as a counter
								
								// Display a message about the first sound
	printf("Playing synchronous sound...\n");

								// PlaySound() is the function we use to "play a sound".
								// The first parameter is the name of the .wav file.
								// The second parameter is NULL, unless we have the file in a resource.
								// In the resource case, we would put SND_RESOURCE in the second parameter.
								// The third parameter is the most important.  You will see that I put 2 parameters in.
								// By using the '|' operator we can tell the function we want multiple options.
								// Another example of this might be, "SND_FILENAME | SND_SYNC | SND_LOOP".
								// The first option, "SND_FILENAME" tells PlaySound we are getting the sound from a file.
								// The second option, "SND_SYNC" tells PlaySound that we want to play the sound synchronously.
								// Playing a synchronous sound means that NOTHING will continue until the sound finished.
								// In other words, that means that the program will not continue until the sound finishes.
	PlaySound("mysound.wav", NULL, SND_FILENAME | SND_SYNC);

	Sleep(1500);				// This sleeps for about a second and a half

								// The above code will take the .wav file, play the sound, then continue on once it is finished.
								// Well, you might ask... what if we want the sound to play while something is going on?
								// Good question.  All we have to do is, use SND_ASYNC instead of SND_SYNC.
								// This would give us an asynchronous sound.  This means that the sound will play in the background. 

	PlaySound("mysound2.wav", NULL, SND_FILENAME | SND_ASYNC);

								// This message will be displayed while the above sound is playing
	printf("Playing asynchronous sound");
								
								// Here we create a simple for loop to show that we are doing something while the sound is being played.
	for(i = 0; i < 50; i++)
	{
		printf(".");			// Print out a dot each iteration of the loop
		Sleep(50);				// Pause the program for 1/20 of a second (1000 = 1 second)
	}

	printf("\n");				// Position the cursor to the next line
}								// The end of our program


//////////////////////////// Quick Notes ///////////////////////////////////////////////
//
//  Here is the simplest example on how to play sounds during your application.
//  This can be done in console or Win32 (Windows programming).  In this program
//  you are shown how to play a sound synchronously (SND_SYNC). This of course won't let 
//  the program go on until the sound is finished playing.  If you want to play a sound
//  in the background, otherwise known as asynchronous, then you say SND_ASYNC instead.
//
//  You might be tempted to load in a huge 3 minute sound to play during your game.  
//  There is a problem with this.  PlaySound() is a really simple, primitive API.
//  If you do this, it will take a long time to load the sound, and then it will jump
//  in and out.  If you want to actually play long music, you will want to either do MIDI,
//  or look into doing another API or library.  MP3 would also be a good idea.  Pretty much, 
//  PlaySound() should most likely be used for sound effects of small intros.  If you want the sound
//  to LOOP, then you can add SND_LOOP.  It will continue playing until you call PlaySound()
//  again with these parameters :  PlaySound(NULL, NULL, SND_PURGE); 
//  This will purge the sounds currently being played.
//  
//  *Warning*  If you get a error that say:  Unresolved External: PlaySound(),
//             you did not add the "winmm.lib" correctly, or you haven't included windows.h.
//
//  If you go into my settings you will see winmm.lib at the very end.  A library is just
//  some code that is put into a file so you can include it, instead of adding more .c files.
//  You can build you own library if you want.  Just create a "Static Library" under the projects
//  tab, instead of choosing Console Application.  Usually you only make libraries if you have
//  code that you won't need to mess with any more.  Like, a graphics library, or a 3D library,
//  or a math library.
//
// © 2000-2005 GameTutorials
