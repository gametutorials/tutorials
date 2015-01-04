#ifndef _WindowManager_H
#define _WindowManager_H

#include <string>										// Used for our STL string objects
#include <fstream>										// Used for fprintf and stderr									
#include "InputManager.h"								// Added for the new InputManager

// This is our class definition for managing the creation of the window, input and the
// creating of the OpenGL context.  This class should be inherited from to create
// specific implementations like GLFW (i.e. GLFWManager), Win32, GLUT or SDL.
class WindowManager
{
public:

	// This is needed so that the class inheriting this will have it's deconstructor called
	virtual ~WindowManager() {}

	// This initialized the window and creates the OpenGL context
	virtual int Initialize(int width, int height, std::string strTitle, bool bFullScreen = false) = 0;

	// This swaps the backbuffer to the front and should be called every frame
	virtual void SwapTheBuffers() = 0;

	// This processes any needed input for the application, like the Escape key
	virtual bool ProcessInput(bool continueGame) = 0;

	// This destroys the window and OpenGL context
	virtual void Destroy() = 0;

	// Add a way to access the application's InputManager for camera movements
	virtual InputManager *GetInputManager() { return &InputManager; }

protected:

	// We created a variable to an InputManager to control camera movements
	InputManager InputManager;
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Use this class as the abstract base class for your desired API.  We are using 
// the cross-platform GLFW in our tutorials with GLFWManager class, but you can 
// add your own for other APIs like GLUT, Win32 or SDL.  Checkout the Win32
// implementation online, there is a Github "Gist" referenced on the first OpenGL
// tutorial page's comments.
// 
// Nothing new was added to this file since the last tutorial.
// 
// © 2000-2015 GameTutorials