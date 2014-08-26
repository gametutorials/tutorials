#ifndef _3DS_H
#define _3DS_H

//>------ Primary Chunk, at the beginning of each file
#define PRIMARY       0x4D4D

//>------ Main Chunks
#define OBJECTINFO    0x3D3D				// This gives the version of the mesh and is found right before the material and object information
#define VERSION       0x0002				// This gives the version of the .3ds file

//>------ sub defines of OBJECTINFO
#define MATERIAL	  0xAFFF				// This stored the texture info
#define OBJECT		  0x4000				// This stores the faces, vertices, etc...

//>------ sub defines of MATERIAL
#define MATNAME       0xA000				// This holds the material name
#define MATDIFFUSE    0xA020				// This holds the color of the object/material
#define MATMAP        0xA200				// This is a header for a new material
#define MATMAPFILE    0xA300				// This holds the file name of the texture

#define OBJECT_MESH   0x4100				// This lets us know that we are reading a new object

//>------ sub defines of OBJECT_MESH
#define OBJECT_VERTICES     0x4110			// The objects vertices
#define OBJECT_FACES		0x4120			// The objects faces
#define OBJECT_MATERIAL		0x4130			// This is found if the object has a material, either texture map or color
#define OBJECT_UV			0x4140			// The UV texture coordinates


#define	KEYFRAME					0xB000	// This lets us know that we are reading in a keyframe
#define KEYFRAME_MESH_INFO			0xB002
#define KEYFRAME_OBJECT_NAME		0xB010 
#define KEYFRAME_START_AND_END		0xB008
#define PIVOT						0xB013 
#define POSITION_TRACK_TAG			0xB020 
#define ROTATION_TRACK_TAG			0xB021 
#define SCALE_TRACK_TAG				0xB022

// This holds the chunk info
struct tChunk
{
	unsigned short int ID;					// The chunk's ID		
	unsigned int length;					// The length of the chunk
	size_t bytesRead;						// The amount of bytes read within that chunk
};

// This class handles all of the loading code
class CLoad3DS
{
public:
	CLoad3DS();								// This inits the data members

	// This is the function that you call to load the 3DS
	bool Import3DS(t3DModel *pModel, char *strFileName);

private:
	// This reads in a string and saves it in the char array passed in
	int GetString(char *);

	// This reads the next chunk
	void ReadChunk(tChunk *);

	// This reads the next large chunk
	void ProcessNextChunk(t3DModel *pModel, tChunk *);

	// This reads the object chunks
	void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);

	// This reads the material chunks
	void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);

	// This reads the keyframe chunks (animation data)
	void ProcessNextKeyFrameChunk(t3DModel *pModel, tChunk *);

	// This reads the RGB value for the object's color
	void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);

	// This reads the objects vertices
	void ReadVertices(t3DObject *pObject, tChunk *);

	// This reads the objects face information
	void ReadVertexIndices(t3DObject *pObject, tChunk *);

	// This reads the texture coodinates of the object
	void ReadUVCoordinates(t3DObject *pObject, tChunk *);

	// This reads in the material name assigned to the object and sets the materialID
	void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);

	// This reads in the positions of each frame in the animation
	void ReadKeyFramePositions(t3DModel *pModel, tChunk *pPreviousChunk);

	// This reads in the rotation key frames
	void ReadKeyFrameRotations(t3DModel *pModel, tChunk *pPreviousChunk);

	// This reads in the scale key frames
	void ReadKeyFrameScales(t3DModel *pModel, tChunk *pPreviousChunk);

	// This sets the current model that animation is being read in for by it's name
	void SetCurrentObject(t3DModel *pModel, char *strObjectName);

	// This computes the vertex normals for the object (used for lighting)
	void ComputeNormals(t3DModel *pModel);

	// This frees memory and closes the file
	void CleanUp();
	
	// The file pointer
	FILE *m_FilePointer;
	
	// These are used through the loading process to hold the chunk information
	tChunk *m_CurrentChunk;
	tChunk *m_TempChunk;

	// This will hold the object that the animation data is being read in for
	t3DObject *m_CurrentObject;
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// This file is created in the hopes that you can just plug it into your code
// easily.  You will probably want to query more chunks though for animation, etc..
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
