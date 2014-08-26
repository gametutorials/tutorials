#ifndef _MD2_H
#define _MD2_H

// These are the needed defines for the max values when loading .MD2 files
#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This stores the speed of the animation between each key frame
#define kAnimationSpeed			5.0f

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// This holds the header information that is read in at the beginning of the file
struct tMd2Header
{ 
   int magic;					// This is used to identify the file
   int version;					// The version number of the file (Must be 8)
   int skinWidth;				// The skin width in pixels
   int skinHeight;				// The skin height in pixels
   int frameSize;				// The size in bytes the frames are
   int numSkins;				// The number of skins associated with the model
   int numVertices;				// The number of vertices (constant for each frame)
   int numTexCoords;			// The number of texture coordinates
   int numTriangles;			// The number of faces (polygons)
   int numGlCommands;			// The number of GL commands
   int numFrames;				// The number of animation frames
   int offsetSkins;				// The offset in the file for the skin data
   int offsetTexCoords;			// The offset in the file for the texture data
   int offsetTriangles;			// The offset in the file for the face data
   int offsetFrames;			// The offset in the file for the frames data
   int offsetGlCommands;		// The offset in the file for the GL commands data
   int offsetEnd;				// The end of the file offset
};


// This is used to store the vertices that are read in for the current frame
struct tMd2AliasTriangle
{
   byte vertex[3];
   byte lightNormalIndex;
};

// This stores the normals and vertices for the frames
struct tMd2Triangle
{
   float vertex[3];
   float normal[3];
};

// This stores the indices into the vertex and texture coordinate arrays
struct tMd2Face
{
   short vertexIndices[3];
   short textureIndices[3];
};

// This stores UV coordinates
struct tMd2TexCoord
{
   short u, v;
};

// This stores the animation scale, translation and name information for a frame, plus verts
struct tMd2AliasFrame
{
   float scale[3];
   float translate[3];
   char name[16];
   tMd2AliasTriangle aliasVertices[1];
};

// This stores the frames vertices after they have been transformed
struct tMd2Frame
{
   char strName[16];
   tMd2Triangle *pVertices;
};

// This stores a skin name
typedef char tMd2Skin[64];


// This class handles all of the loading code
class CLoadMD2
{

public:
	CLoadMD2();								// This inits the data members

	// This is the function that you call to load the MD2
	bool ImportMD2(t3DModel *pModel, char *strFileName, char *strTexture);

private:
	
	// This reads in the data from the MD2 file and stores it in the member variables
	void ReadMD2Data();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This parses the animations name and calculates the number of animations and info
	void ParseAnimations(t3DModel *pModel);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// This converts the member variables to our pModel structure
	void ConvertDataStructures(t3DModel *pModel);

	// This frees memory and closes the file
	void CleanUp();
	
	// The file pointer
	FILE *m_FilePointer;

	// Member variables		

	tMd2Header				m_Header;			// The header data
	tMd2Skin				*m_pSkins;			// The skin data
	tMd2TexCoord			*m_pTexCoords;		// The texture coordinates
	tMd2Face				*m_pTriangles;		// Face index information
	tMd2Frame				*m_pFrames;			// The frames of animation (vertices)
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// For the second .MD2 tutorial we added a animation speed for our model: kAnimationSpeed.
// This is used for interpolation between each key frame.  A function was also added to
// parse through the animation names and create a list of animations for our model
// to cycle through.  This function is called ParseAnimations().  We did however, take 
// out the code for calculating the face normals.  This is because the texture maps are
// designed to create their own lighting for the model.  It just makes the model darker
// to turn lighting on.  More of this is explained at the top of Main.cpp.
//
// The Quake2 .Md2 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake, Quake2 and Quake3 are trademarks of ID Software.
// All trademarks used are properties of their respective owners. 
//
// ©2000-2005 GameTutorials
