//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Exporter Part 2								 //
//																		 //
//		$Description:	3DS Max weighted-animation exporter	series		 //
//																		 //
//***********************************************************************//

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This tutorial is the second part of our 3D Studio Max® tutorial series.
// In this version we will handle exporting the materials (textures) for
// each model.  The exporter handles multiple textures per object.  This
// is necessary because many characters and worlds have many textures per
// object.  The only downside about using multiple textures per object is
// the slowdown it takes to have to change the texture state possibly every
// few faces.  This is where vertex buffers and display lists become helpful.
//
// * Additions *
//
// We didn't add much code to this version, but enough to split up into
// a different tutorial.  Basically, what we do is store a material
// ID for every face, which enables us to apply many textures to a single
// mesh.  We then have the same process for getting vertices as we do for
// texture coordinates.  We want to only have unique UV coordinates, so
// we have to store a list of texture coordinates and an array of indices.
//
// Like the vertices, we write the UV coordinates to our file, as well as
// writing the texture-map names.  Some meshes just have 1 texture so it's
// easy, but if they have multiple textures then you have to do a little
// more work to get all of the sub-material names.  I will explain this
// step-by-step in the new functions below.
//
// This file is the only file that really has changed.  The header file
// just includes the new functions into the Exporter class.
//
// * New .GTF file format *
//
// Below is the new file format for .GTF:
//
//
// * "GT3D" (4 characters)
// * Exporter Version (1 float)
// * Number of Objects/Meshes (1 int)
// *	Number of Materials (1 int)
// *	Material Name ( Number of Materials * char[MAX_FILE_NAME])
// *	Number of Vertices (1 int)
// *	Vertices ( Number of Vertices * CVector3)
// *	Number of Indices (1 int)
// *	Indices ( Number of Indices * int)
// *	Number of UV Coordinates (1 int)
// *	UV Coords ( Number of UV Coords * CVector3)
// *	Number of Texture Indices (1 int)
// *	Texture Indices ( Number of Texture Indices * int)
//
// The next tutorial is the most complicated as it handles exporting the
// bone animation.  Let's hurry and get through this tutorial so we can
// get to the advanced and amazing stuff :)
//
//

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


#include "Exporter.h"


///////////////////////////////// Exporter \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This initializes all of our exporter data
/////
///////////////////////////////// Exporter \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

Exporter::Exporter()
{
	m_numMeshes = 0;
	m_lastTexture = 0;		// (* NEW *)
	m_fp = NULL;
	bExportSelected = false;
	m_pInterface = NULL;
}


///////////////////////////////// ~Exporter \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will deinitialize any needed data for our exporter class
/////
///////////////////////////////// ~Exporter \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

Exporter::~Exporter() 
{
}

///////////////////////////////// ExtCount \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns how many file extensions that we will use
/////
///////////////////////////////// ExtCount \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int Exporter::ExtCount()
{
	// We just have one file extension to support (.GTF)
	return 1;
}


///////////////////////////////// Ext \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the file extension we will use
/////
///////////////////////////////// Ext \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

const TCHAR *Exporter::Ext(int n)
{		
	// GTF is our own file type we created
	return _T("GTF");
}


///////////////////////////////// LongDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a longer description of our exporter
/////
///////////////////////////////// LongDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

const TCHAR *Exporter::LongDesc()
{
	// We just want to return a simple phrase about our exporter
	return _T("GameTutorials Exporter");
}
	

///////////////////////////////// ShortDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a short explanation of our exporter
/////
///////////////////////////////// ShortDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

const TCHAR *Exporter::ShortDesc() 
{			
	// This is just a small explanation of our exporter
	return _T("GT Exporter");
}


///////////////////////////////// AuthorName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the author's name of the exporter
/////
///////////////////////////////// AuthorName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

const TCHAR *Exporter::AuthorName()
{			
	// Since I wrote this exporter I figure I should take credit for it :)
	return _T("Ben Humphrey");
}


///////////////////////////////// CopyrightMessage \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the message of copyright for our exporter
/////
///////////////////////////////// CopyrightMessage \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

const TCHAR *Exporter::CopyrightMessage() 
{	
	// GT owns the rights to this exporter
	return _T("Copyright GameTutorials 2006");
}


///////////////////////////////// OtherMessage1 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This apparently isn't used, but need in the exporter class
/////
///////////////////////////////// OtherMessage1 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

const TCHAR *Exporter::OtherMessage1() 
{		
	return _T("");
}


///////////////////////////////// OtherMessage2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This apparently isn't used, but need in the exporter class
/////
///////////////////////////////// OtherMessage2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

const TCHAR *Exporter::OtherMessage2() 
{		
	return _T("");
}


///////////////////////////////// Version \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns our version of the exporter
/////
///////////////////////////////// Version \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

unsigned int Exporter::Version()
{				
	// Apparently we want to return the version number in hundreds (1.00)
	return 100;
}


///////////////////////////////// ShowAbout \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is used if we want to show an about message/box for our exporter
/////
///////////////////////////////// ShowAbout \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Exporter::ShowAbout(HWND hWnd)
{			
}


///////////////////////////////// SupportsOptions \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns true if we support certain options for our exporter
/////
///////////////////////////////// SupportsOptions \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

BOOL Exporter::SupportsOptions(int ext, DWORD options)
{
	// We just say that we support all the options for now
	return TRUE;
}


///////////////////////////////// DoExport \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our main starting function for our exporter
/////
///////////////////////////////// DoExport \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int	Exporter::DoExport(const TCHAR *szFile, ExpInterface *pEI, Interface *pInterface, BOOL bSuppressPrompts, DWORD options)
{
	// This tells us if the user is exporting the whole scene or not
	bExportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	// Here we store a pointer to the main interface in our exporter class
	m_pInterface = pInterface;
	
	// This tell us how many children the root node has for traversing
	int numChildren = pInterface->GetRootNode()->NumberOfChildren();

	// Now we open the file that we are exporting to as a binary file
	m_fp = fopen(szFile, "wb");

	// The first thing written to the file is our KEY ("GT3D")
	char cKey[] = GT_KEY;
	fwrite(&cKey, sizeof(char), 4, m_fp);
	
	// The second thing written to the file is the current exporter version
	float exporterVersion = GT_VERSION;
	fwrite(&exporterVersion, sizeof(float), 1, m_fp);

	// Go through each child node and count the meshes
	for(int i=0; i < numChildren; i++)
	{
		// Check if the user cancelled the export yet or not
		if (pInterface->GetCancel())
			break;

		// Get the mesh count
		GetObjectCount(pInterface->GetRootNode()->GetChildNode(i));
	}

	// We should have the number of meshes by now to write to our file
	fwrite(&m_numMeshes, sizeof(int), 1, m_fp);

	// Now we can go through each node and start exporting the data
	for(int i=0; i < numChildren; i++)
	{
		// Check if the user cancelled the export yet or not
		if (pInterface->GetCancel())
			break;

		// Process each child node and export the necessary data
		ProcessNode(pInterface->GetRootNode()->GetChildNode(i));
	}

	// Finally, close the file and we are done!
	fclose(m_fp);

	// Everything thing went well so return true
	return true;
}


///////////////////////////////// ProcessNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This recursively goes through each node gets us to the mesh data
/////
///////////////////////////////// ProcessNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool Exporter::ProcessNode(INode *pNode)
{
	// Check if we are exporting selected data only
	if(bExportSelected && pNode->Selected() == FALSE)
		return false;

	// Get the current object data
	ObjectState objState = pNode->EvalWorldState(0);

	// If there is an object and the .obj pointer isn't NULL
	if (objState.obj) 
	{
		// We do a switch on the CLASS_ID for the object
		switch(objState.obj->SuperClassID()) 
		{
			// If the object is a geometric object, export it!
			case GEOMOBJECT_CLASS_ID:
				ExportGeometry(pNode); 
				break;
		};
	}

	// If there are more children stored in this node, we need to
	// continue to recurse through them and export their data too.
	for(int i = 0; i < pNode->NumberOfChildren(); i++) 
	{
		// Go through and export the child nodes' data as well
		if(!ProcessNode(pNode->GetChildNode(i)))
			return false;
	}

	// We recursed all the children of the current node
	return true;
}


///////////////////////////////// IsMiscGeometry \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns false only if the object is actual geometry
/////
///////////////////////////////// IsMiscGeometry \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool Exporter::IsMiscGeometry(INode *pNode, ObjectState *pObjectState)
{
	// Skip this object if it's just a target
	if (!pObjectState->obj || (pObjectState->obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0)))
		return true;

	// Skip the bone information about the biped
	if(!strncmp(pNode->GetName(), "Bip", 3))
		return true;
	
	// Skip the bone information about the biped
	if(!strncmp(pNode->GetName(), "bip", 3))
		return true;

	// Skip unneeded tag information about our model
	if(!strncmp(pNode->GetName(), "tag_", 4))
		return true;

	// If we get here, the object is a normal mesh that we can export
	return false;
}


///////////////////////////////// GetObjectCount \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This recurses through the entire world and increase the mesh count
/////
///////////////////////////////// GetObjectCount \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool Exporter::GetObjectCount(INode* pNode)
{
	// Check if we are exporting selected data only
	if(bExportSelected && pNode->Selected() == FALSE)
		return false;

	// Get the current object data
	ObjectState objState = pNode->EvalWorldState(0);

	// If there is an object and the .obj pointer isn't NULL
	if (objState.obj) 
	{
		// We do a switch on the CLASS_ID for the object
		switch(objState.obj->SuperClassID()) 
		{
			// If the object is a geometric object
			case GEOMOBJECT_CLASS_ID:
				
				// First we need to test if the object isn't unneeded info
				if(IsMiscGeometry(pNode, &objState))
					break;

				// If we get here the object must have valid vertex data
				// so we can increase our number of meshes by 1.
				m_numMeshes++;
				break;
		};
	}

	// Recurse the rest of child nodes if needed to check for more meshes
	for(int i = 0; i < pNode->NumberOfChildren(); i++) 
	{
		if(!GetObjectCount(pNode->GetChildNode(i)))
			return false;
	}

	// If we get here we have gone through all of the child nodes
	return true;
}


///////////////////////////////// ExportGeometry \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This starts the exporting process for the current mesh
/////
///////////////////////////////// ExportGeometry \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Exporter::ExportGeometry(INode *pNode)
{
	// Grab the current state of the object depending on the set animation frame
	ObjectState objState = pNode->EvalWorldState(m_pInterface->GetTime());

	// Make sure this isn't a mesh without valid vertex data
	if(IsMiscGeometry(pNode, &objState))
		return;

	// This boolean tells us if we converted a tri object that needs to be freed
	bool bDeleteTriObject = false;

	// This stores the pointer to our converted tri object
	TriObject *pTriObject = NULL;

	// Here we check to see if we can convert our mesh to a tri object
	if (objState.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
	{ 
		// If the object isn't already a tri object this will create a new object
		pTriObject = (TriObject *)objState.obj->ConvertToType(0,Class_ID(TRIOBJ_CLASS_ID, 0));
		
		// If the tri object isn't the same pointer as the original object
		// we need to be sure to free the pointer after we are done exporting.
		if (objState.obj != pTriObject) 
			bDeleteTriObject = true;
	}

	// If there isn't a valid pointer to our mesh we need to return
	if (!pTriObject) 
		return;

	// Clear our vertex and index lists before exporting
	m_vVertices.clear();
	m_vIndices.clear();


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// To store all the data for our materials, we had to create three new
	// lists.  Here we make sure to clear these lists before exporting a mesh.
	m_vTexCoords.clear();
	m_vTexIndices.clear();
	m_vMaterials.clear();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// From our tri object we can access a pointer to a "Mesh" class that
	// holds the face information we will be exporting.
	Mesh *pMesh = &pTriObject->GetMesh();

	// Now we can extract the vertex and indices to our lists.  We just
	// need to pass in the current node and current mesh pointer.
	ExtractFaceData(pNode, pMesh);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Before we write the face data, let's save the material info first.
	// This saves the number of materials and the texture-map names.
	WriteMaterials(pNode);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Now that all the data is stored in our exporter class we'll save it
	WriteFaceData();

	// If we had to create a new tri object we need to free it
	if (bDeleteTriObject)
	{
		delete pTriObject;
		pTriObject = NULL;
	}
}


///////////////////////////////// ExtractFaceData \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This goes through the mesh class and extracts all the face data
/////
///////////////////////////////// ExtractFaceData \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Exporter::ExtractFaceData(INode *pNode, Mesh *pMesh)
{
	// Here we get the transformation matrix for the current mesh 
	Matrix3 mTransform = pNode->GetObjTMAfterWSM(0);

	// This returns a boolean telling us if our polygon order is normal or not.
	bool bNormal = NormalPolygonOrder(mTransform);

	// This will store our index order for our triangles
	int indexOrder[3];

	// If the polygon order is not normal, reverse it, otherwise do the normal order
	if(!bNormal)
	{
		indexOrder[0] = 2;
		indexOrder[1] = 1;
		indexOrder[2] = 0;
	}
	else
	{
		indexOrder[0] = 0;
		indexOrder[1] = 1;
		indexOrder[2] = 2;
	}

	// Now we actually get to go through all the face data and extract data
	for(int i = 0; i < pMesh->getNumFaces(); i++) 
	{
		// For every face there is three triangles, so loop through each one
		for(int j = 0; j < 3; j++)
		{
			// Point3 is a Max class that stores a x, y and z for a vertex
			Point3 vVertex;

			// Get the current order of our polygon for the current vertice
			int index = indexOrder[j];
			
			// Here we get the current vertex
			vVertex = mTransform * pMesh->verts[pMesh->faces[i].v[index]];

			// Assume the vertex is unique at first
			bool bUnique = true;		

			// Get the current size of the vertex list before we add a new vertice
			int lastIndex = (int)m_vVertices.size();

			// Go through all of the vertices and check if we have a duplicate
			for(int c = 0; c < (int)m_vVertices.size(); c++)
			{
				// Check if we have a duplicate (remember z and y are swapped)
				if(m_vVertices[c].x == vVertex.x && m_vVertices[c].y == vVertex.z && m_vVertices[c].z == vVertex.y)
				{
					// We have a duplicate!  Don't add this to our list, but save 
					// the index to be added to our indices array below.
					bUnique = false;
					lastIndex = c;
					break;
				}
			}

			// Add the current index to our unique vertices array to the indices list
			m_vIndices.push_back(lastIndex);

			// Do a simple swap to change the y and z to be OpenGL friendly.
			// Max has Z pointing up, where OpenGL has Y pointing up.
			float temp = vVertex.y;
			vVertex.y = vVertex.z;
			vVertex.z = temp;

			// Create our own CVector3 variable to be added to our vertex list
			CVector3 vMyVertex = {vVertex.x, vVertex.y, vVertex.z};

			// If the vertex is unique, add it to the list
			if(bUnique)
			{
				m_vVertices.push_back(vMyVertex);
			}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
			
			// Alright, here is the beef to our tutorial.  Most of this is
			// exactly like how the vertices are exported, so the process
			// should be familiar from the last tutorial.  We just store
			// the current face's material ID, then store unique texture
			// coordinates and the indices.

			// If there are UV coordinates for this mesh
			if(pMesh->numTVerts)
			{
				// Create a Point3 variable to store the variable.  Apparently
				// the UV coordinates array is of type Point3.
				Point3 vTexCoord;

				// Initially set our boolean to true to assume it's unique
				bUnique = true;

				// Here we extract the material ID for this current face
				int materialID = pMesh->faces[i].getMatID();

				// Go through all of the materials and check if the ID is unique
				for(int c = 0; c < (int)m_vMaterials.size(); c++)
				{
					// Check if the material ID is the current material
					if(m_vMaterials[c] == materialID)
					{
						// The ID is already used so break
						bUnique = false;
						break;
					}
				}

				// If we have a unique ID, let's add it to our list
				if(bUnique)
					m_vMaterials.push_back(materialID);

				// Extract the current texture coordinate from the UV list
				// using the current face information.
				vTexCoord = pMesh->tVerts[pMesh->tvFace[i].t[index]];

				// Assume that this texture coordinate is unique
				bUnique = true;		

				// Grab the size of our UV coordinate list
				lastIndex = (int)m_vTexCoords.size();

				// Go through all the texture coordinates and see if this
				// one is unique or not.
				for(int c = 0; c < (int)m_vTexCoords.size(); c++)
				{
					// Is this texture coordinate already stored?
					if(m_vTexCoords[c].x == vTexCoord.x && m_vTexCoords[c].y == vTexCoord.y)
					{
						// Not a unique texture coordinate, so store c and break.
						bUnique = false;
						lastIndex = c;
						break;
					}
				}

				// Add the correct index for our texture coordinate
				m_vTexIndices.push_back(lastIndex);
				
				// If the UV coordinate is unique...
				if(bUnique)
				{
					// Go through all of the materials and find the right 
					// material ID in our materials list.
					for(int m = 0; m < m_vMaterials.size(); m++)
					{
						// Check for a match
						if(m_vMaterials[m] == materialID)
						{
							// Assign the ID that is already in our list
							materialID = m;
						}
					}

					// To be more efficient, we store the UV coordinate and the material
					// ID in a CVector3.  We then add that to our texture coordinate list.
					// We add m_lastTexture because other meshes have their own textures so
					// we need to give this mesh the ID that is AFTER the last mesh's IDs.
					CVector3 vTexCoord2 = {vTexCoord.x, vTexCoord.y, materialID + m_lastTexture};
					m_vTexCoords.push_back(vTexCoord2);
				}
			}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


		}
	}
}


///////////////////////////////// WriteFaceData \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This saves our face data for the current mesh to the file
/////
///////////////////////////////// WriteFaceData \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Exporter::WriteFaceData()
{
	// Grab the number of vertices and write them to the file 
	int numVertices = (int)m_vVertices.size();
	fwrite(&numVertices, sizeof(int), 1, m_fp);
	
	// Go through all of our vertices and save them to the file
	for(int i = 0; i < (int)m_vVertices.size(); i++)
	{
		fwrite(&m_vVertices[i], sizeof(CVector3), 1, m_fp);
	}

	// Grab the number of indices and write them to the file
	int numIndices = (int)m_vIndices.size();
	fwrite(&numIndices, sizeof(int), 1, m_fp);

	// Go through all of our indices and save them to the file
	for(int i = 0; i < (int)m_vIndices.size(); i++)
	{
		fwrite(&m_vIndices[i], sizeof(int), 1, m_fp);
	}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Grab the number of texture coordinates and write them to the file
	int numTexCoords = (int)m_vTexCoords.size();
	fwrite(&numTexCoords, sizeof(int), 1, m_fp);

	// Go through all of our texture coordinates and save them to the file
	for(int i = 0; i < (int)m_vTexCoords.size(); i++)
	{
		fwrite(&m_vTexCoords[i], sizeof(CVector3), 1, m_fp);
	}

	// Grab the number of texture indices and write them to the file
	int numTexIndices = (int)m_vTexIndices.size();
	fwrite(&numTexIndices, sizeof(int), 1, m_fp);
	
	// Go through all of our texture indices and save them to the file
	for(int i = 0; i < (int)m_vTexIndices.size(); i++)
	{
		fwrite(&m_vTexIndices[i], sizeof(int), 1, m_fp);
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


}


///////////////////////////////// NormalPolygonOrder \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This tells us if our polygon order is normal
/////
///////////////////////////////// NormalPolygonOrder \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool Exporter::NormalPolygonOrder(Matrix3 &m)
{
	// Check to see if there was any mirroring on the model that needs adjusting.
	// This technique was found in the Max SDK Knowledgebase.
	return (DotProd( CrossProd(m.GetRow(0), m.GetRow(1) ), m.GetRow(2) ) < 0.0) ? 1:0;
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// WriteMaterials \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This saves all the material names to our file
/////
///////////////////////////////// WriteMaterials \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Exporter::WriteMaterials(INode *pNode)
{
	// In order to write the material names we need to first extract them.
	// GetMaterialName() is a function we created to do just that.  You
	// can view the contents of GetMaterialName() below this function.
	// So, every node stores a pointer to a Mtl class, which can be used
	// to extract one or many texture-map file names.  If the mesh has
	// many materials then we need to go through all of the sub-materials.
	// Below, we first check if there is valid materials, then check if the
	// mesh has multiple textures or not.

	// Grab the material class pointer from the node
	Mtl *pMaterial = pNode->GetMtl();

	// If we have valid materials
	if(pMaterial)
	{
		// If there are no sub-materials, just write one texture map
		if(pMaterial->NumSubMtls() == 0)
		{
			// There is only one material
			int numMaterials = 1;

			// Get the name of the texture-map applied to the mesh
			string strMaterial = GetMaterialName(pMaterial);

			// If a file name wasn't returned there are no materials
			if(!strMaterial.size())
				numMaterials = 0;
			
			// Write the number of materials to the file
			fwrite(&numMaterials, sizeof(int), 1, m_fp);

			// Below we copy the material to a fixed buffer and save it to the file
			char szMaterial[MAX_FILE_NAME] = {0};
			strncpy(szMaterial, strMaterial.c_str(), MAX_FILE_NAME-1);
			fwrite(szMaterial, sizeof(char), MAX_FILE_NAME, m_fp);

			// Increase the last texture ID
			m_lastTexture++;
		}
		else
		{
			// Grab the number of materials we found earlier and save them
			int numMaterials = (int)m_vMaterials.size();
			fwrite(&numMaterials, sizeof(int), 1, m_fp);

			// Go through all of the materials and extract their names
			for(int i = 0; i < numMaterials; i++)
			{
				// Get the number of sub-materials for this mesh
				int subMaterials = pMaterial->NumSubMtls();

				// Here we get a pointer to the current submaterial.  The Max SDK
				// recommends that we do it this way using % just to make sure we
				// don't have cases where the material is higher than it should be.
				Mtl *subMaterial = pMaterial->GetSubMtl(m_vMaterials[i] % subMaterials);
				
				// Finally, we can get the sub-material name
				string strMaterial = GetMaterialName(subMaterial);

				// Move the material name over to a fixed buffer and save it
				char szMaterial[MAX_FILE_NAME] = {0};
				strncpy(szMaterial, strMaterial.c_str(), MAX_FILE_NAME-1);
				fwrite(szMaterial, sizeof(char), MAX_FILE_NAME, m_fp);

				// Increase the last texture ID
				m_lastTexture++;
			}
		}
	}
	else
	{
		// There are no materials for this mesh so we just save a 0
		int numMaterials = 0;
		fwrite(&numMaterials, sizeof(int), 1, m_fp);
	}
}


///////////////////////////////// GetMaterialName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a pointer to a material class and returns the texture name
/////
///////////////////////////////// GetMaterialName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

string Exporter::GetMaterialName(Mtl *pMaterial)
{
	// If we have a pointer to a Mtl class, we can get a pointer to a
	// Texmap class.  The flag ID_DI stands for Diffuse.  In Max, the texture
	// maps that are files are stored in the diffuse portion of the material.
	Texmap* pTexture = pMaterial->GetSubTexmap(ID_DI);

	// Here we make sure there is a valid pointer to our texture, and if so
	// we can check if the class ID is a bitmap (or other types).
	if(pTexture && (pTexture ->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)))
	{
		// Next, we extract the full path of the texture map by casting
		// the texture class to BitmapTex pointer, which has GetMapName().
		char *szFullPath = ((BitmapTex*)pTexture)->GetMapName();

		// We don't want the full path, just the file name so we just store
		// a char pointer to the start of the actual file name.
		char *szFile = strrchr(szFullPath, '\\');

		// If the pointer is invalid, just store the original name
		if(!szFile)
			szFile = szFullPath;
		// Otherwise, move the pointer up one so we don't have the '\' char in the name
		else
			++szFile;

		// Return the texture-map name
		return szFile;
	}
	
	// There is no valid name found, so return NULL.
	return "";
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Like we said at the top, there wasn't a great deal of changes.  The process of
// obtaining the UV coordinates is almost identical to getting the vertices.  We
// needed to create indices for the texture coordinates just like our indices for
// the vertices.  
//
// To obtain the texture map names we just extract them from the Mesh class.
//
// You can test this exporter by using the Loader Part2 tutorial.  You will be
// able to see materials attached to your exported model.  Be sure to check
// out Exporter.h to see the functions and variables added to the GTExpoter
// class.  Get ready for the next tutorial where we will be able to have amazing
// bone animation.
//
//
// © 2000-2006 GameTutorials