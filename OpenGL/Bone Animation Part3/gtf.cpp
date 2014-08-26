//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		GT Loader Part 3										  //
//																				  //
//		$Description:	Loads the GT Skeletal Animation format .GTF				  //
//																				  //
//********************************************************************************//

#include "main.h"
#include "gtf.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// Below we have a bunch of functions that we need for some basic
// calculations when dealing with 3D vectors and quaternions.  Below
// those are some matrix-math functions, as well as the complicated
// SLERP (Spherical Linear Interpolation) function.  The rest of the
// additions are for loading the animation data, rendering the animations,
// then handling the timing and updating of the animation frames.

////////////////////////////// OPERATOR * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This multiplies a scalar by a CVector3 and returns a CVector3
/////
////////////////////////////// OPERATOR * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*


CVector3 operator*(float scalar, const CVector3& v) 
{
	// Return the multiplied scalar by the vector
	return CVector3(v.x * scalar, v.y * scalar, v.z * scalar);
}


////////////////////////////// OPERATOR * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This multiplies a quaternion by a CVector3
/////
////////////////////////////// OPERATOR * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 CQuaternion::operator*(CVector3 v)
{
	// This function is a complex function that calculates the
	// result of multiplying a quaternion by a vector.  We perform
	// three calculations for different parts of the whole equation
	// and then add them all up to return the result:
	//
	// ResultVector = 
	// ( vec * ( 2w^2 - 1) ) + (2 * (q_xyz dot vec) * q_xyz) + (2w * (q_xyz cross vec))

	// First we store the x,y,z of the quaternion in a CVector3
	CVector3 q(x, y, z);
	
	// The is the first part of the equation: ( vec * ( 2w^2 - 1) )
	CVector3 t1 = v * (2.0f * w * w - 1.0f);

	// This is the second part of the equation: (2 * (q_xyz dot vec) * q_xyz)
	CVector3 t2 = 2.0f * q.Dot(v) * q;

	// This is the third and final part of the equation: (2 * w * (q_xyz cross vec))
	CVector3 t3 = 2.0f * w * q.Cross(v);

	// Now add up all the vectors and return the final resultant vector
	return t1 + t2 + t3;
}


////////////////////////////// OPERATOR * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This multiplies 2 quaternions and returns the resultant quaternion
/////
////////////////////////////// OPERATOR * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuaternion CQuaternion::operator*(const CQuaternion& q)
{
	// This is how to multiply a quaternion by a quaternion
	return CQuaternion(w * q.x + x * q.w + y * q.z - z * q.y,
					   w * q.y + y * q.w + z * q.x - x * q.z,
					   w * q.z + z * q.w + x * q.y - y * q.x,
					   w * q.w - x * q.x - y * q.y - z * q.z);
}


////////////////////////////// OPERATOR + \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This adds 2 quaternions and returns the resultant quaternion
/////
////////////////////////////// OPERATOR + \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuaternion CQuaternion::operator+(const CQuaternion& q)
{
	// Add the quaternion to the current quaternion and return the result
	return CQuaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}



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


///////////////////////////////// CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our CModelGTF constructor
/////
///////////////////////////////// CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CModelGTF::CModelGTF()
{
}


///////////////////////////////// ~CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our CModelGTF deconstructor
/////
///////////////////////////////// ~CMODEL GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CModelGTF::~CModelGTF()
{
	// Free all the model data
	DestroyModel();
}	


///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This frees our model and all it's associated data
/////
///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::DestroyModel()
{
	// Go through all the objects in the model
	for(int i = 0; i < m_Model.numOfObjects; i++)
	{
		// Free the faces and vertices
		if(m_Model.pObject[i].pFaces)		delete [] m_Model.pObject[i].pFaces;
		if(m_Model.pObject[i].pVerts)		delete [] m_Model.pObject[i].pVerts;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// Go through all of our weight lists and free the pointers
		for(int w = 0; w < (int)m_Model.pObject[i].vWeightInfo.size(); w++)
		{
			// Check if there is a valid pointer for this weight list
			if(m_Model.pObject[i].vWeightInfo[w].pWeightInfo)
			{
				// Delete the pointer and set it to NULL
				delete [] m_Model.pObject[i].vWeightInfo[w].pWeightInfo;
				m_Model.pObject[i].vWeightInfo[w].pWeightInfo = NULL;
			}
		}

		// Clear our STL list for the weight influences
		m_Model.pObject[i].vWeightInfo.clear();

		// Go through all of the bone data and free the lists
		for(int b = 0; b < (int)m_Model.pObject[i].vBoneInfo.size(); b++)
		{
			// Check if the matrice pointer is valid
			if(m_Model.pObject[i].vBoneInfo[b].pBoneMatrices)
			{
				// Delete and NULL the matrix list
				delete [] m_Model.pObject[i].vBoneInfo[b].pBoneMatrices;
				m_Model.pObject[i].vBoneInfo[b].pBoneMatrices = NULL;
			}
		}

		// Clear the bone list
		m_Model.pObject[i].vBoneInfo.clear();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	}
}
	

///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads our model from the given path and file name
/////
///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CModelGTF::LoadModel(LPSTR strPath, LPSTR strModel)
{
	// Create a loader class instance
	CLoadGTF loadGTF;

	// Make sure valid model name was passed in
	if(!strModel) return false;

	char szFullPath[MAX_PATH] = {0};

	// If there is a valid path we want to add the file name to it,
	// otherwise just use the file name with no path.
	if(strPath)
		sprintf(szFullPath, "%s\\%s", strPath, strModel);
	else
		strcpy(szFullPath, strModel);

	// Here we load the .GTF file.  We also make sure it loaded correctly.
	if(!loadGTF.ImportGTF(&m_Model, szFullPath))
	{
		// Display an error message telling us the file could not be found
		MessageBox(g_hWnd, "Unable to load the model!", "Error", MB_OK);
		return false;
	}

	// After all the data is loaded we can go through and load all the textures.
	LoadModelTextures(strPath);

	// The model was loaded correctly so return true
	return true;
}


///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the textures for the model in a specified directory
/////
///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::LoadModelTextures(LPSTR strPath)
{
	// Grab a pointer to the model
	t3DModel *pModel = &m_Model;

	// Go through all the materials that are assigned to this model
	for(int i = 0; i < (int)pModel->pMaterials.size(); i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(pModel->pMaterials[i].strFile) > 0)
		{
			// Create a boolean to tell us if we have a new texture to load
			bool bNewTexture = true;

			// Go through all the textures in our string list to see if it's already loaded
			for(unsigned int j = 0; j < strTextures.size(); j++)
			{
				string strTemp = pModel->pMaterials[i].strFile;
				string strTemp2 = strTextures[j];

				// If the texture name is already in our list of texture, don't load it again.
				if(strstr(pModel->pMaterials[i].strFile, strTextures[j].c_str()) )
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

			// Check if there is a valid path
			if(strlen(strPath) > 0)
			{
				// Add the file name and path together so we can load the texture
				sprintf(strFullPath, "%s\\%s", strPath, pModel->pMaterials[i].strFile);
			}
			else
			{
				// If there is no path, don't attach a directory to our file name
				sprintf(strFullPath, "%s", strPath, pModel->pMaterials[i].strFile);
			}

			// We pass in a reference to an index into our texture array member variable.
			CreateTexture(strFullPath, m_Textures[strTextures.size()]);								

			// Set the texture ID for this material by getting the current loaded texture count
			pModel->pMaterials[i].texureId = (int)strTextures.size();

			// Now we increase the loaded texture count by adding the texture name to our list
			strTextures.push_back(pModel->pMaterials[i].strFile);
		}
	}
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets time t for the interpolation between the current and next key frame
/////
///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::SetCurrentTime()
{
	static float elapsedTime   = 0.0f;

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

	elapsedTime += g_DT;

	// Store the 1.0f / animation speed for this animation in a local variable
	float oneOverAnimSpeed = 1.0f / (float)m_Model.animSpeed;

	// To find the current t we divide the elapsed time by the ratio of:
	//
	// (1_second / the_animation_frames_per_second)
	//
	// Since we are dealing with milliseconds, we need to use 1000
	// milliseconds instead of 1 because we are using GetTickCount(), which is in 
	// milliseconds. 1 second == 1000 milliseconds.  The t value is a value between 
	// 0 to 1.  It is used to tell us how far we are from the current key frame to 
	// the next key frame.
	float t = elapsedTime / oneOverAnimSpeed;
	
	// If our elapsed time goes over the desired time segment, start over and go 
	// to the next key frame.
	if (elapsedTime >= oneOverAnimSpeed )
	{
		// Set our current frame to the next key frame (which could be the start of the anim)
		m_Model.currentFrame = m_Model.nextFrame;
		m_Model.nextFrame = (m_Model.currentFrame + 1) % m_Model.pObject[0].numFrames;

		t = 0.0f;
		elapsedTime = 0.0f;
	}

	// Set the t for the model to be used in interpolation
	m_Model.t = t;
}


///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the current frame of animation, depending on it's FPS and t
/////
///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*	

void CModelGTF::UpdateModel()
{
	// Grab a pointer to the model
	t3DModel *pModel = &m_Model;

	// Make sure there are objects and animation speed in the model
	if(!pModel->pObject.size() || pModel->animSpeed <= 0) return;

	// Initialize a start and end frame.  In this tutorial we don't add a way
	// to keep track of each animation, so we just play all of the animations
	// in a sequence.  If we wanted to only play certain animations the start
	// frame and end frame would need to be changed for each animation.
	int startFrame = 0;
	int endFrame   = pModel->pObject[0].numFrames;
	
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
	SetCurrentTime();
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This recursively draws all the character nodes, starting with the legs.
/////
///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::DrawModel()
{


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Before we render the model we need to update the animation frames
	UpdateModel();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Render the model and all it's objects
	RenderModel();
}


///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This goes through all the meshes and renders them to the screen
/////
///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelGTF::RenderModel()
{
	// Go through all of the objects stored in this model
	for(int i = 0; i < m_Model.numOfObjects; i++)
	{
		// Get the current object that we are displaying
		t3DObject *pObject = &m_Model.pObject[i];
		
		// Check if there are materials assigned to this object and 
		// enable texture mapping if necessary, otherwise turn it off.
		if(pObject->numOfMaterials >= 1)
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);

		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			// Now let's see if there are any materials that need binding
			if(pObject->numOfMaterials >= 1)
			{
				// Create a static int to hold the last texture ID
				static int lastTexID = -1;

				// Extract the current textureID for this face from our face data
				int textureID = m_Model.pMaterials[pObject->pFaces[j].textureID].texureId;

				// If the current texture ID isn't the same as last time, bind the texture.
				if(textureID != lastTexID && textureID >= 0 && textureID < (int)strTextures.size())
					glBindTexture(GL_TEXTURE_2D, m_Textures[textureID]);

				// Store the current textureID in our static int for next time
				lastTexID = textureID;
			}

			// Start drawing our model triangles
			glBegin(GL_TRIANGLES);

				// Go through each vertex of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Make sure there are texture coordinates for this
					if(pObject->pTexVerts) 
					{
						// Here we grab the texture index from our face information
						int texIndex = pObject->pFaces[j].coordIndex[whichVertex];

						// Assign the texture coordinate to this vertex
						glTexCoord2f(pObject->pTexVerts[ texIndex ].x, 
									 pObject->pTexVerts[ texIndex ].y);
					}

					// Grab the current vertex index from our face information
					int vertIndex = pObject->pFaces[j].vertIndex[whichVertex];


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

					// Now we get to the beef of the tutorial.  Below is where we
					// handle the calculations for our animation.  Basically what
					// happens is we grab the current and the next frame of animation
					// data.  With that data (the weights and transformations) we
					// do spherical interpolation (quaternion rotations) and linear
					// interpolation (the translations) to get a smooth transformation
					// between each frame of animation.  There isn't anything to
					// difficult about the concepts, but the math is the challenging
					// part.  The only math that is new to this tutorial is the
					// calculations of multiplying a quaternion by a 3D vector.

					// Extract the current vertex and draw it
					CVector3 vPos = pObject->pVerts[vertIndex];					

					// If there is no animation, just render the original
					// vertex and continue (skip the animation calculations).
					if(!pObject->bAnimated)
					{
						glVertex3f(vPos.x, vPos.y, vPos.z);
						continue;
					}

					// This will be our final vertex position to render
					CVector3 vNewPos(0, 0, 0);

					// Just to specify the index, we store our vertex index as a blendIndex.
					// This will index into our weight list.
					int blendIndex = vertIndex;

					// Now comes the crazy code :)  We now will go through every single
					// weight influence and add up all the influences and transformations
					// to our current vertex.
					for(int b=0; b < pObject->vWeightInfo[blendIndex].numBlendLinks; b++)
					{
						// Grab the current bone index from our weight list.  Remember that
						// the X variable stores the bone index and the Y stores the weight.
						int boneIndex = (int)pObject->vWeightInfo[blendIndex].pWeightInfo[b].x;

						// Make sure that the index we got isn't out of range of our bones
						if(boneIndex > pObject->numBones)
							return;

						// Now we want to grab the translation and rotation data for both the
						// current and the next frame of animation.  That way we can interpolate
						// between the two frames to make the animation look smoother.  So here
						// we grab pointers to the current and next matrix information for the
						// current bone we are working with.
						tBoneMatrix *pBoneMatrix1 = &(pObject->vBoneInfo[m_Model.currentFrame].pBoneMatrices[boneIndex]);
						tBoneMatrix *pBoneMatrix2 = &(pObject->vBoneInfo[m_Model.nextFrame].pBoneMatrices[boneIndex]);

						// First we want to perform the spherical-linear interpolation between
						// the two quaternions (rotations) for the current and next frame.
						CQuaternion	qPos(pBoneMatrix1->qRotation); 
						CQuaternion	qPos2(pBoneMatrix2->qRotation); 
						CQuaternion qNew = qPos.Slerp(qPos, qPos2, m_Model.t);

						// To apply the interpolated rotation we multiply our rotation by
						// the original position of the vertex.  This will then use our
						// function that we created at the top of this file which multiplies a
						// quaternion by a vector.  Once we do that, we store the new 3D vector.
						CVector3 vTempPos = qNew * vPos;

						// Next we grab the translations for the current and next frame
						CVector3 vPosition = pBoneMatrix1->vTranslation;
						CVector3 vNextPosition = pBoneMatrix2->vTranslation;
						
						// Using a simple linear-interpolation equation, we use our time "t"
						// value to interpolate between the current and next translation.  This
						// gives us a smooth transition between each frame of animation.
						vTempPos.x += vPosition.x + m_Model.t * (vNextPosition.x - vPosition.x);
						vTempPos.y += vPosition.y + m_Model.t * (vNextPosition.y - vPosition.y);
						vTempPos.z += vPosition.z + m_Model.t * (vNextPosition.z - vPosition.z);

						// If there is a special weight for this vertex we want to extract
						// that and apply it to our current vertex.  If the model isn't using
						// weighted animation then the weight will just be 1.0, which will do
						// nothing to the vertex when applied.  Remember that the Y value of
						// our weight-info list is the actual weight value to be applied.
						float weight = pObject->vWeightInfo[blendIndex].pWeightInfo[b].y;

						// Apply the weight value to our current vertex and add it to our
						// final transformed vertex position.
						vNewPos += vTempPos * weight;
					}

					// vNewPos has our final position that was transformed, so render it.
					glVertex3f(vNewPos.x, vNewPos.y, vNewPos.z);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


				}

			// Stop drawing polygons
			glEnd();
		}
	}
}


//////////////////////////  BELOW IS THE LOADER CLASS //////////////////////////////



///////////////////////////////// CLOAD GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This constructor initializes the gtf data
/////
///////////////////////////////// CLOAD GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CLoadGTF::CLoadGTF()
{
	// Set the pointers to null
	m_pVertices=NULL;
	m_pIndices=NULL;
	m_pTexCoords=NULL;
	m_pTexIndices=NULL;	
}


///////////////////////////////// IMPORT GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is called by the client to open the .GTF file, read it, then clean up
/////
///////////////////////////////// IMPORT GTF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadGTF::ImportGTF(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// Open the GTF file in binary
	m_FilePointer = fopen(strFileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!m_FilePointer) 
	{
		// Display an error message and don't load anything if no file was found
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	char fileID[4] = {0};
	float version = 0;

	// Read in the GTF key and exporter version
	fread(&fileID, sizeof(char), 4, m_FilePointer);
	fread(&version, sizeof(float), 1, m_FilePointer);

	// Get the 4 character ID
	char *ID = fileID;

	// The ID MUST equal "GT3D" and the version MUST be greater than or equal to 1, 
	// or else it isn't a valid .GTF file.

	// Make sure the ID == GT3D and the version greater than or equal t0 1.0
	if((ID[0] != 'G' || ID[1] != 'T' || ID[2] != '3' || ID[3] != 'D') || version < 1.0f)
	{
		// Display an error message for bad file format, then stop loading
		sprintf(strMessage, "Invalid file format: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// Read in the model and animation data
	ReadGTFData(pModel);

	// Clean up after everything
	CleanUp();

	// Return a success
	return true;
}


///////////////////////////////// READ GTF DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in all of the model's data, except the animation frames
/////
///////////////////////////////// READ GTF DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadGTF::ReadGTFData(t3DModel *pModel)
{
	// Read in the number of objects for this scene
	int numObjects = 0;
	fread(&numObjects, sizeof(int), 1, m_FilePointer);
	
	// Go through all of the objects in the scene
	for(int i = 0; i < numObjects; i++)
	{
		// Create a structure to hold the size of the data lists
		tMeshInfo meshInfo = {0};

		// In our loading code we first want to load in all of the materials.
		// First we load the number of materials in the file.
		fread(&meshInfo.numMaterials, sizeof(int), 1, m_FilePointer);
		
		// Go through all of our materials and load them
		for(int m = 0; m < meshInfo.numMaterials; m++)
		{
			// Load in only MAX_FILE_NAME characters for the texture name
			char szMaterial[MAX_FILE_NAME] = {0};
			fread(szMaterial, sizeof(char), MAX_FILE_NAME, m_FilePointer);

			// Create a material structure instance and store the material info
			tMaterialInfo newMaterial;
			strcpy(newMaterial.strFile, szMaterial);
			newMaterial.texureId = (int)pModel->pMaterials.size();

			// Add a new material
			pModel->pMaterials.push_back(newMaterial);
		}
		
		// Read the number of vertices, allocate memory and read them from the file.
		fread(&meshInfo.numVertices, sizeof(int), 1, m_FilePointer);
		m_pVertices  = new CVector3 [meshInfo.numVertices];
		fread(m_pVertices, sizeof(CVector3), meshInfo.numVertices, m_FilePointer);

		// Read the number of indices, allocate memory and read them from the file.
		fread(&meshInfo.numIndices, sizeof(int), 1, m_FilePointer);
		m_pIndices = new int [meshInfo.numIndices];
		fread(m_pIndices, sizeof(int), meshInfo.numIndices, m_FilePointer);
		
		// Read the number of UV coordinates, allocate memory and read them from the file.
		fread(&meshInfo.numTexCoords, sizeof(int), 1, m_FilePointer);
		m_pTexCoords = new CVector3 [meshInfo.numTexCoords];
		fread(m_pTexCoords, sizeof(CVector3), meshInfo.numTexCoords, m_FilePointer);

		// Read the number of UV indices, allocate memory and read them from the file.
		fread(&meshInfo.numTexIndices, sizeof(int), 1, m_FilePointer);
		m_pTexIndices = new int [meshInfo.numTexIndices];
		fread(m_pTexIndices, sizeof(int), meshInfo.numTexIndices, m_FilePointer);

		// Now that we have the data, let's convert it to our model structure
		ConvertDataStructures(pModel, meshInfo);

		
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// Everything below in this *NEW* block is for loading the animation
		// and bone data.  First we grab a pointer to our current object.
		t3DObject *pObject = &pModel->pObject[pModel->numOfObjects-1];

		// Grab the boolean that tells us if there is animation or not
		fread(&pObject->bAnimated, sizeof(bool), 1, m_FilePointer);

		// If there is animation let's load it
		if(pObject->bAnimated)
		{
			// We need to go through every vertice and load the corresponding
			// weight-influence data.  
			for(int w = 0; w < meshInfo.numVertices; w++)
			{
				// Create an instance of our weight info class to hold weights
				tWeightInfo weightInfo;

				// Read in the number of blend links for this vertex and add it to our list
				fread(&weightInfo.numBlendLinks, sizeof(int), 1, m_FilePointer);
				pObject->vWeightInfo.push_back(weightInfo);

				// Since we are working with pointers, just to be safe we wanted to add 
				// the weight info to our list first, and then grab a pointer to it.
				// We can then use that pointer to allocate memory for a list of all the
				// blend links (weight influences).  Then we will read in the links into
				// our pWeightInfo pointer that is allocated.

				// Grab a pointer to the last weight info in our list (that was just added).
				tWeightInfo *pWeightInfo = &(pObject->vWeightInfo[pObject->vWeightInfo.size() - 1]);
				
				// Allocate memory for the weight links.  It's a CVector3 because we store
				// the bone index and the weight value (between 0 and 1).
				pWeightInfo->pWeightInfo = new CVector2 [pWeightInfo->numBlendLinks];

				// Read in all the influences for the weight according to this vertex.
				fread(pWeightInfo->pWeightInfo, sizeof(CVector2), pWeightInfo->numBlendLinks, m_FilePointer);
			}

			// Read in the number of bones, animation frame rate and number of anim frames.
			fread(&pObject->numBones, sizeof(int), 1, m_FilePointer);
			fread(&pModel->animSpeed, sizeof(int), 1, m_FilePointer);
			fread(&pObject->numFrames, sizeof(int), 1, m_FilePointer);

			// Now we will go through every frame of animation and get the matrix data
			for(int a = 0; a < pObject->numFrames; a++)
			{
				// Create a structure to hold the matrix information
				tBoneInfo boneInfo;

				// Store the number of bones so we know how big the list is, then
				// add our structure to the list of vBoneInfo.
				boneInfo.numBones = pObject->numBones;
				pObject->vBoneInfo.push_back(boneInfo);
				
				// Just like when we loaded our weight influences we want to grab
				// a pointer to the last item we just added to our list.  We then
				// allocate memory for the matrix information for each bone and
				// read in the data.
				tBoneInfo *pBoneInfo = &pObject->vBoneInfo[pObject->vBoneInfo.size() - 1];
				pBoneInfo->pBoneMatrices = new tBoneMatrix [pObject->numBones];
				fread(pBoneInfo->pBoneMatrices, sizeof(tBoneMatrix), pObject->numBones, m_FilePointer);
			}
		}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Free all the memory for this mesh since we just converted it to our structures
		delete [] m_pVertices;   
		delete [] m_pIndices; 
		delete [] m_pTexCoords;
		delete [] m_pTexIndices; 
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// Make sure the current frame of animation is set to 0
	pModel->currentFrame = 0;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function converts the .gtf structures to our own model and object structures
/////
///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadGTF::ConvertDataStructures(t3DModel *pModel, tMeshInfo meshInfo)
{
	int i = 0;

	// Increase the number of objects (sub-objects) in our model since we are loading a new one
	pModel->numOfObjects++;
		
	// Create a empty object structure to store the object's info before we add it to our list
	t3DObject currentMesh = {0};

	// Assign the vertex, texture coord and face count to our new structure.
	// Currently we have no UV coordinates so we set the size to 0.
	currentMesh.numOfVerts   = meshInfo.numVertices;
	currentMesh.numTexVertex = meshInfo.numTexCoords;
	currentMesh.numOfFaces   = meshInfo.numIndices/3;

	// Allocate memory for the vertices, texture coordinates and face data.
	// We set the texture coordinates to NULL for now (until the next tutorial).
	currentMesh.pVerts    = new CVector3 [currentMesh.numOfVerts];
	currentMesh.pTexVerts = new CVector2 [currentMesh.numTexVertex];
	currentMesh.pFaces    = new tFace [currentMesh.numOfFaces];

	// Go through all of the vertices and assign them over to our structure
	for (i=0; i < currentMesh.numOfVerts; i++)
	{
		currentMesh.pVerts[i].x =  m_pVertices[i].x/1.0f;
		currentMesh.pVerts[i].y =  m_pVertices[i].y/1.0f;
		currentMesh.pVerts[i].z =  m_pVertices[i].z/1.0f;
	}

	// Go through all of the UV coords and assign them over to our structure
	for (i=0; i < currentMesh.numTexVertex; i++)
	{
		currentMesh.pTexVerts[i].x = m_pTexCoords[i].x;
		currentMesh.pTexVerts[i].y = m_pTexCoords[i].y;
	}

	// Go through all of the face data and assign it over to OUR structure.
	// Currently we don't support texture coordinates.
	for(i=0; i < currentMesh.numOfFaces; i++)
	{
		// Assign the vertex indices to our face data
		currentMesh.pFaces[i].vertIndex[0] = m_pIndices[i*3];
		currentMesh.pFaces[i].vertIndex[1] = m_pIndices[i*3 + 1];
		currentMesh.pFaces[i].vertIndex[2] = m_pIndices[i*3 + 2];

		// Assign the texture coord indices to our face data (same as the vertex indices)
		currentMesh.pFaces[i].coordIndex[0] = m_pTexIndices[i*3];
		currentMesh.pFaces[i].coordIndex[1] = m_pTexIndices[i*3 + 1];
		currentMesh.pFaces[i].coordIndex[2] = m_pTexIndices[i*3 + 2];

		// Set the texture ID for this face (smooth groups)
		currentMesh.pFaces[i].textureID = (int)m_pTexCoords[currentMesh.pFaces[i].coordIndex[0]].z;
	}

	// Store the number of materials for this mesh
	currentMesh.numOfMaterials = meshInfo.numMaterials;

	// Here we add the current object to our list object list
	pModel->pObject.push_back(currentMesh);
}


///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadGTF::CleanUp()
{
	// Close the current file pointer
	fclose(m_FilePointer);						
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// We made it!  This tutorial should add a great skill to your programming skill'z.
// Hopefully you weren't overwhelmed by all the mathematics that we had to add
// in order to perform our interpolated animation.
//
// We added many math functions for quaternions and 3D vectors to this file,
// along with a couple functions for updating and keeping track of the time for
// our animation frame rate.  Finally, we added the loading and rendering code
// for our animation.  It wasn't so bad was it?  The next challenge is to go
// and great a nice animation system to keep track of each animation.  You can
// check the MD2 and MD3 animation tutorials to get an idea of how this can be
// done.  You might also want to add the option of exporting and drawing the 
// colors of the meshes.  
//
//
// © 2000-2006 GameTutorials