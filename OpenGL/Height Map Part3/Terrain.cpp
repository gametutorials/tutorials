//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		HeightMap3										 //
//																		 //
//		$Description:	Show how to apply a detail texture to a terrain  //
//																		 //
//***********************************************************************//

#include "main.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// In this version of our Height Map series, we tile a detail texture
// over top of the textured terrain to dilute the stretched texture look.
// Of course, even if you use multiple terrain textures so it's not just
// one large texture, you will still want to add detail textures regardless.
// To render the detail texture, multitexturing is used.  This assumes that
// your card and drivers support the common ARB extensions.  Any TNT or
// Geforce brand card should support this.  If not, get a new card or convert
// the code to a second blended pass.
//
//

// This tells us if we want detail texturing turned on or off
extern bool g_bDetail;

// This holds the current tile scale value for our detail texture
extern int g_DetailScale;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
	// Find the (u, v) coordinate for the current vertex
	float u =  (float)x / (float)MAP_SIZE;
	float v = -(float)z / (float)MAP_SIZE;
	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Instead of using the normal API for assigning the current texture
	// coordinate, we want to use the glMultiTexCoord2fARB() function.
	// This will allow us to choose the (u, v) coordinate for every texture
	// map, since we are using multitexturing.  Due to the fact that we
	// are going to be using the texture matrix to set the tiling of our
	// detail texture, we just assign the same (u, v) coordinate for both
	// textures.  

	// Give OpenGL the current terrain texture coordinate for our height map
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);

	// Give OpenGL the current detail texture coordinate for our height map
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Now comes the juice of our detail texture.  We want to combine the
	// terrain texture and the detail texture together.  To blend the two
	// textures nicely, we will want to use some ARB extension defines
	// for our texture properties.  The first one, GL_COMBINE_ARB, allows
	// us to use combine texture properties, which brings us to our second
	// one: GL_RGB_SCALE_ARB.  With this texture property, we can now 
	// increase the gamma of our second texture so that it does not darken
	// the texture beneath it.  This works great for lightmaps and detail 
	// textures.  I believe that the GL_RGB_SCALE_ARB property only accepts
	// 1, 2 or 4 as a valid scale value.  2 works just fine, where 4 is
	// too bright.  
	// 
	// To tile the detail texture appropriately, the texture matrix is
	// effected.  When we enter into the texture matrix mode, it allows us
	// to effect the current position of the selected texture.  This isn't
	// necessary to use, since we could just calculate the detail texture
	// coordinates on our own, but I thought it would be nice to introduce
	// this functionality.  All we do is just scale the texture by a certain
	// amount, which can provide different levels of detail.  By hitting the
	// SPACE bar on the keyboard, you are able to cycle through different
	// scale values to see the one that works best for you.  It is good to
	// add a third texture on top that is the same detail texture, but with
	// a different scale value.

	// Activate the first texture ID and bind the tree background to it
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

	// If we want detail texturing on, let's render the second texture
	if(g_bDetail)
	{
		// Activate the second texture ID and bind the fog texture to it
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		
		// Here we turn on the COMBINE properties and increase our RGB
		// gamma for the detail texture.  2 seems to work just right.
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
		
		// Bind the detail texture
		glBindTexture(GL_TEXTURE_2D, g_Texture[1]);
	
		// Now we want to enter the texture matrix.  This will allow us
		// to change the tiling of the detail texture.
		glMatrixMode(GL_TEXTURE);

			// Reset the current matrix and apply our chosen scale value
			glLoadIdentity();
			glScalef((float)g_DetailScale, (float)g_DetailScale, 1);

		// Leave the texture matrix and set us back in the model view matrix
		glMatrixMode(GL_MODELVIEW);
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// We want to render triangle strips
	glBegin( GL_TRIANGLE_STRIP );			

	// Go through all of the rows of the height map
	for ( X = 0; X <= MAP_SIZE; X += STEP_SIZE )
	{
		// Check if we need to render the opposite way for this column
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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Now let's clean up our multitexturing so it doesn't effect anything else

	// Turn the second multitexture pass off
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the first multitexture pass off
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
// Very few things were changed in this version of our height map series, but
// what was changed gives us a neat effect and increased realism in our scene.
// 
// Multitexturing was added so that we could apply a detail texture over our
// terrain.  This gives the terrain a more detailed look, instead of a stretched
// look.  To do this, we just use the normal multitexturing functions, but we
// change the detail texture's properties with GL_COMBINE_ARB and GL_RGB_SCALE_ARB.
// 
// These 2 flags allow us to increase the gamma on the detail texture so that it
// doesn't over power the texture of the terrain.  The last thing we fiddled with
// was the texture matrix.  Instead of calculating the (u, v) coordinates for the
// tiled detail texture, we just assigned the same (u, v) coordinates as the
// terrain texture (the whole texture stretched over the terrain), then scaled
// the texture coordinates by entering the texture matrix mode and applying our
// scale value.  When we hit the space bar, this changes the scale value to get
// a different tiling effect.  It eventually wraps around again.
// 
// I hope this helps!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
        
