//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Exporter										 //
//																		 //
//		$Description:	3DS Max weighted-animation exporter	series		 //
//																		 //
//***********************************************************************//

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// Welcome to one of the most awaited series of tutorials from GameTutorials.
// In this tutorial we will go over how to setup and create a basic exporter
// for Autodesk's® 3D Studio Max®.  Eventually, the three-part tutorial
// series will export a bone-animated model to our own special format.  The
// exporter can also be used to export not just character models but fully- 
// textured worlds.
//
// * What is 3D Studio Max®? *
//
// Among other things, 3D Studio Max® is 3D editor that can create amazing 3D 
// worlds and animation.  This program is used by many game developers, as well 
// as many of the animation we see in movies.  Another popular 3D editor is Maya®.
//
// * What is an exporter? *
//
// When we create our cool models and worlds in a 3D editor, we need to "export"
// that model data (vertices, texture coordinates, etc.) to a file that can be
// read into our own program.  Since most file formats don't support bone
// animation, we need to create our own file format that stores that information.
// We do this by creating our own "exporter" that will spit out all the needed
// information into a file.  We create a .GTF file format.  The exporter is
// actually a DLL that is changed to a .DLE file and stored in the Plugins\
// folder located in your 3D Studio Max® directory.  When 3DS Max® loads it will
// check the plugins folder and load all the plugins.  You can then choose the
// "File->Export" option, which will bring up file formats that you can export
// your data to.  If your exporter is loaded its file format will be an option.
//
// * The exporter is a DLL? *
//
// Correct!  We need to create a DLL project, not a console or Win32 application.
// If you are unsure how to create a DLL, please refer back to our DLL tutorial.
// Make sure you create the DLL with no added files to it, otherwise Visual 
// Studio will add unneeded files and settings.  
//
// * Video tutorials included *
//
// Be sure to view our video tutorials that are included with this tutorial that 
// show you how to setup your project settings correctly, as well as using the 
// plugin in 3DS Max.  You can access these tutorials by clicking on the 
// "GTExporter.htm" file.  Make sure you have the correct codec installed.  You
// can find a link for the TechSmith codec at the bottom of the html page.
//
// * MAX SDK is needed to compile *
//
// In order to compile a Max exporter you need to have the SDK for the current
// version of Max that you are running.  You can download version 5.0 of the SDK
// online from Discreet's website.  If you are running version 6.0 and up the
// exporters should work for 6.0, 7.0 and 8.0.  I am running version 7.0 for
// this tutorial.
//
// * DEF file needed *
//
// You will notice in the current project's folder there is a file called
// GTExporter.def.  This file is needed for Max to load our exporter.  The
// file consists of the following:
//
// LIBRARY GTExporter
// EXPORTS
// 	 LibDescription			@1
// 	 LibNumberClasses		@2
// 	 LibClassDesc			@3
//	 LibVersion				@4
// SECTIONS
//	 .data READ WRITE
//
// This just tells Max what we called our main class and the rest is just
// standard data that you won't need to change.
//
// * Exporter class explanations *
//
// There are 2 classes that we create in order to have an exporter.  We first
// create a class inherited of type:  ClassDesc2.  You can call the class
// whatever you want, but in this tutorial we use the name: GTExporterClassDesc.
// This class contains basic data access functions for getting things like our 
// library description, ID and hInstance.  
// 
// The last class we create is inherited from this Max class: SceneExport.
// We call the class "GTExporter".  This class is what we will be
// primarily working with.  This handles all of our exporter functionality.
//
// Really, the main functions that you will want to focus on is these:
// ExportGeometry() and ExtractFaceData().  The rest will just be mostly setup
// and stuff that you probably won't ever change.  Of course, the functions
// below those will most likely be changed to fit your own file format.
//
// * GTF file format *
//
// We created a basic file format to save our 3D data to (.GTF).  
// Below is the format:
//
// * "GT3D" (4 characters)
// * Exporter Version (1 float)
// * Number of Objects/Meshes (1 int)
// *	Number of Vertices (1 int)
// *	Vertices ( Number of Vertices * CVector3)
// *	Number of Indices (1 int)
// *	Indices ( Number of Indices * int)
//
//
// That's it!  Since we want to be able to load many objects we first need
// to figure out how many objects there are.  We count the meshes in the
// world, write that to our file header, then we export each object's data.
// Notice that we write a "Key" and a version to our .gtf file.  This is to
// make sure that what we are loading is a true .gtf file.
//
// It is assumed that by reading this tutorial you understand the basics of
// file loading and character animation.  The next tutorials will show how
// to export texture and animation/bone data.
//
// * SDK knowledgebase online *
//
// The SDK knowledgebase has a lot of information (sometimes hard to find)
// about the API used to export/import data from Max.  You can access the 
// knowledgebase for the Max SDK online at:
//
// http://sparks.discreet.com/search/
//
//
// In conjunction with this tutorial series, we also created a GTLoader
// tutorial series that shows how to load and animate the characters we
// export from Max.  Be sure to use GTLoader Part1 for this version of
// the exporter.
//
// Let's get into the code and explain how it works.
//
//

#include "Exporter.h"


///////////////////////////////// Exporter \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This initializes all of our exporter data
/////
///////////////////////////////// Exporter \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

Exporter::Exporter()
{
	m_numMeshes = 0;
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
	// This function is the main function that Max calls to begin exporting
	// our model data.  We receive information through this function about our
	// Mostly what we care about is just the file name (szFile) and the
	// interface pointer to our scene (pInterface).  This allows us to
	// call 3DS Max functions and get at our scene meshes.
	//
	// The outline of this function is as follow: (1) we check to see if
	// the user only is exporting the selected data, (2) then we find out
	// how many children are in the root node so we can traverse them,
	// (3) also we write the GT_KEY, version and number of objects
	// as the header of our .GTF file, (5) finally, we traverse the
	// nodes of our scene to get at the data for each object/mesh.

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

	// I chose to find out how many meshes there are in the scene
	// so we can save that data to the header of the filer, which
	// makes loading easier.  You can just depend on the amount of
	// children for this number, since some of the nodes are tags
	// and biped (bone) information.  We actually have to go through
	// each node and check if it has vertex data.
	for(int i=0; i < numChildren; i++)
	{
		// Check if the user cancelled the export yet or not
		if (pInterface->GetCancel())
			break;

		// As we go through every node we keep adding up how many
		// meshes we found with vertex data and store that number
		// in the m_numMeshes variable.
		GetObjectCount(pInterface->GetRootNode()->GetChildNode(i));
	}

	// We should have the number of meshes by now to write to our file
	fwrite(&m_numMeshes, sizeof(int), 1, m_fp);

	// Now we can go through each node and start exporting the data of
	// our world to the file.  We just need to go through each child node
	// and process each one.
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
	// We don't want to export this node if it's not selected and the
	// user only wants to export the selected data. 
	if(bExportSelected && pNode->Selected() == FALSE)
		return false;

	// With the current child node that was passed in, we can access
	// the state of the current object/mesh.  We pass in 0 because we
	// want the current state of the mesh at frame 0 of animation.
	// We then receive an instance of the ObjectState class, which stores
	// a pointer to the current object in the node.
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
	// We only want to export data that isn't a target, tag, biped or any
	// other miscellaneous data.  If this returns false that means it wasn't
	// miscellaneous data, but the true mesh data (which is what we want).

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
	// Since we want to write the number of meshes to the header of our
	// .GTF file we need to recurse through the entire world and add up all
	// the meshes that aren't miscellaneous object data.

	// We don't want to export this node if it's not selected and the
	// user only wants to export the selected data. 
	if(bExportSelected && pNode->Selected() == FALSE)
		return false;

	// With the current child node that was passed in, we can access
	// the state of the current object/mesh.  We pass in 0 because we
	// want the current state of the mesh at frame 0 of animation.
	// We then receive an instance of the ObjectState class, which stores
	// a pointer to the current object in the node.
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
	// This function is the control tower for exporting our data.  So that we
	// don't have a huge messy function, we will break up the exporting processes
	// into a bunch of smaller functions.  To handle our data that is being
	// exported we created STL vectors for the vertices and the indices.  If
	// you are unfamiliar with how file loading and exporting works, let me give
	// a quick explanation on what we store for vertices and indices.
	//
	// Take a cube for instance.  How many unique vertices are there in a cube?
	// 8 right?  4 on top and 4 on bottom right?  Well, when we extract the vertices
	// from Max's face data, it comes to around 24 vertices; however, only 8 of them
	// are unique.  We only want to save unique vertices because it takes less space
	// in our files, as well as being nice to setup for vertex buffers and such.
	// Instead of saving all the 24 vertex data of the cube, we just save the unique
	// vertices, then create an array of integers called "indices".  This array stores
	// in order the correct index into our vertex array that needs to be draw next.
	// Thus, we have a list for unique vertices and our indices.
	//
	// Also, one thing that needs to be understood is the whole "Tri Object" thing.
	// There are many different way to create 3D models in 3DS Max, like NURBS and
	// such that aren't stored in triangle formats.  We want our data stored in
	// triangles so we need to convert the data to a "Tri Object", which is an
	// object that is comprised of triangles.  We then have to remember to free it.

	// Like in the previous functions, we want to get the pointer to the current
	// object.  Some meshes have animation, so to show you how to just get the data
	// at the current animation frame the user is on I use the GetTime() function
	// instead of just passing in 0.  This returns the current state of the object.
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

	// From our tri object we can access a pointer to a "Mesh" class that
	// holds the face information we will be exporting.
	Mesh *pMesh = &pTriObject->GetMesh();

	// Now we can extract the vertex and indices to our lists.  We just
	// need to pass in the current node and current mesh pointer.
	ExtractFaceData(pNode, pMesh);

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
	// We've now reached one of the most important, yet the messiest function
	// in the exporter.  There is a lot of looping and checking to extract
	// only the unique vertices and such.  It would be easier and much faster
	// to have your own hash table for each of the data lists, but I chose not
	// to add a hash table for the sake of minimizing the code and confusion.
	// This exporter is in no way optimized for speed, but intended to be
	// easily understood.
	//
	// Before looping through the face data of the mesh, we need to first check
	// the order of the polygons.  This is important for calculating our normals
	// and rendering.  To do this we use some math (see NormalPolygonOrder()).

	// Here we get the transformation matrix for the current mesh 
	Matrix3 mTransform = pNode->GetObjTMAfterWSM(0);

	// This returns a boolean telling us if our polygon order is normal or not.
	// This comes in handy when artists create characters that has mirroring.
	// This way normals and polygons are rendered backwards when they shouldn't be.
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
			
			// Here we get the current vertex.  We need to multiply the transformation
			// matrix by the current vertex in the face.  This gives us the world position.
			// Notice that we use the face data to index into the verts[] array.
			vVertex = mTransform * pMesh->verts[pMesh->faces[i].v[index]];

			// Now comes the part where we make sure it's a unique vertice.
			// Below we go through all of our vertices and check to see if
			// there is a duplicate.

			// Assume the vertex is unique at first
			bool bUnique = true;		

			// Get the current size of the vertex list before we add a new vertice
			int lastIndex = (int)m_vVertices.size();

			// Go through all of the vertices and check if we have a duplicate
			for(int c = 0; c < (int)m_vVertices.size(); c++)
			{
				// Notice that we check if y == z and z == y.  This is because
				// we switch the y and z below so that it conforms with OpenGL's axises.
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
}


///////////////////////////////// NormalPolygonOrder \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This tells us if our polygon order is normal
/////
///////////////////////////////// NormalPolygonOrder \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool Exporter::NormalPolygonOrder(Matrix3 &m)
{
	// We take our transformation matrix for the current mesh and then
	// take the cross product between the first and second row.  We then take 
	// the dot product of that result with the third row in the matrix.
	// By checking the result of the dot product we can tell if the object's
	// transform matrix is inside out, which might be caused by mirroring.
	// This technique was found in the Max SDK Knowledgebase.
	return (DotProd( CrossProd(m.GetRow(0), m.GetRow(1) ), m.GetRow(2) ) < 0.0) ? 1:0;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Not sure bad is it?  The hard part is just knowing how to traverse the nodes
// and which Max functions do what.  This portion of the tutorial series will just
// export basic face information with vertices.  The next tutorial will delve into
// extracting texture coordinates and materials.
//
// You can test this exporter by using the Loader Part1 tutorial.  Be sure to
// check out DLLEntry.cpp and Exporter.h to see the basic classes needed to have
// an exporter.
//
//
// © 2000-2006 GameTutorials