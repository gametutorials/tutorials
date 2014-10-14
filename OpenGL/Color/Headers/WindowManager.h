#ifndef _WindowManager_H
#define _WindowManager_H

#include <string>										// Used for our STL string objects
#include <fstream>										// Used for fprintf and stderr									

// This is our class definition for managing the creation of the window, input and the
// creating of the OpenGL context.  This class should be inherited from to create other
// implementations like Win32, GLUT or SDL.
class WindowManager
{
public:

	// Create an empty constructor and have the deconstructor release our memory.
	WindowManager()		{ }
	~WindowManager()	{ Destroy(); }

	// This initialized the window and creates the OpenGL context
	virtual int Initialize(int width, int height, std::string strTitle, bool bFullScreen = false);
	
	// This swaps the backbuffer to the front and should be called every frame
	virtual void SwapBuffers();

	// This processes any needed input for the application, like the Escape key
	virtual bool ProcessInput(bool continueGame);

	// This destroys the window and OpenGL context
	virtual void Destroy();

private:

	// This is a void pointer for the reference to the window
	void* Window;
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Use this class as the base class for your desired API.  We are using the
// cross-platform GLFW in the implementation, but you can override that with
// your own Win32, GLUT or SDL implementation.
// 
// 
// © 2000-2014 GameTutorials