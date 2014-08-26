#ifndef _OBJ_H
#define _OBJ_H

#include "main.h"

// This class can be used to load a .Obj file.  You will need the grab the
// structures in main.h as well if you want to use this, or create your own.
// I added a couple functions at the end of this class to help with the material
// problem that a .obj file has.  Obj files do not save material information, like
// the color, material name or the texture map file assigned to it.  Though the .obj
// file format isn't the best used for games, I thought I would help out a little.
// To be honest, it would be better to just add the information to the .obj and
// modify my code to read it in.  For instance, put a : c 255 0 255   for the color.

// This is our class for loading .obj files
class CLoadObj {

public:
	
	// This will be the only function that needs to be called by you.
	// Just pass in your model that will hold the information and the file name.
	bool ImportObj(t3DModel *pModel, char *strFileName);

	// This is the main loading loop that gets called in ImportObj()
	void ReadObjFile(t3DModel *pModel);

	// This is called in ReadObjFile() if we find a line starting with 'v'
	void ReadVertexInfo();

	// This is called in ReadObjFile() if we find a line starting with 'f'
	void ReadFaceInfo();

	// This is called when we are finished loading in the face information
	void FillInObjectInfo(t3DModel *pModel);

	// This isn't necessary for the loader, but it's nice to have vertex normals for lighting
	void ComputeNormals(t3DModel *pModel);

	// Since .obj files don't have texture/material names, we create a function to set them manually.
	// The materialID is the index into the pMaterial array for our model.
	void SetObjectMaterial(t3DModel *pModel, int whichObject, int materialID);

	// To make it easier to assign a material to a .obj object we create a funtion to do so.
	// We can pass in the model, the material name, the texture file name and the RGB colors.
	// If we just want a color, pass in NULL for strFile.
	void AddMaterial(t3DModel *pModel, char *strName, char *strFile, 
					 int r = 255,      int g = 255,   int b = 255);

private:

	// This holds our file pointer for reading in the file
	FILE *m_FilePointer;

	// This is an STL (Standard Template Library) vector that holds a list of vertices
	vector<CVector3>  m_pVertices;

	// This is an STL (Standard Template Library) vector that holds a list of faces
	vector<tFace> m_pFaces;

	// This is an STL (Standard Template Library) vector that holds a list of UV Coordinates
	vector<CVector2>  m_pTextureCoords;

	// This tells us if the current object has texture coordinates
	bool m_bObjectHasUV;

	// This tells us if we just read in face data so we can keep track of multiple objects
	bool m_bJustReadAFace;
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows how to load an .obj file.  These files are ASCII text files
// that contain very few information about the models, but are great for importing/exporting
// into almost every 3D software package as well as being easy to read in.
// 
// This header file holds the class information for CLoadObj.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//

