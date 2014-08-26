//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		3DS Loader										 //
//																		 //
//		$Description:	Demonstrates how to load a .3ds file format		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "3ds.h"										// Include the 3DS header file.

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial will demonstrate how to load a .3DS file.  This 3D file format
// is created from 3D Studio Max.  It can be exported and important into many
// other programs.  A great tool for converting 3D file formats is "3D Exploration".
// This is a shareware utility that can be found for download on the web.
// This 3DS loader only loads the texture names, object colors, the vertices, the faces, and the UV coordinates.
// The key frame information is ignored since we have a Key Frame Animation tutorial later.
// I didn't want to over complicate this tutorial, but just show the basics.  
//
// In this tutorial, there is a picture of a face rotating around.  It is a single object
// with a single texture.  The .3ds loader does load multiple objects though.
// The controls are: 

// Left Mouse Button - Changes the Render mode from normal to wireframe.
// Right Mouse Button - Turns lighting On/Off
// Left Arrow Key - Spins the model to the left
// Right Arrow Key - Spins the model to the right
// Escape - Quits

#define FILE_NAME  "face.3ds"							// This is the 3D file we will load.

UINT g_Texture[MAX_TEXTURES] = {0};						// This holds the texture info, referenced by an ID

CLoad3DS g_Load3ds;										// This is 3DS class.  This should go in a good model class.
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

	// First we need to actually load the .3DS file.  We just pass in an address to
	// our t3DModel structure and the file name string we want to load ("face.3ds").

	g_Load3ds.Import3DS(&g_3DModel, FILE_NAME);			// Load our .3DS file into our model structure

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
			// We pass in our global texture array, the name of the texture, and an ID to reference it.	
			CreateTexture(g_3DModel.pMaterials[i].strFile, g_Texture[i]);			
		}

		// Set the texture ID for this material
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
		elapsedTime -= desiredFPS; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function Handles the main game loop
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinate loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasnt to quit
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
	gluLookAt(		0, 1.5f, 8,		0, 0.5f, 0,			0, 1, 0);
	
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
					// Get the index for each point of the face
					int index = pObject->pFaces[j].vertIndex[whichVertex];
			
					// Give OpenGL the normal for this vertex.
					glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
				
					// If the object has a texture associated with it, give it a texture coordinate.
					if(pObject->bHasTexture) {

						// Make sure there was a UVW map applied to the object or else it won't have tex coords.
						if(pObject->pTexVerts) {
							glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
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
					glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
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
 
    return 0;											// If we handled the messge return 0
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows how to load a .3ds file.  This is a good addition to an engine.
// In the next tutorial we will show you how to load a text file format called .obj.
// This is the most common 3D file format that almost ANY 3D software will import.
// 
// I hope the model loading code wasn't to intimidating.  The first time I read model
// loading code I didn't get the vertex indexing and such, but after careful reading, 
// It all came together.  Please email me if you are confused on ANY of this and I will
// be happy to clarify it until you get it.
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
// Once again I should point out that the coordinate system of OpenGL and 3DS Max are different.
// Since 3D Studio Max Models with the Z-Axis pointing up (strange and ugly I know! :), 
// we need to flip the y values with the z values in our vertices.  That way it
// will be normal, with Y pointing up.  Also, because we swap the Y and Z we need to negate 
// the Z to make it come out correctly.  This is also explained and done in ReadVertices().
//
// Below explains what was explained in 3ds.cpp regarding chucks:
// 
// CHUNKS: What is a chunk anyway?
// 
// "The chunk ID is a unique code which identifies the type of data in this chunk 
// and also may indicate the existence of subordinate chunks. The chunk length indicates 
// the length of following data to be associated with this chunk. Note, this may 
// contain more data than just this chunk. If the length of data is greater than that 
// needed to fill in the information for the chunk, additional subordinate chunks are 
// attached to this chunk immediately following any data needed for this chunk, and 
// should be parsed out. These subordinate chunks may themselves contain subordinate chunks. 
// Unfortunately, there is no indication of the length of data which is owned by the current 
// chunk, only the total length of data attached to the chunk, which means that the only way 
// to parse out subordinate chunks is to know the exact format of the owning chunk. On the 
// other hand, if a chunk is unknown, the parsing program can skip the entire chunk and 
// subordinate chunks in one jump. " - Jeff Lewis (werewolf@worldgate.com)
//
// Below is a list of the order that you will find the chunks and all the know chunks.
// If you go to www.wosit.org you can find the 3DS file format for a huge document explaining
// the little things I skipped.
//
//
//
//      MAIN3DS  (0x4D4D)
//     |
//     +--EDIT3DS  (0x3D3D)
//     |  |
//     |  +--EDIT_MATERIAL (0xAFFF)
//     |  |  |
//     |  |  +--MAT_NAME01 (0xA000) (See mli Doc) 
//     |  |
//     |  +--EDIT_CONFIG1  (0x0100)
//     |  +--EDIT_CONFIG2  (0x3E3D) 
//     |  +--EDIT_VIEW_P1  (0x7012)
//     |  |  |
//     |  |  +--TOP            (0x0001)
//     |  |  +--BOTTOM         (0x0002)
//     |  |  +--LEFT           (0x0003)
//     |  |  +--RIGHT          (0x0004)
//     |  |  +--FRONT          (0x0005) 
//     |  |  +--BACK           (0x0006)
//     |  |  +--USER           (0x0007)
//     |  |  +--CAMERA         (0xFFFF)
//     |  |  +--LIGHT          (0x0009)
//     |  |  +--DISABLED       (0x0010)  
//     |  |  +--BOGUS          (0x0011)
//     |  |
//     |  +--EDIT_VIEW_P2  (0x7011)
//     |  |  |
//     |  |  +--TOP            (0x0001)
//     |  |  +--BOTTOM         (0x0002)
//     |  |  +--LEFT           (0x0003)
//     |  |  +--RIGHT          (0x0004)
//     |  |  +--FRONT          (0x0005) 
//     |  |  +--BACK           (0x0006)
//     |  |  +--USER           (0x0007)
//     |  |  +--CAMERA         (0xFFFF)
//     |  |  +--LIGHT          (0x0009)
//     |  |  +--DISABLED       (0x0010)  
//     |  |  +--BOGUS          (0x0011)
//     |  |
//     |  +--EDIT_VIEW_P3  (0x7020)
//     |  +--EDIT_VIEW1    (0x7001) 
//     |  +--EDIT_BACKGR   (0x1200) 
//     |  +--EDIT_AMBIENT  (0x2100)
//     |  +--EDIT_OBJECT   (0x4000)
//     |  |  |
//     |  |  +--OBJ_TRIMESH   (0x4100)      
//     |  |  |  |
//     |  |  |  +--TRI_VERTEXL          (0x4110) 
//     |  |  |  +--TRI_VERTEXOPTIONS    (0x4111)
//     |  |  |  +--TRI_MAPPINGCOORS     (0x4140) 
//     |  |  |  +--TRI_MAPPINGSTANDARD  (0x4170)
//     |  |  |  +--TRI_FACEL1           (0x4120)
//     |  |  |  |  |
//     |  |  |  |  +--TRI_SMOOTH            (0x4150)   
//     |  |  |  |  +--TRI_MATERIAL          (0x4130)
//     |  |  |  |
//     |  |  |  +--TRI_LOCAL            (0x4160)
//     |  |  |  +--TRI_VISIBLE          (0x4165)
//     |  |  |
//     |  |  +--OBJ_LIGHT    (0x4600)
//     |  |  |  |
//     |  |  |  +--LIT_OFF              (0x4620)
//     |  |  |  +--LIT_SPOT             (0x4610) 
//     |  |  |  +--LIT_UNKNWN01         (0x465A) 
//     |  |  | 
//     |  |  +--OBJ_CAMERA   (0x4700)
//     |  |  |  |
//     |  |  |  +--CAM_UNKNWN01         (0x4710)
//     |  |  |  +--CAM_UNKNWN02         (0x4720)  
//     |  |  |
//     |  |  +--OBJ_UNKNWN01 (0x4710)
//     |  |  +--OBJ_UNKNWN02 (0x4720)
//     |  |
//     |  +--EDIT_UNKNW01  (0x1100)
//     |  +--EDIT_UNKNW02  (0x1201) 
//     |  +--EDIT_UNKNW03  (0x1300)
//     |  +--EDIT_UNKNW04  (0x1400)
//     |  +--EDIT_UNKNW05  (0x1420)
//     |  +--EDIT_UNKNW06  (0x1450)
//     |  +--EDIT_UNKNW07  (0x1500)
//     |  +--EDIT_UNKNW08  (0x2200)
//     |  +--EDIT_UNKNW09  (0x2201)
//     |  +--EDIT_UNKNW10  (0x2210)
//     |  +--EDIT_UNKNW11  (0x2300)
//     |  +--EDIT_UNKNW12  (0x2302)
//     |  +--EDIT_UNKNW13  (0x2000)
//     |  +--EDIT_UNKNW14  (0xAFFF)
//     |
//     +--KEYF3DS (0xB000)
//        |
//        +--KEYF_UNKNWN01 (0xB00A)
//        +--............. (0x7001) ( viewport, same as editor )
//        +--KEYF_FRAMES   (0xB008)
//        +--KEYF_UNKNWN02 (0xB009)
//        +--KEYF_OBJDES   (0xB002)
//           |
//           +--KEYF_OBJHIERARCH  (0xB010)
//           +--KEYF_OBJDUMMYNAME (0xB011)
//           +--KEYF_OBJUNKNWN01  (0xB013)
//           +--KEYF_OBJUNKNWN02  (0xB014)
//           +--KEYF_OBJUNKNWN03  (0xB015)  
//           +--KEYF_OBJPIVOT     (0xB020)  
//           +--KEYF_OBJUNKNWN04  (0xB021)  
//           +--KEYF_OBJUNKNWN05  (0xB022)  
//
// Once you know how to read chunks, all you have to know is the ID you are looking for
// and what data is stored after that ID.  You need to get the file format for that.
// I can give it to you if you want, or you can go to www.wosit.org for several versions.
// Because this is a proprietary format, it isn't a official document.
//
// I know there was a LOT of information blown over, but it is too much knowledge for
// one tutorial.  In the animation tutorial that I eventually will get to, some of
// the things explained here will be explained in more detail.
// 
// I would like to thank wosit.org and Terry Caton (tcaton@umr.edu) for his help on this.
//
// Let me know if this helps you out!
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
