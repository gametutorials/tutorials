#ifndef _Main_H
#define _Main_H

#include "WindowManager.h"					// Include our WindowManager to handle the cross-platform window
#include "Model.h"							// Include our Model class to draw the setup and draw the triangle

// This is our main application class that handles orchestrating the init, game loop and cleanup
class GLApplication
{
public:

	// This is the main function for our application, which is called by the main() function
	// in the WindowManager.cpp file.  This helps abstract out the environments entry point.
	int GLMain();

	// This initializes the application
	void Initialize();

	// This runs the game loop and only quits when the user hits Escape or closes the window
	void GameLoop();

	// This cleans up the application and tells other objects to also cleanup
	void Destroy();

private:

	// Our global window manager to create the window and manage input, which should be
	// environment agnostic.  Our main application shouldn't know if it's Win32 or GLFW.
	WindowManager WindowManager;							
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this file we define our main GLApplication class which runs the tutorial.
// For the initial tutorial instructions please always view the Main.cpp first.
// 
// 
// © 2000-2014 GameTutorials