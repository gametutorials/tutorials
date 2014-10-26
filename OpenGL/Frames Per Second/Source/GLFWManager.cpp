#include "GL\glew.h"
#include "../Headers/GLFWManager.h"
#include "../Headers/Main.h"


// This is the entry point into our application
int main()
{
	// First create our desired WindowManager implementation so we can set it below
	GLFWManager *pWindowManager = new GLFWManager();

	// Create a local instance of our GLApplication (defined in Main.cpp) and set its
	// WindowManager implementation (in this case, GLFW).
	GLApplication application;
	application.SetWindowManager(pWindowManager);

	// Return the GLMain() defined in Main.cpp, which handles the flow of our application
	// and immediately starts our game loop.
	return application.GLMain();
}


// This initializes our window and creates the OpenGL context
int GLFWManager::Initialize(int width, int height, std::string strTitle, bool bFullScreen)
{
	// This tries to first init the GLFW library and make sure it is available
	if ( !glfwInit() )
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// This tells OpenGL that we want a multisampling value of 4 (anti-aliasing)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Set our OpenGL version to 4 using the core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window either in fullscreen or not
	if( bFullScreen )
		Window = glfwCreateWindow(width, height, strTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
	else
		Window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr, nullptr);

	// Make sure the window is valid, if not, throw an error.
	if ( Window == nullptr )
	{
		fprintf(stderr, "Failed to create a GLFW window, you might need to download the latest drivers or change the OpenGL version to 3\n");
		Destroy();

		return -1;
	}

	// Create the OpenGL context from the window and settings specified
	glfwMakeContextCurrent(Window);

	// This turns on STICKY_KEYS for keyboard input
	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);

	// This turns off the vertical sync to your monitor so it renders as fast as possible
	glfwSwapInterval(0);

	// Tell GLEW to grab all the OpenGL functions and extensions even if "experimental"
	glewExperimental = GL_TRUE;

	// Initialize the GLEW library and attach all the OpenGL functions and extensions
	GLenum err = glewInit();

	// If we had an error, return -1.  Be sure to see if glewExperimental isn't true, this worked for me.
	if ( GLEW_OK != err )
	{
		fprintf(stderr, "Failed to initialize glew\n");
		return -1;
	}

	// Return success
	return 0;
}


// This swaps the backbuffer with the front buffer to display the content rendered in OpenGL
void GLFWManager::SwapTheBuffers()
{
	// This takes the Window and swaps the backbuffer to the front
	glfwSwapBuffers(Window);
}


// This function processes all the application's input and returns a bool to tell us if we should continue
bool GLFWManager::ProcessInput(bool continueGame = true)
{
	// Use the GLFW function to check for the user pressing the Escape button, as well as a window close event.
	if ( glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(Window) != 0 )
		return false;

	// Poll the input events to see if the user quit or closed the window
	glfwPollEvents();

	// Return the value passed in to tell the game loop that we should continue or not
	return continueGame;
}


// This destroys the window
void GLFWManager::Destroy()
{
	// This closes the OpenGL window and terminates the application
	glfwTerminate();
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This files uses the GLFW cross-platform library to create an OpenGL window,
// as well as the GLEW library to load the modern OpenGL functions and extensions.
// Nothing new was changed in this file since the last tutorials.
//
//
// © 2000-2014 GameTutorials