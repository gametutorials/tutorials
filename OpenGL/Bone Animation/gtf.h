#ifndef _GTF_H
#define _GTF_H


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this file we created a loader class and a model class for our .GTF
// file format.  Most of the class' code comes from previous model loader
// tutorials.  In the next tutorials we will add more to the classes,
// especially when it comes to animation data.  The loader loads all
// the GTF data, then the model class handles the rendering and such.


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

	// This renders the model to the screen
	void DrawModel();

	// This frees the model's data
	void DestroyModel();

	
private:

	// This renders a .GTF model to the screen
	void RenderModel();

	// This is the model structure that holds all of the objects/meshes
	t3DModel m_Model;
};


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// We changed the loader and model class to fit out GTF conventions.  Most of
// the class' data is the same, just renamed for GTF.  The texture and animation
// data has not yet been added.  These will be added in the next tutorials.
//
//
// © 2000-2006 GameTutorials