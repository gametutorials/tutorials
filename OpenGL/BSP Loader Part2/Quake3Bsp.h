#ifndef _QUAKE3BSP_H
#define _QUAKE3BSP_H


// This is the number that is associated with a face that is of type "polygon"
#define FACE_POLYGON	1


// This is our integer vector structure
struct tVector3i
{
	int x, y, z;				// The x y and z position of our integer vector
};


// This is our BSP header structure
struct tBSPHeader
{
    char strID[4];				// This should always be 'IBSP'
    int version;				// This should be 0x2e for Quake 3 files
}; 


// This is our BSP lump structure
struct tBSPLump
{
	int offset;					// The offset into the file for the start of this lump
	int length;					// The length in bytes for this lump
};


// This is our BSP vertex structure
struct tBSPVertex
{
    CVector3 vPosition;			// (x, y, z) position. 
    CVector2 vTextureCoord;		// (u, v) texture coordinate
    CVector2 vLightmapCoord;	// (u, v) lightmap coordinate
    CVector3 vNormal;			// (x, y, z) normal vector
    byte color[4];				// RGBA color for the vertex 
};


// This is our BSP face structure
struct tBSPFace
{
    int textureID;				// The index into the texture array 
    int effect;					// The index for the effects (or -1 = n/a) 
    int type;					// 1=polygon, 2=patch, 3=mesh, 4=billboard 
    int startVertIndex;			// The starting index into this face's first vertex 
    int numOfVerts;				// The number of vertices for this face 
    int startIndex;				// The starting index into the indices array for this face
    int numOfIndices;			// The number of indices for this face
    int lightmapID;				// The texture index for the lightmap 
    int lMapCorner[2];			// The face's lightmap corner in the image 
    int lMapSize[2];			// The size of the lightmap section 
    CVector3 lMapPos;			// The 3D origin of lightmap. 
    CVector3 lMapVecs[2];		// The 3D space for s and t unit vectors. 
    CVector3 vNormal;			// The face normal. 
    int size[2];				// The bezier patch dimensions. 
};


// This is our BSP texture structure
struct tBSPTexture
{
    char strName[64];			// The name of the texture w/o the extension 
    int flags;					// The surface flags (unknown) 
    int contents;				// The content flags (unknown)
};


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is our BSP lightmap structure which stores the 128x128 RGB values
struct tBSPLightmap
{
    byte imageBits[128][128][3];   // The RGB data in a 128x128 image
};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our lumps enumeration
enum eLumps
{
    kEntities = 0,				// Stores player/object positions, etc...
    kTextures,					// Stores texture information
    kPlanes,				    // Stores the splitting planes
    kNodes,						// Stores the BSP nodes
    kLeafs,						// Stores the leafs of the nodes
    kLeafFaces,					// Stores the leaf's indices into the faces
    kLeafBrushes,				// Stores the leaf's indices into the brushes
    kModels,					// Stores the info of world models
    kBrushes,					// Stores the brushes info (for collision)
    kBrushSides,				// Stores the brush surfaces info
    kVertices,					// Stores the level vertices
    kIndices,					// Stores the level indices
    kShaders,					// Stores the shader files (blending, anims..)
    kFaces,						// Stores the faces for the level
    kLightmaps,					// Stores the lightmaps for the level
    kLightVolumes,				// Stores extra world lighting information
    kVisData,					// Stores PVS and cluster info (visibility)
    kMaxLumps					// A constant to store the number of lumps
};


// This is our bitset class for storing which face has already been drawn.
// The bitset functionality isn't really taken advantage of in this version
// since we aren't rendering by leafs and nodes.
class CBitset 
{

public:

	// Initialize all the data members
    CBitset() : m_bits(0), m_size(0) {}

	// This is our deconstructor
	~CBitset() 
	{
		// If we have valid memory, get rid of it
		if(m_bits) 
		{
			delete m_bits;
			m_bits = NULL;
		}
	}

	// This resizes our bitset to a size so each face has a bit associated with it
	void Resize(int count) 
	{ 
		// Get the size of integers we need
		m_size = count/32 + 1;

		// Make sure we haven't already allocated memory for the bits
        if(m_bits) 
		{
			delete m_bits;
			m_bits = 0;
		}

		// Allocate the bits and initialize them
		m_bits = new unsigned int[m_size];
		ClearAll();
	}

	// This does the binary math to set the desired bit
	void Set(int i) 
	{
		m_bits[i >> 5] |= (1 << (i & 31));
	}

	// This returns if the desired bit slot is a 1 or a 0
	int On(int i) 
	{
		return m_bits[i >> 5] & (1 << (i & 31 ));
	}

	// This clears a bit to 0
	void Clear(int i) 
	{
		m_bits[i >> 5] &= ~(1 << (i & 31));
	}

	// This initializes the bits to 0
	void ClearAll() 
	{
		memset(m_bits, 0, sizeof(unsigned int) * m_size);
	}

private:

	// Our private bit data that holds the bits and size
	unsigned int *m_bits;
	int m_size;
};


// This is our Quake3 BSP class
class CQuake3BSP
{

public:

	// Our constructor
	CQuake3BSP();

	// Our deconstructor
	~CQuake3BSP();

	// This loads a .bsp file by it's file name (Returns true if successful)
	bool LoadBSP(const char *strFileName);

	// This renders the level to the screen, currently the camera pos isn't being used
	void RenderLevel(const CVector3 &vPos);

	// This destroys the level data
	void Destroy();

private:

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This manually changes the gamma levels of an image
	void ChangeGamma(byte *pImage, int size, float factor);

	// This creates a texture map from the lightmap image bits
	void CreateLightmapTexture(UINT &texture, byte *pImageBits, int width, int height);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// This attaches the correct extension to the file name, if found
	void FindTextureExtension(char *strFileName);

	// This renders a single face to the screen
	void RenderFace(int faceIndex);

	int m_numOfVerts;			// The number of verts in the model
	int m_numOfFaces;			// The number of faces in the model
	int m_numOfIndices;			// The number of indices for the model
	int m_numOfTextures;		// The number of texture maps

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	int m_numOfLightmaps;		// The number of light maps
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	int			*m_pIndices;	// The object's indices for rendering
	tBSPVertex  *m_pVerts;		// The object's vertices
	tBSPFace *m_pFaces;			// The faces information of the object
								// The texture and lightmap array for the level
	UINT m_textures[MAX_TEXTURES];	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	UINT m_lightmaps[MAX_TEXTURES];	// The lightmap texture array
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
								
	CBitset m_FacesDrawn;		// The bitset for the faces that have/haven't been drawn
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this version we added our tBSPLightmap structure, along with some
// functions and member variables to our CQuake3Bsp class:
//
//
// This manually changes the gamma levels of an image
// void ChangeGamma(byte *pImage, int size, float factor);
//
// This creates a texture map from the lightmap image bits
// void CreateLightmapTexture(UINT &texture, byte *pImageBits, int width, int height);
//
// int m_numOfLightmaps;			// The number of light maps
// 
// UINT m_lightmaps[MAX_TEXTURES];	// The lightmap texture array
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
