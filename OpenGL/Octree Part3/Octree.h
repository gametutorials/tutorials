#ifndef _OCTREE_H
#define _OCTREE_H

#include "Main.h"
#include "3ds.h"


// The maximum amount of triangles per node
extern int g_MaxTriangles;

// The maximum amount of subdivisions allow (Levels of subdivision)
extern int g_MaxSubdivisions;

// The amount of end nodes created in the octree (That hold vertices)
extern int g_EndNodeCount;

// This stores the amount of nodes that are in the frustum
extern int g_TotalNodesDrawn;

// Turn lighting on initially
extern bool g_bLighting;

// This stores the render mode (LINES = false or TRIANGLES = true)
extern bool g_RenderMode;

// These values (0 - 7) store the index ID's for the octree node array (m_pOctreeNodes)
enum eOctreeNodes
{
	TOP_LEFT_FRONT,			// 0
	TOP_LEFT_BACK,			// 1
	TOP_RIGHT_BACK,			// etc...
	TOP_RIGHT_FRONT,
	BOTTOM_LEFT_FRONT,
	BOTTOM_LEFT_BACK,
	BOTTOM_RIGHT_BACK,
	BOTTOM_RIGHT_FRONT
};


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is used for our pLists in CreateNode() to help partition the world into
// different nodes.

struct tFaceList
{
	// This is a vector of booleans to store if the face index is in the nodes 3D Space
	vector<bool> pFaceList;	

	// This stores the total face count that is in the nodes 3D space (how many "true"'s)
	int totalFaceCount;
};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our debug lines class to view the octree visually
class CDebug 
{

public:

	// This adds a line to our list of debug lines
	void AddDebugLine(CVector3 vPoint1, CVector3 vPoint2);

	// This adds a rectangle with a given center, width, height and depth to our list
	void AddDebugRectangle(CVector3 vCenter, float width, float height, float depth);

	// This renders all of the lines
	void RenderDebugLines();		

	// This clears all of the debug lines
	void Clear();					

private:

	// This is the vector list of all of our lines
	vector<CVector3> m_vLines;		
};


// This is our octree class
class COctree
{

public:

	// The constructor and deconstructor
	COctree();
	~COctree();

	// This returns the center of this node
	CVector3 GetCenter() {	 return m_vCenter;	}

	// This returns the triangle count stored in this node
	int GetTriangleCount()  {   return m_TriangleCount;	}

	// This returns the widht of this node (since it's a cube the height and depth are the same)
	float GetWidth() {	 return m_Width;	}

		// This returns if this node is subdivided or not
	bool IsSubDivided()  {   return m_bSubDivided;	}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This sets the initial width, height and depth for the whole scene
	void GetSceneDimensions(t3DModel *pWorld);

	// This returns the number of polygons in our entire scene
	int GetSceneTriangleCount(t3DModel *pWorld);

	// This returns this nodes display list ID
	int GetDisplayListID()		{   return m_DisplayListID;		}

	// This sets the nodes display list ID
	void SetDisplayListID(int displayListID)	{	m_DisplayListID = displayListID;  }

	// This adds the object index to our end node's list of object indices
	void AddObjectIndexToList(int index);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// This takes in the previous nodes center, width and which node ID that will be subdivided
	CVector3 GetNewNodeCenter(CVector3 vCenter, float width, int nodeID);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This subdivides a node depending on the triangle count and node width
	void CreateNode(t3DModel *pWorld, int numberOfTriangles, CVector3 vCenter, float width);

	// This cleans up the new subdivided node creation process, so our code isn't HUGE!
	void CreateNewNode(t3DModel *pWorld,	vector<tFaceList> pList, int triangleCount,
				  	   CVector3 vCenter,	float width,			 int nodeID);

	// Once we are finished subdividing we need to assign the face indices to the end node.
	void AssignTrianglesToNode(t3DModel *pWorld, int numberOfTriangles);

	// This goes through each of the nodes and then draws the end nodes vertices.
	// This function should be called by starting with the root node.  Now it uses display lists.
	void DrawOctree(COctree *pNode, t3DModel *pRootWorld);

	// This recursively creates a display list ID for every end node in the octree
	void CreateDisplayList(COctree *pNode, t3DModel *pRootWorld, int displayListOffset);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


private:

	// This tells us if we have divided this node into more sub nodes
	bool m_bSubDivided;

	// This is the size of the cube for this current node
	float m_Width;

	// This holds the amount of triangles stored in this node
	int m_TriangleCount;

	// This is the center (X, Y, Z) point in this node
	CVector3 m_vCenter;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This holds all the scene information (verts, normals, texture info, etc..) for this node
	t3DModel *m_pWorld;

	// This stores the indices into the original model's object list
	vector<int>	m_pObjectList;

	// This holds the display list ID for the current node, which increases the rendering speed
	int m_DisplayListID;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// These are the eight nodes branching down from this current node
	COctree *m_pOctreeNodes[8];	
};


// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

// This returns the magnitude of a vector
float Magnitude(CVector3 vNormal);

// This returns a normalized vector
CVector3 Normalize(CVector3 vVector);


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file added the new tFaceList structure, along with a couple functions
// and member variables to the COctree class.  Most of the functions were changed
// to handle a t3DModel instead of just pure vertices.  Since we are using display
// lists, a bunch of display list data and functions were added.  
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
//
