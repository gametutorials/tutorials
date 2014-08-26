//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		ASE Loader										 //
//																		 //
//		$Description:	Demonstrates how to load a .Ase file format		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "Ase.h"										// Include the ASE header file.

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial will demonstrate how to load a .ASE file.  This 3D file format
// is created from 3D Studio Max.  This cannot be imported back in though.
// This ASE loader only loads the texture names, object colors, the vertices, the faces, and the UV coordinates.
// The key frame information is ignored since I don't have any animation tutorials.
// I didn't want to over complicate this tutorial, but just show the basics.  Later I will 
// To better understand model loading I would recommend looking at our .obj and .3ds tutorials too.
//
// In this tutorial, there is a face of a statue rotating around.  It is a single 
// object with a single texture.  The .ASE loader does load multiple objects though.
// The controls are: 

// Left Mouse Button - Changes the Render mode from normal to wireframe.
// Right Mouse Button - Turns lighting On/Off
// Left Arrow Key - Spins the model to the left
// Right Arrow Key - Spins the model to the right
// Escape - Quits

#define FILE_NAME  "Statue.ase"							// This is the 3D file we will load.

UINT g_Texture[MAX_TEXTURES] = {0};						// This holds the texture info, referenced by an ID

CLoadASE g_LoadASE;										// This is ASE class.  This should go in a good model class.
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

	// First we need to actually load the .ASE file.  We just pass in an address to
	// our t3DModel structure and the file name string we want to load ("Statue.Ase").

	g_LoadASE.ImportASE(&g_3DModel, FILE_NAME);			// Load our .Ase file into our model structure

	// Depending on how many textures we found, load each one (Assuming .BMP)
	// If you want to load other files than bitmaps, you will need to adjust CreateTexture().
	// Below, we go through all of the materials and check if they have a texture map to load.
	// Otherwise, the material just holds the color information and we don't need to load a texture.

	// Go through all the materials
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
		{
			// Use the name of the texture file to load the bitmap, with a texture ID (i).
			// We pass in our global texture arrow, the name of the texture, and an ID to reference it.	
			CreateTexture(g_3DModel.pMaterials[i].strFile, g_Texture[i]);			
		}

		// Set the texture ID for this material
		g_3DModel.pMaterials[i].texureId = i;
	}

	// Here, we turn on a lighting and enable lighting.  We don't need to
	// set anything else for lighting because we will just take the defaults.
	// We also want color, so we turn that on

	glEnable(GL_LIGHT0);							// Turn on a light with defaults set
	glEnable(GL_LIGHTING);							// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);					// Allow color

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
		elapsedTime -= desiredFPS; // Adjust the elapsed time

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
	gluLookAt(		0, 1.5f, 18,		0, 0.5f, 0,			0, 1, 0);
	
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

			// Bind the texture map to the object by it's materialID
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} else {

			// Turn off texture mapping and turn on color
			glDisable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);
		}

		// This determines if we are in wireframe or normal mode
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
							float *pColor = g_3DModel.pMaterials[pObject->materialID].fColor;

							// Assign the current color to this model
							glColor3f(pColor[0], pColor[1], pColor[2]);
						}
					}

					// Pass in the current vertex of the object (Corner of current face)
					glVertex3f(pObject->pVerts[ vertIndex ].x, pObject->pVerts[ vertIndex ].y, pObject->pVerts[ vertIndex ].z);
				}
			}

		glEnd();								// End the drawing
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
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

	// Left Mouse Button - Changes the Render mode from normal to wireframe.
	// Right Mouse Button - Turns lighting On/Off
	// Left Arrow Key - Spins the model to the left
	// Right Arrow Key - Spins the model to the right
	// Escape - Quits

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked
		
		if(g_ViewMode == GL_TRIANGLES) {				// We our drawing mode is at triangles
			g_ViewMode = GL_LINE_STRIP;					// Go to line stips
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
 
    return 0;											// We handled message, return 0
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows how to load a .Ase file.  An ASE file is a 3D Studio Max
// ASCII file that saves all the information in a text file instead of in binary.
// The good thing about this is that it's easy to read in, as well as easy
// to change without having the software.  The bad thing is that it's a HUGE
// file size, it takes a lot longer to read in, no other programs support it
// and it can't even be imported back into 3D Studio Max.  I would recommend
// looking at this tutorial before you look at the 3DS tutorial though.  That
// way you can see what is actually being read in.
//
// One thing you will notice that is different in the rendering loop in this
// tutorial verses the .obj and .ase is that we use float values for the color
// instead of 0 to 255.  .ASE saves the color from 0 to 1.
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
// Here is some comments on the format of the file format mentioned in Ase.cpp:
//
// Let's go over some of the format of the .Ase file.  I will only address the
// information that we really care about for object loading.  The rest is Scene stuff.
// I created a single box around the origin that had a texture map over it to show
// the simplest and smallest amount of data.
// 
// This next block is the material block.  This holds all the info on the materials.
//
//	*MATERIAL_LIST {			// This is the tag for the start of the material info
//	*MATERIAL_COUNT 1			// This tells us how many materials there are
//	*MATERIAL 0 {				// This is the material ID that the next data is refering too.
//		*MATERIAL_NAME "Material #1"	// The material name
//		*MATERIAL_CLASS "Standard"
//		*MATERIAL_AMBIENT 0.1791	0.0654	0.0654
//		*MATERIAL_DIFFUSE 0.5373	0.1961	0.1961	// The color of the object
//		*MATERIAL_SPECULAR 0.9000	0.9000	0.9000
//		*MATERIAL_SHINE 0.2500
//		*MATERIAL_SHINESTRENGTH 0.0500
//		*MATERIAL_TRANSPARENCY 0.0000
//		*MATERIAL_WIRESIZE 1.0000
//		*MATERIAL_SHADING Blinn
//		*MATERIAL_XP_FALLOFF 0.0000
//		*MATERIAL_SELFILLUM 0.0000
//		*MATERIAL_FALLOFF In
//		*MATERIAL_XP_TYPE Filter
//		*MAP_DIFFUSE {
//			*MAP_NAME "Map #1"
//			*MAP_CLASS "Bitmap"
//			*MAP_SUBNO 1
//			*MAP_AMOUNT 1.0000
//			*BITMAP "texture.bmp"		// This is the file name for the texture
//			*MAP_TYPE Screen
//			*UVW_U_OFFSET 0.0000		// This is the U offset for the tile
//			*UVW_V_OFFSET 0.0000		// This is the V offset for the tile
//			*UVW_U_TILING 1.0000		// This is the U tiling ratio (1 is standard)
//			*UVW_V_TILING 1.0000		// This is the V tiling ratio (1 is standard)
//			*UVW_ANGLE 0.0000
//			*UVW_BLUR 1.0000
//			*UVW_BLUR_OFFSET 0.0000
//			*UVW_NOUSE_AMT 1.0000
//			*UVW_NOISE_SIZE 1.0000
//			*UVW_NOISE_LEVEL 1
//			*UVW_NOISE_PHASE 0.0000
//			*BITMAP_FILTER Pyramidal
//		}
//    }
// }
//
//
//
//*GEOMOBJECT {							// This tag let's us know that objects are next
//	*NODE_NAME "Box01"					// This is the object name
//	*NODE_TM {
//		*NODE_NAME "Box01"				// Once again, this is the objects name
//		*INHERIT_POS 0 0 0
//		*INHERIT_ROT 0 0 0
//		*INHERIT_SCL 0 0 0
//		*TM_ROW0 1.0000	0.0000	0.0000
//		*TM_ROW1 0.0000	1.0000	0.0000
//		*TM_ROW2 0.0000	0.0000	1.0000
//		*TM_ROW3 0.0000	0.0000	0.0000
//		*TM_POS 0.0000	0.0000	0.0000
//		*TM_ROTAXIS 0.0000	0.0000	0.0000
//		*TM_ROTANGLE 0.0000
//		*TM_SCALE 1.0000	1.0000	1.0000
//		*TM_SCALEAXIS 0.0000	0.0000	0.0000
//		*TM_SCALEAXISANG 0.0000
//	}
//	*MESH {								// This tells us an objects data is next
//		*TIMEVALUE 0					
//		*MESH_NUMVERTEX 8				// This holds the number of vertices for this object
//		*MESH_NUMFACES 12				// This is the number of faces in this object
//		*MESH_VERTEX_LIST {				// The tag for the start of the vertex list
//			*MESH_VERTEX    0	-1.5000	-1.5000	0.0000	// These next 8 are the vertices.
//			*MESH_VERTEX    1	1.5000	-1.5000	0.0000	// The first number is the index
//			*MESH_VERTEX    2	-1.5000	1.5000	0.0000  // and the next 3 are the (X, Y, Z)
//			*MESH_VERTEX    3	1.5000	1.5000	0.0000  // ...
//			*MESH_VERTEX    4	-1.5000	-1.5000	3.0000
//			*MESH_VERTEX    5	1.5000	-1.5000	3.0000
//			*MESH_VERTEX    6	-1.5000	1.5000	3.0000
//			*MESH_VERTEX    7	1.5000	1.5000	3.0000
//		}
//
//		// Below is the tag for the face list.  We have a list of vertices and texture coordinates.
//		// Each of these vertices and coordinates are unique.  This cuts down memory.
//      // We then have a structure that holds index's into the vertex and UV array.
//		// After *MESH_FACE it has the index, then the A B and C values.  These values
//		// are for point1 point2 and point3 indices into the vertex array. The rest can
//      // be ignored.  They have to do with visibility issues.
//
//		*MESH_FACE_LIST {				
//			*MESH_FACE    0:    A:    0 B:    2 C:    3 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 2 	*MESH_MTLID 1
//			*MESH_FACE    1:    A:    3 B:    1 C:    0 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 2 	*MESH_MTLID 1
//			*MESH_FACE    2:    A:    4 B:    5 C:    7 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 3 	*MESH_MTLID 0
//			*MESH_FACE    3:    A:    7 B:    6 C:    4 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 3 	*MESH_MTLID 0
//			*MESH_FACE    4:    A:    0 B:    1 C:    5 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 4 	*MESH_MTLID 4
//			*MESH_FACE    5:    A:    5 B:    4 C:    0 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 4 	*MESH_MTLID 4
//			*MESH_FACE    6:    A:    1 B:    3 C:    7 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 5 	*MESH_MTLID 3
//			*MESH_FACE    7:    A:    7 B:    5 C:    1 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 5 	*MESH_MTLID 3
//			*MESH_FACE    8:    A:    3 B:    2 C:    6 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 6 	*MESH_MTLID 5
//			*MESH_FACE    9:    A:    6 B:    7 C:    3 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 6 	*MESH_MTLID 5
//			*MESH_FACE   10:    A:    2 B:    0 C:    4 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 7 	*MESH_MTLID 2
//			*MESH_FACE   11:    A:    4 B:    6 C:    2 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 7 	*MESH_MTLID 2
//		}
//		*MESH_NUMTVERTEX 36					// This is the number of texture coordinates for this object
//
//		// The next block is the texture coordinates.  The first value after *MESH_TVERT
//      // is the index and the next 2 values are the ones we care about.  The third is ignored.
//      // The 2 we care about are the U and V values for the vertices.
//
//		*MESH_TVERTLIST {					// The tag to let us know there are UV coordiantes
//			*MESH_TVERT 0	0.9995	0.0005	0.0005 // Format: Index   U  V   Ignore
//			*MESH_TVERT 1	0.0005	0.0005	0.9995
//			*MESH_TVERT 2	0.0005	0.0005	0.0005
//			*MESH_TVERT 3	0.0005	0.0005	0.9995
//			*MESH_TVERT 4	0.9995	0.9995	0.0005
//			*MESH_TVERT 5	0.0005	0.9995	0.9995
//			*MESH_TVERT 6	0.0005	0.9995	0.0005
//			*MESH_TVERT 7	0.0005	0.9995	0.9995
//			*MESH_TVERT 8	0.9995	0.0005	0.0005
//			*MESH_TVERT 9	0.9995	0.9995	0.0005
//			*MESH_TVERT 10	0.0005	0.9995	0.0005
//			*MESH_TVERT 11	0.0005	0.9995	0.0005
//			*MESH_TVERT 12	0.0005	0.0005	0.0005
//			*MESH_TVERT 13	0.9995	0.0005	0.0005
//			*MESH_TVERT 14	0.0005	0.0005	0.9995
//			*MESH_TVERT 15	0.9995	0.0005	0.9995
//			*MESH_TVERT 16	0.9995	0.9995	0.9995
//			*MESH_TVERT 17	0.9995	0.9995	0.9995
//			*MESH_TVERT 18	0.0005	0.9995	0.9995
//			*MESH_TVERT 19	0.0005	0.0005	0.9995
//			*MESH_TVERT 20	0.0005	0.0005	0.0005
//			*MESH_TVERT 21	0.9995	0.0005	0.0005
//			*MESH_TVERT 22	0.9995	0.9995	0.0005
//			*MESH_TVERT 23	0.9995	0.9995	0.0005
//			*MESH_TVERT 24	0.0005	0.9995	0.0005
//			*MESH_TVERT 25	0.0005	0.0005	0.0005
//			*MESH_TVERT 26	0.0005	0.0005	0.9995
//			*MESH_TVERT 27	0.9995	0.0005	0.9995
//			*MESH_TVERT 28	0.9995	0.9995	0.9995
//			*MESH_TVERT 29	0.9995	0.9995	0.9995
//			*MESH_TVERT 30	0.0005	0.0005	0.9995
//			*MESH_TVERT 31	0.9995	0.0005	0.9995
//			*MESH_TVERT 32	0.9995	0.9995	0.9995
//			*MESH_TVERT 33	0.9995	0.9995	0.9995
//			*MESH_TVERT 34	0.0005	0.0005	0.0005
//			*MESH_TVERT 35	0.9995	0.9995	0.0005
//		}
//
//		// This next block is the indices for the faces into the texture coordinate array.
//      // Just like the vertex indices with the A B and C values, these do work the same.
//		// We use the 3 values to index the UV coordinates in the texture coordinate array.
//		// The format after the *MESH_TFACE is:  The face index, then the 3 indices into the array.
//      // For example, the top line below has a 0 for the index (not read in)
//      // the next is an 8, which will be used to index the 8th texture coordinate in the
//      // array.  Like so:  int coordIndex = pObject->pFaces[j].coordIndex[whichVertex];
//		// If j was zero and whichVertex was zero, coordIndex would be 8.  Get it?
//
//		*MESH_NUMTVFACES 12				// This is the number of textured faces
//		*MESH_TFACELIST {				// This tag tells us the UV indices are next
//			*MESH_TFACE 0	8	9	10	// Format: faceIndex UVIndex1  UVIndex2  UVIndex3
//			*MESH_TFACE 1	11	12	13  // ...
//			*MESH_TFACE 2	14	15	16
//			*MESH_TFACE 3	17	18	19
//			*MESH_TFACE 4	20	21	22
//			*MESH_TFACE 5	23	24	25
//			*MESH_TFACE 6	26	27	28
//			*MESH_TFACE 7	29	5	1
//			*MESH_TFACE 8	30	31	32
//			*MESH_TFACE 9	33	7	3
//			*MESH_TFACE 10	34	0	35
//			*MESH_TFACE 11	4	6	2
//		}
//	}
//	
// So that's the main stuff.  There is more things like animation and normals but
// I prefer to calculate my own normals, because not all formats have vertex normals
// and animation is a huge topic not to be discussed in a file format tutorial :)
// For those of you who don't know what vertex normals are, it means you have a normal
// for not just every polygon, but for every vertex.  That makes lighting more real looking.
// Also it makes the models look more smooth. 
// 
// *Note* - Remember after saving your .Ase file, to delete the path in front
// of the texture name.  You want: "texture.bmp" verses "C:\MyStuff\Pictures\texture.bmp"
// That way you can just stick the bitmap in the same folder as the program and not be
// dependant on paths.
//
// I would like to thank Nate Miller for the inspiration with his .Ase loader.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
