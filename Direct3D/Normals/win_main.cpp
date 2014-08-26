// Done by TheTutor

/*	
	Inside of this tutorial we are going to explain what a normal is and why we care
	about 'em.  To kick it off lets start with some definitions:
	
		Normal:  A vector perpendicular to a surface (perpendicular to a plane)
		Normalized Vector:  A vector with a length of one
		Unit Normal:  A vector perpendicular to a surface with a length of one
		Face Normal:  A normal to a polygon that is pointing "out" from the polygon
					  **NOTE** Assuming a coordinate system of +X to the right, +Y up and
					  +Z into the screen, a vector that pointed "out" would point in the 
					  -Z direction (pointing out from the monitor)
		Vertex Normal:  A "normal" to a vertex which is computed by some sort of an average of all of
						the face normals that share the vertex
						**NOTE** Different graphics engine employ different averaging schemes.  The
								 simplest scheme is just taking a direct average of all the face 
								 normals.
		
	You'll see this vocab elsewhere so take a mental note :)  Now lets look at normals
	a little bit deeper.  Imagine we have a plane like this:
	
	|													   |
	|		A normal to this plane could be this:		<--|
	|													   |
	
	
	However this is also a normal to the plane:			|
														|-->
														|
														
	Any vector that is perpendicular to a plane is a normal to that plane.  The normal can begin
	at any point on the plane and be any length.  This alone doesn't help us too much, but here
	is where the concept of a face normal is introduced.  Imagine now that we have a box:
	 ____												 ____
	|	 |												|	 |
	|	 |		A face normal to the box would be:		|	 |-->
	|____|												|____|
	
	See how it points "out" from the box.  Usually in games a normal is stored as a unit normal.
	This simplifies equations in which normals are used.  By using normals we can do culling, lighting,
	collisions, decals, projected textures, normal mapping, etc.  While we won't get into how
	normals are utilized in all the previous topics, we will get into how you calculate a normal.
	
	So without further ado, lets get to the code.	
*/

#include <windows.h>
#include "d3d_obj.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/////////////
// Macros //
///////////

#define DEG2RAD(x) (x * (D3DX_PI / 180.0f)) // Converts degrees to radians

////////////////
// Constants //
//////////////

// Width and height of the window
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_D3DNormal"; // WNDCLASSEX's name

// The R, G, B of our triangle
const int kRed = 20;
const int kGreen = 20;
const int kBlue = 200;

// The amount to increment the angle by
const float kAngleInc = DEG2RAD(2);

//////////////
// Globals //
////////////

float gAnglePitch = 0.0f; // Angle of rotation in radians around the X-axis for the triangle
float gAngleYaw = DEG2RAD(30); // Angle of rotation in radians around the Y-axis for the triangle

// We set the verts of triangle so that it is roughly centered around the origin  
SVertex gTriangle[] = 
{	
	{ 1.0f, -0.5f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), },
	{ -1.0f,  -0.5f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), }, 
	{ 0.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), },
};

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void CalcAndDrawNormal(); // Calculates and then draws the normal to the triangle

// Standard WinProc()
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Standard WinMain()
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

    RegisterClassEx(&wndclassex); // Register the WINDCLASSEX

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE; // Window's extended style
	DWORD winStyle = WS_CAPTION | WS_SYSMENU; // Window style

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- Normals",
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

	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Set up our projection matrix
	// **NOTE** We only need to do this once cause we're not going to change it ever
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 1.0f, 8192.0f);
	
	// Set up our view matrix
	// **NOTE** We only need to do this once cause we're not going to change it ever
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f)); 

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running...
    while(1)
	{
		// Handle messages from the OS
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it's time to draw
		{
			g3D->begin(); // Begin the scene
			g3D->clear(); // Clear our viewport
		
			D3DXMATRIXA16 worldMat;
	
			// Create and matrix that rotates everything in the world by the specified 
			// yaw and pitch
			D3DXMatrixRotationYawPitchRoll(&worldMat, gAngleYaw, gAnglePitch, 0.0f);
			g3D->setWorldMatrix(&worldMat);
		
			CalcAndDrawNormal(); // Calculate and then draw the normal
			g3D->render(gTriangle, 3); // Draw the triangle
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Give the OS some time to process other things
	
	} // end of while(1)

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
		
			switch(wparam)
			{
				case VK_LEFT: // Rotate around the Y-axis
					gAngleYaw += kAngleInc;
						break;
					
				case VK_RIGHT: // Rotate around the Y-axis
					gAngleYaw -= kAngleInc;
						break;
					
				case VK_UP: // Rotate around the X-axis
					gAnglePitch += kAngleInc;
						break;
					
				case VK_DOWN: // Rotate around the X-axis
					gAnglePitch -= kAngleInc;
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

// Here is where most of the magic takes place
// This creates the normal to the triangle and then draws it
void CalcAndDrawNormal()
{
	/*
		To calculate the normal to a polygon we are going two vectors that make up the
		edges of the polygon.  But how do we choose these vectors?  Well pay close attention
		because the following is a HUGE stumbling block for those getting into 3D programming.
		First the concept of a winding order needs to be introduced.  The winding order of
		a polygon states in which direction, either clockwise or counterclockwise the vertices
		are stored in.  A left handed coordinate system (D3D) has it's vertices in a clockwise 
		winding order while a right handed coordinate system (OGL) has it's vertices in a 
		counter-clockwise winding order.
		
		The winding order of the vertices determines how you want to create your vectors.
		If you winding order is clockwise, you want to create your vectors so that the head
		of the first vector touches the tail of the second vector with a clockwise turn and then
		cross the second vector with the first.  Vice-versa	for a counter-clockwise winding order.
		Thus our vectors will be created as so:
		
		Vertices of triangle:
		
		  2
		  /\			Create the first vector from vert 0 to vert 1
		 /  \			Create the second vector from vert 1 to vert 2
		1----0
	*/
	
	// Create the vectors
	CVector vec1(gTriangle[1].x - gTriangle[0].x,
				 gTriangle[1].y - gTriangle[0].y,
				 gTriangle[1].z - gTriangle[0].z);
				 
	CVector vec2(gTriangle[2].x - gTriangle[1].x,
				 gTriangle[2].y - gTriangle[1].y,
				 gTriangle[2].z - gTriangle[1].z);
	
	// Compute the normal using the cross product
	// **NOTE** If you change the order in which you cross the vectors, you will
	//			have the normal point in the other direction
	CVector normal = vec1.cross(vec2); 
	
	normal.normalize(); // Make a unit normal
	
	// Set the verts for the line segment representing the unit normal
	SVertex verts[2] = { 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(255,255,0),
						 normal.x, normal.y, normal.z, D3DCOLOR_XRGB(255,255,0) };
						 
	g3D->renderLine(verts, 2); // Draw the normal
}

// Is this normal?

/*
	Normals are used for a wide number of things in game programming.  When computing
	your normal it is important to know the coordinate system you are using and making
	sure the winding order for your vertices is correct for that coordinate system.
	
	Questions and comments should be directed to the forums at www.GameTutorials.com	
*/
	
/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



