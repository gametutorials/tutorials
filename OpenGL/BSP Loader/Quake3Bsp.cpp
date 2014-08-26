//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader										 //
//																		 //
//		$Description:	Loads faces and textures from a Quake3 BSP file  //
//																		 //
//***********************************************************************//

#include "main.h"
#include "camera.h"
#include "Quake3Bsp.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This is the first version of our Quake3Bsp.cpp file.  We only have a small
// number of functions, but it will quickly grow in the next tutorials.  To make
// this first tutorial as simple as possible, we are just going to render the
// level with brute force.  There will be no frustum or cluster culling.  We
// will also just be reading in enough to render the basic world data and textures.
// Lightmaps will be added to our walls in the next tutorial.  This is a REALLY
// excellent effect which brings realism to the whole seen.  
// 
// Be sure to look at the HTML file format that was included with this tutorial.
// It has the whole file format and added information not discussed in this tutorial.
//

// This tells us if we want to render the textures (used later for just lightmaps)
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
	m_numOfIndices  = 0;
	m_numOfTextures = 0;

	m_pVerts = NULL;
	m_pFaces = NULL;
	m_pIndices = NULL;
}


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

	// This function is a very basic way to find the extension
	// of the texture that is being passed in.  Quake saves it's
	// textures with just the name, and omits the extension.  I
	// still haven't figured out why they do this, but I imagine
	// it has to do with allowing you to change images to different 
	// image formats without having to use the level editor again.
	// What we do hear is just assume that it's either going to be
	// a jpeg or targa file.  I haven't seen any other type
	// be used.  If you just add on either one of those extensions
	// to the current name and see if a file with that name exits,
	// then it must be the texture extension.  If fopen() returns
	// a NULL, there is no file with that name.  Keep in mind that
	// most levels use the textures that come with Quake3.  That means
	// you won't be able to load them unless you try and read from their
	// pk3 files if the texture isn't found in the level's .pk3 file.
	// Also, I have found that some shader textures store the file name
	// in the shader.  So, don't be surprised if not all the textures are loaded.

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

	// Otherwise, it must be a special texture or given in the shader file,
	// or possibly a base Quake texture used in the game.  There are some
	// special names like "textures\caulk" and such that mean special things.
	// They aren't actual textures.  This took me a lot of pulling hair to find this out.
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

	// This is the bread and butter of our tutorial.  All the level
	// information is loaded from here.  The HTML file that comes with
	// this tutorial should explain about the .bsp file format, but let
	// me give a quick recap.
	//
	// The .bsp file format stores the data in lumps.  Look at the
	// tBSPLump enum to see all the lumps in their order.  A lump
	// is just like a chunk like in binary formats such as .3ds.
	// It stores an offset into the file for that lump, as well as
	// the lump's size in bytes.  We first want to read the BSP header,
	// then read in all the lump info.  We can then seek to the correct
	// offset for each lump and read in it's data.  To find out how many
	// structures instances the lump has, you just divide the lump's
	// length by the sizeof(<theStructure>).  Check out the HTML file for
	// more detail on this.  The only one that is handled a bit differently
	// is the visibility lump.  There is only one of these for the .bsp file.
	
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
	// We create a local pointer of tBSPTextures because we don't need
	// that information once we create texture maps from it.
	m_numOfTextures = lumps[kTextures].length / sizeof(tBSPTexture);
	tBSPTexture *pTextures = new tBSPTexture [m_numOfTextures];

	// Seek to the position in the file that stores the vertex information
	fseek(fp, lumps[kVertices].offset, SEEK_SET);

	// Since Quake has the Z-axis pointing up, we want to convert the data so
	// that Y-axis is pointing up (like normal!) :)

	// Go through all of the vertices that need to be read
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

	// Now that we have the texture information, we need to load the
	// textures.  Since the texture names don't have an extension, we need
	// to find it first.

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

	// Close the file
	fclose(fp);

	// I decided to put in a really big optimization for rendering.
	// I create a bitset that holds a bit slot for every face in the level.
	// Once the face is drawn, the slot saved for that face is set to 1.
	// If we try and draw it again, it checks first to see if it has already
	// been drawn.  We need to do this because many leafs stores faces that
	// are the same in other leafs.  If we don't check if it's already been drawn,
	// you can sometimes draw double the faces that you need to.  In this first
	// tutorial we draw every face once, so it doesn't matter, but when we get
	// into leafs we will need this.  I just chose to add it in the beginning
	// so I don't cram a ton of code down your throat when we get to the 
	// BSP nodes/leafs.

	// Here we allocate enough bits to store all the faces for our bitset
	m_FacesDrawn.Resize(m_numOfFaces);

	// Return a success
	return true;
}

extern CQuake3BSP g_Level;
//////////////////////////// RENDER FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders a face, determined by the passed in index
/////
//////////////////////////// RENDER FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::RenderFace(int faceIndex)
{
	// Here we grab the face from the index passed in
	tBSPFace *pFace = &m_pFaces[faceIndex];
	
	// To get a huge speed boost to our rendering, we are going to use vertex arrays.
	// This makes it so we don't have to call glVertex*() or glTex*() functions for
	// every vertice.  It also cuts down on slow loops which kill our frame rate.

	// First, we need to give OpenGL a pointer to our first vertice.
	// We also tell OpenGL that there is 3 floats, and the offset between
	// each vertices is the size of tBSPVertex in bytes.  That way we don't have
	// to have all the vertices in one contiguous array of floats, back to back.
	// We need to them give an address to the start of this face's vertices, startVertIndex.
	glVertexPointer(3, GL_FLOAT, sizeof(tBSPVertex), &(m_pVerts[pFace->startVertIndex].vPosition));

	// Next, we pass in the address of the first texture coordinate.  We also tell 
	// OpenGL that there are 2 UV coordinates that are floats, and the offset between 
	// each texture coordinate is the size of tBSPVertex in bytes.  
	// We need to them give an address to the start of this face's indices, startVertIndex.
	glTexCoordPointer(2, GL_FLOAT, sizeof(tBSPVertex), &(m_pVerts[pFace->startVertIndex].vTextureCoord));

	// Finally, we want to turn on the vertex and texture coordinate options for
	// our vertex arrays.  That tells OpenGL to pay attention to our vertices
	// and texture coordinates when we call the appropriate vertex arrays functions.
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// If we want to render the textures
	if(g_bTextures)
	{
		// Turn on texture mapping and bind the face's texture map
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,  m_textures[pFace->textureID]);
	}

	// Now, to draw the face with vertex arrays we just need to tell OpenGL
	// which indices we want to draw and what primitive the format is in.
	// The faces are stored in triangles.  We give glDrawElements() a pointer
	// to our indices, but it's not a normal indice array.  The indices are stored
	// according to the pFace->startVertIndex into the vertices array.  If you were
	// to print all of our indices out, they wouldn't go above the number 5.  If there
	// is over 70 vertices though, how is that possible for the indices to work?  Well,
	// that is why we give our vertex array functions above a pointer to the startVertIndex,
	// then the indice array acts according to that.  This is very important to do it this
	// way, otherwise we will not get more than 5 vertices display, and for all our faces.

	// We are going to draw triangles, pass in the number of indices for this face, then
	// say the indices are stored as ints, then pass in the starting address in our indice
	// array for this face by indexing it by the startIndex variable of our current face.
	glDrawElements(GL_TRIANGLES, pFace->numOfIndices, GL_UNSIGNED_INT, &(m_pIndices[pFace->startIndex]) );
}


//////////////////////////// RENDER LEVEL \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Goes through all of the faces and draws them if the type is FACE_POLYGON
/////
//////////////////////////// RENDER LEVEL \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::RenderLevel(const CVector3 &vPos)
{
	// In our current state, rendering the level is simple.
	// We just need to go through all of the faces and draw them.

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
// Pretty easy to grasp isn't it?  We didn't do much in this tutorial, but we
// got the basics down, and once you have those, the rest is easy.  The next
// tutorial will implement light maps.  The level looks pretty boring without
// any lighting.
// 
// Remember to refer back to the HTML file that comes with this tutorial to
// learn more about the file format.  I skipped a bunch of information because
// it's already in that file.
// 
// Let me know what you think of this tutorial, I would love to hear your
// feedback and any cool projects you create with it.
//
// Enjoy!
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
