#ifndef _QUAKE3BSP_H
#define _QUAKE3BSP_H


// This is the number that is associated with a face that is of type "polygon"
#define FACE_POLYGON	1

#define TYPE_RAY		0				// This is the type for tracing a RAY
#define TYPE_SPHERE		1				// This is the type for tracing a SPHERE


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#define TYPE_BOX		2				// This is the type for tracing a AABB (BOX)

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


const float kEpsilon =  0.03125f;		// This is our small number to compensate for float errors


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
    int textureType;			// The type of texture (solid, water, slime, etc..) (type & 1) = 1 (solid)
};

// This is our BSP lightmap structure which stores the 128x128 RGB values
struct tBSPLightmap
{
    byte imageBits[128][128][3];   // The RGB data in a 128x128 image
};

// This stores a node in the BSP tree
struct tBSPNode
{
    int plane;					// The index into the planes array 
    int front;					// The child index for the front node 
    int back;					// The child index for the back node 
    tVector3i min;				// The bounding box min position. 
    tVector3i max;				// The bounding box max position. 
}; 

// This stores a leaf (end node) in the BSP tree
struct tBSPLeaf
{
    int cluster;				// The visibility cluster 
    int area;					// The area portal 
    tVector3i min;				// The bounding box min position 
    tVector3i max;				// The bounding box max position 
    int leafface;				// The first index into the face array 
    int numOfLeafFaces;			// The number of faces for this leaf 
    int leafBrush;				// The first index for into the brushes 
    int numOfLeafBrushes;		// The number of brushes for this leaf 
}; 

// This stores a splitter plane in the BSP tree
struct tBSPPlane
{
    CVector3 vNormal;			// Plane normal. 
    float d;					// The plane distance from origin 
};

// This stores the cluster data for the PVS's
struct tBSPVisData
{
	int numOfClusters;			// The number of clusters
	int bytesPerCluster;		// The amount of bytes (8 bits) in the cluster's bitset
	byte *pBitsets;				// The array of bytes that holds the cluster bitsets				
};

// This stores the brush data
struct tBSPBrush 
{
	int brushSide;				// The starting brush side for the brush 
	int numOfBrushSides;		// Number of brush sides for the brush
	int textureID;				// The texture index for the brush
}; 

// This stores the brush side data, which stores indices for the normal and texture ID
struct tBSPBrushSide 
{
	int plane;					// The plane index
	int textureID;				// The texture index
}; 

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

	// This traces a single ray and checks collision with brushes
	CVector3 TraceRay(CVector3 vStart, CVector3 vEnd);

	// This traces a sphere along a ray to check for collision with the brushes
	CVector3 TraceSphere(CVector3 vStart, CVector3 vEnd, float radius);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This traces a axis-aligned bounding box (AABB) along a ray to check for collision
	CVector3 TraceBox(CVector3 vStart, CVector3 vEnd, CVector3 vMin, CVector3 vMax);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// This tells us if we have just collided
	bool Collided()	  { return m_bCollided; }

	// This destroys the level data
	void Destroy();

private:

	// This manually changes the gamma levels of an image
	void ChangeGamma(byte *pImage, int size, float factor);

	// This creates a texture map from the lightmap image bits
	void CreateLightmapTexture(UINT &texture, byte *pImageBits, int width, int height);

	// This tells us if a cluster is visible or not
	int IsClusterVisible(int current, int test);

	// This finds a leaf in the BSP tree according to the position passed in
	int FindLeaf(const CVector3 &vPos);

	// This traverses the BSP tree to check our movement vector with the brushes
	CVector3 Trace(CVector3 vStart, CVector3 vEnd);

	// This recursively checks all the nodes until we find leafs that store the brushes
	void CheckNode(int nodeIndex, float startRatio, float endRatio, CVector3 vStart, CVector3 vEnd);
	
	// This checks our movement vector against the brush and it's sides
	void CheckBrush(tBSPBrush *pBrush, CVector3 vStart, CVector3 vEnd);

	// This attaches the correct extension to the file name, if found
	void FindTextureExtension(char *strFileName);

	// This renders a single face to the screen
	void RenderFace(int faceIndex);

	int m_numOfVerts;			// The number of verts in the model
	int m_numOfFaces;			// The number of faces in the model
	int m_numOfIndices;			// The number of indices for the model
	int m_numOfTextures;		// The number of texture maps
	int m_numOfLightmaps;		// The number of light maps
	int m_numOfNodes;			// The number of nodes in the BSP
	int m_numOfLeafs;			// The number of leafs
	int m_numOfLeafFaces;		// The number of faces
	int m_numOfPlanes;			// The number of planes in the BSP
	int m_numOfBrushes;			// The number of brushes in our world
	int m_numOfBrushSides;		// The number of brush sides in our world
	int m_numOfLeafBrushes;		// The number of leaf brushes
	
	int m_traceType;			// This stores if we are checking a ray, sphere or a box
	float m_traceRatio;			// This stores the ratio from our start pos to the intersection pt.
	float m_traceRadius;		// This stores the sphere's radius for a collision offset

	bool m_bCollided;			// This tells if we just collided or not


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	CVector3 m_vTraceMins;		// This stores the minimum values of the AABB (bottom corner)
	CVector3 m_vTraceMaxs;		// This stores the maximum values of the AABB (top corner)
	CVector3 m_vExtents;		// This stores the largest length of the box
	
	CVector3 m_vCollisionNormal;	// This stores the normal of the plane we collided with

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	int			  *m_pIndices;		// The object's indices for rendering
	tBSPVertex    *m_pVerts;		// The object's vertices
	tBSPFace      *m_pFaces;		// The faces information of the object



	tBSPNode      *m_pNodes;		// The nodes in the BSP
	tBSPLeaf      *m_pLeafs;		// The leafs in the BSP
	tBSPPlane     *m_pPlanes;		// The planes in the BSP
	int           *m_pLeafFaces;	// The leaf faces in the BSP
	tBSPVisData   m_clusters;		// The cluster info for frustum culling and portals (PVS)
	tBSPTexture   *m_pTextures;		// This stores our texture info for each brush
	tBSPBrush	  *m_pBrushes;		// This is our brushes
	tBSPBrushSide *m_pBrushSides;	// This holds the brush sides
	int			  *m_pLeafBrushes;  // The indices into the brush array
								
	UINT m_textures[MAX_TEXTURES];	// The texture array for the world
	UINT m_lightmaps[MAX_TEXTURES];	// The lightmap texture array
								
	CBitset m_FacesDrawn;			// The bitset for the faces that have/haven't been drawn
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this file we added some variables to our class to handle AABB collision, along
// with a function to check AABB collision (TraceBox()).
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
