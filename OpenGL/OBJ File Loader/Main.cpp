//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Obj Loader										 //
//																		 //
//		$Description:	Demonstrates how to load a .Obj file format		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "Obj.h"										// Include the Obj header file.

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial will demonstrate how to load a .OBJ file.  This 3D file format
// was originally created from Maya by Alias|Wavefront.  It can be exported into most
// other programs.  A great tool for converting 3D file formats is "3D Exploration".
// This is a shareware utility that can be found for download on the web.
// This OBJ loader loads the vertices, the faces, and the texture UV coordinates.
// Since .obj files don't have material information I created my own to manually add textures/colors.
// In this tutorial, there is a picture of a face rotating around.  It is a single object
// This loader can load multiple objects, like the .3DS tutorial we have on our site.
// Below are the controls to this application:

// Left Mouse Button - Changes the Render mode from normal to wire frame.
// Right Mouse Button - Turns lighting On/Off
// Left Arrow Key - Spins the model to the left
// Right Arrow Key - Spins the model to the right
// Escape - Quits

#define FILE_NAME  "foot.obj"							// This is the 3D file we will load.

UINT g_Texture[MAX_TEXTURES] = {0};						// This holds the texture info, referenced by an ID

CLoadObj g_LoadObj;										// This is OBJ class.  This should go in a good model class.
t3DModel g_3DModel;										// This holds the 3D Model info that we load in

int   g_ViewMode	  = GL_TRIANGLES;					// We want the default drawing mode to be normal
bool  g_bLighting     = true;							// Turn lighting on initially
float g_RotateX		  = 0.0f;							// This is the current value at which the model is rotated
float g_RotationSpeed = 0.8f;							// This is the speed that our model rotates.  (-speed rotates left)

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the game window.
/////
///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// First we need to actually load the .OBJ file.  We just pass in an address to
	// our t3DModel structure and the file name string we want to load ("Bone.Obj").

	g_LoadObj.ImportObj(&g_3DModel, FILE_NAME);			// Load our .Obj file into our model structure

	// Since the .obj file format doesn't hold material information like color
	// and image files, we need to manually do that in code (or add it to the .obj file yourself).
	// I created a couple functions in the CLoadObj to help us do that.

	// Here we create a material with a name, a bitmap name and RGB values.
	// If we didn't want to create a texture map material but a color, just pass in NULL
	// for the file name field.  Also, by default the color is (255, 255, 255) if you choose
	// not to set it.  I did so just to show you how and not to confuse anyone.
	g_LoadObj.AddMaterial(&g_3DModel, "Bone", "Bone.bmp", 255, 255, 255);

	// With this function we can assign a particular model to a material ID.
	// This is so the object can have a color OR texture map.  
	// Here we pass in our model, the index into the object list, and the material ID.
	g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);

	// Here we go through all of the materials and check if we need to make a texture map.
	// We obviously know there is just one material but we should set this up for future use.
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// Check if the current material has a file name
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0) {

			// Create a texture map from the material's file name
			CreateTexture(g_Texture[i], g_3DModel.pMaterials[i].strFile);
		}
		
		// Assign the material ID to the current material
		g_3DModel.pMaterials[i].texureId = i;
	}

	// Here, we turn on a lighting and enable lighting.  We don't need to
	// set anything else for lighting because we will just take the defaults.
	// We also want color, so we turn that on

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// When we are done, we need to free all the model data
	// We do this by walking through all the objects and freeing their information

	// Go through all the objects in the scene
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		delete [] g_3DModel.pObject[i].pFaces;
		delete [] g_3DModel.pObject[i].pNormals;
		delete [] g_3DModel.pObject[i].pVerts;
		delete [] g_3DModel.pObject[i].pTexVerts;
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Give OpenGL our position,	then view,		then up vector
	gluLookAt(		0, 1.5f, 12,		0, 0.5f, 0,			0, 1, 0);
	
	// We want the model to rotate around the axis so we give it a rotation
	// value, then increase/decrease it. You can rotate right of left with the arrow keys.

	glRotatef(g_RotateX, 0, 1.0f, 0);						// Rotate the object around the Y-Axis
	g_RotateX += g_RotationSpeed;							// Increase the speed of rotation

	// I am going to attempt to explain what is going on below up here as not to clutter the 
	// code below.  We have a model that has a certain amount of objects and textures.  We want 
	// to go through each object in the model, bind it's texture map to it, then render it.
	// To render the current object, we go through all of it's faces (Polygons).  
	// What is a face you ask?  A face is just (in this case) a triangle of the object.
	// For instance, a cube has 12 faces because each side has 2 triangles.
	// You might be thinking.  Well, if there are 12 faces in a cube, that makes
	// 36 vertices that we needed to read in for that object.  Not really true.  Because
	// a lot of the vertices are the same, since they share sides, they only need to save
	// 8 vertices, and ignore the duplicates.  Then, you have an array of all the
	// unique vertices in that object.  No 2 vertices will be the same.  This cuts down
	// on memory.  Then, another array is saved, which is the index numbers for each face,
	// which index in to that array of vertices.  That might sound silly, but it is better
	// than saving tons of duplicate vertices.  The same thing happens for UV coordinates.
	// You don't save duplicate UV coordinates, you just save the unique ones, then an array
	// that index's into them.  This might be confusing, but most 3D files use this format.
	// This loop below will stay the same for most file formats that you load, so all you need
	// to change is the loading code.  You don't need to change this loop (Except for animation).

	// Since we know how many objects our model has, go through each of them.
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if(g_3DModel.pObject.size() <= 0) break;

		// Get the current object that we are displaying
		t3DObject *pObject = &g_3DModel.pObject[i];

		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture) {

			// Turn on texture mapping and turn off color
			glEnable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);

			// Bind the texture map to the object by it's materialID (*ID Current Unused*)
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} else {

			// Turn off texture mapping and turn on color
			glDisable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);
		}

		// This determines if we are in wire frame or normal mode
		glBegin(g_ViewMode);					// Begin drawing with our selected mode (triangles or lines)

			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Go through each corner of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the vertex index for each point of the face
					int vertIndex = pObject->pFaces[j].vertIndex[whichVertex];
			
					// Give OpenGL the normal for this vertex.
					glNormal3f(pObject->pNormals[ vertIndex ].x, pObject->pNormals[ vertIndex ].y, pObject->pNormals[ vertIndex ].z);
				
					// If the object has a texture associated with it, give it a texture coordinate.
					if(pObject->bHasTexture) {

						// Make sure there was a UVW map applied to the object or else it won't have tex coords.
						if(pObject->pTexVerts) {

							// Get the texture coordinate index
							int coordIndex = pObject->pFaces[j].coordIndex[whichVertex];

							// Assign the UV coordinates to the current vertex being rendered
							glTexCoord2f(pObject->pTexVerts[ coordIndex ].x, pObject->pTexVerts[ coordIndex ].y);
						}
					} else {

						// Make sure there is a valid material/color assigned to this object.
						// You should always at least assign a material color to an object, 
						// but just in case we want to check the size of the material list.
						// if the size is at least one, and the material ID != -1,
						// then we have a valid material.
						if(g_3DModel.pMaterials.size() && pObject->materialID >= 0) 
						{
							// Get and set the color that the object is, since it must not have a texture
							BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;

							// Assign the current color to this model
							glColor3ub(pColor[0], pColor[1], pColor[2]);
						}
						
					}

					// Pass in the current vertex of the object (Corner of current face)
					glVertex3f(pObject->pVerts[ vertIndex ].x, pObject->pVerts[ vertIndex ].y, pObject->pVerts[ vertIndex ].z);
				}
			}

		glEnd();								// End the drawing
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	SwapBuffers(g_hDC);							// Swap the backbuffers to the foreground
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Below we define our controls for this simple tutorial.
	// The controls are: 

	// Left Mouse Button - Changes the Render mode from normal to wire frame.
	// Right Mouse Button - Turns lighting On/Off
	// Left Arrow Key - Spins the model to the left
	// Right Arrow Key - Spins the model to the right
	// Escape - Quits

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked
		
		if(g_ViewMode == GL_TRIANGLES) {				// We our drawing mode is at triangles
			g_ViewMode = GL_LINE_STRIP;					// Go to line strips
		} else {
			g_ViewMode = GL_TRIANGLES;					// Go to triangles
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

	case WM_KEYDOWN:									// If we pressed a key

		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;

			case VK_LEFT:								// If the LEFT arrow key was pressed
				g_RotationSpeed -= 0.05f;				// Decrease the rotation speed (eventually rotates left)
				break;

			case VK_RIGHT:								// If the RIGHT arrow key is pressed
				g_RotationSpeed += 0.05f;				// Increase the rotation speed (rotates right)
				break;
		}
		break;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


    case WM_CLOSE:										// If the window is being closed
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        return DefWindowProc (hWnd, uMsg, wParam, lParam);
    } 
 
    return 0;											// Return 0 when we handle the message
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows how to load an .obj file.  These files are ASCII text files
// that contain very few information about the models, but are great for importing/exporting
// into almost every 3D software package as well as being easy to read in.
// 
// This tutorial is modified from the 3DS loading tutorial.  I wanted to keep them 
// consistent as possible so you didn't have to see much changed between the 2.
// The failure in that is that .obj files don't have material information, so I had
// to add my own manual ways of doing it.  I personally wouldn't use .obj for a demo
// or game, but It is always great to have something like this around since it's so
// popular.  I would recommend looking at this tutorial actually before the 3DS tutorial.
// 
// * What's An STL (Standard Template Library) Vector? *
// Let me quickly explain the STL vector for those of you who are not familiar with them.
// To use a vector you must include <vector> and use the std namespace: using namespace std;
// A vector is an array based link list.  It allows you to dynamically add and remove nodes.
// This is a template class so it can be a list of ANY type.  To create a vector of type
// "int" you would say:  vector<int> myIntList;
// Now you can add a integer to the dynamic array by saying: myIntList.push_back(10);
// or you can say:  myIntList.push_back(num);.  The more you push back, the larger
// your array gets.  You can index the vector like an array.  myIntList[0] = 0;
// To get rid of a node you use the pop_back() function.  To clear the vector use clear().
// It frees itself so you don't need to worry about it, except if you have data
// structures that need information freed from inside them, like our objects.
//
// Here is some commentary about the .obj file which comes from the bottom of obj.cpp:
//
// First, no .obj file format is going to be the same.  When you import/export
// .obj files anywhere, each application has their own way of saving it.  Some
// save normals, some save extra comments, some save object names, etc...  That is
// why I calculate my own normals because rarely are they including as "vn" in the file.
// The only thing you can depend on is the headers:
//
// "v"  - This is a line that contains a vertex (x y z)
//
//		IE:			v	-1	-1	0
//
// After the 'v' it will list the X Y and Z information for that vertex.
//
// "vt" - This is a line that contains a UV texture coordinate (U V)
//
//		IE:			vt	.99998	.99936
//
// After the "vt" it will list the U and V information for a vertex.
// Note that this will ONLY appear in the file if the object has a UVW map.
// Just dragging a texture onto an object doesn't create a UVW map.
//
// "f"	- This is a line that contains the vertex indices into the vertex array.
//        If there are UV coordinates for that object, it also contains the UV indices.
//
//		IE (Just vertices):			f	1	2	3
//		
//		IE (Verts and UV Indices)	f	1/1 2/2	3/3
//
// After the 'f' it will list the vertex indices, or the vertex / UV indices.
//
// Those are the only 3 you can count on 99% of the time.  The other one that
// isn't always there is "vn".  That is a vertex normal:  vn -1 0 0
// There are some other lines that are sometimes used by they can be generally ignored.
//
//
// Let us know if this helps you out!
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
// © 2000-2005 GameTutorials
