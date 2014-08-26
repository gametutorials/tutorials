//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		GT Loader Part 2										  //
//																				  //
//		$Description:	Loads the GT Skeletal Animation format .GTF				  //
//																				  //
//********************************************************************************//

#include "main.h"
#include "gtf.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// Most of the changes appear in this file.  If you scroll down you can find
// all the * NEW * blocks where we added little things here and there.
// The main changes are in the loading and the rendering functions.  Have a look.


///////////////////////////////// CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our CModelGTF constructor
/////
///////////////////////////////// CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CModelGTF::CModelGTF()
{
}


///////////////////////////////// ~CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our CModelGTF deconstructor
/////
///////////////////////////////// ~CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CModelGTF::~CModelGTF()
{
	// Free all the model data
	DestroyModel();
}	


///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This frees our model and all it's associated data
/////
///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::DestroyModel()
{
	// Go through all the objects in the model
	for(int i = 0; i < m_Model.numOfObjects; i++)
	{
		// Free the faces and vertices
		if(m_Model.pObject[i].pFaces)		delete [] m_Model.pObject[i].pFaces;
		if(m_Model.pObject[i].pVerts)		delete [] m_Model.pObject[i].pVerts;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// We need to destroy the texture vertices like normal
		if(m_Model.pObject[i].pTexVerts)	delete [] m_Model.pObject[i].pTexVerts;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	}
}
	

///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads our model from the given path and file name
/////
///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CModelGTF::LoadModel(LPSTR strPath, LPSTR strModel)
{
	// Create a loader class instance
	CLoadGTF loadGTF;

	// Make sure valid model name was passed in
	if(!strModel) return false;

	char szFullPath[MAX_PATH] = {0};

	// If there is a valid path we want to add the file name to it,
	// otherwise just use the file name with no path.
	if(strPath)
		sprintf(szFullPath, "%s\\%s", strPath, strModel);
	else
		strcpy(szFullPath, strModel);

	// Here we load the .GTF file.  We also make sure it loaded correctly.
	if(!loadGTF.ImportGTF(&m_Model, szFullPath))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the model!", "Error", MB_OK);
		return false;
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// After all the data is loaded we can go through and load all the textures.
	LoadModelTextures(strPath);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// The model was loaded correctly so return true
	return true;
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the textures for the model in a specified directory
/////
///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::LoadModelTextures(LPSTR strPath)
{
	// Just like in our previous model loading tutorials we need to not
	// only load the materials but make sure the texture IDs are assigned
	// correctly to the model.  We only want to add unique materials to
	// our list so there needs to be checks to make sure of this.

	// Grab a pointer to the model
	t3DModel *pModel = &m_Model;

	// Go through all the materials that are assigned to this model
	for(int i = 0; i < (int)pModel->pMaterials.size(); i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(pModel->pMaterials[i].strFile) > 0)
		{
			// Create a boolean to tell us if we have a new texture to load
			bool bNewTexture = true;

			// Go through all the textures in our string list to see if it's already loaded
			for(unsigned int j = 0; j < strTextures.size(); j++)
			{
				string strTemp = pModel->pMaterials[i].strFile;
				string strTemp2 = strTextures[j];

				// If the texture name is already in our list of texture, don't load it again.
				if(strstr(pModel->pMaterials[i].strFile, strTextures[j].c_str()) )
				{
					// We don't need to load this texture since it's already loaded
					bNewTexture = false;

					// Assign the texture index to our current material textureID.
					// This ID will then be used as an index into m_Textures[].
					pModel->pMaterials[i].texureId = j;
				}
			}

			// Make sure before going any further that this is a new texture to be loaded
			if(bNewTexture == false) continue;
			
			char strFullPath[255] = {0};

			// Check if there is a valid path
			if(strlen(strPath) > 0)
			{
				// Add the file name and path together so we can load the texture
				sprintf(strFullPath, "%s\\%s", strPath, pModel->pMaterials[i].strFile);
			}
			else
			{
				// If there is no path, don't attach a directory to our file name
				sprintf(strFullPath, "%s", strPath, pModel->pMaterials[i].strFile);
			}

			// We pass in a reference to an index into our texture array member variable.
			// The size() function returns the current loaded texture count.  Initially
			// it will be 0 because we haven't added any texture names to our strTextures list.
			CreateTexture(strFullPath, m_Textures[strTextures.size()]);								

			// Set the texture ID for this material by getting the current loaded texture count
			pModel->pMaterials[i].texureId = (int)strTextures.size();

			// Now we increase the loaded texture count by adding the texture name to our
			// list of texture names.  Remember, this is used so we can check if a texture
			// is already loaded before we load 2 of the same textures.
			strTextures.push_back(pModel->pMaterials[i].strFile);
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This recursively draws all the character nodes, starting with the legs.
/////
///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::DrawModel()
{
	// Render the model and all it's objects
	RenderModel();
}


///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This goes through all the meshes and renders them to the screen
/////
///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::RenderModel()
{
	// Go through all of the objects stored in this model
	for(int i = 0; i < m_Model.numOfObjects; i++)
	{
		// Get the current object that we are displaying
		t3DObject *pObject = &m_Model.pObject[i];


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// In this tutorial we handle the ability to render multiple texture
		// maps on a single object of the model.  This means that every face
		// could possibly have a new texture.  Of course, we want to avoid
		// calling glBindTexture() as much as possible so we first do a check
		// to see if there is material(s) assigned to the model.  If so we need
		// to enable texture mapping.  Then below we save the previous texture
		// ID and only call glBindTexture() if the next texture ID is different
		// than the previous one.
		
		// Check if there are materials assigned to this object and 
		// enable texture mapping if necessary, otherwise turn it off.
		if(pObject->numOfMaterials >= 1)
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
	
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// Now let's see if there are any materials that need binding
			if(pObject->numOfMaterials >= 1)
			{
				// Create a static int to hold the last texture ID
				static int lastTexID = -1;

				// Extract the current textureID for this face from our face data
				int textureID = m_Model.pMaterials[pObject->pFaces[j].textureID].texureId;

				// If the current texture ID isn't the same as last time, bind the texture.
				// We also do a bit of error checking to make sure it is a valid texture ID.
				if(textureID != lastTexID && textureID >= 0 && textureID < (int)m_Model.pMaterials.size())
					glBindTexture(GL_TEXTURE_2D, m_Textures[textureID]);

				// Store the current textureID in our static int for next time
				lastTexID = textureID;
			}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


			// Start drawing our model triangles
			glBegin(GL_TRIANGLES);

				// Go through each vertex of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					
			
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////					

					// Now we need to apply the UV coordinates for this vertex.
					// We just grab the UV index for the right coordinate.

					// Make sure there are texture coordinates for this
					if(pObject->pTexVerts) 
					{
						// Here we grab the texture index from our face information
						int texIndex = pObject->pFaces[j].coordIndex[whichVertex];

						// Assign the texture coordinate to this vertex
						glTexCoord2f(pObject->pTexVerts[ texIndex ].x, 
									 pObject->pTexVerts[ texIndex ].y);
					}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


					// Grab the current vertex index from our face information
					int vertIndex = pObject->pFaces[j].vertIndex[whichVertex];

					// Extract the current vertex and draw it
					CVector3 vPos = pObject->pVerts[vertIndex];					
					glVertex3f(vPos.x, vPos.y, vPos.z);
				}

			// Stop drawing polygons
			glEnd();
		}
	}
}


//////////////////////////  BELOW IS THE LOADER CLASS //////////////////////////////



///////////////////////////////// CLOAD GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This constructor initializes the gtf data
/////
///////////////////////////////// CLOAD GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CLoadGTF::CLoadGTF()
{
	// Set the pointers to null
	m_pVertices=NULL;
	m_pIndices=NULL;
		

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// Just like normal, initialize the lists
	m_pTexCoords=NULL;
	m_pTexIndices=NULL;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


///////////////////////////////// IMPORT GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is called by the client to open the .GTF file, read it, then clean up
/////
///////////////////////////////// IMPORT GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadGTF::ImportGTF(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// Open the GTF file in binary
	m_FilePointer = fopen(strFileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!m_FilePointer) 
	{
		// Display an error message and don't load anything if no file was found
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	char fileID[4] = {0};
	float version = 0;

	// Read in the GTF key and exporter version
	fread(&fileID, sizeof(char), 4, m_FilePointer);
	fread(&version, sizeof(float), 1, m_FilePointer);

	// Get the 4 character ID
	char *ID = fileID;

	// The ID MUST equal "GT3D" and the version MUST be greater than or equal to 1, 
	// or else it isn't a valid .GTF file.

	// Make sure the ID == GT3D and the version greater than or equal t0 1.0
	if((ID[0] != 'G' || ID[1] != 'T' || ID[2] != '3' || ID[3] != 'D') || version < 1.0f)
	{
		// Display an error message for bad file format, then stop loading
		sprintf(strMessage, "Invalid file format: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// Read in the model and animation data
	ReadGTFData(pModel);

	// Clean up after everything
	CleanUp();

	// Return a success
	return true;
}


///////////////////////////////// READ GTF DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in all of the model's data, except the animation frames
/////
///////////////////////////////// READ GTF DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadGTF::ReadGTFData(t3DModel *pModel)
{
	// Read in the number of objects for this scene
	int numObjects = 0;
	fread(&numObjects, sizeof(int), 1, m_FilePointer);
	
	// Go through all of the objects in the scene
	for(int i = 0; i < numObjects; i++)
	{
		// Create a structure to hold the size of the data lists
		tMeshInfo meshInfo = {0};


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// In our loading code we first want to load in all of the materials.
		// First we load the number of materials in the file.
		fread(&meshInfo.numMaterials, sizeof(int), 1, m_FilePointer);
		
		// Go through all of our materials and load them
		for(int m = 0; m < meshInfo.numMaterials; m++)
		{
			// Load in only MAX_FILE_NAME characters for the texture name
			char szMaterial[MAX_FILE_NAME] = {0};
			fread(szMaterial, sizeof(char), MAX_FILE_NAME, m_FilePointer);

			// Create a material structure instance and store the material info
			tMaterialInfo newMaterial;
			strcpy(newMaterial.strFile, szMaterial);
			newMaterial.texureId = (int)pModel->pMaterials.size();

			// Add a new material
			pModel->pMaterials.push_back(newMaterial);
		}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		
		// Read the number of vertices, allocate memory and read them from the file.
		fread(&meshInfo.numVertices, sizeof(int), 1, m_FilePointer);
		m_pVertices  = new CVector3 [meshInfo.numVertices];
		fread(m_pVertices, sizeof(CVector3), meshInfo.numVertices, m_FilePointer);

		// Read the number of indices, allocate memory and read them from the file.
		fread(&meshInfo.numIndices, sizeof(int), 1, m_FilePointer);
		m_pIndices = new int [meshInfo.numIndices];
		fread(m_pIndices, sizeof(int), meshInfo.numIndices, m_FilePointer);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// Read the number of UV coordinates, allocate memory and read them from the file.
		fread(&meshInfo.numTexCoords, sizeof(int), 1, m_FilePointer);
		m_pTexCoords = new CVector3 [meshInfo.numTexCoords];
		fread(m_pTexCoords, sizeof(CVector3), meshInfo.numTexCoords, m_FilePointer);

		// Read the number of UV indices, allocate memory and read them from the file.
		fread(&meshInfo.numTexIndices, sizeof(int), 1, m_FilePointer);
		m_pTexIndices = new int [meshInfo.numTexIndices];
		fread(m_pTexIndices, sizeof(int), meshInfo.numTexIndices, m_FilePointer);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Now that we have the data, let's convert it to our model structure
		ConvertDataStructures(pModel, meshInfo);

		// Free all the memory for this mesh since we just converted it to our structures
		delete [] m_pVertices;   
		delete [] m_pIndices; 


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// We don't need these arrays anymore so let's free them
		delete [] m_pTexCoords;
		delete [] m_pTexIndices; 

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	}
}


///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function converts the .gtf structures to our own model and object structures
/////
///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadGTF::ConvertDataStructures(t3DModel *pModel, tMeshInfo meshInfo)
{
	int i = 0;

	// Increase the number of objects (sub-objects) in our model since we are loading a new one
	pModel->numOfObjects++;
		
	// Create a empty object structure to store the object's info before we add it to our list
	t3DObject currentMesh = {0};

	// Assign the vertex, texture coord and face count to our new structure.
	// Currently we have no UV coordinates so we set the size to 0.
	currentMesh.numOfVerts   = meshInfo.numVertices;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Store the number of texture coordinates
	currentMesh.numTexVertex = meshInfo.numTexCoords;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	currentMesh.numOfFaces   = meshInfo.numIndices/3;

	// Allocate memory for the vertices, texture coordinates and face data.
	// We set the texture coordinates to NULL for now (until the next tutorial).
	currentMesh.pVerts    = new CVector3 [currentMesh.numOfVerts];
	

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// Allocate memory for the texture vertices
	currentMesh.pTexVerts = new CVector2 [currentMesh.numTexVertex];

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	currentMesh.pFaces    = new tFace [currentMesh.numOfFaces];

	// Go through all of the vertices and assign them over to our structure
	for (i=0; i < currentMesh.numOfVerts; i++)
	{
		currentMesh.pVerts[i].x =  m_pVertices[i].x/1.0f;
		currentMesh.pVerts[i].y =  m_pVertices[i].y/1.0f;
		currentMesh.pVerts[i].z =  m_pVertices[i].z/1.0f;
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Go through all of the UV coords and assign them over to our structure
	for (i=0; i < currentMesh.numTexVertex; i++)
	{
		currentMesh.pTexVerts[i].x = m_pTexCoords[i].x;
		currentMesh.pTexVerts[i].y = m_pTexCoords[i].y;
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Go through all of the face data and assign it over to OUR structure.
	// Currently we don't support texture coordinates.
	for(i=0; i < currentMesh.numOfFaces; i++)
	{
		// Assign the vertex indices to our face data
		currentMesh.pFaces[i].vertIndex[0] = m_pIndices[i*3];
		currentMesh.pFaces[i].vertIndex[1] = m_pIndices[i*3 + 1];
		currentMesh.pFaces[i].vertIndex[2] = m_pIndices[i*3 + 2];

		
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Assign the texture coord indices to our face data (same as the vertex indices)
		currentMesh.pFaces[i].coordIndex[0] = m_pTexIndices[i*3];
		currentMesh.pFaces[i].coordIndex[1] = m_pTexIndices[i*3 + 1];
		currentMesh.pFaces[i].coordIndex[2] = m_pTexIndices[i*3 + 2];

		// Set the texture ID for this face (smooth groups).  Remember that
		// in the exporter we stored the texture ID in the .z value of a CVector3.
		// This is because each face can possibly have a different texture.
		currentMesh.pFaces[i].textureID = (int)m_pTexCoords[currentMesh.pFaces[i].coordIndex[0]].z;
	}

	// Store the number of materials for this mesh
	currentMesh.numOfMaterials = meshInfo.numMaterials;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Here we add the current object to our list object list
	pModel->pObject.push_back(currentMesh);
}


///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadGTF::CleanUp()
{
	// Close the current file pointer
	fclose(m_FilePointer);						
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// If you scroll up you can see all the little additions we added to our file.
// Nothing really special, just the basics for adding texture maps to our model.
// Though there wasn't a large amount of additions, I wanted to make sure all of
// the basics were added before we came to the animation tutorial, since there is
// a lot of information we need to cover.  That is the next tutorial.  Get your
// math hat on and check it out :)
//
//
// © 2000-2006 GameTutorials