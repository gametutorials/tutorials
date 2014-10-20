#ifndef _GLFWManager_H
#define _GLFWManager_H

#include <string>										// Used for our STL string objects
#include <fstream>										// Used for fprintf and stderr									
#include "GLFW\glfw3.h"									// Used to reference the GLFW library
#include "WindowManager.h"								// Used to reference the abstract base class


// This is our implementation of the abstract base class WindowManager, which uses the 
// GLFW cross-platform library to create a window, handle input and create the OpenGL
// context.  You can go online for to this tutorials page for the Github "Gist" to
// find the Win32 implementation for those who would rather work with that API.
class GLFWManager : public WindowManager
{
public:

	// Create an empty constructor and have the deconstructor release our memory
	GLFWManager()	{ }
	~GLFWManager()	{ Destroy(); }

	// This initialized the window and creates the OpenGL context
	virtual int Initialize(int width, int height, std::string strTitle, bool bFullScreen = false);
	
	// This swaps the backbuffer to the front and should be called every frame
	virtual void SwapTheBuffers();

	// This processes any needed input for the application, like the Escape key
	virtual bool ProcessInput(bool continueGame);

	// This destroys the window and OpenGL context
	virtual void Destroy();

protected:

	// This is the GLFW window object that manages the window and input
	GLFWwindow* Window;
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This class inherits the WindowManager abstract base class and is used to handle
// the window management and input of the application, done using the cross-platform
// GLFW library.  Be sure to use the right GLFW libraries for your different
// environment.  This is built using the library for Windows.
// 
// 
// © 2000-2014 GameTutorials