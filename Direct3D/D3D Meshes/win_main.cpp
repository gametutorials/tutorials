// Done by TheTutor

/*	
	Direct3D provides us with a ton of handy functions to do all sort of things.  DirectX has 
	it's own math libraries, texture format, and model format, all complete with easy 
	to use functions for doing the difficultest of 3D tasks.
	
	This tutorial is going to look at the D3D mesh.  Using this mesh object, we're going
	to create two shapes, a sphere, and a torus.
	
	And now lets go to the code...	
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

const char kClassName[] = "GT_D3DMesh"; // WNDCLASSEX's name

//////////////
// Globals //
////////////

// This is the declaration of our D3D mesh objects
ID3DXMesh *gSphere = NULL; // Our D3D mesh object
ID3DXMesh *gTorus = NULL; // Our 2nd D3D mesh object

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate

// This will create a sphere with "radius" and vertex color "color"
bool CreateSphere(float radius, int color, ID3DXMesh **mesh);

// This will create a torus (kinda looks like a doughnut) with a inner radius (the hole 
// part) of "innerRad", an outside radius of "outterRad", and vertex color "color"
bool CreateTorus(float innerRad, float outerRad, int color, ID3DXMesh **mesh);

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Main program
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
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- DirectX Mesh",
						  winStyle, // Window style
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left,
						  rect.bottom - rect.top,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);
						  
	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Init our global 3D object
	if(g3D->init(hwnd) == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint
	
	// Create the sphere	
	if(!CreateSphere(0.5f, D3DCOLOR_XRGB(0,25,225), &gSphere))
		return false; // Couldn't create the sphere... Something very bad happened...
	
	// Create the torus
	if(!CreateTorus(0.5f, 1.0f, D3DCOLOR_XRGB(225,25,25), &gTorus))
		return false; // Couldn't create the sphere... Something very bad happened...

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running...
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
			g3D->begin(); // Begin the frame
			g3D->clear(); // Clear the back buffer
			
			static float angle = 0;
			D3DXMATRIX wMat;
			
			// Set "wMat" to the identity matrix, then set the world matrix
			// to "wMat" so the sphere is centered in the world 
			D3DXMatrixIdentity(&wMat);
			g3D->setWorldMatrix(&wMat);
			g3D->render(gSphere); // Draw the sphere
			
			// Set "wMat" to a rotation around the X, Y, and Z axis, then 
			// set the world matrix to "wMat" so the torus rotates around all
			// three axises
			D3DXMatrixRotationYawPitchRoll(&wMat, DEG2RAD(angle), DEG2RAD(angle), DEG2RAD(angle));
			g3D->setWorldMatrix(&wMat);
			g3D->render(gTorus); // Draw the torus
			
			++angle; // Update the amount to rotate the torus by
			g3D->end(); // End the frame
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other programs
	
	} // end of while(1)

	gSphere->Release(); // Free up the sphere
	gTorus->Release(); // Free up the torus
	
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

// This functions creates a sphere of "radius" and vertex color "color"
bool CreateSphere(float radius, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);
	
	// This is the number of divisions the sphere will have vertically and
	// horizontally
	const unsigned int kSlices = 16; // Vertical divisions
	const unsigned int kStacks = 16; // Horizontal divisions

	ID3DXMesh *temp = NULL; // Temp D3D mesh object

	// Create the sphere
	// By parameter:
	// g3D->mDevice -- Pointer to the Direct3D device to be associated with the sphere
	// radius -- Radius of the sphere
	// kSlices -- Number of vertical divisions in the sphere
	// kStacks -- Number of horizontal divisions in the sphere
	// &temp -- A pointer to a ID3DXMesh*, it will get filled with the 
	//			the created mesh
	// NULL --  Optional pointer to a ID3DXBuffer, if a valid pointer was passed
	//			it would be filled with the adjacency information for each face in 
	//			the mesh.  By passing NULL, we say we don't want this information
	if(D3DXCreateSphere(g3D->mDevice, radius, kSlices, kStacks, &temp, NULL) != D3D_OK)
		return false;

	// Next we clone the mesh.  This does two things.  First, it allows us to
	// specify the vertex format we want on the cloned mesh.  Second, it copies the 
	// current mesh data into the ID3DXMesh we passed to this function.
	// By parameter:
	// D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED -- Flags specifying how we want the mesh to be
	//												cloned.  This particular flag combo says
	//												"Have the vertex buffer and index buffer
	//												associated with this mesh be in pooled memory
	//												that DirectX manages for us."
	// SVertexType -- Flexible vertex format that we want the cloned mesh to be converted to
	// g3D->mDevice -- IDirect3DDevice9 to associate this mesh with
	// mesh -- A pointer to a ID3DXMesh* that will get filled with the cloned mesh   
	if(temp->CloneMeshFVF(D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED, SVertexType, 
						  g3D->mDevice, mesh) != D3D_OK)
	{
		return false;
	}

	// Okay so up to this point we've created a stock D3D sphere, then converted 
	// it a sphere with the FVF that we want.  Now were going to 
	// loop through each vertex and set it to the color that we want it to be

	SVertex *v;

	// Lock the vertex buffer
	if((*mesh)->LockVertexBuffer(0, (void**)&v) != D3D_OK)
	{	
		(*mesh)->Release();
			return false;
	}

	// Loop through all the verts in the mesh, setting each one's 
	// color to the color passed into the function
	for(unsigned int i = 0; i < (*mesh)->GetNumVertices(); ++i)
		v[i].color = color;

	// Don't be stingy, unlock the vertex buffer so others can use it
	(*mesh)->UnlockVertexBuffer();

	temp->Release(); // Last but not least, free up the temporary mesh
		return true;	
}

// Create a torus with the parameters specified
// Returns true for success, false otherwise
bool CreateTorus(float innerRad, float outerRad, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);

	const unsigned int kSides = 16; // Number of divisions looking at the torus form the side
	const unsigned int kRings = 16; // Number of divisions looking at the torus from the top
									// so you can see the hole in the middle

	ID3DXMesh *temp = NULL; // Temp D3D mesh object

	// Create the torus
	// By paramter:
	// g3D->mDevice -- Pointer to the Direct3D device to be associated with the torus
	// innerRad -- Inner radius of the torus
	// outerRad -- Outside radius of the torus
	// kSides -- Number of sides in a cross-section of the torus 
	// kRings -- Number of rings in a cross-section of the torus
	// &temp -- A pointer to a ID3DXMesh*, it will get filled with the 
	//			the created mesh
	// NULL --  Optional pointer to a ID3DXBuffer, if a valid pointer was passed
	//			it would be filled with the adjacency information for each face in 
	//			the mesh.  By passing NULL, we say we don't want this information
	if(D3DXCreateTorus(g3D->mDevice, innerRad, outerRad, kSides, kRings, &temp, NULL) != D3D_OK)
		return false;

	// Next we clone the mesh.  This does two things.  First, it allows us to
	// specify the vertex format we want on the cloned mesh.  Second, it copies the 
	// current mesh data into the ID3DXMesh we passed to this function.
	// By parameter:
	// D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED -- Flags specifying how we want the mesh to be
	//												cloned.  This particular flag combo says
	//												"Have the vertex buffer and index buffer
	//												associated with this mesh be in pooled memory
	//												that DirectX manages for us."
	// SVertexType -- Flexible vertex format that we want the cloned mesh to be converted to
	// g3D->mDevice -- IDirect3DDevice9 to associate this mesh with
	// mesh -- A pointer to a ID3DXMesh* that will get filled with the cloned mesh   
	if(temp->CloneMeshFVF(D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED, SVertexType, 
						  g3D->mDevice, mesh) != D3D_OK)
	{
		return false;
	}

	// Okay so up to this point we've created a stock D3D torus, then converted 
	// it a torus with the FVF that we want.  Now were going to loop through each
	// vertex and set it to the color that we want it to be.

	SVertex *v;

	// Lock the vertex buffer
	if((*mesh)->LockVertexBuffer(0, (void**)&v) != D3D_OK)
	{	
		(*mesh)->Release();
		return false;
	}

	// Loop through all the verts in the mesh, setting each one's 
	// color to the color passed into the function
	for(unsigned int i = 0; i < (*mesh)->GetNumVertices(); ++i)
		v[i].color = color;

	// We're done with the vertex buffer, so unlock it so it may be used
	// by others
	(*mesh)->UnlockVertexBuffer();

	temp->Release(); // Last but not least, free up the temporary mesh
		return true;
}

// Hopefully this will mesh well... 

/*
	The ID3DXMesh is a great and easy to way to make stock shapes (like a sphere) or
	load in models.  One drawback with using D3D meshs is if you're going to want your
	3D application to run on something besides D3D.  Then you'd want to come up with your
	own model format.
	
	This tutorial is easily extendable to models.  Find your favourite D3D model (these
	are the ones with the ".x" extensions) on the Internet and see if you can get it
	to load into a ID3DXMesh. 
		
	Questions, comments, advice, and good gossip should all be posted to the forums
	at www.GameTutorials.com
*/
	

/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
