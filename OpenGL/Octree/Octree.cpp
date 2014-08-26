//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Octree											 //
//																		 //
//		$Description:	Demonstrates octree space partitioning			 //
//																		 //
//***********************************************************************//

// Include our associate .h file
#include "Octree.h"


// This file contains all the code for our debug and octree classes.  Just so
// we could visualize the subdivision of the octree, I created a class that draws
// wire frame cubes and lines.  A new cube is added to our list of points each time a 
// new node is created.  The cubes are yellow.  You will notice that there is only one
// cube surrounding the terrain in the beginning.  If you press '+' 8 more will be
// created.  Of course, when really using an octree you would not have these.  It
// is created just for the tutorial, so don't assume when you have many nodes that it
// will slow down the game.  It's because you have tons of lines being draw to represent
// the subdivision :)  It's good to note that the line list is created with an STL vector.
// Check out our STL vector tutorial at www.GameTutorials.com if you need help on these.
// Don't worry about the debug class if you don't understand any of it, it's not important.

// Extern our debug object because we use it in the octree code
extern CDebug g_Debug;

// The current amount of subdivisions we are currently at.
// This is used to make sure we don't go over the max amount
int g_CurrentSubdivision = 0;

///////////////////////////////// RENDER DEBUG LINES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This goes through all of the lines that we stored in our list and draws them
/////
///////////////////////////////// RENDER DEBUG LINES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CDebug::RenderDebugLines()				// This renders all of the lines
{
	glDisable(GL_LIGHTING);					// Turn OFF lighting so the debug lines are bright yellow

	glBegin(GL_LINES);						// Start rendering lines

		glColor3ub(255, 255, 0);			// Turn the lines yellow

		// Go through the whole list of lines stored in the vector m_vLines.
		for(unsigned int i = 0; i < m_vLines.size(); i++)
		{
			// Pass in the current point to be rendered as part of a line
			glVertex3f(m_vLines[i].x, m_vLines[i].y, m_vLines[i].z);
		}	

	glEnd();								// Stop rendering lines

	// If we have lighting turned on, turn the lights back on
	if(g_bLighting) 
		glEnable(GL_LIGHTING);
}

///////////////////////////////// ADD DEBUG LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This adds a debug LINE to the stack of lines
/////
///////////////////////////////// ADD DEBUG LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CDebug::AddDebugLine(CVector3 vPoint1, CVector3 vPoint2)
{
	// Add the 2 points that make up the line into our line list.
	m_vLines.push_back(vPoint1);
	m_vLines.push_back(vPoint2);
}


///////////////////////////////// ADD DEBUG RECTANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This adds a debug RECTANGLE to the stack of lines
/////
///////////////////////////////// ADD DEBUG RECTANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CDebug::AddDebugRectangle(CVector3 vCenter, float width, float height, float depth)
{
	// So we can work with the code better, we divide the dimensions in half.
	// That way we can create the cube from the center outwards.
	width /= 2.0f;	height /= 2.0f;	depth /= 2.0f;

	// Below we create all the 8 points so it will be easier to input the lines
	// of the cube.  With the dimensions we calculate the points.
	CVector3 vTopLeftFront( vCenter.x - width, vCenter.y + height, vCenter.z + depth);
	CVector3 vTopLeftBack(  vCenter.x - width, vCenter.y + height, vCenter.z - depth);
	CVector3 vTopRightBack( vCenter.x + width, vCenter.y + height, vCenter.z - depth);
	CVector3 vTopRightFront(vCenter.x + width, vCenter.y + height, vCenter.z + depth);

	CVector3 vBottom_LeftFront( vCenter.x - width, vCenter.y - height, vCenter.z + depth);
	CVector3 vBottom_LeftBack(  vCenter.x - width, vCenter.y - height, vCenter.z - depth);
	CVector3 vBottomRightBack( vCenter.x + width, vCenter.y - height, vCenter.z - depth);
	CVector3 vBottomRightFront(vCenter.x + width, vCenter.y - height, vCenter.z + depth);

	////////// TOP LINES ////////// 

	// Store the top front line of the box
	m_vLines.push_back(vTopLeftFront);		m_vLines.push_back(vTopRightFront);

	// Store the top back line of the box
	m_vLines.push_back(vTopLeftBack);  		m_vLines.push_back(vTopRightBack);

	// Store the top left line of the box
	m_vLines.push_back(vTopLeftFront);		m_vLines.push_back(vTopLeftBack);

	// Store the top right line of the box
	m_vLines.push_back(vTopRightFront);		m_vLines.push_back(vTopRightBack);

	////////// BOTTOM LINES ////////// 

	// Store the bottom front line of the box
	m_vLines.push_back(vBottom_LeftFront);	m_vLines.push_back(vBottomRightFront);

	// Store the bottom back line of the box
	m_vLines.push_back(vBottom_LeftBack);	m_vLines.push_back(vBottomRightBack);

	// Store the bottom left line of the box
	m_vLines.push_back(vBottom_LeftFront);	m_vLines.push_back(vBottom_LeftBack);

	// Store the bottom right line of the box
	m_vLines.push_back(vBottomRightFront);	m_vLines.push_back(vBottomRightBack);

	////////// SIDE LINES ////////// 

	// Store the bottom front line of the box
	m_vLines.push_back(vTopLeftFront);		m_vLines.push_back(vBottom_LeftFront);

	// Store the back left line of the box
	m_vLines.push_back(vTopLeftBack);		m_vLines.push_back(vBottom_LeftBack);

	// Store the front right line of the box
	m_vLines.push_back(vTopRightBack);		m_vLines.push_back(vBottomRightBack);

	// Store the front left line of the box
	m_vLines.push_back(vTopRightFront);		m_vLines.push_back(vBottomRightFront);
}


///////////////////////////////// CLEAR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This clears all of the debug lines
/////
///////////////////////////////// CLEAR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CDebug::Clear()						
{
	// Destroy the list using the standard vector clear() function
	m_vLines.clear();
}


//-------------------------------------------------------------------------\\ 


///////////////////////////////// OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	The COctree contstructor which calls our init function
/////
///////////////////////////////// OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

COctree::COctree()
{
	// We didn't just put the init code in here because it will allow us
	// to destroy the octree and initialize the octree without having to
	// create a new instance.  The same goes for our deconstructor

	// Initialize the data members
	InitOctree();	
}


///////////////////////////////// ~OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	The COctree destructor which calls our destroy function
/////
///////////////////////////////// ~OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

COctree::~COctree()
{
	// Call our destroy function
	DestroyOctree();
}


///////////////////////////////// INIT OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This initialize our octree data members
/////
///////////////////////////////// INIT OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void COctree::InitOctree()
{
	// Set the subdivided flag to false
	m_bSubDivided = false;

	// Set the dimensions of the box to false
	m_Width = 0; 

	// Initialize the triangle count
	m_TriangleCount = 0;

	// Initialize the center of the box to the 0
	m_vCenter = CVector3(0, 0, 0);

	// Set the triangle list to NULL
	m_pVertices = NULL;

	// Set the sub nodes to NULL
	memset(m_pOctreeNodes, 0, sizeof(m_pOctreeNodes));	
}


///////////////////////////////// OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets our initial width of the scene, as well as our center point
/////
///////////////////////////////// OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void COctree::GetSceneDimensions(CVector3 *pVertices, int numberOfVerts)
{
	// We pass in the list of vertices and the vertex count to get the
	// center point and width of the whole scene.  We use this information
	// to subdivide our octree.  Eventually, in the next tutorial it won't
	// just be a list of vertices, but another structure that holds all the
	// normals and texture information.  It's easy to do once you understand vertices.

	// Initialize some temporary variables to hold the max dimensions found
	float maxWidth = 0, maxHeight = 0, maxDepth = 0;

	// Return from this function if we passed in bad data
	if(!pVertices || numberOfVerts <= 0) return;

	// Below we calculate the center point of the scene.  To do this, all you
	// need to do is add up ALL the vertices, then divide that total by the
	// number of vertices added up.  So all the X's get added up together, then Y's, etc..
	// This doesn't mean in a single number, but 3 separate floats (totalX, totalY, totalZ).
	// Notice that we are adding 2 vectors together.  If you look in the CVector3 class
	// I overloaded the + and - operator to handle it correctly.  It cuts down on code
	// instead of added the x, then the y, then the z separately.  If you don't want
	// to use operator overloading just make a function called CVector AddVector(), etc...

	// Go through all of the vertices and add them up to eventually find the center
	for(int i = 0; i < numberOfVerts; i++)
	{
		// Add the current vertex to the center variable (Using operator overloading)
		m_vCenter = m_vCenter + pVertices[i];
	}

	// Divide the total by the number of vertices to get the center point.
	// We could have overloaded the / symbol but I chose not to because we rarely use it.
	m_vCenter.x /= numberOfVerts;
	m_vCenter.y /= numberOfVerts;	
	m_vCenter.z /= numberOfVerts;

	// Now that we have the center point, we want to find the farthest distance from
	// our center point.  That will tell us how big the width of the first node is.
	// Once we get the farthest height, width and depth, we then check them against each
	// other.  Which ever one is higher, we then use that value for the cube width.

	// Go through all of the vertices and find the max dimensions
	for(int i = 0; i < numberOfVerts; i++)
	{
		// Get the current dimensions for this vertex.  We use the fabsf() function
		// to get the floating point absolute value because it might return a negative number.
		float currentWidth  = fabsf(pVertices[i].x - m_vCenter.x);	
		float currentHeight = fabsf(pVertices[i].y - m_vCenter.y);		
		float currentDepth  = fabsf(pVertices[i].z - m_vCenter.z);	

		// Check if the current width value is greater than the max width stored.
		if(currentWidth  > maxWidth)	maxWidth  = currentWidth;

		// Check if the current height value is greater than the max height stored.
		if(currentHeight > maxHeight)	maxHeight = currentHeight;

		// Check if the current depth value is greater than the max depth stored.
		if(currentDepth > maxDepth)		maxDepth  = currentDepth;
	}

	// Set the member variable dimensions to the max ones found.
	// We multiply the max dimensions by 2 because this will give us the
	// full width, height and depth.  Otherwise, we just have half the size
	// because we are calculating from the center of the scene.
	maxWidth *= 2;		maxHeight *= 2;		maxDepth *= 2;

	// Check if the width is the highest value and assign that for the cube dimension
	if(maxWidth > maxHeight && maxWidth > maxDepth)
		m_Width = maxWidth;

	// Check if the height is the heighest value and assign that for the cube dimension
	else if(maxHeight > maxWidth && maxHeight > maxDepth)
		m_Width = maxHeight;

	// Else it must be the depth or it's the same value as some of the other ones
	else
		m_Width = maxDepth;
}


///////////////////////////////// GET NEW NODE CENTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the center point of the new subdivided node, depending on the ID
/////
///////////////////////////////// GET NEW NODE CENTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 COctree::GetNewNodeCenter(CVector3 vCenter, float width, int nodeID)
{
	// I created this function which takes an enum ID to see which node's center
	// we need to calculate.  Once we find that we need to subdivide a node we find
	// the centers of each of the 8 new nodes.  This is what that function does.
	// We just tell it which node we want.

	// Initialize the new node center
	CVector3 vNodeCenter(0, 0, 0);

	// Create a dummy variable to cut down the code size
	CVector3 vCtr = vCenter;

	// Switch on the ID to see which subdivided node we are finding the center
	switch(nodeID)							
	{
		case TOP_LEFT_FRONT:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x - width/4, vCtr.y + width/4, vCtr.z + width/4);
			break;

		case TOP_LEFT_BACK:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x - width/4, vCtr.y + width/4, vCtr.z - width/4);
			break;

		case TOP_RIGHT_BACK:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x + width/4, vCtr.y + width/4, vCtr.z - width/4);
			break;

		case TOP_RIGHT_FRONT:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x + width/4, vCtr.y + width/4, vCtr.z + width/4);
			break;

		case BOTTOM_LEFT_FRONT:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x - width/4, vCtr.y - width/4, vCtr.z + width/4);
			break;

		case BOTTOM_LEFT_BACK:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x - width/4, vCtr.y - width/4, vCtr.z - width/4);
			break;

		case BOTTOM_RIGHT_BACK:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x + width/4, vCtr.y - width/4, vCtr.z - width/4);
			break;

		case BOTTOM_RIGHT_FRONT:
			// Calculate the center of this new node
			vNodeCenter = CVector3(vCtr.x + width/4, vCtr.y - width/4, vCtr.z + width/4);
			break;
	}

	// Return the new node center
	return vNodeCenter;
}


///////////////////////////////// CREATE NEW NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This figures out the new node information and then passes it into CreateNode()
/////
///////////////////////////////// CREATE NEW NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void COctree::CreateNewNode(CVector3 *pVertices, vector<bool> pList, int numberOfVerts,
					  	    CVector3 vCenter,	 float width,        int triangleCount, int nodeID)
{
	// This function helps us set up the new node that is being created.  We only
	// want to create a new node if it found triangles in it's area.  If there were
	// no triangle found in this node's cube, then we ignore it and don't create a node.

	// Check if the first node found some triangles in it
	if(triangleCount)		
	{
		// Allocate memory for the triangles found in this node (tri's * 3 for vertices)
		CVector3 *pNodeVertices = new CVector3 [triangleCount * 3];

		// Create an counter to count the current index of the new node vertices
		int index = 0;

		// Go through all the vertices and assign the vertices to the node's list
		for(int i = 0; i < numberOfVerts; i++)
		{
			// If this current triangle is in the node, assign it's vertices to it
			if(pList[i / 3])	
			{
				pNodeVertices[index] = pVertices[i];
				index++;
			}
		}

		// Now comes the initialization of the node.  First we allocate memory for
		// our node and then get it's center point.  Depending on the nodeID, 
		// GetNewNodeCenter() knows which center point to pass back (TOP_LEFT_FRONT, etc..)

		// Allocate a new node for this octree
		m_pOctreeNodes[nodeID] = new COctree;

		// Get the new node's center point depending on the nodexIndex (which of the 8 subdivided cubes).
		CVector3 vNodeCenter = GetNewNodeCenter(vCenter, width, nodeID);
		
		// Below, before and after we recurse further down into the tree, we keep track
		// of the level of subdivision that we are in.  This way we can restrict it.

		// Increase the current level of subdivision
		g_CurrentSubdivision++;

		// Recurse through this node and subdivide it if necessary
		m_pOctreeNodes[nodeID]->CreateNode(pNodeVertices, triangleCount * 3, vNodeCenter, width / 2);

		// Decrease the current level of subdivision
		g_CurrentSubdivision--;

		// Free the allocated vertices for the triangles found in this node
		delete [] pNodeVertices;
	}
}


///////////////////////////////// CREATE NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our recursive function that goes through and subdivides our nodes
/////
///////////////////////////////// CREATE NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void COctree::CreateNode(CVector3 *pVertices, int numberOfVerts, CVector3 vCenter, float width)
{
	// This is our main function that creates the octree.  We will recurse through
	// this function until we finish subdividing.  Either this will be because we
	// subdivided too many levels or we divided all of the triangles up.

	// Create a variable to hold the number of triangles
	int numberOfTriangles = numberOfVerts / 3;

	// Initialize this node's center point.  Now we know the center of this node.
	m_vCenter = vCenter;

	// Initialize this nodes cube width.  Now we know the width of this current node.
	m_Width = width;

	// Add the current node to our debug rectangle list so we can visualize it.
	// We can now see this node visually as a cube when we render the rectangles.
	// Since it's a cube we pass in the width for width, height and depth.
	g_Debug.AddDebugRectangle(vCenter, width, width, width);

	// Check if we have too many triangles in this node and we haven't subdivided
	// above our max subdivisions.  If so, then we need to break this node into
	// 8 more nodes (hence the word OCTree).  Both must be true to divide this node.
	if( (numberOfTriangles > g_MaxTriangles) && (g_CurrentSubdivision < g_MaxSubdivisions) )
	{
		// Since we need to subdivide more we set the divided flag to true.
		// This let's us know that this node does NOT have any vertices assigned to it,
		// but nodes that perhaps have vertices stored in them (Or their nodes, etc....)
		// We will querey this variable when we are drawing the octree.
		m_bSubDivided = true;

		// Create a list for each new node to store if a triangle should be stored in it's
		// triangle list.  For each index it will be a true or false to tell us if that triangle
		// is in the cube of that node.  Below we check every point to see where it's
		// position is from the center (I.E. if it's above the center, to the left and 
		// back it's the TOP_LEFT_BACK node).  Depending on the node we set the pList 
		// index to true.  This will tell us later which triangles go to which node.
		// You might catch that this way will produce doubles in some nodes.  Some
		// triangles will intersect more than 1 node right?  We won't split the triangles
		// in this tutorial just to keep it simple, but the next tutorial we will.

		// Create the list of booleans for each triangle index
		vector<bool> pList1(numberOfTriangles);		// TOP_LEFT_FRONT node list
		vector<bool> pList2(numberOfTriangles);		// TOP_LEFT_BACK node list
		vector<bool> pList3(numberOfTriangles);		// TOP_RIGHT_BACK node list
		vector<bool> pList4(numberOfTriangles);		// TOP_RIGHT_FRONT node list
		vector<bool> pList5(numberOfTriangles);		// BOTTOM_LEFT_FRONT node list
		vector<bool> pList6(numberOfTriangles);		// BOTTOM_LEFT_BACK node list
		vector<bool> pList7(numberOfTriangles);		// BOTTOM_RIGHT_BACK node list
		vector<bool> pList8(numberOfTriangles);		// BOTTOM_RIGHT_FRONT node list
	
		// Create this variable to cut down the thickness of the code below (easier to read)
		CVector3 vCtr = vCenter;

		// Go through all of the vertices and check which node they belong too.  The way
		// we do this is use the center of our current node and check where the point
		// lies in relationship to the center.  For instance, if the point is 
		// above, left and back from the center point it's the TOP_LEFT_BACK node.
		// You'll see we divide by 3 because there are 3 points in a triangle.
		// If the vertex index 0 and 1 are in a node, 0 / 3 and 1 / 3 is 0 so it will
		// just set the 0'th index to TRUE twice, which doesn't hurt anything.  When
		// we get to the 3rd vertex index of pVertices[] it will then be checking the
		// 1st index of the pList*[] array.  We do this because we want a list of the
		// triangles in the node, not the vertices.
		for(int i = 0; i < numberOfVerts; i++)
		{
			// Create some variables to cut down the thickness of the code (easier to read)
			CVector3 vPoint = pVertices[i];

			// Check if the point lines within the TOP LEFT FRONT node
			if( (vPoint.x <= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z >= vCtr.z) ) 
				pList1[i / 3] = true;

			// Check if the point lines within the TOP LEFT BACK node
			if( (vPoint.x <= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z <= vCtr.z) ) 
				pList2[i / 3] = true;

			// Check if the point lines within the TOP RIGHT BACK node
			if( (vPoint.x >= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z <= vCtr.z) ) 
				pList3[i / 3] = true;

			// Check if the point lines within the TOP RIGHT FRONT node
			if( (vPoint.x >= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z >= vCtr.z) ) 
				pList4[i / 3] = true;

			// Check if the point lines within the BOTTOM LEFT FRONT node
			if( (vPoint.x <= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z >= vCtr.z) ) 
				pList5[i / 3] = true;

			// Check if the point lines within the BOTTOM LEFT BACK node
			if( (vPoint.x <= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z <= vCtr.z) ) 
				pList6[i / 3] = true;

			// Check if the point lines within the BOTTOM RIGHT BACK node
			if( (vPoint.x >= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z <= vCtr.z) ) 
				pList7[i / 3] = true;

			// Check if the point lines within the BOTTOM RIGHT FRONT node
			if( (vPoint.x >= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z >= vCtr.z) ) 
				pList8[i / 3] = true;
		}	

		// Here we create a variable for each list that holds how many triangles
		// were found for each of the 8 subdivided nodes.
		int triCount1 = 0;	int triCount2 = 0;	int triCount3 = 0;	int triCount4 = 0;
		int triCount5 = 0;	int triCount6 = 0;	int triCount7 = 0;	int triCount8 = 0;
		
		// Go through each of the lists and increase the triangle count for each node.
		for(int i = 0; i < numberOfTriangles; i++)  
		{
			// Increase the triangle count for each node that has a "true" for the index i.
			if(pList1[i])	triCount1++;	if(pList2[i])	triCount2++;
			if(pList3[i])	triCount3++;	if(pList4[i])	triCount4++;
			if(pList5[i])	triCount5++;	if(pList6[i])	triCount6++;
			if(pList7[i])	triCount7++;	if(pList8[i])	triCount8++;
		}
	
		// Next we do the dirty work.  We need to set up the new nodes with the triangles
		// that are assigned to each node, along with the new center point of the node.
		// Through recursion we subdivide this node into 8 more nodes.

		// Create the subdivided nodes if necessary and then recurse through them.
		// The information passed into CreateNewNode() are essential for creating the
		// new nodes.  We pass the 8 ID's in so it knows how to calculate it's new center.
		CreateNewNode(pVertices, pList1, numberOfVerts, vCenter, width, triCount1, TOP_LEFT_FRONT);
		CreateNewNode(pVertices, pList2, numberOfVerts, vCenter, width, triCount2, TOP_LEFT_BACK);
		CreateNewNode(pVertices, pList3, numberOfVerts, vCenter, width, triCount3, TOP_RIGHT_BACK);
		CreateNewNode(pVertices, pList4, numberOfVerts, vCenter, width, triCount4, TOP_RIGHT_FRONT);
		CreateNewNode(pVertices, pList5, numberOfVerts, vCenter, width, triCount5, BOTTOM_LEFT_FRONT);
		CreateNewNode(pVertices, pList6, numberOfVerts, vCenter, width, triCount6, BOTTOM_LEFT_BACK);
		CreateNewNode(pVertices, pList7, numberOfVerts, vCenter, width, triCount7, BOTTOM_RIGHT_BACK);
		CreateNewNode(pVertices, pList8, numberOfVerts, vCenter, width, triCount8, BOTTOM_RIGHT_FRONT);
	}
	else
	{
		// If we get here we must either be subdivided past our max level, or our triangle
		// count went below the minimum amount of triangles so we need to store them.
		
		// Assign the vertices to this node since we reached the end node.
		// This will be the end node that actually gets called to be drawn.
		// We just pass in the vertices and vertex count to be assigned to this node.
		AssignVerticesToNode(pVertices, numberOfVerts);
	}
}


//////////////////////////// ASSIGN VERTICES TO NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This allocates memory for the vertices to assign to the current end node
/////
//////////////////////////// ASSIGN VERTICES TO NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void COctree::AssignVerticesToNode(CVector3 *pVertices, int numberOfVerts)
{
	// Since we did not subdivide this node we want to set our flag to false
	m_bSubDivided = false;

	// Initialize the triangle count of this end node (total verts / 3 = total triangles)
	m_TriangleCount = numberOfVerts / 3;

	// Allocate enough memory to hold the needed vertices for the triangles
	m_pVertices = new CVector3 [numberOfVerts];

	// Initialize the vertices to 0 before we copy the data over to them
	memset(m_pVertices, 0, sizeof(CVector3) * numberOfVerts);

	// Copy the passed in vertex data over to our node vertice data
	memcpy(m_pVertices, pVertices, sizeof(CVector3) * numberOfVerts);

	// Increase the amount of end nodes created (Nodes with vertices stored)
	g_EndNodeCount++;
}

// *Note* The code below was thrown in to calculate the normals of the triangles
// that we are displaying.  Instead of complicating the tutorial with separating
// the normals too, I decided to just calculate face normals on the fly so we can
// the depth of the terrain better when we have lighting turned on. 


/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a perpendicular vector from 2 given vectors by taking the cross product.
/////
/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
												
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;	

	// Calculate the cross product with the non communitive equation
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// Return the cross product
	return vNormal;										 
}


/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a vector
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVector3 vNormal)
{
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2) : Where V is the vector
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a normalize vector (A vector exactly of length 1)
/////
/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Normalize(CVector3 vVector)
{
	// Get the magnitude of our normal
	float magnitude = Magnitude(vVector);				

	// Now that we have the magnitude, we can divide our vector by that magnitude.
	// That will make our vector a total length of 1.  
	vVector = vVector / magnitude;		
	
	// Finally, return our normalized vector
	return vVector;										
}


//////////////////////////////// DRAW OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function recurses through all the nodes and draws the end node's vertices
/////
//////////////////////////////// DRAW OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void COctree::DrawOctree(COctree *pNode)
{
	// We should already have the octree created before we call this function.
	// This goes through all nodes until it reaches their ends and draws the
	// vertices stored in those end nodes.  Before we draw a node we check to
	// make sure it is a subdivided node (from m_bSubdivided).  If it is, then
	// we haven't reaches the end and we need to keep recursing through the tree.
	// Once we get to a node that isn't subdivided we draws it's vertices.

	// Make sure a valid node was passed in, otherwise go back to the last node
	if(!pNode) return;

	// Check if this node is subdivided. If so, then we need to recurse and draw it's nodes
	if(pNode->IsSubDivided())
	{
		// Recurse to the bottom of these nodes and draw the end node's vertices
		// Like creating the octree, we need to recurse through each of the 8 nodes.
		DrawOctree(pNode->m_pOctreeNodes[TOP_LEFT_FRONT]);
		DrawOctree(pNode->m_pOctreeNodes[TOP_LEFT_BACK]);
		DrawOctree(pNode->m_pOctreeNodes[TOP_RIGHT_BACK]);
		DrawOctree(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT]);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT]);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK]);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK]);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT]);
	}
	else
	{
		// Make sure we have valid vertices assigned to this node
		if(!pNode->m_pVertices) return;

		// Render using triangles
		glBegin(GL_TRIANGLES);

		// Turn the polygons green
		glColor3ub(0, 255, 0);

		// Store the vertices in a local pointer to keep code more clean
		CVector3 *pVertices = pNode->m_pVertices;

		// Go through all of the vertices (the number of triangles * 3)
		for(int i = 0; i < pNode->GetTriangleCount() * 3; i += 3)
		{
			// Before we render the vertices we want to calculate the face normal
			// of the current polygon.  That way when lighting is turned on we can
			// see the definition of the terrain more clearly.  In reality you wouldn't do this.
			
			// Here we get a vector from each side of the triangle
			CVector3 vVector1 = pVertices[i + 1] - pVertices[i];
			CVector3 vVector2 = pVertices[i + 2] - pVertices[i];

			// Then we need to get the cross product of those 2 vectors (The normal's direction)
			CVector3 vNormal = Cross(vVector1, vVector2);

			// Now we normalize the normal so it is a unit vector (length of 1)
			vNormal = Normalize(vNormal);

			// Pass in the normal for this triangle so we can see better depth in the scene
			glNormal3f(vNormal.x, vNormal.y, vNormal.z);

			// Render the first point in the triangle
			glVertex3f(pVertices[i].x, pVertices[i].y, pVertices[i].z);

			// Render the next point in the triangle
			glVertex3f(pVertices[i + 1].x, pVertices[i + 1].y, pVertices[i + 1].z);

			// Render the last point in the triangle to form the current triangle
			glVertex3f(pVertices[i + 2].x, pVertices[i + 2].y, pVertices[i + 2].z);
		}

		// Quit Drawing
		glEnd();
	}
}


//////////////////////////////// DESTROY OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This frees the memory allocated in the octree
/////
//////////////////////////////// DESTROY OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void COctree::DestroyOctree()
{
	// Free the triangle data if it's not NULL
	if( m_pVertices )
	{
		delete[] m_pVertices;	
		m_pVertices = NULL;
	}

	// Go through all of the nodes and free them if they were allocated
	for(int i = 0; i < 8; i++)
	{
		// Make sure this node is valid
		if(m_pOctreeNodes[i])
		{
			// Free this array index.  This will call the deconstructor which will
			// free the octree data correctly.  This allows us to forget about it's clean up
			delete m_pOctreeNodes[i];
			m_pOctreeNodes[i] = NULL;
		}
	}

	// Initialize the octree data members
	InitOctree();
}	


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// At first an octree (or any space partitioning) can be intimidating.  Once you
// sit down and start coding it, you become to fully understand the wonderful
// benefits and it's simplicity.  If you can create a tree with nodes, you can
// create an octree.  The only complicated part at first might be the splitting
// of polygons that intersect more than one node.  We don't do that in this tutorial
// so it didn't get to complicated and overhauled with code.  It's way to frustrating
// to learn a ton of things at once, instead of the main idea and basic functionality.
// This octree will work fine, except there will possibly be some doubling of vertices
// because we don't split them into separate one.  That is fine, but not idea for a real
// product.  Eventually you want to fix this.  The last octree tutorial will read in
// a fully textured world that will be split correctly.  Also a more modular octree class
// will be created because we are missing UV coordinates, normals, material ID's, and
// more importantly frustum culling.  The next tutorial will tackle the frustum culling.
//
// Let me explain once more briefly the intent of an octree is.  If you have a huge world
// with 10's of thousands of triangles, you don't want to do a for loop and pass those
// all into OpenGL to be rendered.  You only want to pass in the triangles that you can
// see (the camera's view).  To do this, you want to subdivide your world into cubes
// that holds the triangle data for that area/region of the world.  Then, instead of
// checking if every triangle is in your view, you can just check if a cube intersects
// your frustum, meaning it's in the view of your camera.  This is one of the fastest
// way to do this type of space partitioning.  Just one subdivision puts the world
// into 8 cubes which could cut your triangle rendered count down to 1/8 of what it was.
// If just one subdivision could do that, think about 2, 3 or 4 levels of subdivision?
//
// Let's go over the steps to using this octree class.  Once you have your list of vertices
// you can begin.  The first thing you want to do is call this function:
//
// GetSceneDimensions(g_pVertices, g_NumberOfVerts);
//
// This passes in the list of vertices and the vertex count.  This function calculates
// the center point of the world, as well as the width of the cube that is needed to
// create the first node.  Once we have these values stored, we want to create our tree.
// We can call CreateNode() and pass in the starting data for the first node:
//
// CreateNode(g_pVertices, g_NumberOfVerts, g_Octree.GetCenter(), g_Octree.GetWidth());
//
// This takes the vertices, the vertex count, then the starting center and width.
// This will recurse through and create the whole tree, while assigning the vertices
// to the end nodes.  That's it!  You just created the octree.  The last part is drawing
// the octree.  This is simple, you just call:
//
// DrawOctree();
//
// This should be called in your RenderScene() function.  This goes through all the
// nodes until it gets to the end, then it draws the vertices assigned to those end nodes.
// The deconstructor takes care of the clean up by calling DestroyOctree().
//
// The CDebug class has nothing to do with the octree other than the fact that we use
// it to display the nodes visually.  I suggest you keep something like this around so
// you can visualize your subdivided world and get a better idea of how efficient it is.
//
// Be sure to check out the HTML tutorial on the concept of an octree.  It will give
// good visuals on the whole subdivision process so you can get a better grasp of the
// concepts.  That's about it, so enjoy this tutorial and please let us know what you
// do with it.  We are always curious to know where the knowledge goes. :)
//
// Remember, this tutorial is 1 of a 3 part series.  We still have not delved into the
// frustum culling and polygon splitting part of the octree.  Right now your octree does 
// you no good.  This just explains on how to create an octree.  Check out the second 
// tutorial which focuses on the frustum culling.
//
// Ben Humphrey
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
