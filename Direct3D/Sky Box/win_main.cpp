// Done by TheTutor

/*	
	What lies herein, is an example of how to do sky boxes.  Before we get to far, it's
	important to understand what a sky box is.  A sky box, is a large cube that is 
	wrapped around your entire world environment and textured in such a way that it
	gives the appearance of geometry (usually a sky and landscape) in the distance.
	
	Sky boxes are an easy and effective way to portray a sky and some far off land
	mass in a game.  They've been used by countless games, and will probably be used
	by countless more.  One very important thing to remember when creating a sky box
	is that it's intended that the player is inside the sky box at all times.  This 
	is crucial on how you go about constructing the sky box.
	
	And with that, lets get coding...
*/

#include <windows.h>
#include <assert.h>
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

const char kClassName[] = "GT_SkyBox"; // WNDCLASSEX's name

const int kMaxVerts = 24; // Number of vertices needed to make our sky box
						 // How do we get 24?  Well you need a minimum of 
						// 8 vertices to draw a box.  Draw it out on paper 
					   // if you don't believe me %)  Now once you have that
					  // box, you'll need 3 sets of texture coordinates per
					 // vertex, because each vertex will get used by 3 different
					// sides of the sky box.  So 8 * 3 = 24.
const int kMaxTextures = 6; // A 3D box has 6 sides, thus we'll need 6 different textures

// Amount to move camera via keyboard input
// We're locking the frame rate to 60 frames a second, so every second we 
// we want to move no more than 10 units
const float kMoveAmt = 10.0f * (1.0f / 60.0f);

//////////////
// Globals //
//////////// 

// The vertices that we'll use to draw our sky box to the screen
SVertex gCubeVerts[kMaxVerts] = {0};

// The textures that we'll use to map to our quad
CD3DTexture gTexture[kMaxTextures];

bool LockFrameRate(int fps = 60); // Locks the frame rate to the FPS passed in
void CameraMouseInput(); // Handles camera input

// Loads all of the textures needed for the sky box into
// the global texture array
bool LoadTextures();

// Fills a SVertex with the passed in values
void FillSVertex(SVertex *vert, float X, float Y, float Z, float U, float V);

// Fills "verts" with the vertices to make a 3D sky box centered around "center"
// With 'w' width, 'h' height, and 'd' depth
void CreateSkyBox(SVertex *verts, const CPos &center, float w, float h, float d);

// Renders our sky box
void RenderSkyBox(); 

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
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
						  "www.GameTutorials.com -- Sky Box",
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
		
	if(LoadTextures() == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint

	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// We set up our projection matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// Create our sky box
	CreateSkyBox(gCubeVerts, CPos(0.0f, 0.0f, 0.0f), 2000.0f, 2000.0f, 2000.0f);
	
	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);
	
	ShowCursor(FALSE); // Hide the cursor

    while(1) // While the app is running...
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Handle messages from the OS
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it is time to render, do so
		{
			CameraMouseInput(); // Update the camera via mouse control
		
			g3D->begin(); // Begin the scene
			g3D->clear(); // Clear the Z-buffer
			
			// Set view matrix
			g3D->setViewMatrix(gCamera);
			
			RenderSkyBox(); // Draw the sky box
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Otherwise give the OS a little bit of time to process other things
	
	} // end of while(1)
	
	ShowCursor(TRUE); // Reshow the cursor
	
	g3D->deinit(); // Free up CD3DObj
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_SYSKEYDOWN:

			// Toggle on ALT + ENTER				
			if(wparam == VK_RETURN && (lparam & (1 << 29)))
				g3D->toggleFullScreen();

			break; // Allow other system keys to be handled by DefWindowProc()
    
		case WM_KEYDOWN: // If we get a key down message, do stuff

			switch(wparam)
			{
				case VK_ESCAPE:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
						break;
						
				case 'C':
					gCamera->reset(); // Recenter the camera
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

// Handles camera mouse input
void CameraMouseInput() 
{
	const float kMaxAngle = 89.5f; // Max degree of rotation
	static float pitchAmt = 0.0f; // Amout we've looked up or down

	// Get the middle of the screen
	int midScrX = GetSystemMetrics(SM_CXSCREEN) >> 1;
	int midScrY = GetSystemMetrics(SM_CYSCREEN) >> 1;

	float amt;
	POINT pt = {0};

	GetCursorPos(&pt); // Get the current mouse position

	// Rotate left/right
	amt = float(pt.x - midScrX) * kMoveAmt;
	gCamera->rotateY(DEG2RAD(amt));

	// Calculate amount to rotate up/down
	amt = float(midScrY - pt.y) * kMoveAmt;

	// Clamp pitch amount
	if(pitchAmt + amt <= -kMaxAngle)
	{
		amt = -kMaxAngle - pitchAmt;
		pitchAmt = -kMaxAngle;
	}
	else if(pitchAmt + amt >= kMaxAngle)
	{
		amt = kMaxAngle - pitchAmt;
		pitchAmt = kMaxAngle;
	}
	else
	{
		pitchAmt += amt;
	}

	// Pitch camera
	gCamera->pitch(DEG2RAD(amt)); 	

	SetCursorPos(midScrX, midScrY); // Set our cursor back to the middle of the screen
}	

// Loads all the needed textures for the sky box
bool LoadTextures()
{
	/*
		This function uses good ole' brute force to load our 
		textures in.  Now it is VERY IMPORTANT to note the order
		in which we load the textures in.  The textures have
		been named in such a way that the name corresponds to the side
		of the sky box they belong on.  When we go to create the
		sky box we will do so in the same order that the textures are being
		loaded in.
	*/
	
	// The names of our textures
	char textureNames[kMaxTextures][64] = { "Back.bmp", "Front.bmp", "Bottom.bmp", 
											"Top.bmp", "Left.bmp", "Right.bmp" };

	// Loop through and load all of 'em								 
	for(int i = 0; i < kMaxTextures; ++i)
	{
		if(gTexture[i].load(textureNames[i]) == false)
			return false;
	}
		
	return true; // We got completely loaded :)
}

// Sets the value of "vert" to the passed in parameters
void FillSVertex(SVertex *vert, float X, float Y, float Z, float U, float V)
{
		// Error Check
		if(!vert) return;
	
	vert->x = X;
	vert->y = Y;
	vert->z = Z;
	vert->u = U;
	vert->v = V;
}

// Fills "verts" with the needed info to make a sky box centered at "cen" with
// the passed in width, height, and depth
//
// Here is an attempt at an ASCII visual of what we're creating.  The 'C' is the center
// of the box.
//					 
//              _____________________________
//             /|                           /|
//            / |                          / |
//           /  |                         /  |
//          /   |                        /   |
//         /    |      top              /    |
//        /     |                      /     |
//       /      |                     /      |
//      /       |           back     /       | height
//     /        |                   /        |
//    /         |                  /     t   |
//   /----------------------------/     h    |
//   |          |                 |    g     |
//   |          |       C         |   i      |      
//   |          |                 |  r       | 
//   |     t    |-----------------|----------|      
//   |    f    /                  |         /     
//   |   e    /                   |        /        
//   |  l    /     front          |       /         
//   |      /                     |      /          
//   |     /                      |     /  depth         
//   |    /           bottom      |    /            
//   |   /                        |   /             
//   |  /                         |  /              
//   | /                          | /               
//   |/                           |/                
//   ------------------------------
//			      width  
void CreateSkyBox(SVertex *verts, const CPos &pos, float wid, float hgt, float depth)
{
	// Error Check
	if(!verts)
		return;
	
	// Calculate half the width, height, and depth	
	float w = wid * 0.5f;
	float h = hgt * 0.5f;
	float d = depth * 0.5f;
	
	// Create a temporary pointer to use to walk through our verts list
	SVertex *v = verts;
	
	/*
		Okay we're about to fill in the vertices and UV coordinates for the sky
		box and it's important to keep in mind a couple of things.  One thing
		is the order in which we load the sky box textures.  We could do this
		in any order, but we choose the order of:  back, front, bottom, top, left,
		right.  So that means we have to build the sides of the sky box in that
		exact same order.
		
		Second, we need to remember that we are always INSIDE of a sky box, that’s
		why we have this specific function CreateSkyBox() instead of just a generic
		CreateBox() function or something similar.  The vertices and UV's will be set
		in such a way that everything will render properly as long as we're inside of 
		the box. 
	*/
	
	// Fill the BACK side
	// We will follow a similar pattern for each side.  Depending on the 
	// side we'll either want to add or subtract half of the width (X-dimension), 
	// height (Y-dimension), and depth (Z-dimension) from the center of the box.
	// Each time we set a vertex, notice how we use the ++ operator to advance
	// to the next vertex in the list.  If this doesn't make sense (and chances
	// are if this is the first time you've done anything like this it won't, which
	// is totally okay) I highly suggest you go through this step by step on paper.
	// Once you plot it out a few times, it will become crystal clear. 
	FillSVertex(v++, pos.x + w, pos.y - h, pos.z - d, 1.0f, 1.0f);
	FillSVertex(v++, pos.x + w, pos.y + h, pos.z - d, 1.0f, 0.0f);
	FillSVertex(v++, pos.x - w, pos.y + h, pos.z - d, 0.0f, 0.0f);
	FillSVertex(v++, pos.x - w, pos.y - h, pos.z - d, 0.0f, 1.0f);
	
	// Fill the FRONT side
	FillSVertex(v++, pos.x - w, pos.y - h, pos.z + d, 1.0f, 1.0f);
	FillSVertex(v++, pos.x - w, pos.y + h, pos.z + d, 1.0f, 0.0f);
	FillSVertex(v++, pos.x + w, pos.y + h, pos.z + d, 0.0f, 0.0f);
	FillSVertex(v++, pos.x + w, pos.y - h, pos.z + d, 0.0f, 1.0f);
		
	// Fill the BOTTOM side
	FillSVertex(v++, pos.x - w, pos.y - h, pos.z - d, 1.0f, 1.0f);
	FillSVertex(v++, pos.x - w, pos.y - h, pos.z + d, 1.0f, 0.0f);
	FillSVertex(v++, pos.x + w, pos.y - h, pos.z + d, 0.0f, 0.0f);
	FillSVertex(v++, pos.x + w, pos.y - h, pos.z - d, 0.0f, 1.0f);
	
	// Fill the TOP side
	FillSVertex(v++, pos.x + w, pos.y + h, pos.z - d, 0.0f, 0.0f);
	FillSVertex(v++, pos.x + w, pos.y + h, pos.z + d, 0.0f, 1.0f);
	FillSVertex(v++, pos.x - w, pos.y + h, pos.z + d, 1.0f, 1.0f);
	FillSVertex(v++, pos.x - w, pos.y + h, pos.z - d, 1.0f, 0.0f);
	
	// Fill the LEFT side
	FillSVertex(v++, pos.x - w, pos.y + h, pos.z - d, 1.0f, 0.0f);
	FillSVertex(v++, pos.x - w, pos.y + h, pos.z + d, 0.0f, 0.0f);
	FillSVertex(v++, pos.x - w, pos.y - h, pos.z + d, 0.0f, 1.0f);
	FillSVertex(v++, pos.x - w, pos.y - h, pos.z - d, 1.0f, 1.0f);
	
	// Fill the RIGHT side
	FillSVertex(v++, pos.x + w, pos.y - h, pos.z - d, 0.0f, 1.0f);
	FillSVertex(v++, pos.x + w, pos.y - h, pos.z + d, 1.0f, 1.0f);
	FillSVertex(v++, pos.x + w, pos.y + h, pos.z + d, 1.0f, 0.0f);
	FillSVertex(v++, pos.x + w, pos.y + h, pos.z - d, 0.0f, 0.0f);                        
}

// Draw the sky box to the screen
void RenderSkyBox()
{
	// We're using an indexed buffer to draw to the screen so how 
	// many indices do we need?  Well we know we want to draw
	// 6 sides (back, front, top, bottom, left and right).  Each side
	// is a quad (square), but we can't draw quads directly so we have
	// to draw triangles.  We know 2 triangles make up a quad so that
	// means we have 6 * 2 or 12 triangles to draw.  Since each triangle
	// has 3 points (vertices), we get 12 * 3 or 36 indices needed to
	// render our sky box
	const int kMaxIndices = 36;
	
	// Number of indices for each side of the sky box
	const int kIndicesPerQuad = 6;
	
	// Here is the indices to our vertex buffer containing our 
	// sky box.  Lets quickly look at ONLY the back side to see the pattern
	// for how the indices are laid out:
	/*
		   2 _______ 1
			|		| 
			|		|
			|_______|
		   3         0
		   
		  Our first triangle is made up vertices 0,1,2 which as you'll notice 
		  go in a counter-clockwise order.  Normally we draw vertices in a clockwise
		  order because we are drawing in a left-handed coordinate system.  But since
		  we want to see the INSIDE of the box, we have to flip the winding order.
		  The second triangle is made up vertices 2,3,0 which again is in a counter-clockwise
		  fashion.  All the other sides follow a similar a pattern.
		  
		  If this doesn't make perfect sense, I'd suggest doing it a couple of
		  times on paper.  Once you do that, chances are that light bulb in your 
		  head will become fully illuminated %)
	*/
	WORD indexList[kMaxIndices] = {0, 1, 2,
								   2, 3, 0,
								   4, 5, 6,
								   6, 7, 4,
								   8, 9, 10,
								   10, 11, 8,
								   12, 13, 14,
								   14, 15, 12,
								   16, 17, 18,
								   18, 19, 16,
								   20, 21, 22,
								   22, 23, 20};
	
	
	// Okay last but not least, we'll loop through and render each
	// side.  Since each side has it's own unique texture, we can use
	// the max number of textures as our loop quit condition.		   
	for(int i = 0; i < kMaxTextures; ++i)
	{
		WORD curIndexList[kIndicesPerQuad] = {0};
		
		// Here we get a little tricky.  Above "indexList" has the full index list
		// for our skybox.  However, we are drawing it one side at a time, meaning we
		// only want to pass kIndicesPerQuad (which is 6) indices for each side.  So
		// by paramter the below memcpy is saying:
		// curIndexList -- The current index list (which is 6 indices large) to use
		//				   in rendering this side of the skybox 
		// indexList + (i * kIndicesPerQuad) -- The starting address of where to
		//										start coping the data.  "indexList" points
		//										to the beginning of the array and (i * kIndicesPerQuad)
		//										we'll move us over 6 indices for each unique side
		// sizeof(WORD) * kIndicesPerQuad -- The size, in bytes, to copy
		memcpy(curIndexList, indexList + (i * kIndicesPerQuad), sizeof(WORD) * kIndicesPerQuad);
	
		gTexture[i].select(); // Select the texture for the current side
		g3D->render(gCubeVerts, kMaxVerts, curIndexList, kIndicesPerQuad); // Render it
	}
}

// DirectXion

/*
	Sky boxes are a great way to close off your environment.  Here's a quick recap of
	the things that can trip you up when implementing a sky box:
	
	1)  Coordinate system.  This can rain on anybody's 3D parade if you not 
	    using the correct one.  Here we are using a left-handed coordinate system
	    with +X to the right, +Y up, and +Z into the screen.
	2)  Not creating the sky box geometry correctly.  A sky box is intended to
	    always enclose the player and thus it's vertices and UV's are set-up with
	    that fact in mind.  If this becomes not the case, the sky box simply won't 
	    look right.
	3)  Getting things out of order.  We were very careful to load the 
	    images, and create the geometry for the sky box so that the order
	    was:  back, front, bottom, top, left, and right side.  If you screw
	    up the order in just one place, the whole sky box will look screwed.
	4)  Not using D3DTEXF_POINT as your minification and magnification texture filter.  If 
		you use any texturing filtering, your sky box textures will get sampled and thus
		blended which will more than likely cause seems.  Using D3DTEXF_POINT should prevent
		any seems, provided your artwork lines up.
	    
	If you have any questions, compliments, suggestions, or constructive critiques,
	be sure to post 'em at www.GameTutorials.com

*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/




