//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Vertex Arrays									 //
//																		 //
//		$Description:	Shows how to render quickly using vertex arrays  //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

UINT g_Texture[MAX_TEXTURES];							// This will reference to our texture data stored with OpenGL UINT is an unsigned int (only positive numbers)

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This tutorial shows you how to use vertex arrays to speed up your rendering.
// Vertex arrays allow you to give OpenGL your data in array format, which is
// then rendered directly to the card from the arrays.  This cuts down on doing
// long loops that will of course slow down the performance of your rendering.
//
// For instance, say you have a huge object that you are loading in which has
// thousands of vertices.  You would usually do a big for loop to go through every
// vertice and render it using glBegin() and glVertex3f() right?  Well, instead of
// the overhead of thousands of calls to glVertex3f() we can do just ONE call to
// something like: glDrawArrays(...).  Have I got you interested yet?  This is a
// necessity in any professional project that deals with rendering.  If you check
// out any large game like Quake, you will notice that they use vertex arrays.  
// Games need every little bit of performance that they can get since the world
// data is so massive.  
//
// Using vertex arrays will increase your performance by double
// if not 10 or 20 fold, depending on how much data you are rendering.  Of course,
// in this tutorial we just throw up some triangles to the screen so it doesn't make
// that big of difference, but I wanted to create a "simple" demo showing most of the
// ways that you would use vertex arrays.  Add about 5000 more triangles and then
// test the difference :).  We cover these types of vertex arrays: 
// 
// glDrawArrays(),	glDrawElements(),	glInterleavedArrays(),	glArrayElement()
//
// * Setting Up Your Arrays *
//
// Before you get tangled in the code let's go over real fast how you do the setup for
// vertex arrays.  You will need these functions (depending on what you want to render):
//
// glEnableClientState(...);
// glTexCoordPointer(...);
// glVertexPointer(...);
// glNormalPointer(...);
// 
// glEnableClientState() takes one parameter to set which type of array you want to use.
// If you want to render vertex arrays with texture coordinates, you need to pass in,
// GL_TEXTURE_COORD_ARRAY, GL_VERTEX_ARRAY for vertices and GL_NORMAL_ARRAY.  Check MSDN
// (msdn.microsoft.com) for more defines (Color, etc...).  You only pass in one at time
// because after you call that function you need to call a gl***Pointer() function to
// tell OpenGL which array the data will come from.  Below is an simple code example:
//
// glEnableClientState(GL_TEXTURE_COORD_ARRAY);
// glTexCoordPointer(2, GL_FLOAT, 0, pObject->pTexVerts);
// 
// glEnableClientState(GL_VERTEX_ARRAY);
// glVertexPointer(3, GL_FLOAT, 0, pObject->pVerts);
//
// glEnableClientState(GL_NORMAL_ARRAY);
// glNormalPointer(GL_FLOAT, 0, pObject->pNormals);
//
// What we did above is point OpenGL to our data and told it what type of data it was.
// Just like glEnable() and glDisable() works, you can glEnableClientState() and
// glDisableClientState() the same way.  We pass into the pointer functions the type
// of data it is (float), sometimes how many (3 for vertices) and before passing in the
// actual data array (pObject->***) we pass in 0 for a "stride".  This is because we
// have have an array with just one type of data.  We don't have a complicated structure
// (like CVertex) that stores multiple data types.  If we did, we need to pass in the
// sizeof() the data type, so it knows how to extract the texture/vertex/normal data.
// Look at our use of glInterleavedArrays() for an example of this 
// (or see our Quake BSP Loader).
//
// Let's go and dive into the actaul rendering functions for vertex arrays:
//
// * glDrawArrays() * 
//
// This is probably the most popular way to use vertex arrays.  All you pass in
// is the type (triangles, etc...), the starting index in the array and the number of 
// elements of the array to render:
//
// glDrawArrays(GL_TRIANGLES, 0, 9);
//
// The code above will render triangles, starting at index 0 through index 9.  Remember, it
// is important to note that this does not mean it will render 9 triangles, but 9 vertices.
// This of course makes up 3 triangles.
//
// * glDrawElements() *
//
// This function is a bit more complicated, but more useful when your data deals with
// objects that have face data.  For instance, when you load a 3D file format, it isn't
// just set up to have the vertices rendering in a row.  You have to create a "face"
// structure that is setup to use the same vertices for different triangles.  This
// glDrawElements() function allows you to pass in indices that will tell OpenGL to
// draw in that order.  Let's see an example:
//
// glDrawElements(GL_TRIANGLES, pObject->m_numberOfFaces * 3, GL_UNSIGNED_INT, pObject.m_pIndices);
//
// As you can see, you pass in the type (triangles, etc...), how many indices you want to
// render (the number of faces of the object * 3 [assuming it's triangles]), then the
// type that the indices are stored as (unsigned int) and finally, the array of indices).
//
// The only downside to this is that when you load an object from a 3D file format, usually
// there is more texture coordinates and/or normals than there is vertices.  Since you can't
// specify different arrays of indices for each data type (vertex, textureCoords, etc...) it
// makes it a pay to render all that information using vertex arrays.  It will render the 
// vertices fine, but the textures will look funny.  To get around this you have about 2 options.
// 1) The first is to go through and just double your vertex data and stick it in a structure
// like CVertex (see below in the tutorial) which has the vertice, texture and normal information
// in one structure.  This of course doubles your memory, but in the long run (if added memory 
// isn't a problem with your computer) it does render faster.  The second method is to create
// the structure that holds that information, but set up your data so that you render using
// triangle fans or triangle strips.  Many games do this, which is always the fastest result.
// You can check out our Quake3 BSP Loader tutorials to see this in action.
// 
// * glInterleavedArrays() *
//
// This function is kind of misleading.  This function doesn't actually draw anything, but
// allows you to set up what you want to draw.  Meaning, that instead of making all the
// called to glEnableClientState() and gl***Pointer(), you just pass in the type of data
// that you want to render (be it texture coordiantes, vertices, colors, and normals).
// Here is an example of this:
//
// glInterleavedArrays(GL_T2F_V3F, sizeof(CVertex), &g_InterleavedVertices);
//
// Check below for the definition of CVertex, but essentially what we are doing is
// telling OpenGL that we have a structure that has texture and vertex data (GL_T2F_V3F).
// We then tell OpenGL the size of our structure to know how much stride to do when
// accessing the arrays for the texture and vertex data.  Finally, we give OpenGL a
// pointer to our data (array) which has all of the texture and vertex data.
//
// * REMEMBER * You need to make sure that you set up the data in your structure (or class) 
// in the same order as you tell OpenGL it is.  By passing in GL_T2F_V3F we told OpenGL that 
// our texture data comes before the vertex data.  In our class/structure make sure that
// you define your data as such (see CVertex).  If you don't it won't work.  The data will
// be confused.
// 
// There are many other defines for interleaved arrays: GL_V3F, GL_N3F_V3F, GL_T2F_C4UB_V3F
//														GL_T2F_C4F_N3F_V3F, etc...
// See MSDN for all the defines available for this function.  Anyway, once we call this
// function, we didn't draw anything yet, so we need to call something like glDrawArrays().
// This is what actually renders the data, not glInterleavedArrays().
//
// * glArrayElement() *
//
// Finally, the simplest function is glArrayElement().  This only draws ONE of the elements
// in the arrays that you passed into OpenGL when you setup the vertex arrays.  You might
// think that this isn't any different than just doing it without vertex arrays, but
// you only call one function for rendering each vertex, instead of calling many more
// functions when you do the texture and normal information.  It can cut down your overhead
// of functions by a great deal.  Naturally, this isn't what you would usually use, but
// OpenGL thought somebody might want to use it somewhere :)
//
// When using glArrayElement(), you need to call it inside of glBegin() and glEnd():
//
// glBegin(GL_TRIANGLES);
//    glArrayElement(0);	glArrayElement(1);		glArrayElement(2);
// glEnd();
//
// The above code just renders 1 triangle from the vertex arrays with index 0, 1 and 2.
//
// * So What Does This Tutorial Do? *
//
// In this tutorial we just display a bunch of triangle using different vertex array methods
// to give you some working SIMPLE code on how to use them.  So, sit back and enjoy the
// "spinning triangles of love" :)
//
//

// This is a simple class to demonstrate using vertex arrays with an object     
class CObject
{
public:
	CVector3 *m_pVertices;			// This stores all the object's vertices
	CVector2 *m_pTextureCoords;		// This stores all the objects UV coordinates
	UINT	 *m_pIndices;			// This stores the face indices for the object

	int m_numOfTriangles;			// This stores the....  Just read the variable name :)
};

// This is a simple class to demonstrate using interleaved arrays (UV and vertex info together)
class CVertex
{
public:
	CVector2 m_textureCoord;
	CVector3 m_vertex;	
};

// Create a global 3D object
CObject g_Object;

// Create an array of just 3 triangles (9 vertices) to demonstrate interleaved arrays
CVertex g_InterleavedVertices[9];

// Create a prototype for our add triangle function so Init() can use it
void AddTriangle(float, float, float);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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

	glEnable(GL_DEPTH_TEST);							// Turn depth testing on
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we load 4 textures from a file to texture map onto our triangles
	CreateTexture(g_Texture[1], "Texture1.bmp");
	CreateTexture(g_Texture[2], "Texture2.bmp");
	CreateTexture(g_Texture[3], "Texture3.bmp");
	CreateTexture(g_Texture[4], "Texture4.bmp");

	// Here we allocate memory for our data arrays
	g_Object.m_pTextureCoords = new CVector2 [MAX_VERTICES];
	g_Object.m_pVertices = new CVector3 [MAX_VERTICES];
	g_Object.m_pIndices  = new UINT [MAX_VERTICES];

	// Here we create 5 groups of triangle groups
	for(int triangleGroup = 0; triangleGroup < 5; triangleGroup++)
	{
		AddTriangle(0, 0, 0);			// Make a centered triangle
		AddTriangle(-2.0f, -4.0f, 0);	// Make a triangle on the left
		AddTriangle(2.0f, -4.0f, 0);	// Make a triangle on the right
	}

	// Since we don't have any special faces, just create indices for every index in a row.
	// Of course, if this was the case, we wouldn't need to do this, just use glDrawArrays()
	// instead of glDrawElements().  This is just to demonstrate the functionality.
	for(int i = 0; i < MAX_VERTICES; i++)
		g_Object.m_pIndices[i] = i;  

	// Here we just copy the first triangle group data to the interleaved array
	for(int n = 0; n < 9; n++)
	{
		g_InterleavedVertices[n].m_textureCoord = g_Object.m_pTextureCoords[n];
		g_InterleavedVertices[n].m_vertex = g_Object.m_pVertices[n];
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Free the allocated memory
	delete [] g_Object.m_pVertices;
	delete [] g_Object.m_pTextureCoords;
	delete [] g_Object.m_pIndices;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// ADD TRIANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This adds a triangle to our list of triangles and moves it by an offset
/////
///////////////////////////////// ADD TRIANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void AddTriangle(float xOffset, float yOffset, float zOffset)
{
	// Set up the left, top and right corner of a new triangle, depending on the offset
	CVector3 vLeftCorner(xOffset - 2.0f, yOffset, zOffset);
	CVector3 vTopCorner(xOffset, yOffset + 4.0f , zOffset);
	CVector3 vRightCorner(xOffset + 2.0f, yOffset, zOffset);

	// Create the basic UV coordinates for the triangle corners
	CVector2 vLeftUV = CVector2(0.0f, 1.0f);
	CVector2 vTopUV  = CVector2(0.5f, 0.0f);
	CVector2 vRightUV = CVector2(1.0f, 1.0f);

	// Add a new triangle to our array of vertices
	g_Object.m_pVertices[g_Object.m_numOfTriangles*3] = vLeftCorner;
	g_Object.m_pVertices[g_Object.m_numOfTriangles*3 + 1] = vTopCorner;
	g_Object.m_pVertices[g_Object.m_numOfTriangles*3 + 2] = vRightCorner;

	// Add the triangle's UV coordinates to our texture coordinate array
	g_Object.m_pTextureCoords[g_Object.m_numOfTriangles*3] = vLeftUV;
	g_Object.m_pTextureCoords[g_Object.m_numOfTriangles*3 + 1] = vTopUV;
	g_Object.m_pTextureCoords[g_Object.m_numOfTriangles*3 + 2] = vRightUV;

	// Increase the triangle count
	g_Object.m_numOfTriangles++;
}


///////////////////////////// DRAW VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws triangles using vertex arrays with glDrawArrays()
/////
///////////////////////////// DRAW VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawVertexArrays()
{
	// Create a static rotating variable for rotating around the y-axis
	static float rotateY = 0;

	// Bind our first texture map to the triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[1]);

	// Below, what we do is not render all of the triangles at a time,
	// but in order to rotate and translate our triangles we brake up
	// the calls the glDrawArrays().  This is also to show functionality.

	// Push on a new matrix so we don't effect the other triangles, then
	// rotate the first 3 triangles (9 vertices) around the y-axis.
	glPushMatrix();
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawArrays(GL_TRIANGLES, 0, 9);
	glPopMatrix();

	// Move the next 3 rotating triangles over to the right.  Notice that we start
	// from 9 and then render 9 indices (9 vertices).
	glPushMatrix();
		glTranslatef(8.0f, 0.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawArrays(GL_TRIANGLES, 9, 9);
	glPopMatrix();

	// Move 3 more rotating triangles over to the left (starting from the 18th index)
	glPushMatrix();
		glTranslatef(-8.0f, 0.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawArrays(GL_TRIANGLES, 18, 9);
	glPopMatrix();

	// Move 3 more rotating triangles down
	glPushMatrix();
		glTranslatef(0.0f, -8.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawArrays(GL_TRIANGLES, 27, 9);
	glPopMatrix();

	// Move 3 more rotating triangles up
	glPushMatrix();
		glTranslatef(0.0f, 8.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawArrays(GL_TRIANGLES, 36, 9);
	glPopMatrix();

	// Increase our rotation angle
	rotateY += 3;
}

///////////////////////////// DRAW INDEXED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws triangles using vertex arrays with glDrawElements()
/////
///////////////////////////// DRAW INDEXED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawIndexedVertexArrays()
{
	// Create a static rotating angle for the y-axis
	static float rotateY = 0;

	// Bind the second texture to these triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[2]);

	// Here we push on a matrix to flip the triangles upside down
	glPushMatrix();

	// Flip the triangles upside down
	glRotatef(180, 1.0f, 0.0f, 0.0f);

	// Draw 3 rotating triangles by passing in our indices and only rendering 9 vertices
	glPushMatrix();
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, g_Object.m_pIndices);
	glPopMatrix();

	// Move another rotating triangle group over to the right.  Notice that we
	// pass in the address to the 9th index of our object.  This makes sure we
	// pass in the next 9 indices in our vertex arrays.
	glPushMatrix();
		glTranslatef(8.0f, 0.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[9]);
	glPopMatrix();

	// Move another rotating triangle group over to the left.
	glPushMatrix();
		glTranslatef(-8.0f, 0.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[18]);
	glPopMatrix();

	// Move another rotating triangle group down.
	glPushMatrix();
		glTranslatef(0.0f, -8.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[27]);
	glPopMatrix();

	// Move another rotating triangle group up.
	glPushMatrix();
		glTranslatef(0.0f, 8.0f, 0.0f);
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[36]);
	glPopMatrix();

	// Go back to normal before we flipped the triangles upside down
	glPopMatrix();

	// Increase the rotation angle
	rotateY += 1;
}

///////////////////////////// DRAW INTERLEAVED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws triangles using vertex arrays with glInterleavedArrays()
/////
///////////////////////////// DRAW INTERLEAVED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawInterleavedVertexArrays()
{
	// Create a static rotation angle for the y-axis
	static float rotateY = 0;

	// Bind the third texture for these triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[3]);

	// Push on a new matrix to do some translation and rotations.  We want to rotate
	// a group of 3 triangles around the whole group of spinning triangles.  Notice that
	// we tell OpenGL that we want to pass in a structure that stores our texture and
	// vertex data (in that exact order), then we draw the vertex interleaved arrays
	// with glDrawArrays().
	glPushMatrix();
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glTranslatef(0.0f, 0.0f, -10.0f);
		glInterleavedArrays(GL_T2F_V3F, sizeof(CVertex), &g_InterleavedVertices);
		glDrawArrays(GL_TRIANGLES, 0, 9);
	glPopMatrix();

	// Increase our rotation angle
	rotateY += 2;
}

///////////////////////////// DRAW ARRAY ELEMENT VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws triangles using vertex arrays with glArrayElement()
/////
///////////////////////////// DRAW ARRAY ELEMENT VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawArrayElementVertexArrays()
{
	// Create a static rotation angle for the y-axis
	static float rotateY = 0;

	// Bind the fourth texture for these triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[4]);

	// Push on a new matrix to do some rotations and translations around the other
	// groups of triangles.  For rendering vertex arrays with glArrayElement() we
	// just go through 9 vertices of our vertex arrays inside of a glBegin()/glEnd()
	// scope.
	glPushMatrix();

		// Rotate around the axis and move away from the origin
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				
		glTranslatef(0.0f, 0.0f, 10.0f);

		// Draw 9 vertices from our vertex array
		glBegin (GL_TRIANGLES);
			for(int i = 0; i < 9; i++)
				glArrayElement(i);
		glEnd();

	glPopMatrix();

	// Increase the rotation angle
	rotateY += 2;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	
void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	// 	       Position        View	     Up Vector
	gluLookAt(0, 0, 40,     0, 0, 39,     0, 1, 0);		// Move the camera back

	glClearColor(0.0f, 0.0f, 1.0f, 1);					// Turn the background color blue	


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we set up the vertex arrays.  We tell OpenGL where we want
	// to get the data for the texture coordinates and the vertices.
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, g_Object.m_pTextureCoords);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, g_Object.m_pVertices);

	// Draw spinning triangles using glDrawArrays()
	DrawVertexArrays();

	// Draw upside down spinning triangles using glDrawElements()
	DrawIndexedVertexArrays();

	// Draw 3 rotating triangles that are interleaved data using glInterleavedArrays()
	DrawInterleavedVertexArrays();

	// Draw 3 rotating triangles using glArrayElement()
	DrawArrayElementVertexArrays();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
	LRESULT    lRet = 0; 
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

	case WM_KEYDOWN:
		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
		}
		break;
 
    case WM_CLOSE:										// If the window is being closes
        PostQuitMessage(0);								// Send a QUIT Message to the window
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
// This should give you a pretty solid base to using the functionality of vertex
// arrays in OpenGL.  We went over the usage of glDrawArrays(), glDrawElements(),
// glInterleavedArrays() and glArrayElement().
// 
// We didn't add normals and color to this tutorial, but in order to make this 
// tutorial as simple as possible I left out this, since it's as easy to setup as 
// the texture coordinates.  I gave a small example of setting this up in comments
// at the top of this page.
// 
// Probably the most annoying thing with vertex arrays is that you can't specify
// different arrays of indices when using DrawElements().  This means you have to
// bunch your data together in one structure, which can sometimes cause doubling
// of information.  But, that's OK, I forgive OpenGL :)
// 
// Here is a great source for more OpenGL information on many different subjects:
// 
// http://www.parallab.uib.no/SGI_bookshelves/SGI_Developer/books/OpenGL_PG/sgi_html/ch03.html
// 
// This is a book that explains many OpenGL concepts and give examples, like vertex arrays :)
// This can be used as a great reference.
//
// Hopefully this tutorial gives you more confidence when using vertex arrays in your
// own projects.
// 
// * Where To Go From Here? *
// 
// Just because you know how to use vertex arrays, doesn't mean there isn't faster ways to
// render :)  You will want to look at "Compiled Vertex Arrays" next.  This allows you
// to "Lock" your rendered data in memory so when you do multiple passes you can increase
// your rendering time by a great deal.  This is an extra extension to OpenGL.
// 
// (This tutorial was taken from the base code of the RenderToTexture tutorial)
//
// Good luck!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//

