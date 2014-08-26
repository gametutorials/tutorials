// Done by TheTutor 

/*	
	Anyone wanna get lit?  If you answered "Yes", you are at the right place.  We are going to look
	at per-pixel lighting and how it is implemented.  To start the party we'll first look at
	what is known as the "standard lighting equation".  All right, the first thing you
	might ask yourself, is "Does lighting in a 3D app follow the rules of the real world?"  The
	answer to the question is no.  Why the implementation of the standard lighting equation
	has roots in how light works in the real world, at the end of the day it is an approximation
	that happens to look good.  Lets begin by looking at some topics associated with lighting:
	
		Lighting Equation (How the color the light shines is computed)
		Materials (How an object reflects/absorbs light)
		Light Types (Spot light, point light, etc)
		Attenuation (Falloff of light)
	
	This tutorial is only going to tackle the lighting equation.  So lets begin by looking at an
	overview of the lighting equation and breaking down it's individual components.
	
	LC = Diffuse + Specular + Ambient + Emissive
	
	LC -- The lit color.  This is the RGB value of the pixel to be drawn.  It color is 
		  arrived upon by a combination of the lighting equation and other colors of the
		  object such as texture map color, vertex color, etc.
	Diffuse -- The diffuse component of the light.  It models light that strikes
			   the surface of an object and is randomly scattered across that surface.
			   You can think of this as the lights main color.
	Specular -- The specular component of the light.  It also models light that strikes
				the surface of an object, however, it only models light that is directly
				reflected back to the viewer of the surface.  Have you ever seen the shiny
				white hot spots on a freshly waxed automobile.  Those hot spots are modeled
				as specular light.  Thus the specular component is the color of the any hot spots
				caused by the light
	Ambient -- In the real world, light waves keep on bouncing and bouncing and bouncing, so even
			   a dark room with no lights on you can typically still see a little bit, because 
			   light is entering the room after bouncing several times from some where.  The ambient
			   component models the lighting color, where no direct light strikes.			    
	Emissive -- Lastly, the emissive component is the light that is emanated from the source of 
				the light.  So for instance, the glow comes from a flourescent bulb would be 
				modeled by the emissive component.  We will ignore this component in the
				tutorial.
				
	Okay so that's a quick lighting equation overview, now time for some per-pixel splendor.  You'll
	find most of the per-pixel fuzzy stuff in the .fx file.
*/

#include <windows.h>
#include "d3d_obj.h"

// Add the following libraries
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/////////////
// Macros //
///////////

#define DEG2RAD(x) (x * (D3DX_PI / 180.0f)) // Converts degrees to radians
#define RAD2DEG(x) (x * (180.0f / D3DX_PI)) // Converts radians to degrees

////////////////
// Constants //
//////////////

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_D3DPerPixelLighting"; // WNDCLASSEX's name

const CPos kEyePos(0.0f, 2.5f, -5.0f); // Eye position of the camera

const CColor kDiffuse(1.0f, 1.0f, 1.0f); // Diffuse color of light
const CColor kSpecular(1.0f, 1.0f, 1.0f); // Specular color of light
const CColor kAmbient(0.1f, 0.1f, 0.1f); // Ambient color of light

//////////////
// Globals //
////////////

ID3DXMesh *gSphere = NULL; // A D3D mesh object that will be a sphere
ID3DXMesh *gLight = NULL; // A D3D mesh object (a small sphere) representing the position of the light
CPos gLightPos(-2.0f, 2.0f, -1.0f); // The lights position in world space

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
bool CreateSphere(float radius, int color, ID3DXMesh **mesh); // Creates a D3D mesh object as sphere

void UpdateLightPos(); // Moves the light position in a square pattern

// Standard WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Our Win32 main
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd; // Handle to our window
	MSG msg; // 
    WNDCLASSEX wndclassex = {0};

	// Init fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always set to size of WNDCLASSEX
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = kClassName;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);
    RegisterClassEx(&wndclassex);

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- Per-Pixel Lighting (HLSL)",
						  winStyle,
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
	
	if(!CreateSphere(0.05f, D3DCOLOR_XRGB(255,255,255), &gLight))
		return false; // Couldn't create the sphere... Something very bad happened...
		
	if(!CreateSphere(1.0f, D3DCOLOR_XRGB(0,128,0), &gSphere))
		return false; // Couldn't create the sphere... Something very bad happened...
		
	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(kEyePos, CPos(0.0f, 0.0f, 0.0f));
	
	// Set data for our light that will never change
	g3D->mEffect->SetFloatArray("gEyePos", &kEyePos.x, 3); // Set the camera's eye position
	g3D->mEffect->SetFloatArray("gDiffuse", &kDiffuse.r, 3); // Lights diffuse color
	g3D->mEffect->SetFloatArray("gSpecular", &kSpecular.r, 3); // Lights specular color
	g3D->mEffect->SetFloatArray("gAmbient", &kAmbient.r, 3); // Lights ambient color

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

    while(1) // While the app is alive
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // If the OS has a message for us
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // Else, if it's time to draw
		{
			D3DXMATRIX wMat; // This will hold the world matrix of objects we want rendered 

			g3D->begin(); // Begin the scene
			g3D->clear(D3DCOLOR_XRGB(0, 0, 64)); // Clear to dark blue
			
			// The light is going to move, so we need to
			// set it's new position every frame 
			g3D->mEffect->SetFloatArray("gLightPos", &gLightPos.x, 3);
			
			// Draw first sphere to be lit with vertex lighting on the 
			// left side of the screen
			D3DXMatrixTranslation(&wMat, -1.5, 0, 0);
			g3D->setWorldMatrix(&wMat);
			g3D->render("VertexLighting", gSphere);
			
			// Draw second sphere to be lit with per-pixel lighting on the 
			// right side of the screen
			D3DXMatrixTranslation(&wMat, 1.5, 0, 0);
			g3D->setWorldMatrix(&wMat);
			g3D->render("PerPixelLighting", gSphere);

			// Draw the tiny sphere representing the light as it 
			// moves around the world	
			D3DXMatrixTranslation(&wMat, gLightPos.x, gLightPos.y, gLightPos.z);
			g3D->setWorldMatrix(&wMat);
			g3D->render(gLight); 
			
			UpdateLightPos(); // Update the position of the light
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other programs
	
	} // end of while(1)

	gLight->Release(); // Free up the light
	gSphere->Release(); // Free up the sphere
	
	g3D->deinit(); // Free up the D3D object
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_KEYDOWN:
		
			// If the user presses ESC, close the app
			if(wparam == VK_ESCAPE)
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				
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

// Creates a sphere.  Returns true on success, false otherwise
bool CreateSphere(float radius, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);
	
	const unsigned int kSlices = 12;
	const unsigned int kStacks = 12;

	ID3DXMesh *tempMesh; // Temp D3D mesh object

	// Create the sphere
	if(D3DXCreateSphere(g3D->mDevice, radius, kSlices, kStacks, &tempMesh, NULL) != D3D_OK)
		return false;

	// Copy the sphere, converting to our FVF 
	if(tempMesh->CloneMeshFVF(D3DXMESH_VB_MANAGED, SVertexType, g3D->mDevice, mesh) != D3D_OK)
		return false;

	SVertex *v;

	// Lock the vertex data of our sphere
	if((*mesh)->LockVertexBuffer(0, (void**)&v) != D3D_OK)
	{	
		(*mesh)->Release();
			return false;
	}

	// Set the sphere's color
	for(unsigned int i = 0; i < (*mesh)->GetNumVertices(); ++i)
		v[i].color = color;

	// Unlock the vertex data
	(*mesh)->UnlockVertexBuffer();

	tempMesh->Release(); // Free up the temporary mesh
		return true;	
}

// Moves the light position in a square around the screen
void UpdateLightPos()
{
	// Bounding area of the light's position
	const float kMinX = -2.5f;
	const float kMinY = -2.0f;
	const float kMaxX = 2.5f;
	const float kMaxY = 2.0f;
	const float kInc = 0.01f;
	
	// This will representing a 2D vector for moving the light around
	static float vec[2] = { kInc, 0.0f };
	
	// Move the light
	gLightPos.x += vec[0];
	gLightPos.y += vec[1];
	
	// Clamp the light to it's bounding area 
	if(gLightPos.x > kMaxX)
	{	
		gLightPos.x = kMaxX;
		vec[0] = 0.0f;
		vec[1] = -kInc;
	}		
	else if(gLightPos.y < kMinY)
	{
		gLightPos.y = kMinY;
		vec[0] = -kInc;
		vec[1] = 0.0f;
	}
	else if(gLightPos.x < kMinX)
	{
		gLightPos.x = kMinX;
		vec[0] = 0.0f;
		vec[1] = kInc;
	}
	else if(gLightPos.y > kMaxY)
	{
		gLightPos.y = kMaxY;
		vec[0] = kInc;
		vec[1] = 0.0f;
	}
}

// Let the light shine in and cast away the darkness!

/*
	Per-pixel lighting is a great way to make things look a whole lot brighter and better.  Used
	in conjuction with bump mapping or parallax mapping, you can add the appearance of depth to 
	an otherwise flat surface.
	
	Questions?  Comments?  If so head over to the forums at www.GameTutorials.com and post away.
	You might just be surprised at how quickly you get a helpful answer!	
*/
	
/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/