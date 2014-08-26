#ifndef __Exporter__H
#define __Exporter__H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This file holds our basic includes, defines and class definitions.

#include <iostream>					// Include for basic C++ code
#include <vector>					// Include for vector lists
#include "Max.h"					// Max's main header file
#include "iparamb2.h"				// Needed for ClassDesc2 class
using namespace std;				// Use the standard namespace


#define GT_KEY			"GT3D"		// Our GT key for the file format
#define GT_VERSION		1.0			// The exporter version
#define MAX_FILE_NAME	25			// The max number of characters for files

extern HINSTANCE g_hInstance;		// The global hInstance for our DLL

// We need to create a unique class ID for each exporter we create.
// This ID must not be used by any other DLL's that Max loads.
// There is a program with the SDK that can help to create such IDs.
#define Exporter_CLASS_ID	Class_ID(0x84e0051, 0x73fde51f)

// Since we want to make sure that our classes have the same data as our
// loader, we don't use the Max point classes, but use our own to save
// to our file.  We will eventually use all of these in later tutorials.

// This is our 4D point class used for quaternions and such
class CVector4 
{
public:
	float x, y, z, w;
};

// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3 
{
public:
	float x, y, z;
};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2 
{
public:
	float x, y;
};


// This is our main exporter class, which inherits from Max's SceneExport class.
class Exporter : public SceneExport 
{
public:

	int				ExtCount();					// Number of extensions supported
	const TCHAR *	Ext(int n);					// Returns our extension (.GTF)
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1 (unused)
	const TCHAR *	OtherMessage2();			// Other message #2 (unused)
	unsigned int	Version();					// Version number * 100 (i.e. version 3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box (not using)

	// This currently just returns TRUE for all options
	BOOL			SupportsOptions(int ext, DWORD options);

	// This functions is called when we actually try and export our model
	int				DoExport(const TCHAR *szFile, ExpInterface *ei, Interface *pInterface, BOOL bSuppressPrompts=FALSE, DWORD options=0);

	// Constructor / Destructor
	Exporter();
	~Exporter();		

private:

	// This traverses down each node of our scene
	bool			ProcessNode(INode *pNode);

	// This tells us if a current object is a real geometric object
	bool			IsMiscGeometry(INode *pNode, ObjectState *pObjectState);

	// This returns how many true meshes are in our scene
	bool			GetObjectCount(INode *pNode);

	// This is the control function for extract and writing data
	void			ExportGeometry(INode *pNode);

	// This extracts all the face data
	void            ExtractFaceData(INode *pNode, Mesh *pMesh);

	// This writes out the face data to our file
	void			WriteFaceData();

	// This tells us if the polygon order is mirrored or not
	bool			NormalPolygonOrder(Matrix3 &m);

	bool			bExportSelected;			// Tells if we only want to export selected data
	int             m_numMeshes;				// The number of meshes in the scene
	FILE*			m_fp;						// The file pointer
	Interface*		m_pInterface;				// Our Max interface pointer
	vector<CVector3> m_vVertices;				// The unique list of vertices
	vector<int>		 m_vIndices;				// The list of indices
};


// This is our class description for our exporter, inherited from ClassDesc2
class ExporterClassDesc: public ClassDesc2 
{
public:

	// These functions return basic information about our exporter
	int 			IsPublic()	   { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new Exporter(); }
	const TCHAR *	ClassName()    { return "Exporter"; }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID()      { return Exporter_CLASS_ID; }
	const TCHAR* 	Category()     { return "Exporters"; }
	const TCHAR*	InternalName() { return _T("Exporter"); }	
	HINSTANCE		HInstance()    { return g_hInstance; }

};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We are done!  This is the main header file for our exporter.  It contains
// two needed classes to have an exporter.  Everything in this tutorial is really
// the very bare minimum for having an exporter.  In the next sections of our
// tutorial series we will add texture and animation data.
//
// Go check out the next tutorial in this series and start learning.
//
//
// © 2000-2006 GameTutorials