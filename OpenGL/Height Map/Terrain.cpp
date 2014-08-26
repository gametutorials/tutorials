//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Height Map										 //
//																		 //
//		$Description:	This shows how render a height map from a file.  //
//																		 //
//***********************************************************************//

#include "main.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This file handles all of the terrain functions.  
//
// * NOTE * 
//
// The terrain might look small on the screen, and considering how fast we
// move about it, but it's actually quite huge and could be a whole world 
// to walk around.  So don't be fooled :)
//
//
//

///////////////////////////////// HEIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the height into the height map
/////
///////////////////////////////// HEIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int Height(BYTE *pHeightMap, int X, int Y)
{
	// This is used to index into our height map array.
	// When ever we are dealing with arrays, we want to make sure
	// that we don't go outside of them, so we make sure that doesn't
	// happen with a %.  This way x and y will cap out at (MAX_SIZE - 1)

	int x = X % MAP_SIZE;					// Error check our x value
	int y = Y % MAP_SIZE;					// Error check our y value

	if(!pHeightMap) return 0;				// Make sure our data is valid

	// Below, we need to treat the single array like a 2D array.
	// We can use the equation: index = (x + (y * arrayWidth) ).
	// This is assuming we are using this assumption array[x][y]
	// otherwise it's the opposite.  Now that we have the correct index,
	// we will return the height in that index.

	return pHeightMap[x + (y * MAP_SIZE)];	// Index into our height array and return the height
}


///////////////////////////////// SET VERTEX COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the color value for a particular index, depending on the height index
/////
///////////////////////////////// SET VERTEX COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void SetVertexColor(BYTE *pHeightMap, int x, int y)
{
	if(!pHeightMap) return;					// Make sure our height data is valid

	// Here we set the color for a vertex based on the height index.
	// To make it darker, I start with -0.15f.  We also get a ratio
	// of the color from 0 to 1.0 by dividing the height by 256.0f;
	float fColor = -0.15f + (Height(pHeightMap, x, y ) / 256.0f);

	// Assign this green shade to the current vertex
	glColor3f(0, fColor, 0 );
}


///////////////////////////////// RENDER HEIGHT MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders the height map as QUADS
/////
///////////////////////////////// RENDER HEIGHT MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderHeightMap(BYTE pHeightMap[])
{
	int X = 0, Y = 0;						// Create some variables to walk the array with.
	int x, y, z;							// Create some variables for readability
	float fColor = 0.0f;					// Create a variable to hold our color of the polygon

	if(!pHeightMap) return;					// Make sure our height data is valid

	glBegin( GL_QUADS );					// Render Quads

	// Next we actually need to draw the terrain from the height map.
	// To do that, we just walk the array of height data and pluck out
	// some heights to plot our points.  If we could see this happening,
	// it would draw the columns first (Y), then draw the rows.
	// Notice that we have a STEP_SIZE.  This determines how defined our
	// height map is.  The higher the STEP_SIZE, the more blocky the terrain
	// looks, while the lower it gets, the more rounded it becomes.
	// If we set STEP_SIZE = 1 it would create a vertex for every pixel in the height map.
	// I chose 16 as a decent size.  Anything too much less gets to be insane and slow.
	// Of course, you can increase the number when you get lighting in.
	// Then vertex lighting would cover up the blocky shape.  Instead of lighting,
	// we just put a color value associated with every poly to simplify the tutorial.
	// The higher the polygon, the brighter the color is.

	for ( X = 0; X < MAP_SIZE; X += STEP_SIZE )
		for ( Y = 0; Y < MAP_SIZE; Y += STEP_SIZE )
		{
			// Get the (X, Y, Z) value for the bottom left vertex		
			x = X;							
			y = Height(pHeightMap, X, Y );	
			z = Y;							

			// Set the color value of the current vertice
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered (integer points are faster)

			// Get the (X, Y, Z) value for the top left vertex		
			x = X;										
			y = Height(pHeightMap, X, Y + STEP_SIZE );  
			z = Y + STEP_SIZE ;							
			
			// Set the color value of the current vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered

			// Get the (X, Y, Z) value for the top right vertex		
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE ); 
			z = Y + STEP_SIZE ;

			// Set the color value of the current vertex
			SetVertexColor(pHeightMap, x, z);
			
			glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered

			// Get the (X, Y, Z) value for the bottom right vertex		
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y ); 
			z = Y;

			// Set the color value of the current vertice
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered
		}
	glEnd();

	// Reset the color
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


///////////////////////////////// LOAD RAW FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads a .raw file into an array of bytes.  Each value is a height value.
/////
///////////////////////////////// LOAD RAW FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	// Let's open the file in Read/Binary mode.
	pFile = fopen( strName, "rb" );

	// Check to see if we found the file and could open it
	if ( pFile == NULL )	
	{
		// Display our error message and stop the function
		MessageBox(NULL, "Can't find the height map!", "Error", MB_OK);
		return;
	}

	// Here we load the .raw file into our pHeightMap data array.
	// We are only reading in '1', and the size is the (width * height)
	fread( pHeightMap, 1, nSize, pFile );

	// After we read the data, it's a good idea to check if everything read fine.
	int result = ferror( pFile );

	// Check if we received an error.
	if (result)
	{
		MessageBox(NULL, "Can't get data!", "Error", MB_OK);
	}

	// Close the file.
	fclose(pFile);
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Though being one of the simplest ways to display terrain from a height map,
// it's a good start.  The next step we would take would be to make a quad tree
// that only displayed the vertices that we could see from our view frustum.
// This would allow us to display large terrains, but not have to push so many polygons.
// This is a form of space partitioning.  There isn't as many good places to learn
// about space partitioning as I would like, but the best place is www.GameInstitute.com.
// They have an excellent terrain rendering course that deals with all of these problems,
// including line of sight, terrain rendering techniques, adaptable meshes, geomorphing, etc..
// 
// Let's go over the steps that we accomplished during this tutorial
//
// 1) First, we need to read the height map from the .raw file.  This is simple because
//    there is no header to a .raw file, it is just the image bits.  This file format
//    isn't what you generally want to use because you have to either know what the
//    size and type are, or guess, but I thought it fitting for this tutorial.
// 
// 2) After we read our height map data, we then needed to display it.  This was
//    also a simple function because we are just making QUADS with a set size.
//    I chose to do 16 by 16 quads, but you can change this to what ever you want.
//    With our height map array, we treated it as a 2D array and did 2 for loops
//    to draw each quad for each row and column.  Instead of doing lighting, I
//    just gave each vertex a green intensity. depending on it's height.  This makes
//    the terrain look like there is lighting applied.  This also makes it easier to
//    see the definition in the terrain until lighting and texture maps are applied.
//    
// 
// That's pretty much it.  As for doing texturing, this would be a good way to do
// texture tiling.  You can go into a paint program and create a blank
// file the size of your height map.  This will allow you to set up your textures
// to see how you want it to look.  Then, make a 2D array that stores an index
// into your texture array, then just bind that texture to the current quad that matches. 
// it's X and Y position.  Maybe we will write a tutorial on this. 
// 
// The other option is to let OpenGL find the texture coordinates for you, or you can
// find them yourself by stretching a texture over the whole thing (which isn't always desirable),
// or wrap it.  Remember, (0, 0) is the top left corner and (1,1) is the bottom right corner.
// You will have to find the ratio you want to wrap it with.  Here is an example:
// 
// glTexCoord2f(   (float)x / (float)MAP_SIZE,	
//				 - (float)z / (float)MAP_SIZE	);
//
// Make sure you cast integers to floats when necessary otherwise it will truncate it.
// The z is actually the Y value of the height map, but the z value in world coordinates.
// Check out the next height map tutorial to see texturing a height map in action,
// along with rendering the terrain using triangle strips.
//
// Good luck!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
        

