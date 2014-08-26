//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		HeightMap2										 //
//																		 //
//		$Description:	Show how to texture map a height mapped terrain  //
//																		 //
//***********************************************************************//


#include "main.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This file handles all of the terrain functions.  We changed
// RenderHeightMap() so that it puts a texture over the entire terrain.
// The terrain is rendered using triangle strips instead of quads.
//
//


///////////////////////////////// HEIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the height into the height map
/////
///////////////////////////////// HEIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int Height(BYTE *pHeightMap, int X, int Y)
{
	// Make sure we don't go past our array size
	int x = X % MAP_SIZE;					// Error check our x value
	int y = Y % MAP_SIZE;					// Error check our y value

	if(!pHeightMap) return 0;				// Make sure our data is valid

	// Use the equation: index = (x + (y * arrayWidth) ) to find the current height
	return pHeightMap[x + (y * MAP_SIZE)];	// Index into our height array and return the height
}


///////////////////////////////// SET TEXTURE COORD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the current texture coordinate of the terrain, based on the X and Z
/////
///////////////////////////////// SET TEXTURE COORD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void SetTextureCoord(float x, float z)
{
	// We want to calculate the texture coordinate for the current vertex.
	// To do this, we just take the current x and y and divide them by the
	// MAP_SIZE of our terrain.  This is of course assuming the height map
	// has the same width and height.  Notice that we negate the v value (IE, (u, v)).
	// This is because we need to flip the texture upside down so it lines up correctly.
	// This simple math we use to find the texture coordinate can be explained easily.
	// We know that our terrain is made up of a grid, and that if we want to stretch
	// the entire texture over top of that grid, we just divide the current section
	// of the grid by the total section of the grid, which gives us a ratio from 0
	// to 1.  This works great because the (u, v) coordinates for a texture map go
	// from (0, 0) in the top left corner to (1, 1) in the bottom right corner.

	// Give OpenGL the current texture coordinate for our height map
	glTexCoord2f(   (float)x / (float)MAP_SIZE,	
				  - (float)z / (float)MAP_SIZE	);
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
	bool bSwitchSides = false;

	// Make sure our height data is valid
	if(!pHeightMap) return;		
	
	// The difference from the way we render the terrain from
	// the original way we started with, is that we don't use GL_QUADS
	// anymore, we now use a GL_TRIANGLE_STIP.  This means that we don't
	// need to pass in the same vertex more than once.  Each 2 vertices
	// are connected to the next 2.  Since we want to do this in one strip,
	// we are going to need to reverse the order every other column.  It's
	// like moving the lawn.  Go to the end and turn around and come back
	// the way you came.  If you don't do it this way, you will get polygons
	// stretching across the whole terrain.  We could just do a new glBegin()
	// and glEnd() for every column, but I believe this way is faster.  
	// Not that that really matters though, because rendering a terrain
	// with glVertex*() calls in incredibly slow.  We will most likely want
	// to eventually switch this to vertex arrays.

	// Bind the terrain texture to our terrain
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

	// We want to render triangle strips
	glBegin( GL_TRIANGLE_STRIP );			

	// Go through all of the rows of the height map
	for ( X = 0; X <= MAP_SIZE; X += STEP_SIZE )
	{
		// Chechk if we need to render the opposite way for this column
		if(bSwitchSides)
		{	
			// Render a column of the terrain, for this current X.
			// We start at MAP_SIZE and render down to 0.
			for ( Y = MAP_SIZE; Y >= 0; Y -= STEP_SIZE )
			{
				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;							
				y = Height(pHeightMap, X, Y );	
				z = Y;							

				// Set the current texture coordinate and render the vertex
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);		

				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + STEP_SIZE; 
				y = Height(pHeightMap, X + STEP_SIZE, Y ); 
				z = Y;

				// Set the current texture coordinate and render the vertex
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);			
			}
		}
		else
		{	
			// Render a column of the terrain, for this current X.
			// We start at 0 and render down up to MAP_SIZE.
			for ( Y = 0; Y <= MAP_SIZE; Y += STEP_SIZE )
			{
				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + STEP_SIZE; 
				y = Height(pHeightMap, X + STEP_SIZE, Y ); 
				z = Y;

				// Set the current texture coordinate and render the vertex
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);

				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;							
				y = Height(pHeightMap, X, Y );	
				z = Y;							

				// Set the current texture coordinate and render the vertex
				SetTextureCoord( (float)x, (float)z );
				glVertex3i(x, y, z);		
			}
		}

		// Switch the direction the column renders to allow the fluid tri strips
		bSwitchSides = !bSwitchSides;
	}

	// Stop rendering triangle strips
	glEnd();
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


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Not much changed, but it gives us a cool wire frame view! :)
// We now render with triangle strips and have a texture stretched
// across the entire terrain.  The next tutorial for this genre will
// be detail texturing.  This is done so that you don't notice the
// horrible stretch marks all over the terrain.  Instead, it looks
// detailed.  The technique is done by blending a tiled detail 
// texture map across the entire terrain.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
        
