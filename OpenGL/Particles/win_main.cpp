// Done by TheTutor

/*
	Particles and particle emitters are typically responsible for all the kick-nut eye candy you
	see in your favorite games.
	
	Particles are usually formed one of two ways.  The first way is to texture a quad and billboard
	it so it is always facing the camera.  The second way is to use point sprites which are usually
	implemented as triangle strips or the point primitive.  This tutorial is going to use the first
	method to display it's particles.
	
	So what makes up a particle?  Well you can have lots of different variables
	but here's what we're going to use:
	
	Position -- Holds the world coordinates of the center of the particle
	Velocity -- Specifies the direction and speed in which a particle is moving
	Color -- A 32-bit color for the particle
	Size -- The width and height of the particle quad
	Life -- Length of time in seconds of how long a particle is active (ie visible)
	Angle -- An angle of rotation for the particle's UVs
	A Texture -- A class that stores a texture
	
	Also all of particles are effected by a gravity.  The same gravity applies to 
	all particles.  Some other examples of variables you could have with your particle
	are:  wind velocity, fade out time, individual gravity, parent (what object emitted it),
	and so on and so forth.  Ultimately what you do and don't store in your particle is
	up to you.
	
	To demonstrate this particle madness, this app makes a flame effect.  The orange color
	used for the effect is:  red = 215, green = 115, and blue = 40.  Because of the blend mode
	chosen, the flame effect will look the same as long as the frame buffer color, for each
	component, is less than the color of orange for the particle.  To demonstrate this we slowly
	change the background color.  You notice as the background color approaches a lighter whitish
	color, so does the flame effect.
	
	Still not crystal clear?  Well what better way to answer all your questions than jumping
	into the thick of the code... 
*/

//#include <assert.h>
#include "gl_obj.h"
#include "gl_texture.h"
#include "particle.h"

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

// Make sure we load all the libraries we need
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define class_name "GT_OGL_Particles"

////////////////
// Constants //
//////////////

const int kWinWid = 800; // width of our window
const int kWinHgt = 600; // height of our window
const int kMaxParticles = 256; // Maximum number of particles



// Globals ***

CGLObj gGLObj; // Our OpenGL object
CParticle gParticles[kMaxParticles]; // An array of particles
float gRGB[3] = {0}; // The current RGB background color of the scene

// *** End of Globals

void UpdateBkgrndColor(); // Slowly changes the background color of the scene
bool LockFrameRate(int fps = 60); // Locks the frame rate to "fps"

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	MSG msg = {0};
	HWND hwnd = NULL;
	WNDCLASSEX wndclassex = {0};

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WinProc;					
	wndclassex.hInstance = hinstance;	
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);

	RegisterClassEx(&wndclassex); // Register the class

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_APPWINDOW;
	DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS; // Window style

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create our window
	hwnd = CreateWindowEx(winStyleEx,
						  class_name,
						  "www.GameTutorials.com -- Particles",
						  winStyle,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left, // Window width
						  rect.bottom - rect.top, // Window height
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE; // Something bad happened
			
	// Get the client RECT
	GetClientRect(hwnd, &rect);
	
	// If the client width or height isn't what we requested, there's been
	// an error somewhere and we're going to bail ship
	if(rect.right != kWinWid || rect.bottom != kWinHgt)
		return EXIT_FAILURE; // Something bad happened
		
	srand(GetTickCount()); // Seed random number generator
	
	HDC hdc = GetDC(hwnd); // Get the window's HDC
		
	// Initialize OpenGL
	if(gGLObj.init(hdc, 32, 16) == false)
	{
		ReleaseDC(hwnd, hdc); // Free up the HDC
			return EXIT_FAILURE; // Something bad happened
	}
	
	// Set up the view port
	gGLObj.setViewPort(kWinWid,kWinHgt,60.0);

	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
    
    for(int i = 0; i < kMaxParticles; ++i)
    {
		// If we fail to initialize any of the particles, exit the app
		if(!gParticles[i].init(CPos(0,0,0), // Position
							   CVector(RAND(-0.5f, 0.5f), RAND(0.5f, 1.5f), 0.0f), // Velocity
							   RAND(0.0f, 2.0f), // Lifespan
							   1.5f, // Size 
							   30.0f, // UV rotation angle
							   ARGB(255, 215, 115, 40), // Color
							   "particle.bmp")) // Texture name
		{
			ReleaseDC(hwnd, hdc);
				return EXIT_FAILURE;
		}
	}

	// We are locking the frame rate to 60 frames a second, that
	// means each frame time is 1.0f / 60.0f of a second.  So 
	// we'll use that time slice to update our particles
	float dt = 1.0f / 60.0f; // Delta time (time slice for each rendered frame)
	
	// Angle to rotate the emitted particles by
	int angle = 0;

	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it's time to draw
		{
			gGLObj.begin();
			
			// Rotate the emitted particles in a circle
			glRotatef((GLfloat)angle++, 0, 0, 1);
						
				// Draw and process all the particles/
				for(int i = 0; i < kMaxParticles; ++i)
				{
					gParticles[i].render();
					gParticles[i].process(dt);
				}
				
			gGLObj.end();
		}
		
	} // end of while(1)

	ReleaseDC(hwnd, hdc); // Free up the HDC
	UnregisterClass(class_name,hinstance); // Free up the WNDCLASSEX
		return (int)msg.wParam;
}

// Simple WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_KEYDOWN:

			switch(wparam)
			{
				case VK_ESCAPE: // On ESC quit the application
					SendMessage(hwnd, WM_CLOSE, 0, 0); // Send the window close message 
						break;						
			}
			
			return 0;
	
		case WM_DESTROY:
			
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Locks the frame rate at "frame_rate"
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate(int frame_rate)
{
	static float lastTime = 0.0f;
	
	// Get current time in seconds (milliseconds * .001 = seconds)
	float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	// If the desired frame rate amount of seconds has passed -- return true (ie Blit())
	if((currentTime - lastTime) > (1.0f / frame_rate))
	{
		// Reset the last time
		lastTime = currentTime;	
			return true;
	}

	return false;
}

/*
	Particle Rewind --
	
		Hopefully you've got a first step grasp of what a particle is and you have a 
		new insight of how you might go and program your very own particle engine.  A
		few short cuts that this tutorial took should be mentioned.  First, we never
		do any billboarding of the particles.  Since it's the only thing we're drawing and
		we never move the camera, we simply always drawn them in the XY plane.  Second we
		don't have a emitter object.  Robust particle systems, usually contain particles and
		emitters, with the emitter being responsible for the overall particle effect.  And
		last but not least, if we wanted to take advantage of an alpha component with our
		particle, either in the texture of in the color, we'd have to change up how we
		our doing the blending.
		
		So keep these things in mind when you go to make your own kickin' particle system %)
		
		If you have any questions regarding particles be sure to post them at:
		www.GameTutorials.com	
*/


/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
