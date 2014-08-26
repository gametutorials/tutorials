//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		ASE Loader										 //
//																		 //
//		$Description:	Demonstrates how to load a .Ase file format		 //
//																		 //
//***********************************************************************//

#include "Main.h"
#include "Ase.h"


///////////////////////////////// IMPORT ASE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function is called to load in an .ase file by the file name
/////
///////////////////////////////// IMPORT ASE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadASE::ImportASE(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};				// This will be used for error messages

	// Make sure we have a valid model and file name
	if(!pModel || !strFileName) return false;

	// Here we open the desired file for read only and return the file pointer
	m_FilePointer = fopen(strFileName, "r");

	// Check to make sure we have a valid file pointer
	if(!m_FilePointer) {
		// Create an error message for the attempted file
		sprintf(strMessage, "Unable to find or open the file: %s", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Now that we have a valid file and it's open, let's read in the info!
	ReadAseFile(pModel);

	// Now that we have the file read in, let's compute the vertex normals for lighting
	ComputeNormals(pModel);

	// Close the .ase file that we opened
	fclose(m_FilePointer);

	// Return a success!
	return true;
}


///////////////////////////////// READ ASE FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads the data for every object and it's associated material
/////
///////////////////////////////// READ ASE FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::ReadAseFile(t3DModel *pModel)
{
	tMaterialInfo newMaterial = {0};		// This will be used to push on a new material
	t3DObject     newObject   = {0};		// This will be used to push on a new object
	int i = 0;	

	// This function is the head honcho for reading in the .ase data and information.
	// What happens is, we go through the whole file and count the objects, then we
	// rewind the file pointer and go through the whole file again and count the materials.
	// Then we go through the file for each material and each object and read the data in.

	// This will return the number of objects stored in the .ase file
	pModel->numOfObjects   = GetObjectCount();

	// This will return the number of materials stored in the .ase file
	pModel->numOfMaterials = GetMaterialCount();

	// Go through all the materials and fill in their data and info
	for(i = 0; i < pModel->numOfMaterials; i++)
	{
		// Add a new material to our list of materials using the STL "vector" class
		pModel->pMaterials.push_back(newMaterial);

		// Get the material info for the current material.  We add 1 because we
		// want to start at 1 and i starts at 0.
		GetTextureInfo(&(pModel->pMaterials[i]), i + 1);
	}

	// Go through all the objects and fill in their data and info
	for(i = 0; i < pModel->numOfObjects; i++)
	{	
		// Add a new object to our list of objects using the STL "vector" class
		pModel->pObject.push_back(newObject);

		// Set the material ID to -1 to initialize it.  This will be changed
		// if there is a texture/material assigned to this object.
		pModel->pObject[i].materialID = -1;

		// Move the file pointer to the desired object.  We add one because our
		// object count starts at 1 and i starts at 0
		MoveToObject(i + 1);

		// Find out the number of vertices, faces and texture coordinates for this object, 
		// then allocate the memory needed to store that amount that needs to be read in.
		ReadObjectInfo(&(pModel->pObject[i]), i + 1);
		
		// Read the vertices, face indices and texture coordinates for this object
		ReadObjectData(pModel, &(pModel->pObject[i]), i + 1);
	}
}


///////////////////////////////// GET OBJECT COUNT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns the total object count in the .ase file
/////
///////////////////////////////// GET OBJECT COUNT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CLoadASE::GetObjectCount()
{
	char strWord[255] = {0};
	int objectCount = 0;

	// This function goes through the file and reads each word.  When it
	// finds an OBJECT tag, it increases the object count.

	// Go to the beginning of the file.  Then we can start fresh and get a good count.
	rewind(m_FilePointer);

	// Go through the whole file and end when we reached the END
	while (!feof(m_FilePointer))
	{
		// Get each word in the file
		fscanf(m_FilePointer, "%s", &strWord);

		// Check if we hit the start of an object
		if (!strcmp(strWord, OBJECT))
		{
			// Increase the current object we are at 
			objectCount++;
		}
		else
		{
			// Go to the next line
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// Return the object count in the .ase file
	return objectCount;
}


///////////////////////////////// GET MATERIAL COUNT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns the total material count for the .ase file
/////
///////////////////////////////// GET MATERIAL COUNT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CLoadASE::GetMaterialCount()
{
	char strWord[255] = {0};
	int materialCount = 0;

	// This function goes through the file and reads each word.  When it
	// finds the MATERIAL_COUNT tag, it reads the material count.

	// Go to the beginning of the file
	rewind(m_FilePointer);

	// GO through the whole file until we hit the end
	while (!feof(m_FilePointer))
	{
		// Get each word in the file
		fscanf(m_FilePointer, "%s", &strWord);

		// Check if we hit the start of an object
		if (!strcmp(strWord, MATERIAL_COUNT))
		{
			// Read in the material count
			fscanf(m_FilePointer, "%d", &materialCount);

			// Return the material count
			return materialCount;
		}
		else
		{
			// Go to the next line
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// Return NO materials if we get here
	return 0;
}


///////////////////////////////// GET TEXTURE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the information about the desired material
/////
///////////////////////////////// GET TEXTURE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::GetTextureInfo (tMaterialInfo *pTexture, int desiredMaterial)
{
	char strWord[255] = {0};
	int materialCount= 0;
	
	// In this function we check for an MATERIAL tag, then see if it's the
	// material we want by the desiredMaterial number.  If so we start reading it in.

	// Go to the beginning of the file
	rewind(m_FilePointer);

	// Go through the whole file until we reach the end
	while (!feof(m_FilePointer))
	{
		// Get each word from the file
		fscanf(m_FilePointer, "%s", &strWord);

		// Check if we hit the start of an object
		if (!strcmp(strWord, MATERIAL))
		{
			// Increase the current object we are at 
			materialCount++;

			// Check if it's the one we want to stop at, if so break
			if(materialCount == desiredMaterial)
				break;
		}
		else
		{
			// Go to the next line
			fgets(strWord, 100, m_FilePointer);
		}
	}

	// Now we are at the material we want, so let's read it's data in

	// Go through the rest of the file until we hit the end
	while (!feof(m_FilePointer))
	{
		// Get each word from the file
		fscanf(m_FilePointer, "%s", &strWord);

		// If we found a MATERIAL tag stop because we went to far
		if (!strcmp (strWord, MATERIAL))
		{
			// We hit a new material, so we need to stop
			return;
		}
		// If we hit a MATERIAL_COLOR tag, we need to get the material's color
		else if (!strcmp(strWord, MATERIAL_COLOR))
		{
			// Get the material RGB color of the object
			fscanf(m_FilePointer, " %f %f %f", &(pTexture->fColor[0]), 
											   &(pTexture->fColor[1]), 
											   &(pTexture->fColor[2]));
		}
		// If we hit a TEXTURE tag, we need to get the texture's name
		else if (!strcmp(strWord, TEXTURE))
		{
			// Get the file name of the texture
			GetTextureName(pTexture);
		}
		// If we hit a MATERIAL_NAME tag, we need to get the material's name
		else if (!strcmp(strWord, MATERIAL_NAME))
		{
			// Get the material name of the object
			GetMaterialName(pTexture);
		}
		// If we hit a UTILE tag, we need to get the U tile ratio
		else if(!strcmp(strWord, UTILE))
		{
			// Read the U tiling for the U coordinates of the texture
			pTexture->uTile = ReadFloat();
		}
		// If we hit a VTILE tag, we need to get the V tile ratio
		else if(!strcmp(strWord, VTILE))
		{
			// Read the V tiling for the V coordinates of the texture
			pTexture->vTile = ReadFloat();
		}
		// Otherwise ignore the data and read past it
		else
		{
			// Go to the next line
			fgets (strWord, 100, m_FilePointer);
		}
	}
}


///////////////////////////////// MOVE TO OBJECT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function moves the file pointer in the .ase file to the desired object
/////
///////////////////////////////// MOVE TO OBJECT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::MoveToObject (int desiredObject)
{
	char strWord[255] = {0};
	int objectCount = 0;

	// This function takes the number of the desired object to move to.
	// We read through the file from the beginning and increase a counter.
	// When that counter gets to the desiredObject we then stop.  Now the file
	// pointer can read the object data from that specific object

	// Go to the beginning of the file
	rewind(m_FilePointer);

	// Go through the whole file until we reach the end
	while(!feof(m_FilePointer))
	{
		// Get each word
		fscanf(m_FilePointer, "%s", &strWord);

		// Check if we hit the start of an object
		if(!strcmp(strWord, OBJECT))
		{
			// Increase the current object we are at 
			objectCount++;

			// Check if it's the one we want to stop at, if so stop reading
			if(objectCount == desiredObject)
				return;
		}
		else
		{
			// Go to the next line and skip this current line
			fgets(strWord, 100, m_FilePointer);
		}
	}
}


///////////////////////////////// READ FLOAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in and returns a float from the .ase file
/////
///////////////////////////////// READ FLOAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float CLoadASE::ReadFloat()
{
	float v = 0.0f;

	// Read in a float
	fscanf(m_FilePointer, " %f", &v);

	// Return the float
	return v;
}


///////////////////////////////// READ OBJECT INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the information about the object, but not the data
/////
///////////////////////////////// READ OBJECT INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::ReadObjectInfo(t3DObject *pObject, int desiredObject)
{
	char strWord[255] = {0};

	// In this function we position the file pointer to the object we want
	// to read in, then we read until we find the associated tags, then read the info.

	// Go to the desired object to read from in the file
	MoveToObject(desiredObject);
	
	// While we are not at the end of the file
	while (!feof(m_FilePointer))
	{
		// Read in each word from the file to check against
		fscanf(m_FilePointer, "%s", &strWord);

		// If we hit the number of vertices tag
		if (!strcmp(strWord, NUM_VERTEX))
		{
			// Read in the number of vertices for this object
			fscanf(m_FilePointer, "%d", &pObject->numOfVerts);

			// Allocate enough memory to hold the vertices
			pObject->pVerts = new CVector3 [pObject->numOfVerts];
		}
		// If we hit the number of faces tag
		else if (!strcmp(strWord, NUM_FACES))
		{
			// Read in the number of faces for this object
			fscanf(m_FilePointer, "%d", &pObject->numOfFaces);

			// Allocate enough memory to hold the faces
			pObject->pFaces = new tFace [pObject->numOfFaces];
		}
		// If we hit the number of texture vertices tag
		else if (!strcmp(strWord, NUM_TVERTEX))
		{
			// Read in the number of texture coordinates for this object
			fscanf(m_FilePointer, "%d", &pObject->numTexVertex);

			// Allocate enough memory for the UV coordinates
			pObject->pTexVerts = new CVector2 [pObject->numTexVertex];
		}
		// If we hit the object tag we want to stop cause we went to far
		else if (!strcmp(strWord, OBJECT))	
		{
			// Return if we get to the next object
			return;
		}
		else 
		{
			// We didn't find anything we want to check for so we read to the next line
			fgets(strWord, 100, m_FilePointer);
		}
	}	
}


///////////////////////////////// GET TEXTURE NAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the file name of the texture assigned to the object
/////
///////////////////////////////// GET TEXTURE NAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::GetTextureName(tMaterialInfo *pTexture)
{
	// Read in the texture's file name
	fscanf (m_FilePointer, " \"%s", &(pTexture->strFile));
	
	// Put a NULL character at the end of the string
	pTexture->strFile[strlen (pTexture->strFile) - 1] = '\0';
}


///////////////////////////////// GET MATERIAL NAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the material name of the object
/////
///////////////////////////////// GET MATERIAL NAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::GetMaterialName(tMaterialInfo *pTexture)
{
	// Read in the material's name (Make sure this is just one word or it won't work well)
	fscanf (m_FilePointer, " \"%s", &(pTexture->strName));
	
	// Put a NULL character at the end of the string just in case
	pTexture->strName[strlen (pTexture->strName)] = '\0';
}

///////////////////////////////// READ OBJECT DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function calls the functions that query and read in the desired data
/////
///////////////////////////////// READ OBJECT DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::ReadObjectData(t3DModel *pModel, t3DObject *pObject, int desiredObject)
{
	// This will go through the file every time for each data tag and
	// read in the data for that tag.  We pass in the desired object to read it from.
	// This isn't the fastest way in the world, but it works fine.  The best way
	// for reading models in from this format would be to create a program that
	// converts the data to binary so you don't have to query, but load it in straightway.

	// Load the material ID for this object
	GetData(pModel, pObject, MATERIAL_ID, desiredObject);

	// Load the vertices for this object
	GetData(pModel, pObject, VERTEX,		 desiredObject);

	// Load the texture coordinates for this object
	GetData(pModel, pObject, TVERTEX,	 desiredObject);

	// Load the vertex faces list for this object
	GetData(pModel, pObject, FACE,		 desiredObject);

	// Load the texture face list for this object
	GetData(pModel, pObject, TFACE,		 desiredObject);

	// Load the texture for this object
	GetData(pModel, pObject, TEXTURE,	 desiredObject);

	// Load the U tile for this object
	GetData(pModel, pObject, UTILE,		 desiredObject);

	// Load the V tile for this object
	GetData(pModel, pObject, VTILE,		 desiredObject);
}


///////////////////////////////// GET DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function is the loop for reading in the object data
/////
///////////////////////////////// GET DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::GetData(t3DModel *pModel, t3DObject *pObject, char *strDesiredData, int desiredObject)
{
	char strWord[255] = {0};

	// In here, we move the file pointer to the desired object we want to read in,
	// then we query all the tags that are being read in.  If we find a valid tag,
	// then we need to check if that's the tag that we want to read in.  If so,
	// then we read it in.

	// Move the file pointer to the desired object
	MoveToObject(desiredObject);

	// Go through the file until we reach the end
	while(!feof(m_FilePointer))
	{
		// Read in every word to check it against tags
		fscanf(m_FilePointer, "%s", &strWord);

		// If we reached an object tag, stop read because we went to far
		if(!strcmp(strWord, OBJECT))	
		{
			// Stop reading because we are done with the current object
			return;
		}
		// If we hit a vertex tag
		else if(!strcmp(strWord, VERTEX))
		{
			// Make sure that is the data that we want to read in
			if(!strcmp(strDesiredData, VERTEX)) 
			{
				// Read in a vertex
				ReadVertex(pObject);
			}
		}
		// If we hit a texture vertex
		else if(!strcmp(strWord, TVERTEX))
		{
			// Make sure that is the data that we want to read in
			if(!strcmp(strDesiredData, TVERTEX)) 
			{
				// Read in a texture vertex
				ReadTextureVertex(pObject, pModel->pMaterials[pObject->materialID]);
			}
		}
		// If we hit a vertice index to a face
		else if(!strcmp(strWord, FACE))
		{
			// Make sure that is the data that we want to read in
			if(!strcmp(strDesiredData, FACE)) 
			{
				// Read in a face
				ReadFace(pObject);
			}
		}
		// If we hit a texture index to a face
		else if(!strcmp(strWord, TFACE))
		{
			// Make sure that is the data that we want to read in
			if(!strcmp(strDesiredData, TFACE))
			{
				// Read in a texture indice for a face
				ReadTextureFace(pObject);
			}
		}
		// If we hit the material ID to the object
		else if(!strcmp(strWord, MATERIAL_ID))
		{
			// Make sure that is the data that we want to read in
			if(!strcmp(strDesiredData, MATERIAL_ID))
			{
				// Read in the material ID assigned to this object
				pObject->materialID = (int)ReadFloat();
				return;
			}				
		}
		else 
		{
			// We must not care about this tag read so read past the whole line
			fgets(strWord, 100, m_FilePointer);
		}
	}
}


///////////////////////////////// READ VERTEX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the vertices for the object
/////
///////////////////////////////// READ VERTEX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::ReadVertex(t3DObject *pObject)
{
	int index = 0;

	// Read past the vertex index
	fscanf(m_FilePointer, "%d", &index);
	
	// Here we read in the vertice's (X, Y, Z).  Since 3D Studio Max has Z pointing
	// up, we are going to swap the Y and Z to make it normal.  Otherwise we would need
	// to rotate it around the X axis by -90 degrees: glRotatef(-90, 1.0f, 0.0f, 0.0f);
	fscanf(m_FilePointer, "%f %f %f", &pObject->pVerts[index].x, 
									  &pObject->pVerts[index].z,
									  &pObject->pVerts[index].y);
		
	// Also in 3D studio max the Z goes out of the screen so we want to swap that too.
	// We do that by negating the Z value of each vertex.
	pObject->pVerts[index].z = -pObject->pVerts[index].z;
}


///////////////////////////////// READ TEXTURE VERTEX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the texture coordinates
/////
///////////////////////////////// READ TEXTURE VERTEX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::ReadTextureVertex(t3DObject *pObject, tMaterialInfo texture)
{
	int index = 0;

	// Here we read past the index of the texture coordinate
	fscanf(m_FilePointer, "%d", &index);

	// Next, we read in the (U, V) texture coordinates.
	fscanf(m_FilePointer, "%f %f", &(pObject->pTexVerts[index].x), &(pObject->pTexVerts[index].y));

	// What is being done here is we are multiplying a X and Y tile factor
	// to the UV coordinate.  Usually the uTile and vTile is 1, but if it depends on
	// your UVW map.  If you made the texture tile more along the object, these would change.
	pObject->pTexVerts[index].x *= texture.uTile;
	pObject->pTexVerts[index].y *= texture.vTile;

	// We know this object has a texture so let's set this true
	pObject->bHasTexture = true;
}


///////////////////////////////// READ FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the indices to the array of vertices
/////
///////////////////////////////// READ FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::ReadFace(t3DObject *pObject)
{
	int index = 0;

	// Read past the index of this Face
	fscanf(m_FilePointer, "%d:", &index);

	// Now we read in the actual vertex indices; One index for each point in the triangle.
	// These indices will index into the vertex array pVerts[].
	fscanf(m_FilePointer, "\tA:\t%d B:\t%d C:\t%d", &(pObject->pFaces[index].vertIndex[0]), 
													&(pObject->pFaces[index].vertIndex[1]), 
													&(pObject->pFaces[index].vertIndex[2])); 
}


///////////////////////////////// READ TEXTURE FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the indices to the texture coordinate array
/////
///////////////////////////////// READ TEXTURE FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadASE::ReadTextureFace(t3DObject *pObject)
{
	int index = 0;

	// Read past the index for this texture coordinate
	fscanf(m_FilePointer, "%d:", &index);

	// Now we read in the UV coordinate index for the current face.
	// This will be an index into pTexCoords[] for each point in the face.
	fscanf(m_FilePointer, "%d %d %d", &pObject->pFaces[index].coordIndex[0], 
									  &pObject->pFaces[index].coordIndex[1], 
									  &pObject->pFaces[index].coordIndex[2]);
}

  
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
	double Magnitude;							// This holds the magnitude			

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

void CLoadASE::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// If there are no objects, we can skip this part
	if(pModel->numOfObjects <= 0)
		return;

	// What are vertex normals?  And how are they different from other normals?
	// Well, if you find the normal to a triangle, you are finding a "Face Normal".
	// If you give OpenGL a face normal for lighting, it will make your object look
	// really flat and not very round.  If we find the normal for each vertex, it makes
	// the smooth lighting look.  This also covers up blocky looking objects and they appear
	// to have more polygons than they do.    Basically, what you do is first
	// calculate the face normals, then you take the average of all the normals around each
	// vertex.  It's just averaging.  That way you get a better approximation for that vertex.

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
// This tutorial shows how to load a .Ase file.  An ASE file is a 3D Studio Max
// ASCII file that saves all the information in a text file instead of in binary.
// The good thing about this is that it's easy to read in, as well as easy
// to change without having the software.  The bad thing is that it's a HUGE
// file size, it takes a lot longer to read in, no other programs support it
// and it can't even be imported back into 3D Studio Max.  I would recommend
// looking at this tutorial before you look at the 3DS tutorial though.  That
// way you can see what is actually being read in.
//
// Let me mention that the math functions above are not so important and don't
// need to be in this file.  I wanted to show how to calculate vertex normals so your
// models would have some excellent lighting.  You most likely have functions already
// that do the basics so I encourage you to just use those.  This will cut down the code
// significantly.
// 
// * What's An STL (Standard Template Library) Vector? *
// Let me quickly explain the STL vector for those of you who are not familiar with them.
// To use a vector you must include <vector> and use the std namespace: using namespace std;
// A vector is an array based link list.  It allows you to dynamically add and remove nodes.
// This is a template class so it can be a list of ANY type.  To create a vector of type
// "int" you would say:  vector<int> myIntList;
// Now you can add a integer to the dynamic array by saying: myIntList.push_back(10);
// or you can say:  myIntList.push_back(num);.  The more you push back, the larger
// your array gets.  You can index the vector like an array.  myIntList[0] = 0;
// To get rid of a node you use the pop_back() function.  To clear the vector use clear().
// It frees itself so you don't need to worry about it, except if you have data
// structures that need information freed from inside them, like our objects.
//
// Let's go over some of the format of the .Ase file.  I will only address the
// information that we really care about for object loading.  The rest is Scene stuff.
// I created a single box around the origin that had a texture map over it to show
// the simplest and smallest amount of data.
// 
// This next block is the material block.  This holds all the info on the materials.
//
//	*MATERIAL_LIST {			// This is the tag for the start of the material info
//	*MATERIAL_COUNT 1			// This tells us how many materials there are
//	*MATERIAL 0 {				// This is the material ID that the next data is refering too.
//		*MATERIAL_NAME "Material #1"	// The material name
//		*MATERIAL_CLASS "Standard"
//		*MATERIAL_AMBIENT 0.1791	0.0654	0.0654
//		*MATERIAL_DIFFUSE 0.5373	0.1961	0.1961	// The color of the object
//		*MATERIAL_SPECULAR 0.9000	0.9000	0.9000
//		*MATERIAL_SHINE 0.2500
//		*MATERIAL_SHINESTRENGTH 0.0500
//		*MATERIAL_TRANSPARENCY 0.0000
//		*MATERIAL_WIRESIZE 1.0000
//		*MATERIAL_SHADING Blinn
//		*MATERIAL_XP_FALLOFF 0.0000
//		*MATERIAL_SELFILLUM 0.0000
//		*MATERIAL_FALLOFF In
//		*MATERIAL_XP_TYPE Filter
//		*MAP_DIFFUSE {
//			*MAP_NAME "Map #1"
//			*MAP_CLASS "Bitmap"
//			*MAP_SUBNO 1
//			*MAP_AMOUNT 1.0000
//			*BITMAP "texture.bmp"		// This is the file name for the texture
//			*MAP_TYPE Screen
//			*UVW_U_OFFSET 0.0000		// This is the U offset for the tile
//			*UVW_V_OFFSET 0.0000		// This is the V offset for the tile
//			*UVW_U_TILING 1.0000		// This is the U tiling ratio (1 is standard)
//			*UVW_V_TILING 1.0000		// This is the V tiling ratio (1 is standard)
//			*UVW_ANGLE 0.0000
//			*UVW_BLUR 1.0000
//			*UVW_BLUR_OFFSET 0.0000
//			*UVW_NOUSE_AMT 1.0000
//			*UVW_NOISE_SIZE 1.0000
//			*UVW_NOISE_LEVEL 1
//			*UVW_NOISE_PHASE 0.0000
//			*BITMAP_FILTER Pyramidal
//		}
//    }
// }
//
//
//
//*GEOMOBJECT {							// This tag let's us know that objects are next
//	*NODE_NAME "Box01"					// This is the object name
//	*NODE_TM {
//		*NODE_NAME "Box01"				// Once again, this is the objects name
//		*INHERIT_POS 0 0 0
//		*INHERIT_ROT 0 0 0
//		*INHERIT_SCL 0 0 0
//		*TM_ROW0 1.0000	0.0000	0.0000
//		*TM_ROW1 0.0000	1.0000	0.0000
//		*TM_ROW2 0.0000	0.0000	1.0000
//		*TM_ROW3 0.0000	0.0000	0.0000
//		*TM_POS 0.0000	0.0000	0.0000
//		*TM_ROTAXIS 0.0000	0.0000	0.0000
//		*TM_ROTANGLE 0.0000
//		*TM_SCALE 1.0000	1.0000	1.0000
//		*TM_SCALEAXIS 0.0000	0.0000	0.0000
//		*TM_SCALEAXISANG 0.0000
//	}
//	*MESH {								// This tells us an objects data is next
//		*TIMEVALUE 0					
//		*MESH_NUMVERTEX 8				// This holds the number of vertices for this object
//		*MESH_NUMFACES 12				// This is the number of faces in this object
//		*MESH_VERTEX_LIST {				// The tag for the start of the vertex list
//			*MESH_VERTEX    0	-1.5000	-1.5000	0.0000	// These next 8 are the vertices.
//			*MESH_VERTEX    1	1.5000	-1.5000	0.0000	// The first number is the index
//			*MESH_VERTEX    2	-1.5000	1.5000	0.0000  // and the next 3 are the (X, Y, Z)
//			*MESH_VERTEX    3	1.5000	1.5000	0.0000  // ...
//			*MESH_VERTEX    4	-1.5000	-1.5000	3.0000
//			*MESH_VERTEX    5	1.5000	-1.5000	3.0000
//			*MESH_VERTEX    6	-1.5000	1.5000	3.0000
//			*MESH_VERTEX    7	1.5000	1.5000	3.0000
//		}
//
//		// Below is the tag for the face list.  We have a list of vertices and texture coordinates.
//		// Each of these vertices and coordinates are unique.  This cuts down memory.
//      // We then have a structure that holds index's into the vertex and UV array.
//		// After *MESH_FACE it has the index, then the A B and C values.  These values
//		// are for point1 point2 and point3 indices into the vertex array. The rest can
//      // be ignored.  They have to do with visibility issues.
//
//		*MESH_FACE_LIST {				
//			*MESH_FACE    0:    A:    0 B:    2 C:    3 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 2 	*MESH_MTLID 1
//			*MESH_FACE    1:    A:    3 B:    1 C:    0 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 2 	*MESH_MTLID 1
//			*MESH_FACE    2:    A:    4 B:    5 C:    7 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 3 	*MESH_MTLID 0
//			*MESH_FACE    3:    A:    7 B:    6 C:    4 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 3 	*MESH_MTLID 0
//			*MESH_FACE    4:    A:    0 B:    1 C:    5 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 4 	*MESH_MTLID 4
//			*MESH_FACE    5:    A:    5 B:    4 C:    0 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 4 	*MESH_MTLID 4
//			*MESH_FACE    6:    A:    1 B:    3 C:    7 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 5 	*MESH_MTLID 3
//			*MESH_FACE    7:    A:    7 B:    5 C:    1 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 5 	*MESH_MTLID 3
//			*MESH_FACE    8:    A:    3 B:    2 C:    6 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 6 	*MESH_MTLID 5
//			*MESH_FACE    9:    A:    6 B:    7 C:    3 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 6 	*MESH_MTLID 5
//			*MESH_FACE   10:    A:    2 B:    0 C:    4 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 7 	*MESH_MTLID 2
//			*MESH_FACE   11:    A:    4 B:    6 C:    2 AB:    1 BC:    1 CA:    0	 *MESH_SMOOTHING 7 	*MESH_MTLID 2
//		}
//		*MESH_NUMTVERTEX 36					// This is the number of texture coordinates for this object
//
//		// The next block is the texture coordinates.  The first value after *MESH_TVERT
//      // is the index and the next 2 values are the ones we care about.  The third is ignored.
//      // The 2 we care about are the U and V values for the vertices.
//
//		*MESH_TVERTLIST {					// The tag to let us know there are UV coordiantes
//			*MESH_TVERT 0	0.9995	0.0005	0.0005 // Format: Index   U  V   Ignore
//			*MESH_TVERT 1	0.0005	0.0005	0.9995
//			*MESH_TVERT 2	0.0005	0.0005	0.0005
//			*MESH_TVERT 3	0.0005	0.0005	0.9995
//			*MESH_TVERT 4	0.9995	0.9995	0.0005
//			*MESH_TVERT 5	0.0005	0.9995	0.9995
//			*MESH_TVERT 6	0.0005	0.9995	0.0005
//			*MESH_TVERT 7	0.0005	0.9995	0.9995
//			*MESH_TVERT 8	0.9995	0.0005	0.0005
//			*MESH_TVERT 9	0.9995	0.9995	0.0005
//			*MESH_TVERT 10	0.0005	0.9995	0.0005
//			*MESH_TVERT 11	0.0005	0.9995	0.0005
//			*MESH_TVERT 12	0.0005	0.0005	0.0005
//			*MESH_TVERT 13	0.9995	0.0005	0.0005
//			*MESH_TVERT 14	0.0005	0.0005	0.9995
//			*MESH_TVERT 15	0.9995	0.0005	0.9995
//			*MESH_TVERT 16	0.9995	0.9995	0.9995
//			*MESH_TVERT 17	0.9995	0.9995	0.9995
//			*MESH_TVERT 18	0.0005	0.9995	0.9995
//			*MESH_TVERT 19	0.0005	0.0005	0.9995
//			*MESH_TVERT 20	0.0005	0.0005	0.0005
//			*MESH_TVERT 21	0.9995	0.0005	0.0005
//			*MESH_TVERT 22	0.9995	0.9995	0.0005
//			*MESH_TVERT 23	0.9995	0.9995	0.0005
//			*MESH_TVERT 24	0.0005	0.9995	0.0005
//			*MESH_TVERT 25	0.0005	0.0005	0.0005
//			*MESH_TVERT 26	0.0005	0.0005	0.9995
//			*MESH_TVERT 27	0.9995	0.0005	0.9995
//			*MESH_TVERT 28	0.9995	0.9995	0.9995
//			*MESH_TVERT 29	0.9995	0.9995	0.9995
//			*MESH_TVERT 30	0.0005	0.0005	0.9995
//			*MESH_TVERT 31	0.9995	0.0005	0.9995
//			*MESH_TVERT 32	0.9995	0.9995	0.9995
//			*MESH_TVERT 33	0.9995	0.9995	0.9995
//			*MESH_TVERT 34	0.0005	0.0005	0.0005
//			*MESH_TVERT 35	0.9995	0.9995	0.0005
//		}
//
//		// This next block is the indices for the faces into the texture coordinate array.
//      // Just like the vertex indices with the A B and C values, these do work the same.
//		// We use the 3 values to index the UV coordinates in the texture coordinate array.
//		// The format after the *MESH_TFACE is:  The face index, then the 3 indices into the array.
//      // For example, the top line below has a 0 for the index (not read in)
//      // the next is an 8, which will be used to index the 8th texture coordinate in the
//      // array.  Like so:  int coordIndex = pObject->pFaces[j].coordIndex[whichVertex];
//		// If j was zero and whichVertex was zero, coordIndex would be 8.  Get it?
//
//		*MESH_NUMTVFACES 12				// This is the number of textured faces
//		*MESH_TFACELIST {				// This tag tells us the UV indices are next
//			*MESH_TFACE 0	8	9	10	// Format: faceIndex UVIndex1  UVIndex2  UVIndex3
//			*MESH_TFACE 1	11	12	13  // ...
//			*MESH_TFACE 2	14	15	16
//			*MESH_TFACE 3	17	18	19
//			*MESH_TFACE 4	20	21	22
//			*MESH_TFACE 5	23	24	25
//			*MESH_TFACE 6	26	27	28
//			*MESH_TFACE 7	29	5	1
//			*MESH_TFACE 8	30	31	32
//			*MESH_TFACE 9	33	7	3
//			*MESH_TFACE 10	34	0	35
//			*MESH_TFACE 11	4	6	2
//		}
//	}
//	
// So that's the main stuff.  There is more things like animation and normals but
// I prefer to calculate my own normals, because not all formats have vertex normals
// and animation is a huge topic not to be discussed in a file format tutorial :)
// For those of you who don't know what vertex normals are, it means you have a normal
// for not just every polygon, but for every vertex.  That makes lighting more real looking.
// Also it makes the models look more smooth.
//
// *Note* - Remember after saving your .Ase file, to delete the path in front
// of the texture name.  You want: "texture.bmp" verses "C:\MyStuff\Pictures\texture.bmp"
// That way you can just stick the bitmap in the same folder as the program and not be
// dependant on paths.
//
// I hope this helped someone out.  Let us know at GameTutorials.com what you do with
// this.  We are always interested.  Eventually there will probably be an animation
// tutorial for this format so stay in touch.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
//

