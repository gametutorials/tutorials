// Done by TheTutor

/*	
	Cartoon rendering, also known as toon-shading, also known as cel-shading, is a technique for 
	rendering an object as if it was a hand drawn cartoon.  In practice this happens in two separate ways.
	First, the border of the object is drawn with a black outline.  Secondly, a cartoon style shading
	is blended in with the objects color.
	
	We will acheive this effect by first loading two 1D textures to use for shading.  Then with 
	the combination of a vertex shader and a pixel shader, we will draw our toon-shaded objects.
	
	Continue reading for all the gory details!  
*/

#include <windows.h>
#include "d3d_obj.h"
#include "d3d_texture.h"

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

const char kClassName[] = "GT_D3DToonShading"; // WNDCLASSEX's name

const CPos kEyePos(0.0f, 0.0f, -5.0f); // Eye position of the camera
const CPos kLightPos(-1.5f, 1.5f, -1.5f); // The lights position in world space

//////////////
// Globals //
////////////

ID3DXMesh *gCylinder = NULL; // A D3D mesh object that will be a cylinder
ID3DXMesh *gTorus = NULL; // A D3D mesh object that will be a torus
ID3DXMesh *gLight = NULL; // A D3D mesh object that will be a sphere and represent our light's position

// These the two textures we will load to do the toon-shading with
CD3DTexture gShadeTex; // Controls how much the entire object is shaded
CD3DTexture gEdgeTex; // Controls how much the edge of the object is drawn in black

// Other helper functions.  Their names says it all %)
bool LockFrameRate(int frame_rate = 60);
bool CreateSphere(float radius, int color, ID3DXMesh **mesh);
bool CreateTorus(float innerRad, float outerRad, int color, ID3DXMesh **mesh);
bool CreateCylinder(float radius, float length, int color, ID3DXMesh **mesh);

// The standard WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Windows main
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
	MSG msg;
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
	
	// Window styles
	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- Toon Shading (HLSL)",
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
	
	if(!CreateSphere(0.05f, D3DCOLOR_XRGB(255,255,255), &gLight))
		return false; // Couldn't create the sphere... Something very bad happened...
		
	if(!CreateTorus(0.25f, 0.5f, D3DCOLOR_XRGB(128,16,16), &gTorus))
		return false; // Couldn't create the torus... Something very bad happened...
		
	if(!CreateCylinder(0.25f, 1.5f, D3DCOLOR_XRGB(128,16,16), &gCylinder))
		return false; // Could create the cylinder... This is very bad!

	// Try and load our textures.  Since we know both of textures are 16x1 pixels we
	// specify the heights because we want them to be exactly those dimensions
	if(!gShadeTex.load("shade.bmp", 16, 1) ||
	   !gEdgeTex.load("edge.bmp", 16, 1))
		return false; // No textures, no toon-shading
		
	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(kEyePos, CPos(0.0f, 0.0f, 0.0f));
	
	// Set our camera's eye position in the shader once since it will never change
	g3D->mEffect->SetFloatArray("gEyePos", &kEyePos.x, 3);
	
	// Set the light's world position in the shader once since it will never change
	g3D->mEffect->SetFloatArray("gLightPos", &kLightPos.x, 3);
	
	// Select the toon-shading textures into the shader
	gShadeTex.select("gShadeTexture");
	gEdgeTex.select("gEdgeTexture");

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

    while(1)
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
			static float angle = 0;
			D3DXMATRIX wMat, wRotate;

			g3D->begin();
			g3D->clear(D3DCOLOR_XRGB(196, 196, 32)); // Clear to yellow
			
			// Create the rotation matrix around the X, Y, and Z axis
			D3DXMatrixRotationYawPitchRoll(&wRotate, DEG2RAD(angle), DEG2RAD(angle), DEG2RAD(angle));
			
			// Draw the main cylinder of the dumbbell
			g3D->setWorldMatrix(&wRotate);
			g3D->render("ToonShading", gCylinder);
			
			// Translate and rotate first end of dumbbell
			D3DXMatrixTranslation(&wMat, 0, 0, 0.75f);
			D3DXMatrixMultiply(&wMat, &wMat, &wRotate);
			g3D->setWorldMatrix(&wMat);
			g3D->render("ToonShading", gTorus);
			
			// Translate and rotate second end of dumbbell
			D3DXMatrixTranslation(&wMat, 0, 0, -0.75f);
			D3DXMatrixMultiply(&wMat, &wMat, &wRotate);
			g3D->setWorldMatrix(&wMat);
			g3D->render("ToonShading", gTorus);

			// Draw the tiny sphere representing the light 
			D3DXMatrixTranslation(&wMat, kLightPos.x, kLightPos.y, kLightPos.z);
			g3D->setWorldMatrix(&wMat);
			g3D->render(gLight); 
			
			++angle; // Update our rotation angle
			
			g3D->end(); // Done with the scene
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other programs
	
	} // end of while(1)

	
	gLight->Release(); // Free up the light
	gTorus->Release(); // Free up the sphere
	gCylinder->Release(); // Free up the cylinder
	
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
	
	const unsigned int kSlices = 32;
	const unsigned int kStacks = 32;

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

// Creates a torus.  Returns true on success, false otherwise
bool CreateTorus(float innerRad, float outerRad, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);

	const unsigned int kSides = 32;
	const unsigned int kRings = 32;

	ID3DXMesh *tempMesh = NULL; // Temp D3D mesh object

	// Create the torus
	if(D3DXCreateTorus(g3D->mDevice, innerRad, outerRad, kSides, kRings, &tempMesh, NULL) != D3D_OK)
		return false;

	// Copy the torus, converting to our FVF
	if(tempMesh->CloneMeshFVF(D3DXMESH_VB_MANAGED, SVertexType, g3D->mDevice, mesh) != D3D_OK)
		return false;

	SVertex *v;

	// Lock the mesh's vertex buffer
	if((*mesh)->LockVertexBuffer(0, (void**)&v) != D3D_OK)
	{	
		(*mesh)->Release();
		return false;
	}

	// Set the color of our torus
	for(unsigned int i = 0; i < (*mesh)->GetNumVertices(); ++i)
		v[i].color = color;

	// Unlock the torus vertex buffer, we're ready to be drawn
	(*mesh)->UnlockVertexBuffer();

	tempMesh->Release(); // Free up the temporary mesh
	return true;
}

// Creates a sphere.  Returns true on success, false otherwise
bool CreateCylinder(float rad, float len, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);

	const unsigned int kSlices = 16;
	const unsigned int kStacks = 16;

	ID3DXMesh *tempMesh; // Temp D3D mesh object

	// Create the cylinder
	if(D3DXCreateCylinder(g3D->mDevice, rad, rad, len, kSlices, kStacks, &tempMesh, NULL) != D3D_OK)
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

	// Set the cylinder's color
	for(unsigned int i = 0; i < (*mesh)->GetNumVertices(); ++i)
		v[i].color = color;

	// Unlock the vertex data
	(*mesh)->UnlockVertexBuffer();

	tempMesh->Release(); // Free up the temporary mesh
		return true;	
}

// Don't toon this out...

/*
	Toon-shading can look great given the right context (Sly Cooper for instance).  These shading textures
	were created arbitrarily.  Tweaks to those textures could potentially make the shading better.  Also
	the color of the object was a simple diffuse color.  Adding a base texture with some color can add a
	lot of visual pizazz to your toon-shading.
	
	Questions, comments, rants, raves, and anything else should be posted at www.GameTutorials.com.
*/
	
/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/