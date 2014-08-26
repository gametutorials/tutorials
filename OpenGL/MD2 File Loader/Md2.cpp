//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		MD2 Loader										 //
//																		 //
//		$Description:	Demonstrates how to load a Quake2 MD2 Model		 //
//																		 //
//***********************************************************************//

#include "main.h"
#include "Md2.h"


/////////////////////////////////////////////////////////////////////////
//
// This file holds the code to load the Quake2 models from a .MD2 format.
// The .MD2 file is usually stored in a .zip file (don't let the extension
// fool you, just rename it to .zip), depending on where you get the models
// from.  The CLoadMD2 class handles the loading, but we draw the model
// externally on our own in main.cpp.  I created a converter function
// to convert to our already used model and object structures.  This way
// eventually we can create a model library that can load any type of
// model that we support, as well as use inheritance to create a new class
// for each file format for the small things that each model format needs differently.
// Like the other loading tutorials, we calculate our own vertex normals.
// The .Md2 format is REALLY simple to load.  That is why I chose it.  The
// next tutorial will show how to load and animate MD2 files.  Next, we
// will move from key frame animation to skeletal animation with the Quake3
// .MD3 files.  This is also a wonderfully easy format to load and use.
//
//


///////////////////////////////// CLOAD MD2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This constructor initializes the md2 structures
/////
///////////////////////////////// CLOAD MD2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CLoadMD2::CLoadMD2()
{
	// Here we initialize our structures to 0
	memset(&m_Header, 0, sizeof(tMd2Header));

	// Set the pointers to null
	m_pSkins=NULL;
	m_pTexCoords=NULL;
	m_pTriangles=NULL;
	m_pFrames=NULL;
}


///////////////////////////////// IMPORT MD2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is called by the client to open the .Md2 file, read it, then clean up
/////
///////////////////////////////// IMPORT MD2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD2::ImportMD2(t3DModel *pModel, char *strFileName, char *strTexture)
{
	char strMessage[255] = {0};

	// Open the MD2 file in binary
	m_FilePointer = fopen(strFileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!m_FilePointer) 
	{
		// Display an error message and don't load anything if no file was found
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// Just like most file formats, there is a header that needs to be read
	// from the .MD2 format.  If you look at the tMd2Header structure you will
	// find all the data that will be read in.  It's nice to know up front about
	// the data that we will be reading.  This makes it easy to just to large
	// binary reads using fread, instead of counting and reading chunks.

	// Read the header data and store it in our m_Header member variable
	fread(&m_Header, 1, sizeof(tMd2Header), m_FilePointer);

	// For some reason, .Md2 files MUST have a version of 8.  I am not sure why,
	// but if it doesn't there is something wrong and the header was read in
	// incorrectly, or perhaps the file format is bad.
	if(m_Header.version != 8)
	{
		// Display an error message for bad file format, then stop loading
		sprintf(strMessage, "Invalid file format (Version not 8): %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Now that we made sure the header had correct data, we want to read in the
	// rest of the data.  Once the data is read in, we need to convert it to our structures.
	// Since we are only reading in the first frame of animation, there will only
	// be ONE object in our t3DObject structure, held within our pModel variable.
	ReadMD2Data();
	
	// Here we pass in our model structure to it can store the read Quake data
	// in our own model and object structure data.
	ConvertDataStructures(pModel);

	// After we have read the whole MD2 file, we want to calculate our own vertex normals.
	ComputeNormals(pModel);

	// If there is a valid texture name passed in, we want to set the texture data.
	if(strTexture)
	{
		// Create a local material info structure
		tMaterialInfo texture;

		// Copy the name of the file into our texture file name variable
		strcpy(texture.strFile, strTexture);

		// Since there is only one texture for a .MD2 file, the ID is always 0
		texture.texureId = 0;

		// The tile or scale for the UV's is 1 to 1 (but Quake saves off a 0-256 ratio)
		texture.uTile = texture.uTile = 1;

		// We only have 1 material for a model
		pModel->numOfMaterials = 1;

		// Add the local material info to our model's material list
		pModel->pMaterials.push_back(texture);
	}

	// Clean up after everything
	CleanUp();

	// Return a success
	return true;
}


///////////////////////////////// READ MD2 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in all of the model's data, except the animation frames
/////
///////////////////////////////// READ MD2 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD2::ReadMD2Data()
{
	// Create a larger buffer for the frames of animation (not fully used yet)
	unsigned char buffer[MD2_MAX_FRAMESIZE];
	int j = 0;

	// Here we allocate all of our memory from the header's information
	m_pSkins     = new tMd2Skin [m_Header.numSkins];
	m_pTexCoords = new tMd2TexCoord [m_Header.numTexCoords];
	m_pTriangles = new tMd2Face [m_Header.numTriangles];
	m_pFrames    = new tMd2Frame [m_Header.numFrames];

	// Next, we start reading in the data by seeking to our skin names offset
	fseek(m_FilePointer, m_Header.offsetSkins, SEEK_SET);
	
	// Depending on the skin count, we read in each skin for this model.
	fread(m_pSkins, sizeof(tMd2Skin), m_Header.numSkins, m_FilePointer);
	
	// Move the file pointer to the position in the file for texture coordinates
	fseek(m_FilePointer, m_Header.offsetTexCoords, SEEK_SET);
	
	// Read in all the texture coordinates in one fell swoop
	fread(m_pTexCoords, sizeof(tMd2TexCoord), m_Header.numTexCoords, m_FilePointer);

	// Move the file pointer to the triangles/face data offset
	fseek(m_FilePointer, m_Header.offsetTriangles, SEEK_SET);
	
	// Read in the face data for each triangle (vertex and texCoord indices)
	fread(m_pTriangles, sizeof(tMd2Face), m_Header.numTriangles, m_FilePointer);
			
	// Move the file pointer to the vertices (frames)
	fseek(m_FilePointer, m_Header.offsetFrames, SEEK_SET);

	// Assign our alias frame to our buffer memory
	tMd2AliasFrame *pFrame = (tMd2AliasFrame *) buffer;

	// Allocate the memory for the first frame of animation's vertices
	m_pFrames[0].pVertices = new tMd2Triangle [m_Header.numVertices];

	// Read in the first frame of animation
	fread(pFrame, 1, m_Header.frameSize, m_FilePointer);

	// Copy the name of the animation to our frames array
	strcpy(m_pFrames[0].strName, pFrame->name);
			
	// After we have read in the data for the model, since there is animation,
	// This means that there are scale and translation values to be dealt with.
	// To apply the scale and translation values, we simply multiply the scale (x, y, z)
	// by the current vertex (x, y, z).  Also notice that we switch the Y and Z values
	// so that Y is faces up, NOT Z.
	
	// Store off a vertex array pointer to cut down large lines of code
	tMd2Triangle *pVertices = m_pFrames[0].pVertices;

	// Go through all of the number of vertices and assign the scale and translations.
	// Store the vertices in our current frame's vertex list array, while swapping Y and Z.
	// Notice we also negate the Z axis as well to make the swap correctly.
	for (j=0; j < m_Header.numVertices; j++)
	{
		pVertices[j].vertex[0] = pFrame->aliasVertices[j].vertex[0] * pFrame->scale[0] + pFrame->translate[0];
		pVertices[j].vertex[2] = -1 * (pFrame->aliasVertices[j].vertex[1] * pFrame->scale[1] + pFrame->translate[1]);
		pVertices[j].vertex[1] = pFrame->aliasVertices[j].vertex[2] * pFrame->scale[2] + pFrame->translate[2];
	}
}


///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function converts the .md2 structures to our own model and object structures
/////
///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD2::ConvertDataStructures(t3DModel *pModel)
{
	int j = 0, i = 0;
	
	// Assign the number of objects, which is 1 since we only want 1 frame
	// of animation.  In the next tutorial each object will be a key frame
	// to interpolate between.
	pModel->numOfObjects = 1;

	// Create a local object to store the first frame of animation's data
	t3DObject currentFrame = {0};

	// Assign the vertex, texture coord and face count to our new structure
	currentFrame.numOfVerts   = m_Header.numVertices;
	currentFrame.numTexVertex = m_Header.numTexCoords;
	currentFrame.numOfFaces   = m_Header.numTriangles;

	// Allocate memory for the vertices, texture coordinates and face data.
	currentFrame.pVerts    = new CVector3 [currentFrame.numOfVerts];
	currentFrame.pTexVerts = new CVector2 [currentFrame.numTexVertex];
	currentFrame.pFaces    = new tFace [currentFrame.numOfFaces];

	// Go through all of the vertices and assign them over to our structure
	for (j=0; j < currentFrame.numOfVerts; j++)
	{
		currentFrame.pVerts[j].x = m_pFrames[0].pVertices[j].vertex[0];
		currentFrame.pVerts[j].y = m_pFrames[0].pVertices[j].vertex[1];
		currentFrame.pVerts[j].z = m_pFrames[0].pVertices[j].vertex[2];
	}

	// We can now free the old vertices stored in this frame of animation
	delete m_pFrames[0].pVertices;

	// Go through all of the uv coordinates and assign them over to our structure.
	// The UV coordinates are not normal UV coordinates, they have a pixel ratio of
	// 0 to 256.  We want it to be a 0 to 1 ratio, so we divide the u value by the
	// skin width and the v value by the skin height.  This gives us our 0 to 1 ratio.
	// For some reason also, the v coordinate is flipped upside down.  We just subtract
	// the v coordinate from 1 to remedy this problem.
	for (j=0; j < currentFrame.numTexVertex; j++)
	{
		currentFrame.pTexVerts[j].x = m_pTexCoords[j].u / float(m_Header.skinWidth);
		currentFrame.pTexVerts[j].y = 1 - m_pTexCoords[j].v / float(m_Header.skinHeight);
	}

	// Go through all of the face data and assign it over to OUR structure
	for(j=0; j < currentFrame.numOfFaces; j++)
	{
		// Assign the vertex indices to our face data
		currentFrame.pFaces[j].vertIndex[0] = m_pTriangles[j].vertexIndices[0];
		currentFrame.pFaces[j].vertIndex[1] = m_pTriangles[j].vertexIndices[1];
		currentFrame.pFaces[j].vertIndex[2] = m_pTriangles[j].vertexIndices[2];

		// Assign the texture coord indices to our face data
		currentFrame.pFaces[j].coordIndex[0] = m_pTriangles[j].textureIndices[0];
		currentFrame.pFaces[j].coordIndex[1] = m_pTriangles[j].textureIndices[1];
		currentFrame.pFaces[j].coordIndex[2] = m_pTriangles[j].textureIndices[2];
	}

	// Here we add the current object (or frame) to our list object list
	pModel->pObject.push_back(currentFrame);
}


///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD2::CleanUp()
{
	// This is just the regular cleanup of our md2 model class.  We can free
	// all of this data because we already have it stored in our own structures.

	fclose(m_FilePointer);						// Close the current file pointer

	if(m_pSkins)	 delete [] m_pSkins;		// Free the skins data
	if(m_pTexCoords) delete m_pTexCoords;		// Free the texture coord data
	if(m_pTriangles) delete m_pTriangles;		// Free the triangle face data
	if(m_pFrames)	 delete m_pFrames;			// Free the frames of animation
}



// *Note* 
//
// Below are some math functions for calculating vertex normals.  We want vertex normals
// because it makes the lighting look really smooth and life like.  You probably already
// have these functions in the rest of your engine, so you can delete these and call
// your own.  I wanted to add them so I could show how to calculate vertex normals.
// We don't use this in the next tutorial because lighting is usually already calculated
// in the texture map already, but we will do it just in case you want to use it in your
// own engine.

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

///////////////////////////////// COMPUTE NORMALS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function computes the normals and vertex normals of the objects
/////
///////////////////////////////// COMPUTE NORMALS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD2::ComputeNormals(t3DModel *pModel)
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

		for (int i = 0; i < pObject->numOfVerts; i++)		// Go through all of the vertices
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
// Pretty simple huh?  This is probably the easiest 3D file format I have ever
// worked with, so good job Carmack!  Once again, the next MD2 tutorial will cover
// the key frame animation that is associated with these models.  Then you can 
// actually say you have worked with real Quake characters and know how they did
// their animation.  Let's go over a brief explanation of this loader:
//
// The structures MUST be the same size and data types in order to load the
// Quake2 data.  First we load the header information.  This tells us everything
// about the file and it's contents.
// 
// After the header is loaded, we need to check if the ID is 8.  This is a must.
// Don't ask me why it's 8, ask John Carmack!  If the version ID checks out, then
// we can start loading the data.
// 
// For each set of data you want to load is, we use an fseek() to move the file
// pointer to that location in the file that is given in the header.
//
// After you load the data, you can then convert the data structures to your own
// format, that way you don't have to be stuck with theirs.  I decided to make it
// like the other loaders for future purposes.  We also compute our own normals.
//
// There is one thing I didn't mention that was NOT loaded in.  There is an array
// of OpenGL commands that allow you to render the vertices in triangle strips and
// a triangle fan.  This is the ugliest code I have ever seen to implement it, so
// I left it out :)
//
// I would like to thank Daniel E. Schoenblum <dansch@hops.cs.jhu.edu> for help
// with explaining the file format.
//
// Let me know if this helps you out!
//
// The Quake2 .MD2 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake and Quake2 are trademarks of id Software.
// All trademarks used are properties of their respective owners. 
//
// ©2000-2005 GameTutorials