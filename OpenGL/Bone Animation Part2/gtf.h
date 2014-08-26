#ifndef _GTF_H
#define _GTF_H


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// The maximum amount of character for our material file names
#define MAX_FILE_NAME	25

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// This is used when converting all our data from the loader to a model structure.
// It stores the sizes of each of the face-data lists.
struct tMeshInfo
{
	int numVertices;
	int numTexCoords;
	int numIndices;
	int numTexIndices;
	int numMaterials;
};


// This class handles all of the main loading code
class CLoadGTF
{

public:

	// This inits the data members
	CLoadGTF();								

	// This is the function that you call to load the GTF model
	bool ImportGTF(t3DModel *pModel, char *strFileName);

private:


	// This reads in the data from the GTF file and stores it in the member variables.
	void ReadGTFData(t3DModel *pModel);

	// This converts the member variables to our pModel structure
	void ConvertDataStructures(t3DModel *pModel, tMeshInfo meshInfo);

	// This frees memory and closes the file
	void CleanUp();
	
	// Member Variables		

	// The file pointer
	FILE *m_FilePointer;

	CVector3	*m_pVertices;		// The list of vertices
	int			*m_pIndices;		// The list of indices into the vertex list


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Just like the vertices, we need to load UV coordinates and their indices.
	CVector3	*m_pTexCoords;		// The texture coordinates
	int			*m_pTexIndices;		// The texture coordinate indices

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


};


// This is our model class that we use to load and draw and free the model
class CModelGTF
{

public:

	// These are our init and deinit() C++ functions (Constructor/Deconstructor)
	CModelGTF();
	~CModelGTF();
	
	// This loads the model from a path and file name
	bool LoadModel(LPSTR strPath, LPSTR strModel);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This loads the textures associated with our model.  
	// The path is where the textures are stored.
	void LoadModelTextures(LPSTR strPath);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// This renders the model to the screen
	void DrawModel();

	// This frees the model's data
	void DestroyModel();

	
private:

	// This renders a .GTF model to the screen
	void RenderModel();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This stores the texture array for each of the textures assigned to this model
	UINT m_Textures[MAX_TEXTURES];	

	// This stores a list of all the names of the textures that have been loaded.  
	// This was created so that we could check to see if a texture that is assigned
	// to a mesh has already been loaded.  If so, then we don't need to load it again
	// and we can assign the textureID to the same textureID as the first loaded texture.
	vector<string> strTextures;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// This is the model structure that holds all of the objects/meshes
	t3DModel m_Model;
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// We added a couple lists to the loader class:
//
// CVector3	*m_pTexCoords;	
// int		*m_pTexIndices;
//
// These will be used for our UV coordinates and their face data.  We also added
// a define to tell us how big our texture-map file names are (25).  Plus, in
// our model class we added some lists to store more texture info.
//
// UINT m_Textures[MAX_TEXTURES];	
// vector<string> strTextures;
//
// Now the model can keep track of all of it's texture maps separately.
// Finally, we added our function to actually load the textures and assign
// the appropriate texture IDs.
//
// void LoadModelTextures(LPSTR strPath);
//
//
// © 2000-2006 GameTutorials