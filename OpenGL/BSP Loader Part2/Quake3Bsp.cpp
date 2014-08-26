//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader2										 //
//																		 //
//		$Description:	Loads and renders lightmaps for the level		 //
//																		 //
//***********************************************************************//

#include "main.h"
#include "camera.h"
#include "Quake3Bsp.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// In this version of our tutorial we add lightmaps to our level rendered.  The
// lightmap data is just stored in pure RGB values, so it's easy to convert into
// a texture map.  BUT, First we want to increase it's intensity so it isn't so dark.
// 
// Be sure to look at the HTML file format that was included with this tutorial.
// It has the whole file format and added information not discussed in this tutorial.
//
//

// This tells us if we want to render the lightmaps
extern bool g_bLightmaps;

// This holds the gamma value that was stored in the config file
extern float g_Gamma;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This tells us if we want to render the textures
extern bool g_bTextures;


//////////////////////////// CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our object's constructor to initial all it's data members
/////
//////////////////////////// CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuake3BSP::CQuake3BSP()
{
	// Here we simply initialize our member variables to 0
	m_numOfVerts    = 0;	
	m_numOfFaces    = 0;	
	m_numOfTextures = 0;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	m_numOfLightmaps = 0;
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	m_pVerts = NULL;
	m_pFaces = NULL;
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

//////////////////////////// CHANGE GAMMA \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This manually changes the gamma of an image
/////
//////////////////////////// CHANGE GAMMA \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::ChangeGamma(byte *pImage, int size, float factor)
{
	//  This function was taken from a couple engines that I saw,
	// which most likely originated from the Aftershock engine.
	// Kudos to them!  What it does is increase/decrease the intensity
	// of the lightmap so that it isn't so dark.  Quake uses hardware to
	// do this, but we will do it in code.

	// Go through every pixel in the lightmap
	for(int i = 0; i < size / 3; i++, pImage += 3) 
	{
		float scale = 1.0f, temp = 0.0f;
		float r = 0, g = 0, b = 0;

		// extract the current RGB values
		r = (float)pImage[0];
		g = (float)pImage[1];
		b = (float)pImage[2];

		// Multiply the factor by the RGB values, while keeping it to a 255 ratio
		r = r * factor / 255.0f;
		g = g * factor / 255.0f;
		b = b * factor / 255.0f;
		
		// Check if the the values went past the highest value
		if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
		if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
		if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

		// Get the scale for this pixel and multiply it by our pixel values
		scale*=255.0f;		
		r*=scale;	g*=scale;	b*=scale;

		// Assign the new gamma'nized RGB values to our image
		pImage[0] = (byte)r;
		pImage[1] = (byte)g;
		pImage[2] = (byte)b;
	}
}


////////////////////////////// CREATE LIGHTMAP TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a texture map from the light map image bits
/////
////////////////////////////// CREATE LIGHTMAP TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::CreateLightmapTexture(UINT &texture, byte *pImageBits, int width, int height)
{
	// This function takes in the lightmap image bits and creates a texture map
	// from them.  The width and height is usually 128x128 anyway....

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &texture);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// Change the lightmap gamma values by our desired gamma
	ChangeGamma(pImageBits, width*height*3, g_Gamma);

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, pImageBits);

	//Assign the mip map levels		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


//////////////////////////// FIND TEXTURE EXTENSION \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This attaches the image extension to the texture name, if found
/////
//////////////////////////// FIND TEXTURE EXTENSION \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::FindTextureExtension(char *strFileName)
{
	char strJPGPath[MAX_PATH] = {0};
	char strTGAPath[MAX_PATH]    = {0}; 
	FILE *fp = NULL;

	// Get the current path we are in
	GetCurrentDirectory(MAX_PATH, strJPGPath);

	// Add on a '\' and the file name to the end of the current path.
	// We create 2 seperate strings to test each image extension.
	strcat(strJPGPath, "\\");
	strcat(strJPGPath, strFileName);
	strcpy(strTGAPath, strJPGPath);
	
	// Add the extensions on to the file name and path
	strcat(strJPGPath, ".jpg");
	strcat(strTGAPath, ".tga");

	// Check if there is a jpeg file with the texture name
	if((fp = fopen(strJPGPath, "rb")) != NULL)
	{
		// If so, then let's add ".jpg" onto the file name and return
		strcat(strFileName, ".jpg");
		return;
	}

	// Check if there is a targa file with the texture name
	if((fp = fopen(strTGAPath, "rb")) != NULL)
	{
		// If so, then let's add a ".tga" onto the file name and return
		strcat(strFileName, ".tga");
		return;
	}
}


//////////////////////////// LOAD BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads in all of the .bsp data for the level
/////
//////////////////////////// LOAD BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CQuake3BSP::LoadBSP(const char *strFileName)
{
	FILE *fp = NULL;
	int i = 0;
	
	// Check if the .bsp file could be opened
	if((fp = fopen(strFileName, "rb")) == NULL)
	{
		// Display an error message and quit if the file can't be found.
		MessageBox(g_hWnd, "Could not find BSP file!", "Error", MB_OK);
		return false;
	}

	// Initialize the header and lump structures
	tBSPHeader header = {0};
	tBSPLump lumps[kMaxLumps] = {0};

	// Read in the header and lump data
	fread(&header, 1, sizeof(tBSPHeader), fp);
	fread(&lumps, kMaxLumps, sizeof(tBSPLump), fp);

	// Now we know all the information about our file.  We can
	// then allocate the needed memory for our member variables.

	// Allocate the vertex memory
	m_numOfVerts = lumps[kVertices].length / sizeof(tBSPVertex);
	m_pVerts     = new tBSPVertex [m_numOfVerts];

	// Allocate the face memory
	m_numOfFaces = lumps[kFaces].length / sizeof(tBSPFace);
	m_pFaces     = new tBSPFace [m_numOfFaces];

	// Allocate the index memory
	m_numOfIndices = lumps[kIndices].length / sizeof(int);
	m_pIndices     = new int [m_numOfIndices];

	// Allocate memory to read in the texture information.
	m_numOfTextures = lumps[kTextures].length / sizeof(tBSPTexture);
	tBSPTexture *pTextures = new tBSPTexture [m_numOfTextures];


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Allocate memory to read in the lightmap data.  Like the texture
	// data, we just need to create a local array to be destroyed real soon.
	m_numOfLightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap *pLightmaps = new tBSPLightmap [m_numOfLightmaps ];

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Seek to the position in the file that stores the vertex information
	fseek(fp, lumps[kVertices].offset, SEEK_SET);

	// Go through all of the vertices that need to be read and swap axises
	for(i = 0; i < m_numOfVerts; i++)
	{
		// Read in the current vertex
		fread(&m_pVerts[i], 1, sizeof(tBSPVertex), fp);
		
		// Swap the y and z values, and negate the new z so Y is up.
		float temp = m_pVerts[i].vPosition.y;
		m_pVerts[i].vPosition.y = m_pVerts[i].vPosition.z;
		m_pVerts[i].vPosition.z = -temp;
	}	

	// Seek to the position in the file that stores the index information
	fseek(fp, lumps[kIndices].offset, SEEK_SET);

	// Read in all the index information
	fread(m_pIndices, m_numOfIndices, sizeof(int), fp);

	// Seek to the position in the file that stores the face information
	fseek(fp, lumps[kFaces].offset, SEEK_SET);

	// Read in all the face information
	fread(m_pFaces, m_numOfFaces, sizeof(tBSPFace), fp);

	// Seek to the position in the file that stores the texture information
	fseek(fp, lumps[kTextures].offset, SEEK_SET);
	
	// Read in all the texture information
	fread(pTextures, m_numOfTextures, sizeof(tBSPTexture), fp);

	// Go through all of the textures
	for(i = 0; i < m_numOfTextures; i++)
	{
		// Find the extension if any and append it to the file name
		FindTextureExtension(pTextures[i].strName);
		
		// Create a texture from the image
		CreateTexture(m_textures[i], pTextures[i].strName);
	}

	// We can now free all the texture information since we already loaded them
	delete [] pTextures;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Seek to the position in the file that stores the lightmap information
	fseek(fp, lumps[kLightmaps].offset, SEEK_SET);

	// Go through all of the lightmaps and read them in
	for(i = 0; i < m_numOfLightmaps ; i++)
	{
		// Read in the RGB data for each lightmap
		fread(&pLightmaps[i], 1, sizeof(tBSPLightmap), fp);

		// Create a texture map for each lightmap that is read in.  The lightmaps
		// are always 128 by 128.
		CreateLightmapTexture(m_lightmaps[i], 
							 (unsigned char *)pLightmaps[i].imageBits, 128, 128);
	}

	// Delete the image bits because we are already done with them
	delete [] pLightmaps;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Close the file
	fclose(fp);

	// Here we allocate enough bits to store all the faces for our bitset
	m_FacesDrawn.Resize(m_numOfFaces);

	// Return a success
	return true;
}


//////////////////////////// RENDER FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders a face, determined by the passed in index
/////
//////////////////////////// RENDER FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::RenderFace(int faceIndex)
{
	// Here we grab the face from the index passed in
	tBSPFace *pFace = &m_pFaces[faceIndex];

	// Now, in this function you don't might get all messed up and confused with
	// what function is for vertex arrays and which function is for multi-texturing.
	// The gl*Pointer() and glEnableClientState() functions are for vertex arrays.
	// The glActiveTextureARG() and glClientActiveTextureARB() stuff is for multi-texturing.  
	// Since we allow the user to right or left click the mouse, turning on and off the 
	// light maps and textures, we need to make those checks in this function to know 
	// what we should render.

	// Assign our array of face vertices for our vertex arrays and enable vertex arrays
	glVertexPointer(3, GL_FLOAT, sizeof(tBSPVertex), &(m_pVerts[pFace->startVertIndex].vPosition));
	glEnableClientState(GL_VERTEX_ARRAY);

	// If we want to render the textures
	if(g_bTextures)
	{

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

		// Set the current pass as the first texture (For multi-texturing)
		glActiveTextureARB(GL_TEXTURE0_ARB);

		// Since we are using vertex arrays, we need to tell OpenGL which texture
		// coordinates to use for each texture pass.  We switch our current texture
		// to the first one, then set our texture coordinates.
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(tBSPVertex), 
									   &(m_pVerts[pFace->startVertIndex].vTextureCoord));

		// Set our vertex array client states for allowing texture coordinates
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Turn on texture arrays for the first pass
		glClientActiveTextureARB(GL_TEXTURE0_ARB);

		// To enable each texture pass, we want to turn on the texture coord array
		// state for each pass.  This needs to be done since we are using vertex arrays.

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	

		// Turn on texture mapping and bind the face's texture map
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,  m_textures[pFace->textureID]);
	}

	if(g_bLightmaps)
	{


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

		// Set the current pass as the second lightmap texture_
		glActiveTextureARB(GL_TEXTURE1_ARB);

		// Turn on texture arrays for the second lightmap pass
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Next, we need to specify the UV coordinates for our lightmaps.  This is done
		// by switching to the second texture and giving OpenGL our lightmap array.
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(tBSPVertex), 
									   &(m_pVerts[pFace->startVertIndex].vLightmapCoord));

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


		// Turn on texture mapping and bind the face's lightmap over the texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,  m_lightmaps[pFace->lightmapID]);
	}

	// Render our current face to the screen with vertex arrays
	glDrawElements(GL_TRIANGLES, pFace->numOfIndices, GL_UNSIGNED_INT, &(m_pIndices[pFace->startIndex]) );
}


//////////////////////////// RENDER LEVEL \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Goes through all of the faces and draws them if the type is FACE_POLYGON
/////
//////////////////////////// RENDER LEVEL \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::RenderLevel(const CVector3 &vPos)
{
	// Get the number of faces in our level
	int i = m_numOfFaces;

	// Reset our bitset so all the slots are zero.  This isn't really
	// utilized in this tutorial, but I thought I might as well add it
	// not so that we spread out the code between the tutorials.
	m_FacesDrawn.ClearAll();

	// Go through all the faces
	while(i--)
	{
		// Before drawing this face, make sure it's a normal polygon
		if(m_pFaces[i].type != FACE_POLYGON) continue;

		// If this face already hasn't been drawn
		if(!m_FacesDrawn.On(i)) 
		{
			// Set this face as drawn and render it
			m_FacesDrawn.Set(i);
			RenderFace(i);
		}
	}
}


//////////////////////////// DESTROY \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This cleans up our object and frees allocated memory
/////
//////////////////////////// DESTROY \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::Destroy()
{
	// If we still have valid memory for our vertices, free them
	if(m_pVerts) 
	{
		delete [] m_pVerts;
		m_pVerts = NULL;
	}

	// If we still have valid memory for our faces, free them
	if(m_pFaces)	
	{
		delete [] m_pFaces;
		m_pFaces = NULL;
	}

	// If we still have valid memory for our indices, free them
	if(m_pIndices)	
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
	}

	// Free all the textures
	glDeleteTextures(m_numOfTextures, m_textures);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Delete the lightmap textures
	glDeleteTextures(m_numOfLightmaps, m_lightmaps);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

}


//////////////////////////// ~CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our deconstructor that is called when the object is destroyed
/////
//////////////////////////// ~CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuake3BSP::~CQuake3BSP()
{
	// Call our destroy function
	Destroy();
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// With such a little amount of code to add to our rendered, lightmaps give a
// tremendous improvement to the realism of the level.  All that really needed
// to be done was to load in the lightmaps RGB values, change their gamma levels,
// convert them to a texture map and just assign them as a second pass!
//
// Simple, but Sweeeeeet.
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
