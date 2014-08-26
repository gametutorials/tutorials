//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Octree3	 										 //
//																		 //
//		$Description:	A working octree with a .3ds file format scene   //
//																		 //
//***********************************************************************//


// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


#include "main.h"										
#include "3ds.h"
#include "camera.h"

// *Tutorial Note* 
// By reading this tutorial we assume you have looked at the previous octree,
// frustum culling and model loading tutorials.  If not, we strongly recommend it.
//
// This is the third tutorial of the 3 part series on octrees.  The first 2 showed
// how to create an octree, along with incorporating frustum culling to it.  With
// this final part, the octree class will now load in a 3D world from a .3DS file.
// The file format doesn't really matter because all of our loader tutorials
// use the t3DModel structure to store the data, which is passed into the octree
// object to partition.  The octree is completely independent from the model loader.
// 
// In the previous tutorials we stored vertices in the end nodes.  This version takes
// a different approach, since we are dealing with a model structure.  Instead of
// storing vertices, we just store face indices.  There are many benefits to this
// method.  The first is that we can easily plug in this data to vertex arrays, which
// increases our efficiency.  The second is that we don't have to recalculate the
// face information when we break up the data into nodes.  In any model loader, you
// are given all the vertices in the model, then for every polygon to render, indices
// into that vertex array.  This makes it so you don't have to store duplicate vertices
// in memory or the 3D file.  Like the first advantage, it allows us to easily render
// the model through vertex arrays.  The last benefit of this method is that we don't
// have to deal with splitting polygons across planes and then perhaps triangulating
// them.  
//
// For optimal performance, we will be using display lists and vertex arrays for rendering
// each node.  You really wouldn't want it any other way.  Due to the fact that our world
// isn't as big as a full level you would find in a game, the speed increase that the
// octree provides won't be as noticeable, especially on high end graphics cards.  Keep
// this in mind when working with your own octree implementation.  In my experience, the
// biggest problem is loops and being fill limited.  Sometimes, your problem is getting
// geometry to the screen, it's the amount of pixels that are being drawn to the screen.
// If you make your view port size smaller and you get a significant increase, then
// your bottle neck is fill limited.  For instance, On a TNT2 card, I get about 10 to
// 15 frames more on this tutorial when I change the resolution from 800 x 600 to 640 x 480.
// That is why I set the resolution smaller to increase frame rate.  
//
// This application allows you to walk around the world using our camera class.  Intially,
// fog is turned on to give it a cool effect.  You can turn this off to get a better view
// of the world.  
//
// The controls are: 
//
// Mouse               - Rotates the camera's view
// Left Mouse Button   - Changes the Render mode from normal to wire frame.
// Right Mouse Button  - Turns lighting On/Off
// Up    Arrow Key | W - Moves the camera along the view vector
// Down  Arrow Key | S - Moves the camera opposite the view vector
// F				   - Turns fog on/off
// Space Bar		   - Turns on/off the yellow debug lines for the visual node subdivisions
// Escape - Quits


bool  g_bFullScreen = true;			// Set full screen as default
HWND  g_hWnd;						// This is the handle for the window
RECT  g_rRect;						// This holds the window dimensions
HDC   g_hDC;						// General HDC - (handle to device context)
HGLRC g_hRC;						// General OpenGL_DC - Our Rendering Context for OpenGL
float g_DT;							// This stores the elapsed time between the current and last frame

							

// Initialize the Camera object
CCamera	g_Camera;

// Here we initialize our single Octree object.  This will hold all of our vertices
COctree g_Octree;

// This will be our global frustum object that keeps track of our frustum
CFrustum g_Frustum;

// We created a debug class to show us the nodes visually in yellow cubes.
CDebug g_Debug;

// The current amount of end nodes in our tree (The nodes with vertices stored in them)
int g_EndNodeCount = 0;

// This stores the amount of nodes that are in the frustum
int g_TotalNodesDrawn = 0;

// The maximum amount of triangles per node.  If a node has equal or less 
// than this, stop subdividing and store the face indices in that node
int g_MaxTriangles = 1000;

// The maximum amount of subdivisions allowed (Levels of subdivision)
int g_MaxSubdivisions = 5;

// Set the current render mode to render triangles initially
bool g_RenderMode = true;

// Turn lighting on initially
bool  g_bLighting     = true;							

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is the file name that is to be loaded
#define FILE_NAME	"Park.3ds"						

// This is our 3DS loader object that will load the g_World data in
CLoad3DS g_Load3DS;

// This will store our 3ds scene that we will pass into our octree
t3DModel g_World;

// This stores our current frames per second
char g_strFrameRate[255] = {0};

// This holds the texture info, referenced by an ID
UINT g_Texture[MAX_TEXTURES] = {0};	

// This tells us if we want fog on or off - On by default ('F')
bool g_bFog = true;

// This tells us if we want to display the yellow debug lines for our nodes (Space Bar)
bool g_bDisplayNodes = false;				


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the game window.
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Initialize the camera position
	g_Camera.PositionCamera(0, 3.5f, 30,	0, 0, 0,	0, 1, 0);

	// Here we load the world from a .3ds file
	g_Load3DS.Import3DS(&g_World, FILE_NAME);

	// Go through all the materials
	for(int i = 0; i < g_World.numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(g_World.pMaterials[i].strFile) > 0)
		{
			// Use the name of the texture file to load the bitmap, with a texture ID (i).
			// We pass in our global texture array, the name of the texture, and an ID to reference it.	
			CreateTexture(g_Texture[i], g_World.pMaterials[i].strFile);			
		}

		// Set the texture ID for this material
		g_World.pMaterials[i].texureId = i;
	}

	// The first thing that needs to happen before creating our octree is to find
	// the total width of the initial root node.  Now we pass in our t3DModel object
	// to GetSceneDimensions(), instead of vertices and a vertex count, as done
	// in the last octree tutorials.  This will store the initial root node cube width.
	g_Octree.GetSceneDimensions(&g_World);

	// Since our model structures stores multiple objects, we can't easily access the
	// total triangle count in the scene with out manually going through and counting the total.
	// This is what we do below.  With the result, we pass this into our CreateNode() function.
	int TotalTriangleCount = g_Octree.GetSceneTriangleCount(&g_World);

	// To create the first node we need the world data, the total triangle count in the scene,
	// along with the initial root node center and width.  This function will then recursively
	// subdivide the rest of the world, according to the global restrictions.
	g_Octree.CreateNode(&g_World, TotalTriangleCount, g_Octree.GetCenter(), g_Octree.GetWidth());

	// The octree should be created by now.  To better increase our efficiency we use display
	// lists for every end node.  This way, we just have to call a display list ID to draw
	// a node, verses the slow loops we normal had.  Vertex arrays are also used to optimize
	// our rendering of the octree.

	// Below we get the display list base ID and store it in the root node.  This should return 1
	// since we don't use display lists anywhere before this.  Notice that we use our global
	// variable that stores our end node count to pass in the total amount of list ID's needed.
	// If you are unfamiliar with displays, basically what you do is section off a certain
	// amount of ID's, and then you are returns a base pointer to the start of those ID's.
	// You can use the ID's from the base pointer to the base pointer ID + the number of
	// ID's that were saved off for that base pointer.  Each of the ID's correspond to a
	// bunch of OpenGL commands.  That means that each end node has it's own ID that
	// corresponds to a bunch of OpenGL commands.  So, for instance, if pass in a bunch
	// of vertices to OpenGL, we can assign this action to a display list.  That way we
	// just call a display list ID to perform that action.  Think of it as a function.
	// You just need to call a function to do a bunch of tasks, which eliminates extra
	// code, and also is saved on the video card for faster processing.  
	g_Octree.SetDisplayListID( glGenLists(g_EndNodeCount) );

	// Now we go through every single end node and create a display list for them all.
	// That way, when we draw the end node, we just use it's display list ID to render
	// the node, instead of looping through all the objects and manually give the verts to opengl.
	// The parameters for this function is the node (starting with the root node), 
	// the world data and current display list base ID.  The base ID is stored in the root
	// node's ID, so we just pass that in.  The reason we do this is because, if you create
	// other display lists before you create the octree, you don't want to assume the octree
	// ID's go from 1 to the end node count.  By passing in the base ID, we then will add
	// this ID to other nodes.  Right now, when they are created they are assigned the
	// end node count at the time upon creating them.  This will make more sense when looking
	// at the octree code.
	g_Octree.CreateDisplayList(&g_Octree, &g_World, g_Octree.GetDisplayListID());

	// Hide our cursor since we are using first person camera mode
	ShowCursor(FALSE);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color
}


/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function clamps a specified frame rate for consistency
/////
/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; // Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime; // Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		g_DT = desiredFPS;
		elapsedTime -= desiredFPS; // Adjust our elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop
/////
///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;
			TranslateMessage(&msg);						// Find out what the message does
			DispatchMessage(&msg);						// Execute the message
		}
		else											// if there wasn't a message
		{ 		
			if(AnimateNextFrame(60))					// Make sure we only animate 60 FPS
			{
				g_Camera.Update();						// Update the camera info
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Go through all the objects in the scene and free them
	for(int i = 0; i < g_World.numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		delete [] g_World.pObject[i].pFaces;
		delete [] g_World.pObject[i].pNormals;
		delete [] g_World.pObject[i].pVerts;
		delete [] g_World.pObject[i].pTexVerts;
		delete [] g_World.pObject[i].pIndices;
	}

	// When using display lists, we need to free them when we are finished using
	// the ID's.  This OpenGL function does just that.  We pass in the base ID and
	// the total ID's we used.
	glDeleteLists(g_Octree.GetDisplayListID(), g_EndNodeCount);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The matrix

	// Position the camera
	g_Camera.Look();

	// Each frame we calculate the new frustum.  In reality you only need to
	// calculate the frustum when we move the camera.
	g_Frustum.CalculateFrustum();

	// Initialize the total node count that is being draw per frame
	g_TotalNodesDrawn = 0;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we draw the octree, starting with the root node and recursing down each node.
	// This time, we pass in the root node and just the original world model.  You could
	// just store the world in the root node and not have to keep the original data around.
	// This is up to you.  I like this way better because it's easy, though it could be 
	// more error prone.
	g_Octree.DrawOctree(&g_Octree, &g_World);

	// Render the cubed nodes to visualize the octree (in wire frame mode)
	if( g_bDisplayNodes )
		g_Debug.RenderDebugLines();

	// Create a buffer to store the octree information for the title bar
	static char strBuffer[255] = {0};

	// Display in window mode the current subdivision information.  We now display the
	// max triangles per node, the max level of subdivision, total end nodes, current nodes drawn
	// and frames per second we are receiving.  
	sprintf(strBuffer, "Triangles: %d        Subdivisions: %d        EndNodes: %d          NodesDraw: %d          FPS: %s",
		                g_MaxTriangles,		 g_MaxSubdivisions,		 g_EndNodeCount,	   g_TotalNodesDrawn,	  g_strFrameRate);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Set our window title bar to the subdivision information
	SetWindowText(g_hWnd, strBuffer);

	// Swap the backbuffers to the foreground
	SwapBuffers(g_hDC);									
}


///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
			if(g_hDC) RenderScene();					// Redraw the scene if we have a valid hdc
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		if(g_hDC) RenderScene();						// Redraw the scene
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

	case WM_LBUTTONDOWN:								// If we hit the left mouse button
		
		g_RenderMode = !g_RenderMode;					// Change the rendering mode

		// Change the rendering mode to lines or triangles, depending on it's current status
		if(g_RenderMode) {				
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Render the triangles in fill mode		
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Render the triangles in wire frame mode
		}
		break;

	case WM_RBUTTONDOWN:								// If the right mouse button was clicked.
		
		g_bLighting = !g_bLighting;						// Turn lighting ON/OFF

		if(g_bLighting) {								// If lighting is ON
			glEnable(GL_LIGHTING);						// Enable OpenGL lighting
		} else {
			glDisable(GL_LIGHTING);						// Disable OpenGL lighting
		}
		break;

	case WM_KEYDOWN:									// If we hit a key
		switch (wParam)									// Check which key we hit
		{
			case VK_ESCAPE:								// If we hit ESCAPE
				PostQuitMessage(0);						// Tell windows we want to quit
				break;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			case VK_SPACE:								// If we hit the space bar

				g_bDisplayNodes = !g_bDisplayNodes;		// Turn the debug lines on/off
				break;

			case 'F':									// Check if hit the 'F' key
				
				g_bFog = !g_bFog;						// Turn fog on/off

				if( g_bFog )							// If fog is turned on
				{
					// Set the background color to grey and enable fog
					glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
					glEnable(GL_FOG);
				}
				else
				{
					// Turn the color back to blue'ish purple and disable fog
					glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
					glDisable(GL_FOG);
				}
				break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


		}
		break;

    case WM_DESTROY:									// If the window is destroyed
        PostQuitMessage(0);								// Tell windows we want to quit
        break; 
     
    default:											// Return by default
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// If this tutorial was too much of a jump from the previous tutorials, I apologize.
// I tried to think of people who were advanced and wanted all the optimizations, and
// at the same time try not to lose anyone.  Fog was added just for a cool effect.
// This can actually be used to help speed up the octree.  If you make the fog really
// dense, you can then bring in the near plane for our frustum.  This will make it so
// you can't see so far, which means you won't have to display as many end nodes.
// 
// Let's go over the functions that need to be called from the client to make the
// octree work:
// 
//  First we need to get the cube's bounding box of the world:
//
//  // Gets the scene dimensions for our entire model
// 	g_Octree.GetSceneDimensions(&g_World);
//
//  Then we need to find our how many polygons make up the world:
//
//  // Returns the whole scene's polygon count
//	g_Octree.GetSceneTriangleCount(&g_World);
//
//  Once we find the polygon count, we can pass that into:
//
//  // This starts with the root node and recursively creates the octree
//	g_Octree.CreateNode(&g_World, TotalTriangleCount, g_Octree.GetCenter(), g_Octree.GetWidth());
//
//  Since we are using display lists, we want to find the display list base ID:
//
//  // This creates and sets the base list ID for all the end nodes
//	g_Octree.SetDisplayListID( glGenLists(g_EndNodeCount) );
//
//  To finally create the display list, we take the base list ID and pass it into:
//
//  //  This recursively creates a display list for every end node
//	g_Octree.CreateDisplayList(&g_Octree, &g_World, g_Octree.GetDisplayListID());
//
//  Once the octree is finished, we can finally call our DrawOctree() function in RenderScene():
//
//  // Draw the octree with the root world being passed in, along with the root node.
//  g_Octree.DrawOctree(&g_Octree, &g_World);
// 
// I hope this tutorial helps give you ideas for your own octree implementation.  There is
// no one way to do it.  This is just what I came up with.  I will strongly suggest that you
// stick with using vertex arrays if you haven't before.  The impact is huge without them, 
// especially when you get up to rendering thousands of polygons.
//
// I would like to thank Elmar Moelzer <moelzere@aon.at> for the Park.3ds art.  You can visit
// his company's page for more cool art and game oriented stuff.  He is pretty amazing!
// It is located at www.mediastudio-graz.com.
// 
// Good luck!
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials
