#ifndef _Main_H
#define _Main_H

#include "WindowManager.h"					// Include our WindowManager to handle the cross-platform window
#include "Model.h"							// Include our Model class to draw the setup and draw the triangle

// This is our main application class that handles orchestrating the init, game loop and cleanup
class GLApplication
{
public:

	// The width and height of the application
	static const int ScreenWidth = 1024;
	static const int ScreenHeight = 768;

	// Create a default constructor and deconstructor which cleans up
	GLApplication() {}
	~GLApplication() { Destroy(); }

	// This is the main function for our application, which is called by the main() function
	// in the WindowManager.cpp file.  This helps abstract out the environments entry point.
	int GLMain();

	// This gets and sets the window manager for the application.  This should be set right
	// after the GLApplication is created in the main() function.
	WindowManager *GetWindowManager() { return WindowManager; }
	void SetWindowManager(WindowManager *pWindowManager) { WindowManager = pWindowManager; }

	// This initializes the application
	void Initialize();

	// This runs the game loop and only quits when the user hits Escape or closes the window
	void GameLoop();

	// This cleans up the application and tells other objects to also cleanup
	void Destroy();

protected:

	// Our abstract window manager to create the window and manage input, which should be
	// environment agnostic.  Our main application shouldn't know if it's Win32 or GLFW.
	// This is instantiated and set in GLFWManager.cpp in the main() function.
	WindowManager *WindowManager;
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