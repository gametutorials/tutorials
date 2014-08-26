// Done by TheTutor 

/*	
	
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

// Handy macro for checking for asserting success in debug builds 
#ifdef _DEBUG
	#define ASSERT_TRUE(x) assert((x) == true);
#else
	#define ASSERT_TRUE(x) x
#endif

#define DEG2RAD(x) (x * (D3DX_PI / 180.0f)) // Converts degrees to radians
#define RAD2DEG(x) (x * (180.0f / D3DX_PI)) // Converts radians to degrees

////////////////
// Constants //
//////////////

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_D3DSAQ"; // WNDCLASSEX's name
const float k2PI = D3DX_PI * 2.0f;

const CPos kEyePos(0.0f, 4.0f, -4.0f); // World position of the eye of the camera

//////////////
// Globals //
////////////

CD3DTexture gDiffTex;
CD3DTexture gBlurTexture1;
CD3DTexture gBlurTexture2;
CD3DRenderTarget gRenderTarget;

float gBlurWeights[9] = {0};

bool gToggle = false;
bool LockFrameRate(int frame_rate = 60); // Locks the frame rate

void Make3x3GuassianDistribution(); 
void RenderToTexture();
void RenderGaussianBlur();
void RenderSobelFilter();
void Combined();
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
						  "www.GameTutorials.com -- Render Targets",
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
		
	if(!gBlurTexture1.createRenderTexture(256, 256, D3DFMT_A8R8G8B8))
		return false; // Couldn't create a off screen texture to render to
		
	if(!gBlurTexture2.createRenderTexture(256, 256, D3DFMT_A8R8G8B8))
		return false; // Couldn't create a off screen texture to render to
		
	if(!gDiffTex.load("texture.jpg"))
		return false;
					 
	if(!gRenderTarget.init(256, 256, D3DFMT_A8R8G8B8, D3DFMT_D24S8))
		return false; // Couldn't create a render texture... Does your video card support this?
		
	Make3x3GuassianDistribution();

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), 1.0f, 2048.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(kEyePos, CPos(0.0f, 0.0f, 0.0f));
	
	g3D->mShader->SetFloatArray("gEyePos", &kEyePos.x, 3); // Set the camera's eye position
	g3D->mShader->SetFloatArray("gBlurWeights", gBlurWeights, 9); // Gaussian blur weights

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
			// Render the off screen texture (created directly above) to another 
			// off screen texture using a Gaussian blur
			if(gToggle)
				Combined();
			else
				RenderSobelFilter();
		
			// Render the two textures to the screen
			RenderScene();
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other programs
	
	} // end of while(1)

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
				gRenderTarget.deinit();
				
				g3D->toggleFullScreen(); // Toggle!
				
				// Reinitialize the render texture and target
				gRenderTarget.init(256, 256, D3DFMT_A8R8G8B8, D3DFMT_D24S8);
				
				g3D->setViewMatrix(kEyePos, CPos(0.0f, 0.0f, 0.0f)); // Reset the view of our scene
					return 0;
			}

			break; // Allow other system keys to be handled by DefWindowProc()
    
		case WM_KEYDOWN:
					
			switch(wparam)
			{
				case VK_ESCAPE:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
						break;
						
				case VK_F8:
					gToggle = !gToggle;
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

// r == (u² + v²) == blur radius
// N == Number of dimensions
// S == Standard deviation
// G(u,v) = (1 / (2pi * S²)^(N/2)) * e^(-r² / 2S²)
void Make3x3GuassianDistribution()
{
	// Assume standard deviation is 1
	// We know number of dimensions is 2

	int c = 0;
	const float stdSqd = 0.99f * 0.99f;

	for(int i = -4; i <= 4; ++i)
	{
		for(int j = -4; j <= 4; ++j)
		{
			gBlurWeights[c] += (1.0f / (k2PI * stdSqd)) * expf(-(i*i + j*j) / (2.0f * stdSqd));
		}
		
		c++;
	}
}

void Combined()
{
	// Begin rendering the Sobel Filter along the X-axis
	gRenderTarget.begin(&gBlurTexture2);
	gRenderTarget.clear(D3DCOLOR_XRGB(255, 255, 255));
	
	// Select unfiltered render target to the left side of the screen
	gDiffTex.select();
	g3D->renderSAQ(0, 0, 256, 256, "GuassianBlurVert");
		
	gRenderTarget.end(); // Finish rendering to the shadow map
	
	// Begin rendering the Sobel Filter along the Y-axis
	gRenderTarget.begin(&gBlurTexture1);
	
	gRenderTarget.clear(D3DCOLOR_XRGB(255, 255, 255));
	
	// Select unfiltered render target to the left side of the screen
	gBlurTexture2.select();
	g3D->renderSAQ(0, 0, 256, 256, "GuassianBlurHorz");
		
	gRenderTarget.end(); // Finish rendering to the shadow map
	
	////// Sobel
	
	// Begin rendering the Sobel Filter along the X-axis
	gRenderTarget.begin(&gBlurTexture2);
	gRenderTarget.clear(D3DCOLOR_XRGB(255, 255, 255));
	
	// Select unfiltered render target to the left side of the screen
	gBlurTexture1.select();
	g3D->renderSAQ(0, 0, 256, 256, "SobelFilter");
		
	gRenderTarget.end(); // Finish rendering to the shadow map
}

void RenderSobelFilter()
{
	// Begin rendering the Sobel Filter along the X-axis
	gRenderTarget.begin(&gBlurTexture2);
	gRenderTarget.clear(D3DCOLOR_XRGB(255, 255, 255));
	
	// Select unfiltered render target to the left side of the screen
	gDiffTex.select();
	g3D->renderSAQ(0, 0, 256, 256, "SobelFilter");
		
	gRenderTarget.end(); // Finish rendering to the shadow map
}


void RenderGaussianBlur()
{
	// Begin rendering the Sobel Filter along the X-axis
	gRenderTarget.begin(&gBlurTexture1);
	gRenderTarget.clear(D3DCOLOR_XRGB(255, 255, 255));
	
	// Select unfiltered render target to the left side of the screen
	gDiffTex.select();
	g3D->renderSAQ(0, 0, 256, 256, "GuassianBlurVert");
		
	gRenderTarget.end(); // Finish rendering to the shadow map
	
	// Begin rendering the Sobel Filter along the Y-axis
	gRenderTarget.begin(&gBlurTexture2);
	
	gRenderTarget.clear(D3DCOLOR_XRGB(255, 255, 255));
	
	// Select unfiltered render target to the left side of the screen
	gBlurTexture1.select();
	g3D->renderSAQ(0, 0, 256, 256, "GuassianBlurHorz");
		
	gRenderTarget.end(); // Finish rendering to the shadow map
}

void RenderScene()
{
	g3D->begin(); // Begin the scene
	g3D->clear(D3DCOLOR_XRGB(0,0,0)); // Clear the frame buffer color, z-buffer, and stencil buffer
			
	// Select unfiltered render target to the left side of the screen
	gDiffTex.select();
	g3D->renderSAQ(0,0,256,256);
	
	gBlurTexture2.select();
	g3D->renderSAQ((float)kWinWid-256,0,256,256);

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