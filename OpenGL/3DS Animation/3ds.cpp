//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		3DS Animation									 //
//																		 //
//		$Description:	Shows you how to animate .3DS key frame models   //
//																		 //
//***********************************************************************//

#include "main.h"
#include "3ds.h"

// Global
int gBuffer[50000] = {0};					// This is a global buffer used to read past unwanted data

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This file handles all the coded need to load the .3DS file.
// If you haven't seen our .3DS tutorial then we suggest you go
// look at it before tackling the animation tutorial.  You can
// find it at www.GameTutorials.com.  It has more in depth explanations
// of the .3ds loading process.  In this tutorial we added some simple
// animation code.  It loads the key frame information and we just
// interpolate between the translation, scale and rotations to perform
// our animation.  This is VERY simple animation and should eventually
// be turned into skeletal animation with bones.  That is the next
// tutorial.  Remember, this is only object animation, not bone animation.
// If you try and load a model in that has bone animation it won't work.


///////////////////////////////// ROUND FLOAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This rounds a float down to zero if it's smaller than 0.001 or -0.001
/////
///////////////////////////////// ROUND FLOAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float RoundFloat(float number)
{
	// For some strange reason when really small numbers were being
	// read in, it stored them with scientific notation which didn't
	// pass in correctly to some OpenGL calls.  So we just round them to 0.
	// This might not be necessary though, it just seemed to work better.

	// If the float passed in is a really small number, set it to zero
	if(number > 0 && number <  0.001f) number = 0;
	if(number < 0 && number > -0.001f) number = 0;

	// Return the float changed or unchanged
	return number;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This constructor initializes the tChunk data
/////
///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CLoad3DS::CLoad3DS()
{
	m_CurrentChunk = new tChunk;				// Initialize and allocate our current chunk
	m_TempChunk = new tChunk;					// Initialize and allocate a temporary chunk
}

///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is called by the client to open the .3ds file, read it, then clean up
/////
///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoad3DS::Import3DS(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// Open the 3DS file
	m_FilePointer = fopen(strFileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!m_FilePointer) 
	{
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Read the first chuck of the file to see if it's a 3DS file
	ReadChunk(m_CurrentChunk);

	// Make sure this is a 3DS file
	if (m_CurrentChunk->ID != PRIMARY)
	{
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Now we actually start reading in the data.  ProcessNextChunk() is recursive

	// Begin loading objects, by calling this recursive function
	ProcessNextChunk(pModel, m_CurrentChunk);

	// After we have read the whole 3DS file, we want to calculate our own vertex normals.
	ComputeNormals(pModel);

	// Clean up after everything
	CleanUp();

	return 0;
}

///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::CleanUp()
{

	fclose(m_FilePointer);						// Close the current file pointer
	delete m_CurrentChunk;						// Free the current chunk
	delete m_TempChunk;							// Free our temporary chunk
}


///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads the main sections of the .3DS file, then dives deeper with recursion
/////
///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	t3DObject newObject;						// This is used to add to our object list
	tMaterialInfo newTexture = {0};				// This is used to add to our material list
	int version = 0;							// This will hold the file version

	m_CurrentChunk = new tChunk;				// Allocate a new chunk				

	// Continue to read the sub chunks until we have reached the length.
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read next Chunk
		ReadChunk(m_CurrentChunk);

		// Check the chunk ID
		switch (m_CurrentChunk->ID)
		{
		case VERSION:							// This holds the version of the file
			
			// Read the file version and add the bytes read to our bytesRead variable
			m_CurrentChunk->bytesRead += fread(&version, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);

			// If the file version is over 3, give a warning that there could be a problem
			if (version > 0x03)
				MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly", "Warning", MB_OK);
			break;

		case OBJECTINFO:						// This holds the version of the mesh
			
			// Read the next chunk
			ReadChunk(m_TempChunk);

			// Get the version of the mesh
			m_TempChunk->bytesRead += fread(&version, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);

			// Increase the bytesRead by the bytes read from the last chunk
			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;

			// Go to the next chunk, which is the object has a texture, it should be MATERIAL, then OBJECT.
			ProcessNextChunk(pModel, m_CurrentChunk);
			break;

		case MATERIAL:							// This holds the material information

			// Increase the number of materials
			pModel->numOfMaterials++;

			// Add a empty texture structure to our texture list.
			pModel->pMaterials.push_back(newTexture);

			// Proceed to the material loading function
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case OBJECT:							// This holds the name of the object being read
				
			// Increase the object count
			pModel->numOfObjects++;
		
			// Add a new tObject node to our list of objects (like a link list)
			pModel->pObject.push_back(newObject);
			
			// Initialize the object and all it's data members
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));

			// Get the name of the object and store it, then add the read bytes to our byte counter.
			m_CurrentChunk->bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);
			
			// Now proceed to read in the rest of the object information
			ProcessNextObjectChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), m_CurrentChunk);
			break;

		case KEYFRAME:

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
			
			// This is where we starting to read in all the key frame information.
			// This is read in at the END of the file.  It stores all the animation data.

			// Recurse further to read in all the animation data
			ProcessNextKeyFrameChunk(pModel, m_CurrentChunk);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// Read past this chunk and add the bytes read to the byte counter
			m_CurrentChunk->bytesRead += fread(gBuffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		default: 
			
			// If we didn't care about a chunk, then we get here.  We still need
			// to read past the unknown or ignored chunk and add the bytes read to the byte counter.
			m_CurrentChunk->bytesRead += fread(gBuffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles all the information about the objects in the file
/////
///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	// Allocate a new chunk to work with
	m_CurrentChunk = new tChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read
		switch (m_CurrentChunk->ID)
		{
		case OBJECT_MESH:					// This lets us know that we are reading a new object
		
			// We found a new object, so let's read in it's info using recursion
			ProcessNextObjectChunk(pModel, pObject, m_CurrentChunk);
			break;

		case OBJECT_VERTICES:				// This is the objects vertices
			ReadVertices(pObject, m_CurrentChunk);
			break;

		case OBJECT_FACES:					// This is the objects face information
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;

		case OBJECT_MATERIAL:				// This holds the material name that the object has
			
			// We now will read the name of the material assigned to this object
			ReadObjectMaterial(pModel, pObject, m_CurrentChunk);			
			break;

		case OBJECT_UV:						// This holds the UV texture coordinates for the object

			// This chunk holds all of the UV coordinates for our object.  Let's read them in.
			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;

		default:  

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += fread(gBuffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles all the information about the material (Texture)
/////
///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextMaterialChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	// Allocate a new chunk to work with
	m_CurrentChunk = new tChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read in
		switch (pModel, m_CurrentChunk->ID)
		{
		case MATNAME:							// This chunk holds the name of the material
			
			// Here we read in the material name
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		case MATDIFFUSE:						// This holds the R G B color of our object
			ReadColorChunk(&(pModel->pMaterials[pModel->numOfMaterials - 1]), m_CurrentChunk);
			break;
		
		case MATMAP:							// This is the header for the texture info
			
			// Proceed to read in the material information
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case MATMAPFILE:						// This stores the file name of the material

			// Here we read in the material's file name
			m_CurrentChunk->bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		
		default:  

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += fread(gBuffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// Now this is where the main loop is for reading in the animation data.
// Basically, we get the name of the object it's describing, the amount
// of frames the animation is, the pivot point, the scale, rotation and
// translation key frames.  We then take this data and interpolate
// between each key frame.

///////////////////////////////// PROCESS NEXT KEYFRAME CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles all the information about the keyframes (animation data)
/////
///////////////////////////////// PROCESS NEXT KEYFRAME CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextKeyFrameChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	char strKeyFrameObject[50] = {0};			// This stores the name of the current object being described
	float temp = 0.0f;							// This is used to swap the pivot point's y and z.

	// Allocate a new chunk to work with
	m_CurrentChunk = new tChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read
		switch (m_CurrentChunk->ID)
		{
		case KEYFRAME_MESH_INFO:				// This tells us there is a new object being described

			// This tells us that we have another objects animation data to be read,
			// so let's use recursion again so we read the next chunk and not read past this.
			ProcessNextKeyFrameChunk(pModel, m_CurrentChunk);
			break;

		case KEYFRAME_OBJECT_NAME:				// This stores the current objects name

			// Get the name of the object that the animation data being read is about.
			m_CurrentChunk->bytesRead += GetString(strKeyFrameObject);

			// Now that we have the object that is being described, set the m_CurrentObject.
			// That way we have a pointer to the object in the model to store the anim data.
			SetCurrentObject(pModel, strKeyFrameObject);
			
			// Read past 2 flags and heirarchy number (3 shorts - Not used by this loader).
			// This will be used when we do a bone animation tutorial.
			m_CurrentChunk->bytesRead += fread(gBuffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		case KEYFRAME_START_AND_END:			// This chunk stores the start and end frame
			
			// Read in the beginning frame and the end frame.  We just write over the
			// beginning frame because it is assumed that we will always start at the beginning (0)
			m_CurrentChunk->bytesRead += fread(&(pModel->numberOfFrames), 1, 4, m_FilePointer);
			m_CurrentChunk->bytesRead += fread(&(pModel->numberOfFrames), 1, 4, m_FilePointer);
			break;

		case PIVOT:								// This stores the pivot point of the object
		
			// Here we read in 3 floats which are the (X, Y, Z) for the objects pivot point.
			// The pivot point is the local axis in which the object rotates around.  This is
			// By default (0, 0, 0), but may be changed manually in 3DS Max.
			m_CurrentChunk->bytesRead += 
			fread(&(m_CurrentObject->vPivot), 1, sizeof(CVector3), m_FilePointer);

			// Because 3DS Studio Max is Z up and OpenGL is Y up, we need to swap
			// the pivot point's y and z value, then negate the z value.
			temp = m_CurrentObject->vPivot.y;
			m_CurrentObject->vPivot.y = m_CurrentObject->vPivot.z;
			m_CurrentObject->vPivot.z = -temp;
			break;

		case POSITION_TRACK_TAG:				// This stores the translation position each frame

			// Now we want to read in the positions for each frame of the animation
			ReadKeyFramePositions(pModel, m_CurrentChunk);
			break;

		case ROTATION_TRACK_TAG:				// This stores the rotation values for each KEY frame
			
			// Now we want to read in the rotations for each KEY frame of the animation.
			// This doesn't store rotation values for each frame like scale and translation,
			// so we need to interpolate between each frame.
			ReadKeyFrameRotations(pModel, m_CurrentChunk);
			break;

		case SCALE_TRACK_TAG:					// This stores the scale values for each frame

			// Now we want to read in the scale value for each frame of the animation
			ReadKeyFrameScales(pModel, m_CurrentChunk);
			break;

		default:  

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += fread(gBuffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////



///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in a chunk ID and it's length in bytes
/////
///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadChunk(tChunk *pChunk)
{
	// This reads the chunk ID which is 2 bytes.
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, m_FilePointer);

	// Then, we read the length of the chunk which is 4 bytes.
	pChunk->bytesRead += fread(&pChunk->length, 1, 4, m_FilePointer);
}

///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in a string of characters
/////
///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CLoad3DS::GetString(char *pBuffer)
{
	int index = 0;

	// Read 1 byte of data which is the first letter of the string
	fread(pBuffer, 1, 1, m_FilePointer);

	// Loop until we get NULL
	while (*(pBuffer + index++) != 0) {

		// Read in a character at a time until we hit NULL.
		fread(pBuffer + index, 1, 1, m_FilePointer);
	}

	// Return the string length, which is how many bytes we read in (including the NULL)
	return (int)strlen(pBuffer) + 1;
}


///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the RGB color data
/////
///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk)
{
	// Read the color chunk info
	ReadChunk(m_TempChunk);

	// Read in the R G B color (3 bytes - 0 through 255)
	m_TempChunk->bytesRead += fread(pMaterial->color, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);

	// Add the bytes read to our chunk
	pChunk->bytesRead += m_TempChunk->bytesRead;
}


///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the indices for the vertex array
/////
///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertexIndices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;					// This is used to read in the current face index

	// Read in the number of faces that are in this object (int)
	pPreviousChunk->bytesRead += fread(&pObject->numOfFaces, 1, 2, m_FilePointer);

	// Alloc enough memory for the faces and initialize the structure
	pObject->pFaces = new tFace [pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);

	// Go through all of the faces in this object
	for(int i = 0; i < pObject->numOfFaces; i++)
	{
		// Next, we read in the A then B then C index for the face, but ignore the 4th value.
		// The fourth value is a visibility flag for 3D Studio Max, we don't care about this.
		for(int j = 0; j < 4; j++)
		{
			// Read the first vertice index for the current face 
			pPreviousChunk->bytesRead += fread(&index, 1, sizeof(index), m_FilePointer);

			if(j < 3)
			{
				// Store the index in our face structure.
				pObject->pFaces[i].vertIndex[j] = index;
			}
		}
	}
}


///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the UV coordinates for the object
/////
///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// Read in the number of UV coordinates there are (int)
	pPreviousChunk->bytesRead += fread(&pObject->numTexVertex, 1, 2, m_FilePointer);

	// Allocate memory to hold the UV coordinates
	pObject->pTexVerts = new CVector2 [pObject->numTexVertex];

	// Read in the texture coodinates (an array 2 float)
	pPreviousChunk->bytesRead += fread(pObject->pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}


///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the vertices for the object
/////
///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// Read in the number of vertices (int)
	pPreviousChunk->bytesRead += fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);

	// Allocate the memory for the verts and initialize the structure
	pObject->pVerts = new CVector3 [pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(CVector3) * pObject->numOfVerts);

	// Read in the array of vertices (an array of 3 floats)
	pPreviousChunk->bytesRead += fread(pObject->pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);

	// Go through all of the vertices that we just read and swap the Y and Z values
	for(int i = 0; i < pObject->numOfVerts; i++)
	{
		// Store off the Y value
		float fTempY = pObject->pVerts[i].y;

		// Set the Y value to the Z value
		pObject->pVerts[i].y = pObject->pVerts[i].z;

		// Set the Z value to the Y value, 
		// but negative Z because 3D Studio max does the opposite.
		pObject->pVerts[i].z = -fTempY;
	}
}


///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the material name assigned to the object and sets the materialID
/////
///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	char strMaterial[255] = {0};			// This is used to hold the objects material name

	// Here we read the material name that is assigned to the current object.
	// strMaterial should now have a string of the material name, like "Material #2" etc..
	pPreviousChunk->bytesRead += GetString(strMaterial);

	// Go through all of the textures
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		// If the material we just read in matches the current texture name
		if(strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{
			// Set the material ID to the current index 'i' and stop checking
			pObject->materialID = i;

			// Now that we found the material, check if it's a texture map.
			// If the strFile has a string length of 1 and over it's a texture
			if(strlen(pModel->pMaterials[i].strFile) > 0) {

				// Set the object's flag to say it has a texture map to bind.
				pObject->bHasTexture = true;
			}	
			break;
		}
		else
		{
			// Set the ID to -1 to show there is no material for this object
			pObject->materialID = -1;
		}
	}

	// Read past the rest of the chunk since we don't care about shared vertices
	// You will notice we subtract the bytes already read in this chunk from the total length.
	pPreviousChunk->bytesRead += fread(gBuffer, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}			


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// The next 4 functions are new to the loader.  They read and interpolate
// between each keyframe where needed.

///////////////////////////////// READ KEYFRAME POSITIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the positions of the current object for every frame
/////
///////////////////////////////// READ KEYFRAME POSITIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadKeyFramePositions(t3DModel *pModel, tChunk *pPreviousChunk)
{	
	short frameNumber= 0, flags= 0, ignored= 0;
	long  lunknown= 0;
	int i;
	
	// This function will read in each position for every frame that we need to
	// translate the object too.  Remember, this position is relative to the object's
	// pivot point.  The first 5 short's are ignored because we do not utilize
	// them in this tutorial.  they are flags, the node ID, tension, bias, strength
	// I believe.  Don't worry about them now, the next tutorial we will further explain it.

	// Read past the ignored data
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);

	// Here we read in the number of position frames this object has.
	// In other words, how many times the object moves to a new location
	pPreviousChunk->bytesRead += fread(&(m_CurrentObject->positionFrames), 1, sizeof(short), m_FilePointer);

	// Read past one more ignored short
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);
	
	// Now we need to go through ALL of the frames of animation and set
	// the position of the object for every frame.  Even if we only have one
	// or 50 position changes out of 100 frames, we will set the remaining to
	// the last position it moved too.
	for(i = 0; i <= (pModel->numberOfFrames + 1); i++)
	{
		// Here we add a new CVector3 to our list of positions.  This will
		// store the current position for the current frame of animation 'i'.
		// Using our STL vector, we just pass in a constructor to a CVector3 and
		// it passes back a default CVector3 through the copy constructor.
		// This is the same thing as creating a new CVector3 and passing it to push_back().
		m_CurrentObject->vPosition.push_back(CVector3());

		// If the current frame of animation hasn't gone over the position frames,
		// we want to read in the next position for the current frame.
		if(i < m_CurrentObject->positionFrames)
		{
			// Read in the current frame number (not used ever, we just use i)
			pPreviousChunk->bytesRead += fread(&frameNumber, 1, sizeof(short), m_FilePointer);

			// Next we read past an unknown long
			pPreviousChunk->bytesRead += fread(&lunknown, 1, sizeof(long), m_FilePointer);
			
			// Here we read in 3 floats that store the (x, y, z) of the position.
			// Remember, CVector3 is 3 floats so it's the same thing as sizeof(float) * 3.
			pPreviousChunk->bytesRead += 
			fread(&(m_CurrentObject->vPosition[i]), 1, sizeof(CVector3), m_FilePointer);

			// Since 3D Studio Max has the Z axis up, We need to convert it to OpenGL's
			// Y axis up.  To do that we swap the y and z, then negate the new z value.
			float temp = m_CurrentObject->vPosition[i].y;
			m_CurrentObject->vPosition[i].y = m_CurrentObject->vPosition[i].z;
			m_CurrentObject->vPosition[i].z = -temp;
		}
		// Otherwise we just set the current frames position to the last position read in
		else
		{
			// Set the current frame's position to the last position read in.
			m_CurrentObject->vPosition[i] = m_CurrentObject->vPosition[m_CurrentObject->positionFrames - 1];
		}
	}

	// Now we need to go through and subtract the pivot point from each vertice.
	// 3DS files are saved with their vertices in world space PLUS their pivot point (bad).
	// You will notice we also subtract the current frame's position from each point.
	// We do this because 3DS files store the position of the pivot point for each frame.
	// We want the pivot point to start at zero so when we call glTranslate() it will
	// translate to the first position, not double the first position.  This might be
	// confusing but if the vertices were at the first frame, and then we translated
	// to the first position, it would then translate double of where we needed to be.
	// If you are confused, get rid of the m_CurrentObject->vPosition[0] below and see what happens.
	// If we didn't do this, we would need to translate BACK each frame -position.
	for(i = 0; i < m_CurrentObject->numOfVerts; i++)
	{
		// Subtract the current frames position and pivtor point from each vertice to make it easier.
		m_CurrentObject->pVerts[i].x -= m_CurrentObject->vPosition[0].x + m_CurrentObject->vPivot.x;
		m_CurrentObject->pVerts[i].y -= m_CurrentObject->vPosition[0].y + m_CurrentObject->vPivot.y;
		m_CurrentObject->pVerts[i].z -= m_CurrentObject->vPosition[0].z + m_CurrentObject->vPivot.z;
	}
}


///////////////////////////////// READ KEYFRAME ROTATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the rotations of the current object for every key frame
/////
///////////////////////////////// READ KEYFRAME ROTATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadKeyFrameRotations(t3DModel *pModel, tChunk *pPreviousChunk)
{
	short frameNumber = 0, flags = 0, rotkeys = 0, ignored = 0;
	long lunknown = 0; 
	float rotationDegree = 0;
	vector<int> vFrameNumber;
	vector<float> vRotDegree;
	vector<CVector3> vRotation;
	int i;

	// This function will read in each key frames rotation angle and rotation axis.
	// Remember, this rotation is relative to the object's pivot point.  The first 5 
	// short's are ignored because we do not utilize them in this tutorial.  They are 
	// flags, the node ID, tension, bias, and strength I believe.  Don't worry about 
	// them now, the next tutorial we will further explain it.

	// Read past the ignored data
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1,  sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1,  sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignored, 1,  sizeof(short), m_FilePointer);

	// Read in the number of rotation key frames for the animation.
	// Remember, unlike the scale and translation data, it does not store
	// the rotation degree and axis for every frame, only for every key frame.
	// That is why we need to interpolate below between each key frame.
	pPreviousChunk->bytesRead += fread(&(m_CurrentObject->rotationFrames), 1, sizeof(short), m_FilePointer);

	// Read past an ignored short
	pPreviousChunk->bytesRead += fread(&ignored, 1,  sizeof(short), m_FilePointer);
	
	// Now we need to go through ALL of the frames of animation and set
	// the rotation of the object for every frame.  We will need to interpolate
	// between key frames if there is more than 1 (there is always at least 1).
	for(i = 0; i < m_CurrentObject->rotationFrames; i++)
	{
		// Here we add a new CVector3 to our list of rotations.  This will
		// store the current rotation axis for the current frame of animation 'i'.
		// Using our STL vector, we just pass in a constructor to a CVector3 and
		// it passes back a default CVector3 through the copy constructor.
		// This is the same thing as creating a new CVector3 and passing it to push_back().
		vRotation.push_back(CVector3());

		// Next we read in the frame number that the rotation takes place
		pPreviousChunk->bytesRead += fread(&frameNumber, 1, sizeof(short), m_FilePointer);

		// We add the frame number to our list of frame numbers for later when interpolating
		vFrameNumber.push_back(frameNumber);

		// Then we read past some unknown data
		pPreviousChunk->bytesRead += fread(&lunknown, 1, sizeof(long), m_FilePointer);

		// Read in the current rotation degree for this key frame.  We will
		// also inteprolate between this degree down below if needed.
		pPreviousChunk->bytesRead += fread(&rotationDegree, 1, sizeof(float), m_FilePointer);

		// Because 3DS Max stores the rotation degree in radians, we need to
		// convert it to degrees because glRotatef() takes degree not radians.

		// Convert the radians to degress (Radians * (180 / PI) = degrees)
		rotationDegree = rotationDegree * (180.0f / 3.14159f);

		// Add the rotation degree to our rotation list to be interpolated if needed
		vRotDegree.push_back(rotationDegree);

		// Here we read in the actual axis that the object will rotate around.
		// This will NOT need to be interpolated because the rotation degree is what matters.
		pPreviousChunk->bytesRead += fread(&(vRotation[i]), 1, sizeof(CVector3), m_FilePointer);
		
		// Because I was having problems before with really small numbers getting
		// set to scientific notation and not being passed in correctly to glRotatef(),
		// I just decided to round them down to 0 if they were too small.
		// This isn't ideal, but it seemed to work for me.
		vRotation[i].x = RoundFloat(vRotation[i].x);
		vRotation[i].y = RoundFloat(vRotation[i].y);
		vRotation[i].z = RoundFloat(vRotation[i].z);

		// Since 3DS Max is Z up, we need to convert it to Y up.
		// We just switch the Y and Z values.  This is a bit different
		// through because we don't negate the new z value, but negate
		// the new Y value.  Also, below we negate the X value.
		// Because rotations are different than just points this is how it's done.
		float temp = vRotation[i].y;
		vRotation[i].y = -vRotation[i].z;
		vRotation[i].z = temp;

		// We negate the X value to set it right from Z up to Y up axis'
		vRotation[i].x *= -1;
	}

	// Now we have all of the rotation data for the animation, but we are
	// missing one thing.  Because rotation data is only stored for each key frame,
	// we need to interpolate between each keyframe. (I wish 3DS just stored each frame
	// like the translation and scale values - who knows why... Maybe easier with matrices)

	// Add the rotation axis for the first frame to our list.  
	m_CurrentObject->vRotation.push_back(vRotation[0]);

	// Add the rotation degree for the first frame to our list.  If we did NO rotation
	// in our animation the rotation degree should be 0.
	m_CurrentObject->vRotDegree.push_back(vRotDegree[0]);

	// Create a counter for the current rotation key we are on (Only used if rotKeys are > 1)
	int currentKey = 1;

	// Not to be repetitive or anything, but we need to go through all of the
	// frames of animation again so we can interpolate between each rotation key frame.
	// If there is only one rotation key frame, we just set the rest of the rotation degrees to 0.

	// Go through all of the frames of animation plus 1 because it's zero based
	for(i = 1; i <= (pModel->numberOfFrames + 1); i++)
	{		
		// Check if the current key frame is less than or equal to the max key frames
		if(currentKey < m_CurrentObject->rotationFrames)
		{
			// Get the current and previous key frame number, along with the rotation degree.
			// This just makes it easier code to work with, especially since you can't
			// debug vectors easily because they are operator overloaded.
			int currentFrame = vFrameNumber[currentKey];
			int previousFrame = vFrameNumber[currentKey - 1];
			float degree = vRotDegree[currentKey];

			// Interpolate the rotation degrees between the current and last key frame.
			// Basically, this sickningly simple algorithm is just getting how many
			// frames are between the last and current keyframe (currentFrame - previousFrame),
			// Then dividing the current degree by that number.  For instance, say there
			// is a key frame at frame 0, and frame 50.  Well, 50 - 0 == 50 so it comes out
			// to rotationDegree / 50.  This will give us the rotation needed for each frame.
			float rotDegree = degree / (currentFrame - previousFrame);

			// Add the current rotation degree and vector for this frame
			m_CurrentObject->vRotation.push_back(vRotation[currentKey]);
			m_CurrentObject->vRotDegree.push_back(rotDegree);

			// Check if we need to go to the next key frame 
			// (if the current frame i == the current key's frame number)
			if(vFrameNumber[currentKey] <= i) 
				currentKey++;		
		}
		// Otherwise, we are done with key frames, so no more interpolating
		else
		{
			// Set the rest of the rotations to 0 since we don't need to rotate it anymore
			// The rotation axis doesn't matter since the degree is 0.
			m_CurrentObject->vRotation.push_back(vRotation[currentKey - 1]);
			m_CurrentObject->vRotDegree.push_back(0.0f);
		}
	}

	// One thing I might want to point out is that you can do the rotations 2
	// different ways.  To avoid a complicated tutorial I did NOT use matrices.
	// I started out doing it that way, but at the end changed it to just use a
	// bunch of glRotatef()'s.  You will notice in the AnimateModel() function
	// I do a for loop that calls glRotatef() for every frame.  The other way to
	// do this is create a matrix class that then matrix multiplies the rotation
	// by the current matrix.  Since we don't have a matrix class tutorial up yet
	// I decided not try and teach that at the same time.  In the next bone animation
	// tutorial we WILL use matrices though.  We should also have a matrix class tutorial too.
}


///////////////////////////////// READ KEYFRAME SCALE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the scale value of the current object for every key frame
/////
///////////////////////////////// READ KEYFRAME SCALES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadKeyFrameScales(t3DModel *pModel, tChunk *pPreviousChunk)
{	
	short frameNumber = 0, ignore = 0, flags = 0;
	long lunknown = 0;	
	int i = 0;

	// Like the translation key frame data, the scale ratio is stored for
	// every frame in the animation, not just for every key frame.  This makes
	// it so we don't have to interpolate between frames. 
	// The first 5 short's are ignored because we do not utilize
	// them in this tutorial.  they are flags, the node ID, tension, bias, strength
	// I believe.  Don't worry about them now, the next tutorial we will further explain it.

	// Read past the ignored data
	pPreviousChunk->bytesRead += fread(&ignore, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignore, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignore, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignore, 1, sizeof(short), m_FilePointer);
	pPreviousChunk->bytesRead += fread(&ignore, 1, sizeof(short), m_FilePointer);

	// Here we read in the amount of scale frames there are in the animation.
	// If there is 100 frames of animation and only 50 frames of scaling, we just
	// set the rest of the 50 frames to the last scale frame.
	pPreviousChunk->bytesRead += fread(&(m_CurrentObject->scaleFrames), 1, sizeof(short), m_FilePointer);

	// Read past ignored data
	pPreviousChunk->bytesRead += fread(&ignore, 1, sizeof(short), m_FilePointer);
	
	// Now we need to go through ALL of the frames of animation and set
	// the scale value of the object for every frame.  Even if we only have one
	// or 50 scale changes out of 100 frames, we will set the remaining to
	// the last position it scaled too.
	for(i = 0; i <= (pModel->numberOfFrames + 1); i++)
	{
		// Here we add a new CVector3 to our list of scale values. 
		// Using our STL vector, we just pass in a constructor to a CVector3 and
		// it passes back a default CVector3 through the copy constructor.
		// This is the same thing as creating a new CVector3 and passing it to push_back().
		m_CurrentObject->vScale.push_back(CVector3());

		// If the current frame is less than the amount of scale frames, read scale data in
		if(i < m_CurrentObject->scaleFrames)
		{
			// Read in the current frame number (not used because there is no interpolation)
			pPreviousChunk->bytesRead += fread(&frameNumber, 1, sizeof(short), m_FilePointer);

			// Read past an unknown long
			pPreviousChunk->bytesRead += fread(&lunknown, 1, sizeof(long), m_FilePointer);
			
			// Read in the (X, Y, Z) scale value for the current frame.  We will pass this
			// into glScalef() when animating in AnimateModel()
			pPreviousChunk->bytesRead += 
			fread(&(m_CurrentObject->vScale[i]), 1, sizeof(CVector3), m_FilePointer);

			// Because 3DS Max has Z up, we need to convert it to Y up like OpenGL has.
			// We don't need to negate the z though since it's a scale ratio not a 3D point.
			float temp = m_CurrentObject->vScale[i].y;
			m_CurrentObject->vScale[i].y = m_CurrentObject->vScale[i].z;
			m_CurrentObject->vScale[i].z = temp;
		}
		// Otherwise we are done with the scale frames so set the rest to the last scale value
		else
		{
			// Set the current frame's scale value to the last scale value for the animation
			m_CurrentObject->vScale[i] = m_CurrentObject->vScale[m_CurrentObject->scaleFrames - 1];
		}
	}
}


///////////////////////////////// SET CURRENT OBJECT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the current model that animation is being read in for by it's name
/////
///////////////////////////////// SET CURRENT OBJECT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::SetCurrentObject(t3DModel *pModel, char *strObjectName)
{
	// This function takes a model and name of an object inside of the model.
	// It then searches the objects in the model and finds that object that
	// has the name passed in.  We use this function after we have read in
	// all the model's data, except for the KEY FRAME data.  The key frame
	// data is last.  The key frame animation data stores the objects name
	// that the data is describing, so we need to get that address to
	// that object and then set the animation data being read in for it.

	// Make sure there was a valid object name passed in
	if(!strObjectName) 
	{
		// Set the current object to NULL and return
		m_CurrentObject = NULL;
		return;
	}

	// Go through all of the models objects and match up the name passed in
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// Check if the current model being looked at has the same name as strObjectName
		if(strcmp(pModel->pObject[i].strName, strObjectName) == 0)
		{
			// Get a pointer to the object with the name passed in.
			// This will be the object that the next animation data is describing
			m_CurrentObject =&(pModel->pObject[i]);
			return;
		}
	}

	// Give an error message (better to have an assert()
	MessageBox(NULL, "ERROR: No object in model with given name! (SetCurrentObject)", "Error!", MB_OK);

	// Set the current object to NULL since we didn't find an object with that name
	m_CurrentObject = NULL;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// *Note* 
//
// Below are some math functions for calculating vertex normals.  We want vertex normals
// because it makes the lighting look really smooth and life like.  You probably already
// have these functions in the rest of your engine, so you can delete these and call
// your own.  I wanted to add them so I could show how to calculate vertex normals.

//////////////////////////////	Math Functions  ////////////////////////////////*

// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// This calculates a vector between 2 points and returns the result
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;							// The variable to hold the resultant vector

	vVector.x = vPoint1.x - vPoint2.x;			// Subtract point1 and point2 x's
	vVector.y = vPoint1.y - vPoint2.y;			// Subtract point1 and point2 y's
	vVector.z = vPoint1.z - vPoint2.z;			// Subtract point1 and point2 z's

	return vVector;								// Return the resultant vector
}

// This adds 2 vectors together and returns the result
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector2.x + vVector1.x;		// Add Vector1 and Vector2 x's
	vResult.y = vVector2.y + vVector1.y;		// Add Vector1 and Vector2 y's
	vResult.z = vVector2.z + vVector1.z;		// Add Vector1 and Vector2 z's

	return vResult;								// Return the resultant vector
}

// This divides a vector by a single number (scalar) and returns the result
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector1.x / Scaler;			// Divide Vector1's x value by the scaler
	vResult.y = vVector1.y / Scaler;			// Divide Vector1's y value by the scaler
	vResult.z = vVector1.z / Scaler;			// Divide Vector1's z value by the scaler

	return vResult;								// Return the resultant vector
}

// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;								// The vector to hold the cross product
												// Get the X value
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
												// Get the Y value
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
												// Get the Z value
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;								// Return the cross product
}

// This returns the normal of a vector
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;							// This holds the magitude			

	Magnitude = Mag(vNormal);					// Get the magnitude

	vNormal.x /= (float)Magnitude;				// Divide the vector's X by the magnitude
	vNormal.y /= (float)Magnitude;				// Divide the vector's Y by the magnitude
	vNormal.z /= (float)Magnitude;				// Divide the vector's Z by the magnitude

	return vNormal;								// Return the normal
}

///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function computes the normals and vertex normals of the objects
/////
///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// If there are no objects, we can skip this part
	if(pModel->numOfObjects <= 0)
		return;

	// Go through each of the objects to calculate their normals
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// Get the current object
		t3DObject *pObject = &(pModel->pObject[index]);

		// Here we allocate all the memory we need to calculate the normals
		CVector3 *pNormals		= new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals	= new CVector3 [pObject->numOfFaces];
		pObject->pNormals		= new CVector3 [pObject->numOfVerts];

		// Go though all of the faces of this object
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			// To cut down LARGE code, we extract the 3 points of this face
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)

			vVector1 = Vector(vPoly[0], vPoly[2]);		// Get the vector of the polygon (we just need 2 sides for the normal)
			vVector2 = Vector(vPoly[2], vPoly[1]);		// Get a second vector of the polygon

			vNormal  = Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
			pTempNormals[i] = vNormal;					// Save the un-normalized normal for the vertex normals
			vNormal  = Normalize(vNormal);				// Normalize the cross product to give us the polygons normal

			pNormals[i] = vNormal;						// Assign the normal to the list of normals
		}

		//////////////// Now Get The Vertex Normals /////////////////

		CVector3 vSum = {0.0, 0.0, 0.0};
		CVector3 vZero = vSum;
		int shared=0;

		for (int i = 0; i < pObject->numOfVerts; i++)			// Go through all of the vertices
		{
			for (int j = 0; j < pObject->numOfFaces; j++)	// Go through all of the triangles
			{												// Check if the vertex is shared by another face
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);// Add the un-normalized normal of the shared face
					shared++;								// Increase the number of shared triangles
				}
			}      
			
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// Normalize the normal for the final vertex normal
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);	

			vSum = vZero;									// Reset the sum
			shared = 0;										// Reset the shared
		}
	
		// Free our memory and start over on the next object
		delete [] pTempNormals;
		delete [] pNormals;
	}
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file has support for key frame animation, but is still missing bones.
// Skeletal animation will be in the next tutorial on animation.  This is a HUGE
// concept so this will most likely be done in about 3 seperate tutorials.
//
// * Note *
// 
// 3DS max isn't the best program for animation, but It seems to be the most popular
// in the game industry.  This animation tutorial is not the most robust it could be,
// but should be taken as a learning tool to creating a better animation system.  This
// should be a step up to bone animation, which is the BEST way to make cool characters.
// This animation is just objects rotating, scaling and translating.  It is how they
// used to do it back in the 80's early 90's.  So don't try and take a cool model
// and put bones and IK on it, animate it and expect anything to happen :).  
// One thing to keep in mind, remember that we IGNORE the frame 0 of the animation.
// If you want to use frame zero for some reason, here is a way that it won't screw up
// the rotation key frames (it saves the rotation of the object for the first frame (0)).
// After you model your scene, before you start animating it, select all the objects,
// go to the heirarchy tab (On the right of the modify tab) and select the button
// called "Transform" under the "Reset" section.  If you don't and you have rotations in
// int the first frame (0), it will screw up the animation.  Please refer to the HTML tutorial
// on how to create this type of animation and the steps to take.  This should always 
// accompany that HTML tutorial.  So remember, we want to ignore frame 0 of the animation,
// but if you need to use it, be sure to reset the transforms for every object in your model.
//
// Okay, now that I covered my back :)  Let's start talking about how this works!
// Once again, refer to the .3DS loader tutorial if you are confused on how to do 
// the basics of loading data from a .3DS file.  It is at www.GameTutorials.com in
// the OpenGL section.
//
// The basic concepts of key frame animation is this:  You store a position, rotation
// and scale value on certain frames, and it interpolates between that data.  Let's say,
// if you start at frame 0, with a box centered around (0, 0, 0), then go to frame 60
// and center the box around (10, 10, 10), THEN play the animation it should move the
// box from (0, 0, 0) to (10, 10, 10) smoothly.  The same thing works for rotations
// and scales.  You just set "KEY" frames and it does the rest for you.  The weird thing
// with 3DS files is that it saves the position and scale EVERY frame between each key frame,
// buton only saves the rotation values for each key frame.  That means that we still have
// to do the interpolation for the rotation, where we get the position and scale value for
// every frame, so they did the interpolation for us.  
//
// So that is all we are doing, just reading in the positions/Scale value for ever frame,
// and if there isn't a new position/scale value for the next frame we just set the rest
// of the position/scale values to the last one.  The same thing with rotations, but we
// first get the key frames rotation degree and axis it's rotation along, inteprolate
// between key frames, then if there is no more rotating we just set the rest of the
// frames rotation degrees to 0.  This should ideally be done with a matrix, where it
// matrix multiplies the rotation degree and axis by the current rotation matrix but
// I didn't want to complicate the tutorial any more.  Any other animation tutorials will
// use matrices.  Instead, I just loop through all the degrees and call glRotatef() for
// every frame that has happened.  That way it's doing the same thing, but less efficient.
// 
// There is some data that we ignore in the KEYFRAME chunk, but it will be explained later
// in another tutorial.  This is the jist of it.  Go through the code and read the comments
// around each function to get a more in depth idea of what is going on.  It's pretty basic
// stuff, just a pain to get the data in the format you want it.
//
// I would like to thank www.wosit.org and Terry Caton (tcaton@umr.edu) for his help on this.
//
// Let me know if this helps you out!
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
