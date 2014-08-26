// Done by TheTutor 

/*	
	Certainly games use complicated shadowing techniques or combinations of multiple shadowing techniques, 
	but the basic Xbox360/PS3 shadowing technique is called shadow mapping.
	
	So before we look at how to do shadow mapping.  Lets take a peak at the pros and cons:
	
	Pros:
		* Can render shadows on any surface
		* Self shadowing
		* Pretty good results
		
	Cons:
		* Due to Z-depth precision limitations can have Z-achene (we help conceal this with an offset)
		* Due to texture resolutions limitations, close lights don't render nice shadow maps and their
		  shadows appear cut off
		* The light source only casts shadow in one direction so it can only be a directional type light
		
	There are other flavors of shadow mapping that bandage some of these cons and are worth checking out.
	But for now, lets get the basics coded... 
	
	There are two high level steps to do when shadow mapping:
	
		1)  Render the scene, for all objects that cast shadows, from the perspective of the light 
			into a texture
		2)  Render the scene like normal, for all objects that receive shadows, doing a lookup into
			the shadow texture to determine the "shadow color" for each pixel drawn
			
	See that's not too bad :)  Like all good programming exercises, the devil is in the details, so lets
	look at our steps in a bit more detail.
	
		1)  Create a texture to for the shadow map
		2)  Create a render target interface for drawing to the shadow map texture
		3)  Calculate the view and projection matrices for the light
		4)  Select the shadow map as the current render target
		5)  Render the scene from the perspective of the light, recording the depth of each pixel
		6)  Set the view and projection matrices of the camera
		7)  Render the scene, with per-pixel lighting and multiplying the "shadow color" of each
			pixel pulled from the shadow map.  (All of this take place in the shader!)
			
	Now that we have our game plan, lets get down to business 
*/

#include <windows.h>
#include "d3d_obj.h"
#include "d3d_rendertarget.h"

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

const char kClassName[] = "GT_D3DShadowMapping"; // WNDCLASSEX's name

const CPos kEyePos(0.0f, 5.0f, -7.5f); // World position of the eye of the camera
const CPos kGroundPos(0.0f, -1.0f, 0.0f); // World position of the ground
const CPos kBoxPos(0.0f, 0.0f, -1.0f); // World position of the box
const CPos kSherePos(2.0f, 0.0f, 2.0f); // World position of the sphere

const CColor kDiffuse(1.0f, 1.0f, 1.0f); // Diffuse color of light
const CColor kSpecular(1.0f, 1.0f, 1.0f); // Specular color of light
const CColor kAmbient(0.15f, 0.15f, 0.15f); // Ambient color of light

//////////////
// Globals //
////////////

CD3DTexture gShadowMap;
CD3DRenderTarget gRenderTarget;

ID3DXMesh *gSphere = NULL; // A D3D mesh object that will be a sphere
ID3DXMesh *gBox = NULL; // A D3D mesh object that will be a box
ID3DXMesh *gGround = NULL; // A D3D mesh object that will be the ground
ID3DXMesh *gLight = NULL; // A D3D mesh object (a small sphere) representing the position of the light

// This represents the position of the light in world space 
D3DXVECTOR3 gLightPos(0.0f, 2.0f, -5.0f);

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
bool CreateSphere(float radius, int color, ID3DXMesh **mesh); // Creates a D3D mesh sphere
bool CreateBox(float w, float h, float d, int color, ID3DXMesh **mesh); // Creates a D3D mesh box

void RenderShadowMap();
void RenderScene();

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
						  "www.GameTutorials.com -- Shadow Mapping (HLSL)",
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
	
	if(!CreateBox(16.0f, 0.05f, 16.0f, D3DCOLOR_XRGB(225, 225, 255), &gGround))
		return false; // Couldn't create the ground... Time to bail
				
	if(!gShadowMap.createRenderTexture(512, 512, D3DFMT_R32F))
		return false; // Couldn't create a shadow map texture.  Your video card 
					 // probably doesn't support the D3DFMT_R32F format.  You will need a beefier card to
					// run this tutorial
					 
	if(!gRenderTarget.init(512, 512, D3DFMT_R32F, D3DFMT_D24S8))
		return false; // Couldn't create a shadow map texture.  Your video card doesn't support render
					 // targets or it probably doesn't support the D3DFMT_R32F format.  You will need
					// a beefier card to run this tutorial

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), 1.0f, 2048.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(kEyePos, CPos(0.0f, 0.0f, 0.0f));
	
	D3DXMATRIXA16 projMat;
	
	// Create texture projection matrix
	D3DXMatrixPerspectiveFovLH(&projMat, DEG2RAD(60), 1.0f, 0.1f, 100.0f);
	
	// Set data in our shader that will never change
	g3D->mShader->SetFloatArray("gLightProjMat", &projMat._11, 16);	
	
	g3D->mShader->SetFloatArray("gEyePos", &kEyePos.x, 3); // Set the camera's eye position
	g3D->mShader->SetFloatArray("gDiffuse", &kDiffuse.r, 3); // Lights diffuse color
	g3D->mShader->SetFloatArray("gSpecular", &kSpecular.r, 3); // Lights specular color
	g3D->mShader->SetFloatArray("gAmbient", &kAmbient.r, 3); // Lights ambient color

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
			g3D->mShader->SetFloatArray("gLightPos", &gLightPos.x, 3);
			
			// Render the shadow map
			RenderShadowMap();
		
			// Render the ground, box and sphere with shadows
			RenderScene();
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
				// Shut down are render texture and render target
				gShadowMap.deinit();
				gRenderTarget.deinit();
				
				g3D->toggleFullScreen(); // Toggle!
				
				// Reinitialize the render texture and target
				gShadowMap.createRenderTexture(512, 512, D3DFMT_R32F);
				gRenderTarget.init(512, 512, D3DFMT_R32F, D3DFMT_D24S8);
				
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
		{
			PostQuitMessage(0);
				return 0;
		}
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

void RenderShadowMap()
{
	D3DXMATRIX wMat; // world matrix for each object we want to render
	
	// Begin rendering to the shadow map texture
	gRenderTarget.begin(&gShadowMap);
	gRenderTarget.clear(D3DCOLOR_XRGB(255, 255, 255));
	
	D3DXMATRIXA16 viewMat;
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); // Point where light is focused
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // World's up vector
	
	// Create view matrix for light
	// **NOTE** It is very important to remember that we are treating the light as
	// a point light when rendering the objects, but for shadow casting, we are 
	// assuming the light is always shining towards (0,0,0).  Keep that in mind when thinking about
	// if the shadows look correct or not.
	D3DXMatrixLookAtLH(&viewMat, &gLightPos, &target, &up);
	
	// Set the view matrix of the light
	g3D->mShader->SetFloatArray("gLightViewMat", &viewMat._11, 16);	
	
	// Set world matrix of box and render
	D3DXMatrixTranslation(&wMat, kBoxPos.x, kBoxPos.y, kBoxPos.z);
	g3D->setWorldMatrix(&wMat);
	g3D->render("ShadowMap", gBox);
	
	// Set world matrix of sphere and render
	D3DXMatrixTranslation(&wMat, kSherePos.x, kSherePos.y, kSherePos.z);
	g3D->setWorldMatrix(&wMat);
	g3D->render("ShadowMap", gSphere);
		
	gRenderTarget.end(); // Finish rendering to the shadow map
}

void RenderScene()
{
	g3D->begin(); // Begin the scene
	g3D->clear(D3DCOLOR_XRGB(0,0,25)); // Clear the frame buffer color, z-buffer, and stencil buffer
			
	D3DXMATRIX wMat; // world matrix for each object we want to render
	
	// Select the shadow map as the active texture
	gShadowMap.select();
	
	// Translate and draw the ground
	D3DXMatrixTranslation(&wMat, kGroundPos.x, kGroundPos.y, kGroundPos.z);
	g3D->setWorldMatrix(&wMat);
	g3D->render("PerPixelLightingWithShadowMap", gGround);
			
	// Translate and draw the box
	D3DXMatrixTranslation(&wMat, kBoxPos.x, kBoxPos.y, kBoxPos.z);
	g3D->setWorldMatrix(&wMat);
	g3D->render("PerPixelLightingWithShadowMap", gBox);
	
	// Translate and draw the sphere		
	D3DXMatrixTranslation(&wMat, kSherePos.x, kSherePos.y, kSherePos.z);
	g3D->setWorldMatrix(&wMat);
	g3D->render("PerPixelLightingWithShadowMap", gSphere);
	
	// Draw the tiny sphere representing the light
	D3DXMatrixIdentity(&wMat);
	D3DXMatrixTranslation(&wMat, gLightPos.x, gLightPos.y, gLightPos.z);
	g3D->setWorldMatrix(&wMat);
	g3D->render(gLight);
	
	g3D->end();
}

// Is that a hole in my shadow map !?!

/*
	While shadow maps are nice, they aren't without their limitations.  Most 3D applications still use a
	combined approach of shadowing techniques depending on what is being rendered.  Lots of people have
	spent lots of time coming up with a slew of different shadowing techniques and algorithms.  Shadow 
	mapping serves as a basis for a number of the other algorithms out there.
	
	Questions?  Post 'em at www.GameTutorials.com!
*/
	
/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2007 GameTutorials, LLC |
\*-------------------------*/