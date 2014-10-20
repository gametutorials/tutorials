#ifndef _WindowManager_H
#define _WindowManager_H

#include <string>										// Used for our STL string objects

// This is our abstract base class for managing the creation of the window, input and the
// creating of the OpenGL context.  This class should be inherited from to create
// implementations like GLFW, Win32, GLUT or SDL.  Currently this is used for GLFW which is
// a cross-platform library so Linux, Mac and Windows users can use the same code.
// Notice that the functions have virtual on them so they can be overriden for the
// different API implementations to manage the window, input and OpenGL context creation.
// As a reminder, the " function() = 0;" makes this an abstract base class.
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
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Use this class as the abstract base class for your desired API.  We are using 
// the cross-platform GLFW in our tutorials with GLFWManager class, but you can 
// add your own for other APIs like GLUT, Win32 or SDL.  Checkout the Win32
// implementation online, there is a Github "Gist" referenced on this tutorial
// page.
// 
// 
// © 2000-2014 GameTutorials