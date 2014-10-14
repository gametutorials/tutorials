#ifndef _Main_H
#define _Main_H

#include "WindowManager.h"					// Include our WindowManager to handle the cross-platform window
#include "Model.h"							// Include our Model class to draw the setup and draw the triangle

// This is our main application class that handles orchestrating the init, game loop and cleanup
// of the application.  This is defined in Main.cpp and should be the first place you look for
// the tutorial instructions when opening this tutorial.  The WindowManager.cpp will have the
// entry point function (main() for a console application, WinMain() for a Win32 application).
// The entry point then creates an instance of GLApplication and runs the GLMain() function.
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
// In this file we defined our main GLApplication class which runs the tutorial.
// For the initial tutorial instructions please always view the Main.cpp first.
// 
// 
// © 2000-2014 GameTutorials