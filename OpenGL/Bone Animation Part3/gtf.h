#ifndef _GTF_H
#define _GTF_H


// The maximum amount of character for our material file names
#define MAX_FILE_NAME	25


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
	CVector3	*m_pTexCoords;		// The texture coordinates
	int			*m_pTexIndices;		// The texture coordinate indices
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

	// This loads the textures associated with our model.  
	// The path is where the textures are stored.
	void LoadModelTextures(LPSTR strPath);

	// This renders the model to the screen
	void DrawModel();

	// This frees the model's data
	void DestroyModel();

	
private:

	// This renders a .GTF model to the screen
	void RenderModel();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This updates the models current frame of animation, and calls SetCurrentTime().
	void UpdateModel();

	// This sets the lastTime, t, and the currentFrame of the model's animation when needed.
	void SetCurrentTime();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// This stores the texture array for each of the textures assigned to this model
	UINT m_Textures[MAX_TEXTURES];	

	// This stores a list of all the names of the textures that have been loaded
	vector<string> strTextures;

	// This is the model structure that holds all of the objects/meshes
	t3DModel m_Model;
};



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// Since we need to handle some complex animation with rotation and translations
// we are going to need to have a basic quaternion class.

// This is our quaternion class
class CQuaternion 
{

public:

	// This is our default constructor, which initializes everything to an identity
	// quaternion.  An identity quaternion has x, y, z as 0 and w as 1.
	CQuaternion() { x = y = z = 0.0f;   w = 1.0f;  }
	
	// Creates a constructor that will allow us to initialize the quaternion when creating it
	CQuaternion(float X, float Y, float Z, float W) 
	{ 
		x = X;		y = Y;		z = Z;		w = W;
	}

	// Creates a quaternion from a CVector4
	CQuaternion(CVector4 &v) 
	{ 
		x = v.x;		y = v.y;		z = v.z;		w = v.w;
	}

	// This is our operator overloaded multiplication against a scalar
	CQuaternion operator*(const float scalar) const
	{
		return CQuaternion(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	// This is our operator overloaded multiplication against a CVector3
	CVector3 operator*(CVector3 v);

	// This is our operator overloaded multiplication against a quaternion
	CQuaternion operator*(const CQuaternion& q);

	// This is our operator overloaded addition against another quaternion
	CQuaternion operator+(const CQuaternion& q);

	// This takes in an array of 16 floats to fill in a 4x4 homogeneous matrix from a quaternion
	void CreateMatrix(float *pMatrix);

	// This takes a 3x3 or 4x4 matrix and converts it to a quaternion, depending on rowColumnCount
	void CreateFromMatrix(float *pMatrix, int rowColumnCount);

	// This returns a spherical linear interpolated quaternion between q1 and q2, according to t
	CQuaternion Slerp(CQuaternion &q1, CQuaternion &q2, float t);

private:

	// This stores the 4D values for the quaternion
	float x, y, z, w;
};



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// In this file we added a quaternion class, as well in our model class we also 
// added a couple functions to handle updating our animation frames:
//
//	void UpdateModel();
//
//	void SetCurrentTime();
//
//
// © 2000-2006 GameTutorials