// Done by TheTutor 

/*	
	Shadows are where evil lurks and where a good real-time rendered scene can become great.  Now days
	your favorite video game most likely has shadows in some form or another.
	
	This tutorial is going to look at a solid, while still being pretty easy, technique for doing
	shadows.  Typically this technique is referred to as "planar shadows".  It gets it name because
	it is casts a shadow of an object onto a plane.
	
	So how does it work?  Well for starters we will need to create the "shadow matrix".  The
	shadow matrix is a matrix which projects geoemtry onto a plane (for us this will be 
	the plane representing the ground).  It works very similar to the projection matrix, however
	in this case, the shadow matrix will project our geoemtry to a 2D plane defined in world 
	space.  Once we have the shadow matrix, we can multiply the world matrix by it thus giving
	us the final world coordinates of each vertex to render.
	
	Secondly, when drawing the shadows we want draw over any geometry.  Using the stencil buffer, 
	we will record where we have drawn geometry and only draw shadows in areas where there are 
	no other rendered objects.  So what is a stencil buffer?  Well it's a buffer that is similar
	to a z-buffer and can be used to mask off the rendering of pixels.  Lets do a quick review...
	
	If we make a window that is 800 x 600, this means that the window is 800 pixels wide by 600 pixels
	in height and the frame buffer is also 800 x 600 with each value in the 2D array representing 
	a pixel of the form RGBA.  The Z-buffer is also an 800 x 600 2D with each value representing the 
	depth, in world space, of the rendered pixel.  Likewise a stencil buffer is also an 800 x 600 2D 
	array.  For this tutorial each value in the stencil	buffer will be 8-bits in size which means 
	the values for each element in the stencil buffer will range from (0 - 255).  For this tutorial, 
	zero will mean "Hey you can render a shadow here", and any other positive number will mean 
	"Do NOT render a shadow here".  In summary, a stencil buffer is a buffer that can be used
	as a mask to determine if pixels should be rendered or not.
	
	To showcase planar shadows we will use two rendering passes and a combination of vertex and pixel
	shaders.  The first render pass will draw objects with per-pixel lighting.  The second render
	pass will draw the planar shadow of the object.  To move around the light and effect the
	shadow being rendered use the up/down/left/right arrow keys.
	
	Now on to the code...  
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

const char kClassName[] = "GT_D3DShadows"; // WNDCLASSEX's name

const CPos kEyePos(0.0f, 10.0f, -5.0f); // Eye position of the camera

const CColor kDiffuse(1.0f, 1.0f, 1.0f); // Diffuse color of light
const CColor kSpecular(1.0f, 1.0f, 1.0f); // Specular color of light
const CColor kAmbient(0.05f, 0.05f, 0.05f); // Ambient color of light

/*
	Here we define a D3DXPLANE for our ground plane.  For the ground, we are going
	to draw a VERY thin box centered around (0,-1,0).  It will rest in the XZ plane which
	means the normal to the plane is (0,1,0).  
 
	A D3DXPLANE consists of two values:  the normal to the plane and the distance to the
	plane from the origin.  So using the plane equation Ax + By + Cz = -D, (A,B,C) is the normal 
	and -D is the signed distance from the origin.
	
	The normal to the plane is easy to set because we've defined it to be (0,1,0) because the
	ground will rest in the XZ plane.  To calculate the signed distance we need to take the 
	dot product of the normal to the plane and any point on the plane.  Again we know the 
	normal to the plane and we know the plane resides -1 units on the Y-axis thus (0,-1,0) must
	be a point on the plane.
*/ 
const D3DXPLANE kGroundPlane(0,1,0, -CVector(0,1,0) * CVector(0,-1,0)); // Set the plane

//////////////
// Globals //
////////////

ID3DXMesh *gSphere = NULL; // A D3D mesh object that will be a sphere
ID3DXMesh *gBox = NULL; // A D3D mesh object that will be a box
ID3DXMesh *gGround = NULL; // A D3D mesh object that will be the ground
ID3DXMesh *gLight = NULL; // A D3D mesh object (a small sphere) representing the position of the light

// This represents the position of the light in world space.  So why do we use a 4D vector?
// Well to calculate the shadow matrix, we are going to use the D3D function D3DXMatrixShadow().
// This function uses the 4th component of the vector to say whether the light is a point
// light or a directional light.  Since the light we are using is a point light we set the
// 4th component of the 4D vector to 1.0f
D3DXVECTOR4 gLightPos(0.0f, 5.0f, 0.0f, 1.0f);

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
bool CreateSphere(float radius, int color, ID3DXMesh **mesh); // Creates a D3D mesh sphere
bool CreateBox(float w, float h, float d, int color, ID3DXMesh **mesh); // Creates a D3D mesh box 

// Standard WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Our Win32 main
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd = NULL; // Handle to our window
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
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- Planar Shadows (HLSL)",
						  winStyle,
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
	
	if(!CreateSphere(0.05f, D3DCOLOR_XRGB(255,255,0), &gLight))
		return false; // Couldn't create the sphere... Something very bad happened...
		
	if(!CreateSphere(0.75f, D3DCOLOR_XRGB(0,128,64), &gSphere))
		return false; // Couldn't create the sphere... Something very bad happened...
		
	if(!CreateBox(1.0f, 2.0f, 1.0f, D3DCOLOR_XRGB(0, 128, 64), &gBox))
		return false; // Couldn't create the box... This isn't good!
	
	if(!CreateBox(16.0f, 0.05f, 16.0f, D3DCOLOR_XRGB(200, 200, 200), &gGround))
		return false; // Couldn't create the ground... Time to bail

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(kEyePos, CPos(0.0f, 0.0f, 0.0f));
	
	// Set data in our shader that will never change
	g3D->mEffect->SetFloatArray("gEyePos", &kEyePos.x, 3); // Set the camera's eye position
	g3D->mEffect->SetFloatArray("gDiffuse", &kDiffuse.r, 3); // Lights diffuse color
	g3D->mEffect->SetFloatArray("gSpecular", &kSpecular.r, 3); // Lights specular color
	g3D->mEffect->SetFloatArray("gAmbient", &kAmbient.r, 3); // Lights ambient color

	// Show and update the window
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
			static int angle = 0; // Stores the amount rotated in degrees
			D3DXMATRIX tMat, rMat; // This will hold translation and rotation matrices
			D3DXMATRIX wMat; // This will hold the world matrix of objects we want rendered
			D3DXMATRIX shadowMat; // The matrix for projecting the shadow to a plane
			
			// The light is going to potentially move, so we need to
			// set it's position every frame 
			g3D->mEffect->SetFloatArray("gLightPos", &gLightPos.x, 3);
			
			// Calculate the shadow matrix using D3D's handy utility function
			D3DXMatrixShadow(&shadowMat, &gLightPos, &kGroundPlane);
			
			// Set the shadow matrix
			g3D->mEffect->SetMatrix("gShadowMatrix", &shadowMat);
					
			g3D->begin(); // Begin the scene
			g3D->clear(); // Clear the frame buffer color, z-buffer, and stencil buffer
			
			// Draw the ground first
			D3DXMatrixTranslation(&wMat, 0.0f, -1.0f, 0);
			g3D->setWorldMatrix(&wMat);
			g3D->render(gGround);
			
			// Draw a box centered around the origin of the world
			D3DXMatrixIdentity(&wMat);
			g3D->setWorldMatrix(&wMat);
			g3D->render("PerPixelLightingWithShadows", gBox);
			
			// Draw the sphere to be lit with per-pixel lighting
			D3DXMatrixTranslation(&tMat, 2.0f, 0, 0);
			D3DXMatrixRotationY(&rMat, DEG2RAD(angle++)); 
			D3DXMatrixMultiply(&wMat, &tMat, &rMat);
			g3D->setWorldMatrix(&wMat);
			g3D->render("PerPixelLightingWithShadows", gSphere);

			// Draw the tiny sphere representing the light 
			D3DXMatrixTranslation(&wMat, gLightPos.x, gLightPos.y, gLightPos.z);
			g3D->setWorldMatrix(&wMat);
			g3D->render(gLight);
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other programs
	
	} // end of while(1)

	gLight->Release(); // Free up the light
	gSphere->Release(); // Free up the sphere
	gBox->Release(); // Free up the box
	gGround->Release(); // Free up the ground
	
	g3D->deinit(); // Free up the D3D object
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	const float kMoveAmt = 0.05f; // Amount to move light by

	switch(message)
    {
		case WM_SYSKEYDOWN:

			// Toggle on ALT + ENTER				
			if(wparam == VK_RETURN && (lparam & (1 << 29)))
			{
				g3D->toggleFullScreen();
				g3D->setViewMatrix(kEyePos, CPos(0.0f, 0.0f, 0.0f)); // Reset the view of our scene
					return 0;
			}

			break; // Allow other system keys to be handled by DefWindowProc()
    
		case WM_KEYDOWN:
					
			switch(wparam)
			{
				case VK_LEFT: // Move the light along the -X axis
					gLightPos.x -= kMoveAmt;
						break;
						
				case VK_RIGHT: // Move the light along the +X axis
					gLightPos.x += kMoveAmt;
						break;
						
				case VK_UP: // Move the light along the +Z axis
					gLightPos.z += kMoveAmt;
						break;
						
				case VK_DOWN: // Move the light along the -Z axis
					gLightPos.z -= kMoveAmt;
						break;
					
				case VK_ESCAPE:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
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

// Creates a sphere.  Returns true on success, false otherwise
bool CreateSphere(float radius, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);
	
	const unsigned int kSlices = 16;
	const unsigned int kStacks = 16;

	ID3DXMesh *tempMesh; // Temp D3D mesh object

	// Create the sphere
	if(D3DXCreateSphere(g3D->mDevice, radius, kSlices, kStacks, &tempMesh, NULL) != D3D_OK)
		return false;
		
	// Flag for how to create the D3D mesh.  We want the vertex buffer and index
	// buffer memory to be managed by DirectX	
	DWORD flag = D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED;

	// Copy the sphere, converting to our FVF 
	if(tempMesh->CloneMeshFVF(flag, SVertexType, g3D->mDevice, mesh) != D3D_OK)
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

// Using the passed in parameters, w == width, h == height, d == depth, and color
// creates a D3D mesh object that is a box.
// Returns true of success, false otherwise
bool CreateBox(float w, float h, float d, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);
	
	ID3DXMesh *tempMesh; // Temp D3D mesh object

	// Create the sphere
	if(D3DXCreateBox(g3D->mDevice, w, h, d, &tempMesh, NULL) != D3D_OK)
		return false;
	
	// Flag for how to create the D3D mesh.  We want the vertex buffer and index
	// buffer memory to be managed by DirectX	
	DWORD flag = D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED;

	// Copy the sphere, converting to our FVF 
	if(tempMesh->CloneMeshFVF(flag, SVertexType, g3D->mDevice, mesh) != D3D_OK)
		return false;

	SVertex *v;

	// Lock the vertex data of our box
	if((*mesh)->LockVertexBuffer(0, (void**)&v) != D3D_OK)
	{	
		(*mesh)->Release();
			return false;
	}

	// Set the box's color
	for(unsigned int i = 0; i < (*mesh)->GetNumVertices(); ++i)
		v[i].color = color;

	// Unlock the vertex data
	(*mesh)->UnlockVertexBuffer();

	tempMesh->Release(); // Free up the temporary mesh
		return true;
}

// As I walk through the valley of the shadow of death...

/*
	Real-time rendering needs real time shadows to look fantastic!  Planar shadows provide an
	easy, straightforward approach for hard shadows on flat surfaces.  If the scene was a room 
	containing four walls and the ground, you would simply apply the same technique to each wall
	and the ground.  Of course if there are a lot of flat surfaces that need shadows cast on them
	(such as a outdoor terrain generated from a height map), this can become expensive.  In that 
	case other advanced lighting techniques, such as shadow mapping, may better suit your rendering
	needs.
	
	If you have questions, doubts, helpful insights, or concerns, feel free to post 'em on the 
	forums at:  www.GameTutorials.com		
*/
	
/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/