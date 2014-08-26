//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		MD3 Animation											  //
//																				  //
//		$Description:	Demonstrates animating Quake3 characters with quaternions //
//																				  //
//********************************************************************************//


#include "main.h"
#include "Md3.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////////////////////////////////////////////////////////
//
// This version of the tutorial incorporates the animation data stored in the MD3
// character files.  We will be reading in the .cfg file that stores the animation
// data.  The rotations and translations of the models will be done using a matrix.
// There will be no more calls to glTranslatef().  To create the rotation and
// translation matrix, quaternions will be used.  This is because quaternions
// are excellent for interpolating between 2 rotations, as well as not overriding
// another translation causing "gimbal lock".
// 
// So, why do we need to interpolate?  Well, the animations for the character are
// stored in key frames.  Instead of saving each frame of an animation, key frames
// are stored to cut down on memory and disk space.  The files would be huge if every
// frame was saved for every animation, as well as creating a huge memory footprint.  
// Can you imagine having 10+ models in memory with all of that animation data?  
// 
// The animation key frames are stored in 2 ways.  The torso and legs mesh have vertices
// stored for each of the key frames, along with separate rotations and translations
// for the basic bone animation.  Remember, that each .md3 represents a bone, that needs
// to be connected at a joint.  For instance, the torso is connected to the legs, and the
// head is connected to the torso.  So, that makes 3 bones and 2 joints.  If you add the
// weapon, the weapon is connected to the hand joint, which gives us 4 bones and 3 joints.
// Unlike conventional skeletal animation systems, the main animations of the character's
// movement, such as a gesture or swimming animation, are done not with bones, but with 
// vertex key frames, like in the .md2 format. Since the lower, upper, head and weapon models
// are totally different models, which aren't seamlessly connected to each other, the parent
// node needs to end a message (a translation and rotation) down to all it's child nodes to
// tell them where they need to be in order for the animation to look right.  A good example
// of this is when the legs have the back flip animation.  The legs might kick back into a back
// flip that lands the character on their feet.  Well, since the main models are separate,
// if the legs didn't tell the torso where to go, then the model's torso would stay in the same
// place and the body would detach itself from the legs.  The exporter calculates all this stuff
// for you of course.
// 
// But getting back to the interpolation, since we use key frames, if we didn't interpolate
// between them, the animation would look very jumpy and unnatural.  It would also go too
// fast.  By interpolating, we create a smooth transition between each key frame.
//
// As seen in the .md2 tutorials, interpolating between vertices is easy if we use the
// linear interpolation function:  p(t) = p0 + t(p1 - p0).  The same goes for translations,
// since it's just 2 3D points.  This is not so for the rotations.  The Quake3 character
// stores the rotations for each key frame in a 3x3 matrix.  This isn't a simple linear
// interpolation that needs to be performed.  If we convert the matrices to a quaternion,  
// then use spherical linear interpolation (SLERP) between the current frame's quaternion 
// and the next key frame's quaternion, we will have a new interpolated quaternion that
// can be converted into a 4x4 matrix to be applied to the current model view matrix in OpenGL.
// After finding the interpolated translation to be applied, we can slip that into the rotation
// matrix before it's applied to the current matrix, which will require only one matrix command.
//
// You'll notice that in the CreateFromMatrix() function in our quaternion class, I allow a
// row and column count to be passed in.  This is just a dirty way to allow a 3x3 or 4x4 matrix
// to be passed in.  Instead of creating a whole new function and copy and pasting the main 
// code, it seemed fitting for a tutorial.  It's obvious that the quaternion class is missing
// a tremendous amount of functions, but I chose to only keep the functions that we would use.
// 
// For those of you who don't know what interpolation us, here is a section abstracted 
// from the MD2 Animation tutorial:
//
// -------------------------------------------------------------------------------------
// Interpolation: Gamedev.net's Game Dictionary say interpolation is "using a ratio 
// to step gradually a variable from one value to another."  In our case, this
// means that we gradually move our vertices from one key frame to another key frame.
// There are many types of interpolation, but we are just going to use linear.
// The equation for linear interpolation is this:
//
//				p(t) = p0 + t(p1 - p0)
//
//				t - The current time with 0 being the start and 1 being the end
//				p(t) - The result of the equation with time t
//				p0 - The starting position
//				p1 - The ending position
//
// Let's throw in an example with numbers to test this equation.  If we have
// a vertex stored at 0 along the X axis and we wanted to move the point to
// 10 with 5 steps, see if you can fill in the equation without a time just yet.
//
// Finished?  You should have come up with:
//
//				p(t) = 0 + t(10 - 0)
//				p(t) = 0 + 10t
//				p(t) = 10t
//
// Now, all we need it a time from 0 to 1 to pass in, which will allow us to find any
// point from 0 to 10, depending on the time.  Since we wanted to find out the distance
// we need to travel each frame if we want to reach the end point in 5 steps, we just
// divide 1 by 5: 1/5 = 0.2
//
// We can then pass this into our equation:
//
//				p(0.2) = 10 * 0.2
//				p(0.2) = 2
//
// What does that tell us?  It tells us we need to move the vertex along the x
// axis each frame by a distance of 2 to reach 10 in 5 steps.  Yah, yah, this isn't
// rocket science, but it's important to know that what your mind would have done
// immediately without thinking about it, is linear interpolation.  
//
// Are you starting to see how this applies to our model?  If we only read in key
// frames, then we need to interpolate every vertex between the current and next
// key frame for animation.  To get a perfect idea of what is going on, try
// taking out the interpolation and just render the key frames.  You will notice
// that you can still see what is kinda going on, but it moves at an incredible pace!
// There is not smoothness, just a really fast jumpy animation.
// ------------------------------------------------------------------------------------
//
// Let's jump into the code (hold your breath!)
//
//


////////////////////////////// CREATE MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function converts a quaternion to a rotation matrix
/////
////////////////////////////// CREATE MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*	

void CQuaternion::CreateMatrix(float *pMatrix)
{
	// Make sure the matrix has allocated memory to store the rotation data
	if(!pMatrix) return;
	
	// Fill in the rows of the 4x4 matrix, according to the quaternion to matrix equations

	// First row
	pMatrix[ 0] = 1.0f - 2.0f * ( y * y + z * z );  
	pMatrix[ 1] = 2.0f * ( x * y - w * z );  
	pMatrix[ 2] = 2.0f * ( x * z + w * y );  
	pMatrix[ 3] = 0.0f;  

	// Second row
	pMatrix[ 4] = 2.0f * ( x * y + w * z );  
	pMatrix[ 5] = 1.0f - 2.0f * ( x * x + z * z );  
	pMatrix[ 6] = 2.0f * ( y * z - w * x );  
	pMatrix[ 7] = 0.0f;  

	// Third row
	pMatrix[ 8] = 2.0f * ( x * z - w * y );  
	pMatrix[ 9] = 2.0f * ( y * z + w * x );  
	pMatrix[10] = 1.0f - 2.0f * ( x * x + y * y );  
	pMatrix[11] = 0.0f;  

	// Fourth row
	pMatrix[12] = 0;  
	pMatrix[13] = 0;  
	pMatrix[14] = 0;  
	pMatrix[15] = 1.0f;

	// Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
}


///////////////////////////////// CREATE FROM MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a quaternion from a 3x3 or a 4x4 matrix, depending on rowColumnCount
/////
///////////////////////////////// CREATE FROM MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuaternion::CreateFromMatrix(float *pTheMatrix, int rowColumnCount)
{
	// Make sure the matrix has valid memory and it's not expected that we allocate it.
	// Also, we do a check to make sure the matrix is a 3x3 or a 4x4 (must be 3 or 4).
	if(!pTheMatrix || ((rowColumnCount != 3) && (rowColumnCount != 4))) return;

	// This function is used to take in a 3x3 or 4x4 matrix and convert the matrix
	// to a quaternion.  If rowColumnCount is a 3, then we need to convert the 3x3
	// matrix passed in to a 4x4 matrix, otherwise we just leave the matrix how it is.
	// Since we want to apply a matrix to an OpenGL matrix, we need it to be 4x4.

	// Point the matrix pointer to the matrix passed in, assuming it's a 4x4 matrix
	float *pMatrix = pTheMatrix;

	// Create a 4x4 matrix to convert a 3x3 matrix to a 4x4 matrix (If rowColumnCount == 3)
	float m4x4[16] = {0};

	// If the matrix is a 3x3 matrix (which it is for Quake3), then convert it to a 4x4
	if(rowColumnCount == 3)
	{
		// Set the 9 top left indices of the 4x4 matrix to the 9 indices in the 3x3 matrix.
		// It would be a good idea to actually draw this out so you can visualize it.
		m4x4[0]  = pTheMatrix[0];	m4x4[1]  = pTheMatrix[1];	m4x4[2]  = pTheMatrix[2];
		m4x4[4]  = pTheMatrix[3];	m4x4[5]  = pTheMatrix[4];	m4x4[6]  = pTheMatrix[5];
		m4x4[8]  = pTheMatrix[6];	m4x4[9]  = pTheMatrix[7];	m4x4[10] = pTheMatrix[8];

		// Since the bottom and far right indices are zero, set the bottom right corner to 1.
		// This is so that it follows the standard diagonal line of 1's in the identity matrix.
		m4x4[15] = 1;

		// Set the matrix pointer to the first index in the newly converted matrix
		pMatrix = &m4x4[0];
	}

	// The next step, once we made sure we are dealing with a 4x4 matrix, is to check the
	// diagonal of the matrix.  This means that we add up all of the indices that comprise
	// the standard 1's in the identity matrix.  If you draw out the identity matrix of a
	// 4x4 matrix, you will see that the 1's form a diagonal line.  Notice we just assume
	// that the last index (15) is 1 because it is not effected in the 3x3 rotation matrix.

	// Find the diagonal of the matrix by adding up it's diagonal indices.
	// This is also known as the "trace", but I will call the variable diagonal.
	float diagonal = pMatrix[0] + pMatrix[5] + pMatrix[10] + 1;
	float scale = 0.0f;

	// Below we check if the diagonal is greater than zero.  To avoid accidents with
	// floating point numbers, we substitute 0 with 0.00000001.  If the diagonal is
	// great than zero, we can perform an "instant" calculation, otherwise we will need
	// to identify which diagonal element has the greatest value.  Note, that it appears
	// that %99 of the time, the diagonal IS greater than 0 so the rest is rarely used.

	// If the diagonal is greater than zero
	if(diagonal > 0.00000001)
	{
		// Calculate the scale of the diagonal
		scale = float(sqrt(diagonal ) * 2);

		// Calculate the x, y, x and w of the quaternion through the respective equation
		x = ( pMatrix[9] - pMatrix[6] ) / scale;
		y = ( pMatrix[2] - pMatrix[8] ) / scale;
		z = ( pMatrix[4] - pMatrix[1] ) / scale;
		w = 0.25f * scale;
	}
	else 
	{
		// If the first element of the diagonal is the greatest value
		if ( pMatrix[0] > pMatrix[5] && pMatrix[0] > pMatrix[10] )  
		{	
			// Find the scale according to the first element, and double that value
			scale  = (float)sqrt( 1.0f + pMatrix[0] - pMatrix[5] - pMatrix[10] ) * 2.0f;

			// Calculate the x, y, x and w of the quaternion through the respective equation
			x = 0.25f * scale;
			y = (pMatrix[4] + pMatrix[1] ) / scale;
			z = (pMatrix[2] + pMatrix[8] ) / scale;
			w = (pMatrix[9] - pMatrix[6] ) / scale;	
		} 
		// Else if the second element of the diagonal is the greatest value
		else if ( pMatrix[5] > pMatrix[10] ) 
		{
			// Find the scale according to the second element, and double that value
			scale  = (float)sqrt( 1.0f + pMatrix[5] - pMatrix[0] - pMatrix[10] ) * 2.0f;
			
			// Calculate the x, y, x and w of the quaternion through the respective equation
			x = (pMatrix[4] + pMatrix[1] ) / scale;
			y = 0.25f * scale;
			z = (pMatrix[9] + pMatrix[6] ) / scale;
			w = (pMatrix[2] - pMatrix[8] ) / scale;
		} 
		// Else the third element of the diagonal is the greatest value
		else 
		{	
			// Find the scale according to the third element, and double that value
			scale  = (float)sqrt( 1.0f + pMatrix[10] - pMatrix[0] - pMatrix[5] ) * 2.0f;

			// Calculate the x, y, x and w of the quaternion through the respective equation
			x = (pMatrix[2] + pMatrix[8] ) / scale;
			y = (pMatrix[9] + pMatrix[6] ) / scale;
			z = 0.25f * scale;
			w = (pMatrix[4] - pMatrix[1] ) / scale;
		}
	}
}


/////////////////////////////////////// SLERP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Returns a spherical linear interpolated quaternion between q1 and q2, with respect to t
/////
/////////////////////////////////////// SLERP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuaternion CQuaternion::Slerp(CQuaternion &q1, CQuaternion &q2, float t)
{
	// Create a local quaternion to store the interpolated quaternion
	CQuaternion qInterpolated;

	// This function is the milk and honey of our quaternion code, the rest of
	// the functions are an appendage to what is done here.  Everyone understands
	// the terms, "matrix to quaternion", "quaternion to matrix", "create quaternion matrix",
	// "quaternion multiplication", etc.. but "SLERP" is the stumbling block, even a little 
	// bit after hearing what it stands for, "Spherical Linear Interpolation".  What that
	// means is that we have 2 quaternions (or rotations) and we want to interpolate between 
	// them.  The reason what it's called "spherical" is that quaternions deal with a sphere.  
	// Linear interpolation just deals with 2 points primarily, where when dealing with angles
	// and rotations, we need to use sin() and cos() for interpolation.  If we wanted to use
	// quaternions for camera rotations, which have much more instant and jerky changes in 
	// rotations, we would use Spherical-Cubic Interpolation.  The equation for SLERP is this:
	//
	// q = (((b.a)^-1)^t)a
	//
	// Go here for an a detailed explanation and proofs of this equation:
	//
	// http://www.magic-software.com/Documentation/quat.pdf
	//
	// Now, Let's code it

	// Here we do a check to make sure the 2 quaternions aren't the same, return q1 if they are
	if(q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w) 
		return q1;

	// Following the (b.a) part of the equation, we do a dot product between q1 and q2.
	// We can do a dot product because the same math applied for a 3D vector as a 4D vector.
	float result = (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);

	// If the dot product is less than 0, the angle is greater than 90 degrees
	if(result < 0.0f)
	{
		// Negate the second quaternion and the result of the dot product
		q2 = CQuaternion(-q2.x, -q2.y, -q2.z, -q2.w);
		result = -result;
	}

	// Set the first and second scale for the interpolation
	float scale0 = 1 - t, scale1 = t;

	// Next, we want to actually calculate the spherical interpolation.  Since this
	// calculation is quite computationally expensive, we want to only perform it
	// if the angle between the 2 quaternions is large enough to warrant it.  If the
	// angle is fairly small, we can actually just do a simpler linear interpolation
	// of the 2 quaternions, and skip all the complex math.  We create a "delta" value
	// of 0.1 to say that if the cosine of the angle (result of the dot product) between
	// the 2 quaternions is smaller than 0.1, then we do NOT want to perform the full on 
	// interpolation using.  This is because you won't really notice the difference.

	// Check if the angle between the 2 quaternions was big enough to warrant such calculations
	if(1 - result > 0.1f)
	{
		// Get the angle between the 2 quaternions, and then store the sin() of that angle
		float theta = (float)acos(result);
		float sinTheta = (float)sin(theta);

		// Calculate the scale for q1 and q2, according to the angle and it's sine value
		scale0 = (float)sin( ( 1 - t ) * theta) / sinTheta;
		scale1 = (float)sin( ( t * theta) ) / sinTheta;
	}	

	// Calculate the x, y, z and w values for the quaternion by using a special
	// form of linear interpolation for quaternions.
	qInterpolated.x = (scale0 * q1.x) + (scale1 * q2.x);
	qInterpolated.y = (scale0 * q1.y) + (scale1 * q2.y);
	qInterpolated.z = (scale0 * q1.z) + (scale1 * q2.z);
	qInterpolated.w = (scale0 * q1.w) + (scale1 * q2.w);

	// Return the interpolated quaternion
	return qInterpolated;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// IS IN STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns true if the string strSubString is inside of strString
/////
///////////////////////////////// IS IN STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool IsInString(string strString, string strSubString)
{
	// Make sure both of these strings are valid, return false if any are empty
	if(strString.length() <= 0 || strSubString.length() <= 0) return false;

	// grab the starting index where the sub string is in the original string
	unsigned int index = (int)strString.find(strSubString);

	// Make sure the index returned was valid
	if(index >= 0 && index < strString.length())
		return true;

	// The sub string does not exist in strString.
	return false;
}


///////////////////////////////// CMODEL MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our CModelMD3 constructor
/////
///////////////////////////////// CMODEL MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CModelMD3::CModelMD3()
{
	// Here we initialize all our mesh structures for the character
	memset(&m_Head,  0, sizeof(t3DModel));
	memset(&m_Upper, 0, sizeof(t3DModel));
	memset(&m_Lower, 0, sizeof(t3DModel));
	memset(&m_Weapon, 0, sizeof(t3DModel));
}


///////////////////////////////// ~CMODEL MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our CModelMD3 deconstructor
/////
///////////////////////////////// ~CMODEL MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CModelMD3::~CModelMD3()
{
	// Here we free all of the meshes in our model
	DestroyModel(&m_Head);
	DestroyModel(&m_Upper);
	DestroyModel(&m_Lower);
	DestroyModel(&m_Weapon);
}	


///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This frees our Quake3 model and all it's associated data
/////
///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::DestroyModel(t3DModel *pModel)
{
	// Go through all the objects in the model
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		if(pModel->pObject[i].pFaces)		delete [] pModel->pObject[i].pFaces;
		if(pModel->pObject[i].pNormals)		delete [] pModel->pObject[i].pNormals;
		if(pModel->pObject[i].pVerts)		delete [] pModel->pObject[i].pVerts;
		if(pModel->pObject[i].pTexVerts)	delete [] pModel->pObject[i].pTexVerts;
	}

	// Free the tags associated with this model
	if(pModel->pTags)		delete [] pModel->pTags;

	// Free the links associated with this model (We use free because we used malloc())
	if(pModel->pLinks)		free(pModel->pLinks);
}
	

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// GET MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a specific model from the character (kLower, kUpper, kHead, kWeapon)
/////
///////////////////////////////// GET MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

t3DModel *CModelMD3::GetModel(int whichPart)
{
	// Return the legs model if desired
	if(whichPart == kLower) 
		return &m_Lower;

	// Return the torso model if desired
	if(whichPart == kUpper) 
		return &m_Upper;

	// Return the head model if desired
	if(whichPart == kHead) 
		return &m_Head;

	// Return the weapon model
	return &m_Weapon;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads our Quake3 model from the given path and character name
/////
///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CModelMD3::LoadModel(LPSTR strPath, LPSTR strModel)
{
	char strLowerModel[255] = {0};	// This stores the file name for the lower.md3 model
	char strUpperModel[255] = {0};	// This stores the file name for the upper.md3 model
	char strHeadModel[255]  = {0};	// This stores the file name for the head.md3 model
	char strLowerSkin[255]  = {0};	// This stores the file name for the lower.md3 skin
	char strUpperSkin[255]  = {0};	// This stores the file name for the upper.md3 skin
	char strHeadSkin[255]   = {0};	// This stores the file name for the head.md3 skin
	CLoadMD3 loadMd3;				// This object allows us to load each .md3 and .skin file

	// Make sure valid path and model names were passed in
	if(!strPath || !strModel) return false;

	// Store the correct files names for the .md3 and .skin file for each body part.
	// We concatinate this on top of the path name to be loaded from.
	sprintf(strLowerModel, "%s\\%s_lower.md3", strPath, strModel);
	sprintf(strUpperModel, "%s\\%s_upper.md3", strPath, strModel);
	sprintf(strHeadModel,  "%s\\%s_head.md3",  strPath, strModel);
	
	// Get the skin file names with their path
	sprintf(strLowerSkin, "%s\\%s_lower.skin", strPath, strModel);
	sprintf(strUpperSkin, "%s\\%s_upper.skin", strPath, strModel);
	sprintf(strHeadSkin,  "%s\\%s_head.skin",  strPath, strModel);
	
	// Load the head mesh (*_head.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Head,  strHeadModel))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the HEAD model!", "Error", MB_OK);
		return false;
	}

	// Load the upper mesh (*_head.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Upper, strUpperModel))		
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the UPPER model!", "Error", MB_OK);
		return false;
	}

	// Load the lower mesh (*_lower.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Lower, strLowerModel))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the LOWER model!", "Error", MB_OK);
		return false;
	}

	// Load the lower skin (*_upper.skin) and make sure it loaded properly
	if(!loadMd3.LoadSkin(&m_Lower, strLowerSkin))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the LOWER skin!", "Error", MB_OK);
		return false;
	}

	// Load the upper skin (*_upper.skin) and make sure it loaded properly
	if(!loadMd3.LoadSkin(&m_Upper, strUpperSkin))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the UPPER skin!", "Error", MB_OK);
		return false;
	}

	// Load the head skin (*_head.skin) and make sure it loaded properly
	if(!loadMd3.LoadSkin(&m_Head,  strHeadSkin))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the HEAD skin!", "Error", MB_OK);
		return false;
	}

	// Load the lower, upper and head textures.  
	LoadModelTextures(&m_Lower, strPath);
	LoadModelTextures(&m_Upper, strPath);
	LoadModelTextures(&m_Head,  strPath);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// We added to this function the code that loads the animation config file

	// This stores the file name for the .cfg animation file
	char strConfigFile[255] = {0};	

	// Add the path and file name prefix to the animation.cfg file
	sprintf(strConfigFile,  "%s\\%s_animation.cfg",  strPath, strModel);

	// Load the animation config file (*_animation.config) and make sure it loaded properly
	if(!LoadAnimations(strConfigFile))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the Animation Config File!", "Error", MB_OK);
		return false;
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Link the lower body to the upper body when the tag "tag_torso" is found in our tag array
	LinkModel(&m_Lower, &m_Upper, "tag_torso");

	// Link the upper body to the head when the tag "tag_head" is found in our tag array
	LinkModel(&m_Upper, &m_Head, "tag_head");
		
	// The character was loaded correctly so return true
	return true;
}


///////////////////////////////// LOAD WEAPON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads a Quake3 weapon model from the given path and weapon name
/////
///////////////////////////////// LOAD WEAPON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CModelMD3::LoadWeapon(LPSTR strPath, LPSTR strModel)
{
	char strWeaponModel[255]  = {0};	// This stores the file name for the weapon model
	char strWeaponShader[255] = {0};	// This stores the file name for the weapon shader.
	CLoadMD3 loadMd3;					// This object allows us to load the .md3 and .shader file

	// Make sure the path and model were valid, otherwise return false
	if(!strPath || !strModel) return false;

	// Concatenate the path and model name together
	sprintf(strWeaponModel, "%s\\%s.md3", strPath, strModel);

	// Load the weapon mesh (*.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Weapon,  strWeaponModel))
	{
		// Display the error message that we couldn't find the weapon MD3 file and return false
		MessageBox(g_hWnd, "Unable to load the WEAPON model!", "Error", MB_OK);
		return false;
	}

	// Add the path, file name and .shader extension together to get the file name and path
	sprintf(strWeaponShader, "%s\\%s.shader", strPath, strModel);

	// Load our textures associated with the gun from the weapon shader file
	if(!loadMd3.LoadShader(&m_Weapon, strWeaponShader))
	{
		// Display the error message that we couldn't find the shader file and return false
		MessageBox(g_hWnd, "Unable to load the SHADER file!", "Error", MB_OK);
		return false;
	}

	// We should have the textures needed for each weapon part loaded from the weapon's
	// shader, so let's load them in the given path.
	LoadModelTextures(&m_Weapon, strPath);

	// Link the weapon to the model's hand that has the weapon tag
	LinkModel(&m_Upper, &m_Weapon, "tag_weapon");
		
	// The weapon loaded okay, so let's return true to reflect this
	return true;
}


///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the textures for the current model passed in with a directory
/////
///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::LoadModelTextures(t3DModel *pModel, LPSTR strPath)
{
	// Go through all the materials that are assigned to this model
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(pModel->pMaterials[i].strFile) > 0)
		{
			// Create a boolean to tell us if we have a new texture to load
			bool bNewTexture = true;

			// Go through all the textures in our string list to see if it's already loaded
			for(unsigned int j = 0; j < strTextures.size(); j++)
			{
				// If the texture name is already in our list of texture, don't load it again.
				if(!strcmp(pModel->pMaterials[i].strFile, strTextures[j].c_str()) )
				{
					// We don't need to load this texture since it's already loaded
					bNewTexture = false;

					// Assign the texture index to our current material textureID.
					// This ID will then be used as an index into m_Textures[].
					pModel->pMaterials[i].texureId = j;
				}
			}

			// Make sure before going any further that this is a new texture to be loaded
			if(bNewTexture == false) continue;
			
			char strFullPath[255] = {0};

			// Add the file name and path together so we can load the texture
			sprintf(strFullPath, "%s\\%s", strPath, pModel->pMaterials[i].strFile);

			// We pass in a reference to an index into our texture array member variable.
			// The size() function returns the current loaded texture count.  Initially
			// it will be 0 because we haven't added any texture names to our strTextures list.
			CreateTexture(m_Textures[strTextures.size()], strFullPath);								

			// Set the texture ID for this material by getting the current loaded texture count
			pModel->pMaterials[i].texureId = (int)strTextures.size();

			// Now we increase the loaded texture count by adding the texture name to our
			// list of texture names.  Remember, this is used so we can check if a texture
			// is already loaded before we load 2 of the same textures.  Make sure you
			// understand what an STL vector list is.  We have a tutorial on it if you don't.
			strTextures.push_back(pModel->pMaterials[i].strFile);
		}
	}
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// LOAD ANIMATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the .cfg file that stores all the animation information
/////
///////////////////////////////// LOAD ANIMATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CModelMD3::LoadAnimations(LPSTR strConfigFile)
{
	// This function is given a path and name to an animation config file to load.
	// The implementation of this function is arbitrary, so if you have a better way
	// to parse the animation file, that is just as good.  Whatever works.
	// Basically, what is happening here is that we are grabbing an animation line:
	//
	// "0	31	0	25		// BOTH_DEATH1"
	//
	// Then parsing it's values.  The first number is the starting frame, the next
	// is the frame count for that animation (endFrame would equal startFrame + frameCount),
	// the next is the looping frames (ignored), and finally the frames per second that
	// the animation should run at.  The end of this line is the name of the animation.
	// Once we get that data, we store the information in our tAnimationInfo object, then
	// after we finish parsing the file, the animations are assigned to each model.  
	// Remember, that only the torso and the legs objects have animation.  It is important
	// to note also that the animation prefixed with BOTH_* are assigned to both the legs
	// and the torso animation list, hence the name "BOTH" :)

	// Create an animation object for every valid animation in the Quake3 Character
	tAnimationInfo animations[MAX_ANIMATIONS] = {0};

	// Open the config file
	ifstream fin(strConfigFile);

	// Here we make sure that the file was found and could be opened
	if( fin.fail() )
	{
		// Return an unsuccessful retrieval
		return false;
	}

	string strWord = "";				// This stores the current word we are reading in
	string strLine = "";				// This stores the current line we read in
	int currentAnim = 0;				// This stores the current animation count
	int torsoOffset = 0;				// The offset between the first torso and leg animation

	// Here we go through every word in the file until a numeric number is found.
	// This is how we know that we are on the animation lines, and past header info.
	// This of course isn't the most solid way, but it works fine.  It wouldn't hurt
	// to put in some more checks to make sure no numbers are in the header info.
	while( fin >> strWord)
	{
		// If the first character of the word is NOT a number, we haven't hit an animation line
		if(!isdigit( strWord[0] ))
		{
			// Store the rest of this line and go to the next one
			getline(fin, strLine);
			continue;
		}

		// If we get here, we must be on an animation line, so let's parse the data.
		// We should already have the starting frame stored in strWord, so let's extract it.

		// Get the number stored in the strWord string and create some variables for the rest
		int startFrame = atoi(strWord.c_str());
		int numOfFrames = 0, loopingFrames = 0, framesPerSecond = 0;
		
		// Read in the number of frames, the looping frames, then the frames per second
		// for this current animation we are on.
		fin >> numOfFrames >> loopingFrames >> framesPerSecond;

		// Initialize the current animation structure with the data just read in
		animations[currentAnim].startFrame		= startFrame;
		animations[currentAnim].endFrame		= startFrame + numOfFrames;
		animations[currentAnim].loopingFrames	= loopingFrames;
		animations[currentAnim].framesPerSecond = framesPerSecond;

		// Read past the "//" and read in the animation name (i.e. "BOTH_DEATH1").
		// This might not be how every config file is set up, so make sure.
		fin >> strLine >> strLine;

		// Copy the name of the animation to our animation structure
		strcpy(animations[currentAnim].strName, strLine.c_str());

		// If the animation is for both the legs and the torso, add it to their animation list
		if(IsInString(strLine, "BOTH"))
		{
			// Add the animation to each of the upper and lower mesh lists
			m_Upper.pAnimations.push_back(animations[currentAnim]);
			m_Lower.pAnimations.push_back(animations[currentAnim]);
		}
		// If the animation is for the torso, add it to the torso's list
		else if(IsInString(strLine, "TORSO"))
		{
			m_Upper.pAnimations.push_back(animations[currentAnim]);
		}
		// If the animation is for the legs, add it to the legs's list
		else if(IsInString(strLine, "LEGS"))
		{	
			// Because I found that some config files have the starting frame for the
			// torso and the legs a different number, we need to account for this by finding
			// the starting frame of the first legs animation, then subtracting the starting
			// frame of the first torso animation from it.  For some reason, some exporters
			// might keep counting up, instead of going back down to the next frame after the
			// end frame of the BOTH_DEAD3 anim.  This will make your program crash if so.
			
			// If the torso offset hasn't been set, set it
			if(!torsoOffset)
				torsoOffset = animations[LEGS_WALKCR].startFrame - animations[TORSO_GESTURE].startFrame;

			// Minus the offset from the legs animation start and end frame.
			animations[currentAnim].startFrame -= torsoOffset;
			animations[currentAnim].endFrame -= torsoOffset;

			// Add the animation to the list of leg animations
			m_Lower.pAnimations.push_back(animations[currentAnim]);
		}
	
		// Increase the current animation count
		currentAnim++;
	}	

	// Store the number if animations for each list by the STL vector size() function
	m_Lower.numOfAnimations = (int)m_Lower.pAnimations.size();
	m_Upper.numOfAnimations = (int)m_Upper.pAnimations.size();
	m_Head.numOfAnimations = (int)m_Head.pAnimations.size();
	m_Weapon.numOfAnimations = (int)m_Head.pAnimations.size();

	// Return a success
	return true;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This links the body part models to each other, along with the weapon
/////
///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void  CModelMD3::LinkModel(t3DModel *pModel, t3DModel *pLink, LPSTR strTagName)
{
	// Make sure we have a valid model, link and tag name, otherwise quit this function
	if(!pModel || !pLink || !strTagName) return;

	// Go through all of our tags and find which tag contains the strTagName, then link'em
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// If this current tag index has the tag name we are looking for
		if( !_stricmp(pModel->pTags[i].strName, strTagName) )
		{
			// Link the model's link index to the link (or model/mesh) and return
			pModel->pLinks[i] = pLink;
			return;
		}
	}
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the current frame of animation, depending on it's FPS and t
/////
///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*	

void CModelMD3::UpdateModel(t3DModel *pModel)
{
	// Initialize a start and end frame, for models with no animation
	int startFrame = 0;
	int endFrame   = 1;

	// This function is used to keep track of the current and next frames of animation
	// for each model, depending on the current animation.  Some models down have animations,
	// so there won't be any change.

	// Here we grab the current animation that we are on from our model's animation list
	tAnimationInfo *pAnim = &(pModel->pAnimations[pModel->currentAnim]);

	// If there is any animations for this model
	if(pModel->numOfAnimations)
	{
		// Set the starting and end frame from for the current animation
		startFrame = pAnim->startFrame;
		endFrame   = pAnim->endFrame;
	}
	
	// This gives us the next frame we are going to.  We mod the current frame plus
	// 1 by the current animations end frame to make sure the next frame is valid.
	pModel->nextFrame = (pModel->currentFrame + 1) % endFrame;

	// If the next frame is zero, that means that we need to start the animation over.
	// To do this, we set nextFrame to the starting frame of this animation.
	if(pModel->nextFrame == 0) 
		pModel->nextFrame =  startFrame;

	// Next, we want to get the current time that we are interpolating by.  Remember,
	// if t = 0 then we are at the beginning of the animation, where if t = 1 we are at the end.
	// Anything from 0 to 1 can be thought of as a percentage from 0 to 100 percent complete.
	SetCurrentTime(pModel);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This recursively draws all the character nodes, starting with the legs.
/////
///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::DrawModel()
{
	// Rotate the model to compensate for the z up orientation that the model was saved
	glRotatef(-90, 1, 0, 0);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Since we have animation now, when we draw the model the animation frames need
	// to be updated.  To do that, we pass in our lower and upper models to UpdateModel().
	// There is no need to pass in the head of weapon, since they don't have any animation.

	// Update the leg and torso animations
	UpdateModel(&m_Lower);
	UpdateModel(&m_Upper);
	
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Draw the first link, which is the lower body.  This will then recursively go
	// through the models attached to this model and drawn them.
	DrawLink(&m_Lower);
}


///////////////////////////////// DRAW LINK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws the current mesh with an effected matrix stack from the last mesh
/////
///////////////////////////////// DRAW LINK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::DrawLink(t3DModel *pModel)
{
	// Draw the current model passed in (initially the legs)
	RenderModel(pModel);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Though the changes to this function from the previous tutorial aren't huge, they
	// are pretty powerful.  Since animation is in effect, we need to create a rotational
	// matrix for each key frame, at each joint, to be applied to the child nodes of that 
	// object.  We can also slip in the interpolated translation into that same matrix.
	// The big thing in this function is interpolating between the 2 rotations.  The process
	// involves creating 2 quaternions from the current and next key frame, then using
	// slerp (spherical linear interpolation) to find the interpolated quaternion, then
	// converting that quaternion to a 4x4 matrix, adding the interpolated translation
	// to that matrix, then finally applying it to the current model view matrix in OpenGL.
	// This will then effect the next objects that are somehow explicitly or inexplicitly
	// connected and drawn from that joint.

	// Create some local variables to store all this crazy interpolation data
	CQuaternion qQuat, qNextQuat, qInterpolatedQuat;
	float *pMatrix, *pNextMatrix;
	float finalMatrix[16] = {0};

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Now we need to go through all of this models tags and draw them.
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// Get the current link from the models array of links (Pointers to models)
		t3DModel *pLink = pModel->pLinks[i];

		// If this link has a valid address, let's draw it!
		if(pLink)
		{			

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// To find the current translation position for this frame of animation, we times
			// the currentFrame by the number of tags, then add i.  This is similar to how
			// the vertex key frames are interpolated.
			CVector3 vPosition = pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].vPosition;

			// Grab the next key frame translation position
			CVector3 vNextPosition = pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].vPosition;
		
			// By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
			// we create a new translation position that is closer to the next key frame.
			vPosition.x = vPosition.x + pModel->t * (vNextPosition.x - vPosition.x),
			vPosition.y	= vPosition.y + pModel->t * (vNextPosition.y - vPosition.y),
			vPosition.z	= vPosition.z + pModel->t * (vNextPosition.z - vPosition.z);			

			// Now comes the more complex interpolation.  Just like the translation, we
			// want to store the current and next key frame rotation matrix, then interpolate
			// between the 2.

			// Get a pointer to the start of the 3x3 rotation matrix for the current frame
			pMatrix = &pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].rotation[0][0];

			// Get a pointer to the start of the 3x3 rotation matrix for the next frame
			pNextMatrix = &pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].rotation[0][0];

			// Now that we have 2 1D arrays that store the matrices, let's interpolate them.

			// Convert the current and next key frame 3x3 matrix into a quaternion
			qQuat.CreateFromMatrix( pMatrix, 3);
			qNextQuat.CreateFromMatrix( pNextMatrix, 3 );

			// Using spherical linear interpolation, we find the interpolated quaternion
			qInterpolatedQuat = qQuat.Slerp(qQuat, qNextQuat, pModel->t);

			// Here we convert the interpolated quaternion into a 4x4 matrix
			qInterpolatedQuat.CreateMatrix( finalMatrix );
			
			// To cut out the need for 2 matrix calls, we can just slip the translation
			// into the same matrix that holds the rotation.  That is what index 12-14 holds.
			finalMatrix[12] = vPosition.x;
			finalMatrix[13] = vPosition.y;
			finalMatrix[14] = vPosition.z;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


			// Start a new matrix scope
			glPushMatrix();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

				// Finally, apply the rotation and translation matrix to the current matrix
				glMultMatrixf( finalMatrix );

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

				
				// Recursively draw the next model that is linked to the current one.
				// This could either be a body part or a gun that is attached to
				// the hand of the upper body model.
				DrawLink(pLink);

			// End the current matrix scope
			glPopMatrix();
		}
	}

}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets time t for the interpolation between the current and next key frame
/////
///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::SetCurrentTime(t3DModel *pModel)
{
	float elapsedTime   = 0.0f;

	// This function is very similar to finding the frames per second.
	// Instead of checking when we reach a second, we check if we reach
	// 1 second / our animation speed. (1000 ms / animationSpeed).
	// That's how we know when we need to switch to the next key frame.
	// In the process, we get the t value for how far we are at to going to the
	// next animation key frame.  We use time to do the interpolation, that way
	// it runs the same speed on any persons computer, regardless of their specs.
	// It might look choppier on a junky computer, but the key frames still be
	// changing the same time as the other persons, it will just be not as smooth
	// of a transition between each frame.  The more frames per second we get, the
	// smoother the animation will be.  Since we are working with multiple models 
	// we don't want to create static variables, so the t and elapsedTime data are 
	// stored in the model's structure.
	
	// Return if there are no animations in this model
	if(!pModel->pAnimations.size()) return;

	// Get the current time in milliseconds
	float time = (float)GetTickCount();

	// Find the time that has elapsed since the last time that was stored
	elapsedTime = time - pModel->lastTime;

	// Store the animation speed for this animation in a local variable
	int animationSpeed = pModel->pAnimations[pModel->currentAnim].framesPerSecond;

	// To find the current t we divide the elapsed time by the ratio of:
	//
	// (1_second / the_animation_frames_per_second)
	//
	// Since we are dealing with milliseconds, we need to use 1000
	// milliseconds instead of 1 because we are using GetTickCount(), which is in 
	// milliseconds. 1 second == 1000 milliseconds.  The t value is a value between 
	// 0 to 1.  It is used to tell us how far we are from the current key frame to 
	// the next key frame.
	float t = elapsedTime / (1000.0f / animationSpeed);
	
	// If our elapsed time goes over the desired time segment, start over and go 
	// to the next key frame.
	if (elapsedTime >= (1000.0f / animationSpeed) )
	{
		// Set our current frame to the next key frame (which could be the start of the anim)
		pModel->currentFrame = pModel->nextFrame;
		pModel->nextFrame = (pModel->currentFrame + 1) % pModel->pAnimations[pModel->currentAnim].endFrame;

		// Set our last time for the model to the current time
		pModel->lastTime = time;

		t = 0.0f;
	}

	// Set the t for the model to be used in interpolation
	pModel->t = t;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders the model data to the screen
/////
///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::RenderModel(t3DModel *pModel)
{
	// Make sure we have valid objects just in case. (size() is in the STL vector class)
	if(pModel->pObject.size() <= 0) return;

	// Go through all of the objects stored in this model
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// Get the current object that we are displaying
		t3DObject *pObject = &pModel->pObject[i];


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Now that we have animation for our model, we need to interpolate between
		// the vertex key frames.  The .md3 file format stores all of the vertex 
		// key frames in a 1D array.  This means that in order to go to the next key frame,
		// we need to follow this equation:  currentFrame * numberOfVertices.
		// That will give us the index of the beginning of that key frame.  We just
		// add that index to the initial face index, when indexing into the vertex array.

		// Find the current starting index for the current key frame we are on
		int currentIndex = pModel->currentFrame * pObject->numOfVerts; 

		// Since we are interpolating, we also need the index for the next key frame
		int nextIndex = pModel->nextFrame * pObject->numOfVerts; 

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// If the object has a texture assigned to it, let's bind it to the model.
		// This isn't really necessary since all models have textures, but I left this
		// in here to keep to the same standard as the rest of the model loaders.
		if(pObject->bHasTexture)
		{
			// Turn on texture mapping
			glEnable(GL_TEXTURE_2D);

			// Grab the texture index from the materialID index into our material list
			int textureID = pModel->pMaterials[pObject->materialID].texureId;

			// Bind the texture index that we got from the material textureID
			glBindTexture(GL_TEXTURE_2D, m_Textures[textureID]);
		}
		else
		{
			// Turn off texture mapping
			glDisable(GL_TEXTURE_2D);
		}

		// Start drawing our model triangles
		glBegin(GL_TRIANGLES);

			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Go through each vertex of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the index for the current point in the face list
					int index = pObject->pFaces[j].vertIndex[whichVertex];
								
					// Make sure there is texture coordinates for this (%99.9 likelihood)
					if(pObject->pTexVerts) 
					{
						// Assign the texture coordinate to this vertex
						glTexCoord2f(pObject->pTexVerts[ index ].x, 
									 pObject->pTexVerts[ index ].y);
					}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
					
					// Like in the MD2 Animation tutorial, we use linear interpolation
					// between the current and next point to find the point in between,
					// depending on the model's "t" (0.0 to 1.0).

					// Store the current and next frame's vertex by adding the current
					// and next index to the initial index given from the face data.
					CVector3 vPoint1 = pObject->pVerts[ currentIndex + index ];
					CVector3 vPoint2 = pObject->pVerts[ nextIndex + index];

					// By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
					// we create a new vertex that is closer to the next key frame.
					glVertex3f(vPoint1.x + pModel->t * (vPoint2.x - vPoint1.x),
							   vPoint1.y + pModel->t * (vPoint2.y - vPoint1.y),
							   vPoint1.z + pModel->t * (vPoint2.z - vPoint1.z));

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

				}
			}

		// Stop drawing polygons
		glEnd();
	}
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// SET TORSO ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the current animation that the upper body will be performing
/////
///////////////////////////////// SET TORSO ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::SetTorsoAnimation(LPSTR strAnimation)
{
	// Go through all of the animations in this model
	for(int i = 0; i < m_Upper.numOfAnimations; i++)
	{
		// If the animation name passed in is the same as the current animation's name
		if( !_stricmp(m_Upper.pAnimations[i].strName, strAnimation) )
		{
			// Set the legs animation to the current animation we just found and return
			m_Upper.currentAnim = i;
			m_Upper.currentFrame = m_Upper.pAnimations[m_Upper.currentAnim].startFrame;
			return;
		}
	}
}


///////////////////////////////// SET LEGS ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the current animation that the lower body will be performing
/////
///////////////////////////////// SET LEGS ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::SetLegsAnimation(LPSTR strAnimation)
{
	// Go through all of the animations in this model
	for(int i = 0; i < m_Lower.numOfAnimations; i++)
	{
		// If the animation name passed in is the same as the current animation's name
		if( !_stricmp(m_Lower.pAnimations[i].strName, strAnimation) )
		{
			// Set the legs animation to the current animation we just found and return
			m_Lower.currentAnim = i;
			m_Lower.currentFrame = m_Lower.pAnimations[m_Lower.currentAnim].startFrame;
			return;
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


//// * NOTE * No Changes were made from the MD3 Loader code below this point * NOTE * /////


//////////////////////////  BELOW IS THE LOADER CLASS //////////////////////////////



///////////////////////////////// CLOAD MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This constructor initializes the md3 structures
/////
///////////////////////////////// CLOAD MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CLoadMD3::CLoadMD3()
{
	// Here we initialize our structures to 0
	memset(&m_Header, 0, sizeof(tMd3Header));

	// Set the pointers to null
	m_pSkins=NULL;
	m_pTexCoords=NULL;
	m_pTriangles=NULL;
	m_pBones=NULL;
}


///////////////////////////////// IMPORT MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is called by the client to open the .Md3 file, read it, then clean up
/////
///////////////////////////////// IMPORT MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD3::ImportMD3(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// Open the MD3 file in binary
	m_FilePointer = fopen(strFileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!m_FilePointer) 
	{
		// Display an error message and don't load anything if no file was found
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// Read the header data and store it in our m_Header member variable
	fread(&m_Header, 1, sizeof(tMd3Header), m_FilePointer);

	// Get the 4 character ID
	char *ID = m_Header.fileID;

	// The ID MUST equal "IDP3" and the version MUST be 15, or else it isn't a valid
	// .MD3 file.  This is just the numbers ID Software chose.

	// Make sure the ID == IDP3 and the version is this crazy number '15' or else it's a bad egg.
	if((ID[0] != 'I' || ID[1] != 'D' || ID[2] != 'P' || ID[3] != '3') || m_Header.version != 15)
	{
		// Display an error message for bad file format, then stop loading
		sprintf(strMessage, "Invalid file format (Version not 15): %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// Read in the model and animation data
	ReadMD3Data(pModel);

	// Clean up after everything
	CleanUp();

	// Return a success
	return true;
}


///////////////////////////////// READ MD3 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in all of the model's data, except the animation frames
/////
///////////////////////////////// READ MD3 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD3::ReadMD3Data(t3DModel *pModel)
{
	int i = 0;

	// Here we allocate memory for the bone information and read the bones in.
	m_pBones = new tMd3Bone [m_Header.numFrames];
	fread(m_pBones, sizeof(tMd3Bone), m_Header.numFrames, m_FilePointer);

	// Since we don't care about the bone positions, we just free it immediately.
	// It might be cool to display them so you could get a visual of them with the model.

	// Free the unused bones
	delete [] m_pBones;

	// Next, after the bones are read in, we need to read in the tags.  Below we allocate
	// memory for the tags and then read them in.  For every frame of animation there is
	// an array of tags.
	pModel->pTags = new tMd3Tag [m_Header.numFrames * m_Header.numTags];
	fread(pModel->pTags, sizeof(tMd3Tag), m_Header.numFrames * m_Header.numTags, m_FilePointer);

	// Assign the number of tags to our model
	pModel->numOfTags = m_Header.numTags;
	
	// Now we want to initialize our links.  Links are not read in from the .MD3 file, so
	// we need to create them all ourselves.  We use a double array so that we can have an
	// array of pointers.  We don't want to store any information, just pointers to t3DModels.
	pModel->pLinks = (t3DModel **) malloc(sizeof(t3DModel) * m_Header.numTags);
	
	// Initilialize our link pointers to NULL
	for (i = 0; i < m_Header.numTags; i++)
		pModel->pLinks[i] = NULL;

	// Get the current offset into the file
	long meshOffset = ftell(m_FilePointer);

	// Create a local meshHeader that stores the info about the mesh
	tMd3MeshInfo meshHeader;

	// Go through all of the sub-objects in this mesh
	for (i = 0; i < m_Header.numMeshes; i++)
	{
		// Seek to the start of this mesh and read in it's header
		fseek(m_FilePointer, meshOffset, SEEK_SET);
		fread(&meshHeader, sizeof(tMd3MeshInfo), 1, m_FilePointer);

		// Here we allocate all of our memory from the header's information
		m_pSkins     = new tMd3Skin [meshHeader.numSkins];
		m_pTexCoords = new tMd3TexCoord [meshHeader.numVertices];
		m_pTriangles = new tMd3Face [meshHeader.numTriangles];
		m_pVertices  = new tMd3Triangle [meshHeader.numVertices * meshHeader.numMeshFrames];

		// Read in the skin information
		fread(m_pSkins, sizeof(tMd3Skin), meshHeader.numSkins, m_FilePointer);
		
		// Seek to the start of the triangle/face data, then read it in
		fseek(m_FilePointer, meshOffset + meshHeader.triStart, SEEK_SET);
		fread(m_pTriangles, sizeof(tMd3Face), meshHeader.numTriangles, m_FilePointer);

		// Seek to the start of the UV coordinate data, then read it in
		fseek(m_FilePointer, meshOffset + meshHeader.uvStart, SEEK_SET);
		fread(m_pTexCoords, sizeof(tMd3TexCoord), meshHeader.numVertices, m_FilePointer);

		// Seek to the start of the vertex/face index information, then read it in.
		fseek(m_FilePointer, meshOffset + meshHeader.vertexStart, SEEK_SET);
		fread(m_pVertices, sizeof(tMd3Triangle), meshHeader.numMeshFrames * meshHeader.numVertices, m_FilePointer);

		// Now that we have the data loaded into the Quake3 structures, let's convert them to
		// our data types like t3DModel and t3DObject.  That way the rest of our model loading
		// code will be mostly the same as the other model loading tutorials.
		ConvertDataStructures(pModel, meshHeader);

		// Free all the memory for this mesh since we just converted it to our structures
		delete [] m_pSkins;    
		delete [] m_pTexCoords;
		delete [] m_pTriangles;
		delete [] m_pVertices;   

		// Increase the offset into the file
		meshOffset += meshHeader.meshSize;
	}
}


///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function converts the .md3 structures to our own model and object structures
/////
///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD3::ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader)
{
	int i = 0;

	// Increase the number of objects (sub-objects) in our model since we are loading a new one
	pModel->numOfObjects++;
		
	// Create a empty object structure to store the object's info before we add it to our list
	t3DObject currentMesh = {0};

	// Copy the name of the object to our object structure
	strcpy(currentMesh.strName, meshHeader.strName);

	// Assign the vertex, texture coord and face count to our new structure
	currentMesh.numOfVerts   = meshHeader.numVertices;
	currentMesh.numTexVertex = meshHeader.numVertices;
	currentMesh.numOfFaces   = meshHeader.numTriangles;

	// Allocate memory for the vertices, texture coordinates and face data.
	// Notice that we multiply the number of vertices to be allocated by the
	// number of frames in the mesh.  This is because each frame of animation has a 
	// totally new set of vertices.
	currentMesh.pVerts    = new CVector3 [currentMesh.numOfVerts * meshHeader.numMeshFrames];
	currentMesh.pTexVerts = new CVector2 [currentMesh.numOfVerts];
	currentMesh.pFaces    = new tFace [currentMesh.numOfFaces];

	// Go through all of the vertices and assign them over to our structure
	for (i=0; i < currentMesh.numOfVerts * meshHeader.numMeshFrames; i++)
	{
		// For some reason, the ratio 64 is what we need to divide the vertices by,
		// otherwise the model is gargantuanly huge!  If you use another ratio, it
		// screws up the model's body part position.
		currentMesh.pVerts[i].x =  m_pVertices[i].vertex[0] / 64.0f;
		currentMesh.pVerts[i].y =  m_pVertices[i].vertex[1] / 64.0f;
		currentMesh.pVerts[i].z =  m_pVertices[i].vertex[2] / 64.0f;
	}

	// Go through all of the UV coords and assign them over to our structure
	for (i=0; i < currentMesh.numTexVertex; i++)
	{
		// Since I changed the image from .tga to bitmaps, we need to negate the V ( or y) value.
		currentMesh.pTexVerts[i].x =  m_pTexCoords[i].textureCoord[0];
		currentMesh.pTexVerts[i].y = -m_pTexCoords[i].textureCoord[1];
	}

	// Go through all of the face data and assign it over to OUR structure
	for(i=0; i < currentMesh.numOfFaces; i++)
	{
		// Assign the vertex indices to our face data
		currentMesh.pFaces[i].vertIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh.pFaces[i].vertIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh.pFaces[i].vertIndex[2] = m_pTriangles[i].vertexIndices[2];

		// Assign the texture coord indices to our face data (same as the vertex indices)
		currentMesh.pFaces[i].coordIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh.pFaces[i].coordIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh.pFaces[i].coordIndex[2] = m_pTriangles[i].vertexIndices[2];
	}

	// Here we add the current object to our list object list
	pModel->pObject.push_back(currentMesh);
}


///////////////////////////////// LOAD SKIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the texture information for the model from the *.skin file
/////
///////////////////////////////// LOAD SKIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD3::LoadSkin(t3DModel *pModel, LPSTR strSkin)
{
	// Make sure valid data was passed in
	if(!pModel || !strSkin) return false;

	// Open the skin file
	ifstream fin(strSkin);

	// Make sure the file was opened
	if(fin.fail())
	{
		// Display the error message and return false
		MessageBox(NULL, "Unable to load skin!", "Error", MB_OK);
		return false;
	}

	// These 2 variables are for reading in each line from the file, then storing
	// the index of where the bitmap name starts after the last '/' character.
	string strLine = "";
	int textureNameStart = 0;

	// Go through every line in the .skin file
	while(getline(fin, strLine))
	{
		// Loop through all of our objects to test if their name is in this line
		for(int i = 0; i < pModel->numOfObjects; i++)
		{
			// Check if the name of this object appears in this line from the skin file
			if( IsInString(strLine, pModel->pObject[i].strName) )			
			{			
				// To abstract the texture name, we loop through the string, starting
				// at the end of it until we find a '/' character, then save that index + 1.
				for(int j = (int)strLine.length() - 1; j > 0; j--)
				{
					// If this character is a '/', save the index + 1
					if(strLine[j] == '/')
					{
						// Save the index + 1 (the start of the texture name) and break
						textureNameStart = j + 1;
						break;
					}	
				}

				// Create a local material info structure
				tMaterialInfo texture;

				// Copy the name of the file into our texture file name variable.
				strcpy(texture.strFile, &strLine[textureNameStart]);
				
				// The tile or scale for the UV's is 1 to 1 
				texture.uTile = texture.uTile = 1;

				// Store the material ID for this object and set the texture boolean to true
				pModel->pObject[i].materialID = pModel->numOfMaterials;
				pModel->pObject[i].bHasTexture = true;

				// Here we increase the number of materials for the model
				pModel->numOfMaterials++;

				// Add the local material info structure to our model's material list
				pModel->pMaterials.push_back(texture);
			}
		}
	}

	// Close the file and return a success
	fin.close();
	return true;
}


///////////////////////////////// LOAD SHADER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the basic shader texture info associated with the weapon model
/////
///////////////////////////////// LOAD SHADER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD3::LoadShader(t3DModel *pModel, LPSTR strShader)
{
	// Make sure valid data was passed in
	if(!pModel || !strShader) return false;

	// Open the shader file
	ifstream fin(strShader);

	// Make sure the file was opened
	if(fin.fail())
	{
		// Display the error message and return false
		MessageBox(NULL, "Unable to load shader!", "Error", MB_OK);
		return false;
	}

	// These variables are used to read in a line at a time from the file, and also
	// to store the current line being read so that we can use that as an index for the 
	// textures, in relation to the index of the sub-object loaded in from the weapon model.
	string strLine = "";
	int currentIndex = 0;
	
	// Go through and read in every line of text from the file
	while(getline(fin, strLine))
	{
		// Create a local material info structure
		tMaterialInfo texture;

		// Copy the name of the file into our texture file name variable
		strcpy(texture.strFile, strLine.c_str());
				
		// The tile or scale for the UV's is 1 to 1 
		texture.uTile = texture.uTile = 1;

		// Store the material ID for this object and set the texture boolean to true
		pModel->pObject[currentIndex].materialID = pModel->numOfMaterials;
		pModel->pObject[currentIndex].bHasTexture = true;

		// Here we increase the number of materials for the model
		pModel->numOfMaterials++;

		// Add the local material info structure to our model's material list
		pModel->pMaterials.push_back(texture);

		// Here we increase the material index for the next texture (if any)
		currentIndex++;
	}

	// Close the file and return a success
	fin.close();
	return true;
}


///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD3::CleanUp()
{
	// Close the current file pointer
	fclose(m_FilePointer);						
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// 
// Below I will sum up everything that we went over in this tutorial.  I don't 
// think it was a ton of things to sift through, but certainly model loading and
// animation are huge subjects that need a lot of code.  You can't just call
// glLoadModel() and glAnimateModel() for this stuff :)
// 
// First of all, we added a basic quaternion class to our tutorial.  This is used
// to take a matrix, convert it to a quaternion, interpolate between another 
// quaternion that was converted to a matrix, then turned back into a matrix.
// This is because quaternions are a great way to interpolate between rotations.
// If you wanted to use the glRotatef() and glTranslatef() functions, you would
// want to convert the interpolated quaternion to an axis angle representation,
// which might be less code, but not as useful in the long run.
// 
// The next important thing that was added was the interpolation between vertex
// key frames.  We learned earlier (top of Md3.cpp) that not most of the animation
// is done through key frame vertices, not bones.  The only bone animation that is
// done is with the joints that connect the .md3 models together.  The game Half-Life, 
// for example, uses a full on skeletal animation.
// 
// Also, in this tutorial we added the code to parse the animation config file (.cfg).
// This stores the animation information for each animation.  The order of them are
// important.  For the most part, the config files are the same format.  As discussed
// in the previous MD3 tutorial, there is a few extra things in the config file that
// we don't read in here, such as footstep sounds and initial positions.  The tutorial
// was not designed to be a robust reusable Quake3 character loader, but to teach you
// how it works for the most part.  Other things are extra credit :)
//
// Another important thing was our timing system.  Since we were dealing with multiple
// models that had different frames per second, we needed to add some variables to our
// t3DModel class to hold some things like elapsedTime and the current t value.  This
// way, no static variables had to be created, like in the .MD2 tutorial.
// 
// Let me know if this helps you out!
// 
// The Quake3 .Md3 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake, Quake2 and Quake3 are trademarks of ID Software.
// All trademarks used are properties of their respective owners. 
//
//
// © 2000-2005 GameTutorials