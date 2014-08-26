#include "model.h"
#include "model_defines.h"

//////////////
// Globals //
////////////

FILE *gFile = NULL; // Global file pointer for loading files
vector<SMeshObject*> gMeshObjects;
vector<SMaterialObject*> gMaterialObjects;

// 3DS General Parsing Functions 
bool ReadChunk(SChunk &chunk);
bool SkipBytes(size_t sizeInBytes);
bool ParseString(char str[kMaxStringLen]);

// 3DS Chunk Parsing Functions
bool ParseVersionNumber(); // Parses version number of 3DS file
bool ParseMaterialName(); // Parses material name for current mesh object
bool ParseMaterialTextureName(); // Parses texture name for current mesh object
bool ParseMaterialColor(int id); // Parses material color based on "id" passed in
bool ParseObjectName(); // Parses Max name for object
bool ParseVertexList(); // Parses list of vertices for mesh object
bool ParseFaceList(); // Parses list of faces for mesh object
bool ParseFaceMaterialList(); // Parses and assigns materials for mesh objects
bool ParseTexCoordList(); // Parses texture coordinates for mesh object

bool CModel::load3DS(const char *fileName)
{
	assert(fileName != NULL);
	assert(gFile == NULL);
	
	SChunk chunk = {0};
				
	gFile = fopen(fileName, "rb");
		
		if(!gFile)
			return false;
			
	//TestRead();
			
	if(!ReadChunk(chunk))
		return false;
	
		// Error Check
		if(chunk.id != kBegin)
			return false;
			
	while(1)
	{
		// Parse the next chunk
		if(!ReadChunk(chunk))
		{
			if(feof(gFile))
				break;
			else
				return false;
		}
	
		switch(chunk.id)
		{
			case kVersion:
			{
				if(!ParseVersionNumber())
					return false;
					
				break;
			}
			
			// Start of a new object.  Objects can be a wide
			// range of things (like models or cameras for instance) in 3D Studio Max 
			case kObject:
			{
				break;
			}
			
			// Start of a new material.  
			case kMaterial:
			{
				// Add a new material object
				gMaterialObjects.push_back(new SMaterialObject);
				
				//if(!SkipBytes(chunk.length - sizeof(SChunk)))
				//	return false;
					
				break;
			}

			// Start of key frame data, we're not loading that
			// but we'll acknowledge it then move on 
			case kKeyFrame:
			{
				break;
			}

			case kObjectName:
			{
				if(!ParseObjectName())
					return false;
					
				break;
			}
			
			case kMaterialName:
			{
				if(!ParseMaterialName())
					return false;
					
				SMaterialObject *mat = gMaterialObjects.back();
					
				break;
			}
			
			case kMaterialTexture: // This marks the start of a material texture chunk
			{
				break;
			}
			
			case kMaterialTextureName:
			{
				if(!ParseMaterialTextureName())
					return false;
					
				break;
			}
			
			case kTriMesh:
			{
				// Add a new mesh object
				gMeshObjects.push_back(new SMeshObject);
					break; // Start of triangle mesh found
			}
			
			case kVertList:
			{
				if(!ParseVertexList())
					return false;
						
				break;
			}
			
			case kFaceList:
			{
				if(!ParseFaceList())
					return false;
					
				break;
			} 
			
			case kFaceMaterialList:
			{
				if(!ParseFaceMaterialList())
					return false;
					
				break;
			} 
			
			case kTexCoordList:
			{
				if(!ParseTexCoordList())
					return false;
					
				break;
			}
			
			//case kTransformMatrix:
			//{
			//	if(!SkipBytes(chunk.length))
			//		return false;
					
				//if(!ReadChunk(chunk))
				//	return false;
					
			//	break;
			//}
			
			default:
			{
				if(!SkipBytes(chunk.length - sizeof(SChunk)))
					return false;
				
				break;
			}								
					
		} // end switch(chunk.id)
			
	} // end of while(!feof(gFile))

	// We're done loading the 3DS file, we can now close it
	fclose(gFile);
	gFile = NULL;
	
	// Return the success or failure of building the model from
	// the 3DS file data just read in
	return buildFrom3DS();		
}

// Render the model
void CModel::render()
{
	list<CD3DMesh*>::iterator i = mMeshes.begin();
	
	for(; i != mMeshes.end(); ++i)
		(*i)->render();
}

// *** Private Methods ***

// Builds the model from the 3DS file data
// Returns true for success false otherwise
bool CModel::buildFrom3DS()
{
	// First, free anything that might already be stored in the model 
	freeMem();
	
	// Loop through all the mesh objects
	for(int c = 0; c < (int)gMeshObjects.size(); ++c)
	{
		int numTexCoords = (int)gMeshObjects[0]->texCoords.size();
		int numVerts = (int)gMeshObjects[0]->verts.size();
		int numIndices = (int)gMeshObjects[0]->faces.size() * 3;
	
		// If the number of texture coordinates does not equal
		// the number of (x,y,z) positions, skip this mesh object.  We
		// only handle mesh objects that have an equal number of (x,y,z)'s and
		// (u,v)'s
		if(numTexCoords != numVerts)
			continue;
	
		// Create temp arrays to store data in
		SVertex *verts = new SVertex[numVerts];
		WORD *indices = new WORD[numIndices];
	
		// Error check
		if(!verts || !indices)
			return false; // Ran out of memory
	
		// Loop through all the verts
		for(int i = 0; i < numVerts; ++i)
		{	
			// Get position and UV data
			CVector v = gMeshObjects[c]->verts[i];
			CTexCoord tc = gMeshObjects[c]->texCoords[i];
			
			// Set data
			verts[i].x = v.x;
			verts[i].y = v.z;
			verts[i].z = v.y;
			verts[i].u = tc.u;
			verts[i].v = 1.0f - tc.v;
			
			// **NOTE**	
			// We have to flip the V component so the texture coordinates work in D3D texture space
		}
		
		// Loop through all the faces
		for(int i = 0, j = 0; i < (int)gMeshObjects[0]->faces.size(); ++i)
		{	
			SFace f = gMeshObjects[c]->faces[i]; // Get the current face indices
			
			// Set the indices by REVERSING the winding order of the faces
			indices[j++] = f.indices[2];
			indices[j++] = f.indices[1];
			indices[j++] = f.indices[0];
			
			// **NOTE**	Normally we would just read the faces like this:
			//			
			//			indices[j++] = f.indices[0];   // Start at index zero
			//			indices[j++] = f.indices[1];  // then one...
			//			indices[j++] = f.indices[2]; // then two
			//
			//			But since 3D Studio Max, by default, uses a right handed coordinate
			//			system and Direct3D, by default, uses a left handed coordinate system
			//			we have to reverse the winding order of the faces
		}
	
		CD3DMesh *mesh = new CD3DMesh;
		
			// Error check
			if(!mesh)
				return false;
				
		int materialId = gMeshObjects[c]->materialIndex;
				
		// Set data checking for errors
		if(!mesh->setVertexData(verts, numVerts) ||
		   !mesh->setIndexData(indices,  numIndices) ||
		   !mesh->loadTexture(gMaterialObjects[materialId]->texFileName))
		{
			delete[] verts;
			delete[] indices;
				return false;
		}
		else
		{
			// Free temp data
			delete[] verts;
			delete[] indices;
			
			mMeshes.push_back(mesh); // Add the mesh
		}
	}
	
	return true; // Model has been pumped with steroids and is built!
}

void CModel::freeMem()
{
	list<CD3DMesh*>::iterator i = mMeshes.begin();
	
	// Loop through all the mesh parts and delete them
	for(; i != mMeshes.end(); ++i)
	{
		CD3DMesh *m = *i;
		delete m;
	}
		
	mMeshes.clear(); // Free up the list
}

// *** 3DS Helper Functions ***

// Read a chunk
bool ReadChunk(SChunk &chunk)
{
	assert(gFile != NULL);
	
	chunk.id = 0;
	chunk.length = 0;
	
	// Read the id	
	if(!fread(&chunk.id, 1, sizeof(ushort), gFile))
		return false;
	
	// Read the length
	if(!fread(&chunk.length, 1, sizeof(int), gFile))
		return false;
		
	return true;	
}

// Try to skip "sizeInBytes" bytes
bool SkipBytes(size_t sizeInBytes)
{
	if(sizeInBytes == 0)
		return true; // Nothing to skip
		
	bool skip = true; // Assume we are able to skip bytes

	char *data = new char[sizeInBytes];
	
		if(!data)
			return false; // Memory allocation failure, can't skip
			
	if(!fread(data, 1, sizeInBytes, gFile))
		skip = false; // Can't read the data, no skip :(
		
	delete[] data;
		return skip; // Return if we skipped or not
}

// Parse a string from the file
bool ParseString(char str[kMaxStringLen])
{
	char c = NULL;
	int i = 0;
	
	// Read each character 1 by 1...
	do
	{
		if(!fread(&c, 1, 1, gFile))
			return false;
			
		str[i] = c;
	
	// We know we are done when we hit a NULL or the end of string	
	} while((c != NULL) && (++i < kMaxStringLen));
	
	return true;
}

// Parse the version number
bool ParseVersionNumber()
{
	int versionNum = 0;

	// Read the version number
	if(!fread(&versionNum, 1, sizeof(int), gFile))
		return false;
		
	printf("3DS Version Number = %d", versionNum);
		return true;
}

// Parse a material's name
bool ParseMaterialName()
{
	if(gMaterialObjects.size() == 0)
		return false;

	// Get the current material object
	SMaterialObject *mat = gMaterialObjects.back();

	// Parse the name
	return ParseString(mat->name); 
}

// Paras a material's texture name
bool ParseMaterialTextureName()
{
	if(gMaterialObjects.size() == 0)
		return false;

	// Get the current material object
	SMaterialObject *mat = gMaterialObjects.back();

	// Parse the name
	return ParseString(mat->texFileName);
}

// Parse an object name
bool ParseObjectName()
{	
	// **NOTE** We disregard the object name, but need to parse it anyway to move to the 
	// next chunk.  You may want to keep it around if you want to reference it in you application
	char name[kMaxStringLen];
	return ParseString(name);
}

// Parse the vertex list
bool ParseVertexList()
{
	if(gMeshObjects.size() == 0)
		return false;

	ushort vertexCount = 0;
	SMeshObject *m = gMeshObjects.back(); // Get the current mesh object
	
	// Get the vertex count
	if(!fread(&vertexCount, 1, sizeof(ushort), gFile))
		return false;
	
	// Read them all in	
	while(vertexCount--)
	{
		CVector v;
		
		if(!fread(&v, 1, sizeof(CVector), gFile))
			return false;
			
		m->verts.push_back(v); // Add it to the vertex list
	}
	
	return true;
}

// Parse the index (face) list
bool ParseFaceList()
{
	if(gMeshObjects.size() == 0)
		return false;

	ushort faceCount = 0;
	SMeshObject *m = gMeshObjects.back(); // Get the current mesh object
	
	// Read in the face count
	if(!fread(&faceCount, 1, sizeof(ushort), gFile))
		return false;
	
	// For each face...	
	while(faceCount--)
	{
		SFace f;
		
		// Read in the face (3 triangle indices)
		if(!fread(&f, 1, sizeof(SFace), gFile))
			return false;
			
		m->faces.push_back(f); // Add it to the list
	}
	
	return true;
}

// Parse the face to material list
bool ParseFaceMaterialList()
{
	if(gMaterialObjects.size() == 0)
		return false;
		
	if(gMeshObjects.size() == 0)
		return false;

	SMeshObject *m = gMeshObjects.back(); // Get the current mesh object
	m->materialIndex = -1; // Assume it doesn't have a material attached to it

	char materialName[kMaxStringLen] = {0};

	// Try and parse the material's name
	if(!ParseString(materialName))
		return false;
	
	// Look for a match!	
	for(int i = 0; i < (int)gMaterialObjects.size(); ++i)
	{
		SMaterialObject *mat = gMaterialObjects[i];
	
		if(_stricmp(mat->name, materialName) == 0)
		{
			m->materialIndex = i; // We found it!!!
				break;
		}
	}
	
	// Couldn't find a valid material... Some artist needs to get texturing :)
	if(m->materialIndex == -1)
		return false; // We don't support untextured meshes!		

	ushort materialId = 0;
	ushort primitiveCount = 0;
	
	// **NOTE** Each primitive could have it's own unique material.  We don't care about
	// that are just going to read in the data and ignore it to get to the next chunk.

	// Read the number of primitives
	if(!fread(&primitiveCount, 1, sizeof(ushort), gFile))
		return false;

	// Read the material ids
	while(primitiveCount--)
	{
		if(!fread(&materialId, 1, sizeof(ushort), gFile))
			return false;
	}

	return true;
}

// Parse the texture coordinates
bool ParseTexCoordList()
{
	if(gMeshObjects.size() == 0)
		return false;

	ushort texCoordCount = 0;
	SMeshObject *m = gMeshObjects.back(); // Get the current mesh object
	
	// Read in the number of texture coordinates
	if(!fread(&texCoordCount, 1, sizeof(ushort), gFile))
		return false;
	
	// Read them in...	
	while(texCoordCount--)
	{
		CTexCoord tc;
		
		if(!fread(&tc, 1, sizeof(CTexCoord), gFile))
			return false;
			
		m->texCoords.push_back(tc); // Add them to the list
	}
	
	return true;
}

// *** End of 3DS Helper Functions ***