// Done by TheTutor

/*	Lighting is a crucial component to making any graphics engine look nice.  This tutorial
	is aimed at being the first step to understanding D3D lighting.
	
	A D3D light comes in three main flavors.  Here's a quick break down of each:
	
		Point Light - Also sometimes called an omni light.  This light is a point
					  in space and it emanates light in all directions.  An example
					  would be a fire.
						
		Spot Light - A spot light emanates light in a focused direction within a 
					 limited cone.  An example (besides of course a spot light :) would
					 be a flashlight.
						
		Directional Light -	This light emanates light in a specified direction.  It has
							infinite distance for what it will light.  An example would be the sun.
							
	D3DLIGHT9 is the DirectX light structure that contains all the parameters that make up
	a light.  Lets break down each member of the D3DLIGHT9 struct:
	
	typedef struct _D3DLIGHT9 {
		D3DLIGHTTYPE Type; // Specifies the type of light.  Either point, spot or directional
		D3DCOLORVALUE Diffuse; // The diffuse color component of the light (the lights "color")
		D3DCOLORVALUE Specular; // The specular color component (shininess) of the light
		D3DCOLORVALUE Ambient; // The ambient color of the light (how much something not lit by the light
							  // gets illuminated)
		D3DVECTOR Position; // Where in world space the light is
		D3DVECTOR Direction; // The direction, in world space, the light is pointing
		float Range; // The range in which a light will illuminate an object.  This parameter
					// has no effect for directional lights
		float Falloff; // For spot lights only, it's the falloff of light intensity between a 
					  // spot lights inner cone and outer cone.  This is expensive!  It is generally
					 // always kept at 1.0f
		float Attenuation0; // Value specifying how much a light intensity changes over distance
		float Attenuation1; // Value specifying how much a light intensity changes over distance
		float Attenuation2; // Value specifying how much a light intensity changes over distance
							// **NOTE** You can play with these for different attenuation effects
							// but it is most common to sent Attenuation0 and Attenuation2 to 0.0f
							// and Attenuation1 to 1.0f.  This gives a linear light attenuation.
		float Theta; // Angle, in radians of a spot light's inner cone
		float Phi; // Angle, in radians of a spot light's outer cone
	} D3DLIGHT9;
	
	**NOTE** These values may or may not have effect on all light types.  Additionally, there
			 may be values that will not work for certain parameters.  Your best bet when
			 working with lights in D3D is to look up the light struct in MSDN for all the nitty-
			 gritty details :)
			 
	Now that we know how the D3D light struct is laid out, it is important to understand a little
	bit of how lighting works in D3D.  There are many factors involved to determine how a particular
	object is lit, but ultimately lighting is done via vertex lighting.  This means that every vertex's
	color is influenced by the lights (or lack of lights) in the scene.  In general, the lighting value
	for a vertex is determined by a dot product between the light's direction vector and the normal of 
	each vertex.  You MUST have a normal defined for your vertex, or lighting will not work correctly.
	
	Other factors that determine the final color of the vertex are:  light type, light color, 
	vertex color, an objects material properties, position/range/attenuation of light, 
	and direction of light.
	
	This tutorial focuses only on the properties that can be set with the D3DLIGHT9 light struct and
	shows how the number of vertices in an object affects the way it is lit.
	
	The controls for this tutorial are as follows:
	
		'+' -- Increases the size of the triangles in the tessellated quad
		'-' -- Decreases the size of the triangles in the tessellated quad	
	
*/

#include <windows.h>
#include <stdio.h>
#include "d3d_obj.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

////////////////
// Constants //
//////////////

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_D3DVertexLighting"; // WNDCLASSEX's name

const int kMaxVerts = 512; // Maximum number of vertices in our vertex pool

//////////////
// Globals //
////////////

SVertex gVertPool[kMaxVerts] = {0}; // A giant array of vertices we can use for whatever we want
CPos gLightPos(0, 0, -1); // World position of the light
float gTriSize = 0.25f; // Size of a single triangle in the tessellated quad

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void CenterCursor(HWND hwnd); // Centers the cursor in the window

// Draws a tessellated quad centered at "cen", with "numStrips" triangle strips and "trisPerStrip"
// triangles in each strip.  Triangle size is "triSize" with the color of the quad set by "color"
// Quad is drawn in the XY plane
void DrawTessellatedQuad(const CPos &cen, int numStrips, int trisPerStrip, float triSize, DWORD color);

// WinProc()
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain()
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HWND hwnd = NULL;
	MSG msg = {0};
	WNDCLASSEX wndclassex = {0};

	// Init fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always set to size of WNDCLASSEX
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WinProc;
	wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = kClassName;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);

	RegisterClassEx(&wndclassex); // Register the WNDCLASSEX

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect

	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
					 WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- D3D Vertex Lighting",
						  winStyle, // Window style
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left,
						  rect.bottom - rect.top,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

	// Init our global 3D object
	if(g3D->init(hwnd) == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint

	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Set the perspective matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);

	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));
	
	ShowCursor(FALSE); // Hide cursor
	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

    while(1) // While the app is running...
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Handle messages from the OS
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it's time to draw, do so
		{
			static float addMe = 0.05f;
			const float kMaxY = 1.0f;
			const float kMinY = -1.0f;
		
			g3D->begin(); // Begin drawing
			g3D->clear();
			
			g3D->setLightPos(0, gLightPos); // Set the light position
			g3D->setLightActive(0, true); // Make the light active
										  // **NOTE** You must do this to update the
										  // D3D light's data	
			
			// Move the light up 'n down
			gLightPos.y += addMe;
			
			// Clamp the lights up/down movement to the range of (1.0f,-1.0f)
			if((gLightPos.y > kMaxY) || (gLightPos.y < kMinY))
				addMe = -addMe;			
			
			// Draw a quad made up of 16x16 triangles
			DrawTessellatedQuad(CPos(0,0,0), 16, 16, gTriSize, D3DCOLOR_ARGB(255, 50, 50, 200));
			
			g3D->end(); // Finish drawing
		}
		else
			Sleep(1); // Otherwise give the OS some time to process other things
	
	} // end of while(1)

	ShowCursor(TRUE); // Show the cursor again
	
	g3D->deinit(); // Free up CD3DObj
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	const float kIncTriSize = 0.01f; // Amount to change the size of a triangle by

	switch(message)
    {
		case WM_SYSKEYDOWN:

			// Toggle on ALT + ENTER				
			if(wparam == VK_RETURN && (lparam & (1 << 29)))
			{
				g3D->toggleFullScreen();

				// We need to reset our view
				g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));
			}

			break; // Allow other system keys to be handled by DefWindowProc()
    
		case WM_KEYDOWN: // If we get a key down message, do stuff

			switch(wparam)
			{
				case VK_ESCAPE:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
						break;
						
				case VK_OEM_PLUS: // Increase the size of the triangles in the tri strip
					gTriSize += kIncTriSize;
						break;

				case VK_OEM_MINUS: // Decrease the size of the triangles in the tri strip

					if(gTriSize > kIncTriSize) // Don't allow our triangles too small
						gTriSize -= kIncTriSize;

					break;
			}

			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
				return 0;
    }

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

// Draws a quad in XY plane using a triangle strip
// "cen" is the center of the quad, "numStrips" is how many rows of triangles will make up the
// quad, "trisPerStrip" is the number of triangles in each triangle row, "triSize" is the 
// size of each triangle, "color" is the ARGB of the quad 
void DrawTessellatedQuad(const CPos &cen, int numStrips, int trisPerStrip, float triSize, DWORD color)
{
	assert(trisPerStrip % 2 == 0);
	
	int vertsPerStrip = trisPerStrip + 2; // Number of verts per strip
	int vertCount = 0; // Count of vertices being set and acts as an index into the global vertex pool
	
	// Get lower left corner of quad to be drawn
	float xStart = cen.x - (trisPerStrip / 2.0f * triSize) / 2.0f;
	float yStart = cen.y - (numStrips * triSize / 2.0f);
	
	// Start drawing from the upper left corner of the quad
	float xOffset = xStart;
	float yOffset = yStart;
	
	// Loop through all the rows of triangle strips
	for(int i = 0; i < numStrips; ++i)
	{	
		// Loop through all the vertices in each row 
		for(int j = 0; j < vertsPerStrip; j++)
		{
			if(j % 2 == 0)
			{
				gVertPool[vertCount].x = xOffset;
				gVertPool[vertCount].y = yOffset;
			}
			else
			{
				gVertPool[vertCount].x = xOffset;
				gVertPool[vertCount].y = yOffset + triSize;
				xOffset += triSize;
			}
			
			++vertCount; // Increment vertCount
		}
		
		// Make a degenerate triangle
		gVertPool[vertCount].x = gVertPool[vertCount - 1].x;
		gVertPool[vertCount].y = gVertPool[vertCount - 1].y;
		
		++vertCount;
		
		// Set offsets
		xOffset = xStart;
		yOffset += triSize;
		
		// Make a degenerate triangle
		gVertPool[vertCount].x = xOffset;
		gVertPool[vertCount].y = yOffset;
		
		++vertCount;
	}
	
	// Set all other values
	for(int i = 0; i < vertCount; ++i)
	{
		gVertPool[i].z = cen.z;
		gVertPool[i].color = color;
		gVertPool[i].nx = 0.0f;
		gVertPool[i].ny = 0.0f;
		gVertPool[i].nz = -1.0f;
	}
	
	// Draw the strip
	g3D->renderTriStrip(gVertPool, vertCount);
}

// Lights, Camera, Action...

/*
	Lights are essential to a top quality looking scene.  This tutorial just scratches the surface
	of what you can do with D3D lights.  Be sure to play with all the parameters in	the D3DLIGHT9
	struct to see the different lighting effects you can come up with.
	
	Comments, criticism, and most importantly questions should be directed to the forums at
	www.GameTutoirals.com	
*/
	
/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



