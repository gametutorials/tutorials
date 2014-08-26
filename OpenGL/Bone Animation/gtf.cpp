//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		GT Loader												  //
//																				  //
//		$Description:	Loads the GT Skeletal Animation format .GTF				  //
//																				  //
//********************************************************************************//

#include "main.h"
#include "gtf.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this file we fill in the functions for our loader and model class.
// There is nothing complicated in this file.  We just load the data from
// the file and go through all the objects and render their vertices.
// In the next tutorial we will handle the materials and UV coordinates.


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

	// The model was loaded correctly so return true
	return true;
}


///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This recursively draws all the character nodes, starting with the legs.
/////
///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::DrawModel()
{
	// Right now we don't have any updating or animation to be performed,
	// so all we do when drawing the model currently is just rendering.

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
	// This is the main function that actual draws the model to the screen.
	// We just go through all the objects and draw each one of their faces.

	// Go through all of the objects stored in this model
	for(int i = 0; i < m_Model.numOfObjects; i++)
	{
		// Get the current object that we are displaying
		t3DObject *pObject = &m_Model.pObject[i];

		// We can turn of texturing since there is no texture coordinates
		glDisable(GL_TEXTURE_2D);
	
		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{	
			// Start drawing our model triangles
			glBegin(GL_TRIANGLES);

				// Go through each vertex of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
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
}


///////////////////////////////// IMPORT GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is called by the client to open the .GTF file, read it, then clean up
/////
///////////////////////////////// IMPORT GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadGTF::ImportGTF(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// In this function we load the header for our GTF file.  The file format
	// is written in a way that we first had a 4 character "key" to make sure
	// that the file is a true GTF file.  Then we read in the version of the
	// exporter that created it.  If everything checks out, we can then move
	// on to load the face data.

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
	// Now we finally get to the main code that loads in the GTF data.
	// We first read how many objects there are, then loop through and
	// read in all the vertex and indice information.

	// Read in the number of objects for this scene
	int numObjects = 0;
	fread(&numObjects, sizeof(int), 1, m_FilePointer);
	
	// Go through all of the objects in the scene
	for(int i = 0; i < numObjects; i++)
	{
		// Create a structure to hold the size of the data lists
		tMeshInfo meshInfo = {0};
		
		// Read the number of vertices, allocate memory and read them from the file.
		fread(&meshInfo.numVertices, sizeof(int), 1, m_FilePointer);
		m_pVertices  = new CVector3 [meshInfo.numVertices];
		fread(m_pVertices, sizeof(CVector3), meshInfo.numVertices, m_FilePointer);

		// Read the number of indices, allocate memory and read them from the file.
		fread(&meshInfo.numIndices, sizeof(int), 1, m_FilePointer);
		m_pIndices = new int [meshInfo.numIndices];
		fread(m_pIndices, sizeof(int), meshInfo.numIndices, m_FilePointer);

		// Now that we have the data, let's convert it to our model structure
		ConvertDataStructures(pModel, meshInfo);

		// Free all the memory for this mesh since we just converted it to our structures
		delete [] m_pVertices;   
		delete [] m_pIndices; 
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
	currentMesh.numTexVertex = 0;
	currentMesh.numOfFaces   = meshInfo.numIndices/3;

	// Allocate memory for the vertices, texture coordinates and face data.
	// We set the texture coordinates to NULL for now (until the next tutorial).
	currentMesh.pVerts    = new CVector3 [currentMesh.numOfVerts];
	currentMesh.pTexVerts = NULL;
	currentMesh.pFaces    = new tFace [currentMesh.numOfFaces];

	// Go through all of the vertices and assign them over to our structure
	for (i=0; i < currentMesh.numOfVerts; i++)
	{
		currentMesh.pVerts[i].x =  m_pVertices[i].x/1.0f;
		currentMesh.pVerts[i].y =  m_pVertices[i].y/1.0f;
		currentMesh.pVerts[i].z =  m_pVertices[i].z/1.0f;
	}

	// Go through all of the face data and assign it over to OUR structure.
	// Currently we don't support texture coordinates.
	for(i=0; i < currentMesh.numOfFaces; i++)
	{
		// Assign the vertex indices to our face data
		currentMesh.pFaces[i].vertIndex[0] = m_pIndices[i*3];
		currentMesh.pFaces[i].vertIndex[1] = m_pIndices[i*3 + 1];
		currentMesh.pFaces[i].vertIndex[2] = m_pIndices[i*3 + 2];
	}

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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// That's about it, nothing really difficult, but we just read in all the
// vertex data and loop through and render it.  Check out the next tutorial
// that deals with the material and texture coordinate data.
//
//
// © 2000-2006 GameTutorials