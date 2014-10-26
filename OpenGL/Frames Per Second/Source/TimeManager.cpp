//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Frames Per Second								 //
//																		 //
//		$Description:	Demonstrates calculating the frame rate			 //
//																		 //
//***********************************************************************//

#include "../Headers/TimeManager.h"										// Include for the TimeManager class definition
using namespace std;													// Include so we don't need to complicate our C++ 11 code
using namespace chrono;													// Include so we don't need to complicate our C++ 11 code


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
This file is where we define our implementation of our Singleton class, TimerManager.  This 
handles the frame rate calculation and calculates the time slice that we use to animate our
triangle, and will be used for all the camera tutorials and movement going forward.  We use
C++ 11 to get the current time in our GetTime() function.  This is used to get the current
seconds each frame and get the delta between the last frame.  To calcualte the frame rate,
we simply grab the current time and then the time every frame until it is greater than 1
second.  Then we see how many frames happened between that time and use that as our frame rate.

Notice that we declare the namespaces above for std and chrono, which makes our code for our
C++ 11 functionality more readable without having to do std::chrono:: everywhere.
*/

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// This calculates our current scene's frames per second and displays it in the console
double TimeManager::CalculateFrameRate(bool writeToConsole = false)
{
	// Below we create a bunch of static variables to track the deltas of the time.

	static double framesPerSecond = 0.0f;								// This will store our fps
	static double startTime = GetTime();								// This will hold the time per second to test against
	static double lastTime = GetTime();									// This will hold the time from the last frame
	static char strFrameRate[50] = { 0 };								// We will store the string here for the window title
	static double currentFPS = 0.0f;									// This stores the current frames per second

	// Grab the current time in seconds from our environment's time function wrapped in our WindowManager	
	CurrentTime = GetTime();

	// Calculate our delta time, which is the time that has elapsed since the last time we checked the time
	DeltaTime = CurrentTime - lastTime;

	// Now store the lastTime as the currentTime to then get the time passed since the last frame
	lastTime = CurrentTime;

	// Increase the frame counter
	++framesPerSecond;
	
	// Now we want to subtract the current time by the start time that was stored.  If it is greater than 1
	// that means a second has passed and we need to display the new frame rate.  The 1.0 represents 1 second.  
	// Let's say we got 12031 (12.031) from GetTime() for the CurrentTime, and the startTime had 11230 (11.230).  
	// Well, 12.031 - 11.230 = 0.801, which is NOT a full second.  So we try again the next frame.  Once the 
	// CurrentTime - startTime comes out to be > 1 second, we calculate the frames for this last second.
	if ( CurrentTime - startTime > 1.0f )
	{
		// Here we set the startTime to the currentTime.  This will be used as the starting point for the next second.
		// This is because GetTime() counts up, so we need to create a delta that subtract the current time from.
		startTime = CurrentTime;

		// Show the frames per second in the console window if desired
		if ( writeToConsole )
			fprintf(stderr, "Current Frames Per Second: %d\n", int(framesPerSecond));
		
		// Store the current FPS since we reset it down below and need to store it to return it
		currentFPS = framesPerSecond;

		// Reset the frames per second
		framesPerSecond = 0;
	}

	// Return the most recent FPS
	return currentFPS;
}


// This returns the current time in seconds (uses C++ 11 system_clock)
double TimeManager::GetTime()
{
	// Grab the current system time since 1/1/1970, otherwise know as the Unix Timestamp or Epoch
	auto beginningOfTime = system_clock::now().time_since_epoch();

	// Convert the system time to milliseconds
	auto ms = duration_cast<milliseconds>(beginningOfTime).count();

	// Return the time in seconds and give us a fractional value (important!)
	return ms * 0.001;
}


// This pauses the current thread for an amount of time in milliseconds
void TimeManager::Sleep(int ms)
{
	// Use the C++ 11 sleep_for() function to pause the current thread
	this_thread::sleep_for( milliseconds(ms) );
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Use this class to calculate the frame rate and grab a time slice to make your
// movement the same across all devices and environments.
// 
// 
// © 2000-2014 GameTutorials