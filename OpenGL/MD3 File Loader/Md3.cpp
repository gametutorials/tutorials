//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		MD3 Loader										 //
//																		 //
//		$Description:	Demonstrates how to loader a Quake3 MD3 Model	 //
//																		 //
//***********************************************************************//

#include "main.h"
#include "Md3.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////////////////////////////////////////////////////////
//
// This file includes all of the class definitions for CLoadMD3 and CModelMD3.
// Get ready for some intense code.  There is a lot to look through that might
// discourage you from loading MD3 models, but it's not so bad once you get
// started.  Hopefully I can explain this well enough that you won't be
// intimidated by it.
//
// Let me describe what these classes are for.  CModelMD3 is the class the 
// client (you) uses to load and draw the character.  CLoadMD3 is what the
// CModelMD3 class uses to load the character mesh information.  It is nearly
// identical to the CLoadMD2 class that we implemented in the MD2 Loader tutorial.
//
// Also, like in that tutorial, we convert the Quake3 structures to our own t3DModel
// format that we use in the other loader tutorials, give or take a few variables.
// Once again, I want to keep all the loader tutorials using the same structure so
// there isn't more code to get lost in, which you are unfamiliar with.  Another
// reason for this is that this is a perfect opportunity to create an abstract
// base class for a C3DModel (or perhaps CModel3D) object.  This will allow each
// file loader to have their own inherited class.  This is a great design which will
// enable us to pass in any type of 3D object into functions that deal with objects,
// no matter what file format they are loaded from.
//
// You'll notice that we have a stray function for checking sub-strings, IsInString().
// We use this when we load the skin.  Don't worry about it.  It just returns true
// if the strSubString is inside of the original string, strString.  That way we don't
// have to parse the whole skin texture file line to look for the tag.
//
// Let's get to the code.  Be sure to look down at the Quick Notes near the bottom
// of this file for a overview of what this code all does.  It is assumed that you have
// read the introduction to .md3 loading at the top of Main.cpp
//
//

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
	// To free a model, we need to go through every sub-object and delete
	// their model data.  Since there is only one array of tags and links stored
	// for the model and all of its objects, we need to only free the model's 
	// tags and links once.

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

	// This function is where all the character loading is taken care of.  We use
	// our CLoadMD3 class to load the 3 mesh and skins for the character. Since we
	// just have 1 name for the model, we add that to _lower.md3, _upper.md3 and _head.md3
	// to load the correct mesh files.

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
	
	// Next we want to load the character meshes.  The CModelMD3 class has member
	// variables for the head, upper and lower body parts.  These are of type t3DModel.
	// Depending on which model we are loading, we pass in those structures to ImportMD3.
	// This returns a true of false to let us know that the file was loaded okay.  The
	// appropriate file name to load is passed in for the last parameter.

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

	// Once the models and skins were loaded, we need to load the textures.
	// We don't do error checking for this because we call CreateTexture() and 
	// it already does error checking.  Most of the time there is only
	// one or two textures that need to be loaded for each character.  There are
	// different skins though for each character.  For instance, you could have a
	// army looking character, or the normal look.  You can have multiple types of
	// looks for each model.  Usually it is just color changes though.

	// Load the lower, upper and head textures.  
	LoadModelTextures(&m_Lower, strPath);
	LoadModelTextures(&m_Upper, strPath);
	LoadModelTextures(&m_Head,  strPath);

	// The character data should all be loaded when we get here (except the weapon).
	// Now comes the linking of the body parts.  This makes it so that the legs (lower.md3)
	// are the parent node, then the torso (upper.md3) is a child node of the legs.  Finally,
	// the head is a child node of the upper body.  What I mean by node, is that you can
	// think of the model having 3 bones and 2 joints.  When you translate the legs you want
	// the whole body to follow because they are inseparable (unless a magic trick goes wrong).
	// The same goes for the head, it should go wherever the body goes.  When we draw the
	// lower body, we then recursively draw all of it's children, which happen to be just the
	// upper body.  Then we draw the upper body's children, which is just the head.  So, to
	// sum this all up; to set each body part's children, we need to link them together.
	// For more information on tags, refer to the Quick Notes and the functions below.

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
	
	// Next we want to load the weapon mesh.  The CModelMD3 class has member
	// variables for the weapon model and all it's sub-objects.  This is of type t3DModel.
	// We pass in a reference to this model in to ImportMD3 to save the data read.
	// This returns a true of false to let us know that the weapon was loaded okay.  The
	// appropriate file name to load is passed in for the last parameter.

	// Load the weapon mesh (*.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Weapon,  strWeaponModel))
	{
		// Display the error message that we couldn't find the weapon MD3 file and return false
		MessageBox(g_hWnd, "Unable to load the WEAPON model!", "Error", MB_OK);
		return false;
	}

	// Unlike the other .MD3 files, a weapon has a .shader file attached with it, not a
	// .skin file.  The shader file has it's own scripting language to describe behaviors
	// of the weapon.  All we care about for this tutorial is it's normal texture maps.
	// There are other texture maps in the shader file that mention the ammo and sphere maps,
	// but we don't care about them for our purposes.  I gutted the shader file to just store
	// the texture maps.  The order these are in the file is very important.  The first
	// texture refers to the first object in the weapon mesh, the second texture refers
	// to the second object in the weapon mesh, and so on.  I didn't want to write a complex
	// .shader loader because there is a TON of things to keep track of.  It's a whole
	// scripting language for goodness sakes! :)  Keep this in mind when downloading new guns.

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

	// Just like when we loaded the character mesh files, we need to link the weapon to
	// our character.  The upper body mesh (upper.md3) holds a tag for the weapon.
	// This way, where ever the weapon hand moves, the gun will move with it.

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
	// This function loads the textures that are assigned to each mesh and it's
	// sub-objects.  For instance, our character has a texture for the body
	// and the face/head, and since it has the head as a sub-object in the *_upper.md3 model, 
	// the MD3 file needs to contain texture information for each separate object in the mesh.
	// There is another thing to note too...  Some meshes use the same texture map as another 
	// one. We don't want to load 2 of the same texture maps, so we need a way to keep track of
	// which texture is already loaded so that we don't double our texture memory for no reason.
	// This is controlled with a STL vector list of "strings".  Every time we load a texture
	// we add the name of the texture to our list of strings.  Before we load another one,
	// we make sure that the texture map isn't already in our list.  If it is, we assign
	// that texture index to our current models material texture ID.  If it's a new texture,
	// then the new texture is loaded and added to our character's texture array: m_Textures[].

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
					// This ID will them be used as an index into m_Textures[].
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


///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This links the body part models to each other, along with the weapon
/////
///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void  CModelMD3::LinkModel(t3DModel *pModel, t3DModel *pLink, LPSTR strTagName)
{
	// Make sure we have a valid model, link and tag name, otherwise quit this function
	if(!pModel || !pLink || !strTagName) return;

	// This function is used to link 2 models together at a pseudo joint.  For instance,
	// if we were animating an arm, we would link the top part of the arm to the shoulder,
	// then the forearm would be linked to the top part of the arm, then the hand to
	// the forearm.  That is what is meant by linking.  That way, when we rotate the
	// arm at the shoulder, the rest of the arm will move with it because they are attached
	// to the same matrix that moves the top of the arm.  You can think of the shoulder
	// as the arm's parent node, and the rest are children that are subject to move to where
	// ever the top part of the arm goes.  That is how bone/skeletal animation works.
	//
	// So, we have an array of tags that have a position, rotation and name.  If we want
	// to link the lower body to the upper body, we would pass in the lower body mesh first,
	// then the upper body mesh, then the tag "tag_torso".  This is a tag that quake set as
	// as a standard name for the joint between the legs and the upper body.  This tag was
	// saved with the lower.md3 model.  We just need to loop through the lower body's tags,
	// and when we find "tag_torso", we link the upper.md3 mesh to that tag index in our
	// pLinks array.  This is an array of pointers to hold the address of the linked model.
	// Quake3 models are set up in a weird way, but usually you would just forget about a
	// separate array for links, you would just have a pointer to a t3DModel in the tag
	// structure, which in retrospect, you wouldn't have a tag array, you would have
	// a bone/joint array.

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


///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This recursively draws all the character nodes, starting with the legs
/////
///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::DrawModel()
{
	// This is the function that is called by the client (you) when using the 
	// CModelMD3 class object.  You will notice that we rotate the model by
	// -90 degrees along the x-axis.  This is because most modelers have z up
	// so we need to compensate for this.  Usually I would just switch the
	// z and y values when loading in the vertices, but the rotations that
	// are stored in the tags (joint info) are a matrix, which makes it hard
	// to change those to reflect Y up.  I didn't want to mess with that so
	// this 1 rotate will fix this problem.

	// Rotate the model to compensate for the z up orientation that the model was saved
	glRotatef(-90, 1, 0, 0);

	// You might be thinking to draw the model we would just call RenderModel()
	// 4 times for each body part and the gun right?  That sounds logical, but since
	// we are doing a bone/joint animation... and the models need to be linked together,
	// we can't do that.  It totally would ignore the tags.  Instead, we start at the
	// root model, which is the legs.  The legs drawn first, then we go through each of
	// the legs linked tags (just the upper body) and then it with the tag's rotation
	// and translation values.  I ignored the rotation in this tutorial since we aren't
	// doing any animation.  I didn't want to overwhelm you with quaternions just yet :)
	// Normally in skeletal animation, the root body part is the hip area.  Then the legs
	// bones are created as children to the torso.  The upper body is also a child to
	// the torso.  Since the legs are one whole mesh, this works out somewhat the same way.  
	// This wouldn't work if the feet and legs weren't connected in the same mesh because
	// the feet rotations and positioning don't directly effect the position and rotation
	// of the upper body, the hips do.  If that makes sense...  That is why the root starts
	// at the hips and moves down the legs, and also branches out to the upper body and
	// out to the arms.

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
	// This function is our recursive function that handles the bone animation
	// so to speak.  We first draw the model that is passed in (first the legs),
	// then go through all of it's tags and draw them.  Notice that when we
	// draw the model that is linked to the current model a new matrix scope
	// is created with glPushMatrix() and glPopMatrix().  This is because each tag
	// has a rotation and translation operation assigned to it.  For instance, when
	// we do a back flip death animation, the legs send a rotation and translation 
	// to the rest of the body to be rotated along with the legs as they flip backwards.  
	// If you didn't do this, the body and head would stay in the same place as the
	// legs did a back flipped and landed on the floor.  Of course, this would look really
	// stupid.  A 270-degree rotation to the rest of the body is done for that animation.
	// Keep in mind, the legs mesh is NEVER translated or rotated.  It only rotates and
	// translates the upper parts of the body.  All the rotation and translation of the
	// legs is done in the canned animation that was created in the modeling program.
	// Keep in mind that I ignore the rotation value for that is given in the tag since
	// it doesn't really matter for a static model.  Also, since the rotation is given
	// in a 3x3 rotation matrix, it would be a bit more code that could make you frustrated.

	// Draw the current model passed in (Initially the legs)
	RenderModel(pModel);

	// Now we need to go through all of this models tags and draw them.
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// Get the current link from the models array of links (pointers to models)
		t3DModel *pLink = pModel->pLinks[i];

		// If this link has a valid address, let's draw it!
		if(pLink)
		{			
			// Let's grab the translation for this new model that will be drawn 
			CVector3 vPosition = pModel->pTags[i].vPosition;

			// Start a new matrix scope
			glPushMatrix();
			
				// Translate the new model to be drawn to it's position
				glTranslatef(vPosition.x, vPosition.y, vPosition.z);

				// Recursively draw the next model that is linked to the current one.
				// This could either be a body part or a gun that is attached to
				// the hand of the upper body model.
				DrawLink(pLink);

			// End the current matrix scope
			glPopMatrix();
		}
	}

}


///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders the model data to the screen
/////
///////////////////////////////// RENDER MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CModelMD3::RenderModel(t3DModel *pModel)
{
	// This function actually does the rendering to OpenGL.  If you have checked out
	// our other file loading tutorials, it looks pretty much the same as those.  I
	// left out the normals though.  You can go to any other loader and copy the code
	// from those.  Usually the Quake models create the lighting effect in their textures
	// anyway.  

	// Make sure we have valid objects just in case. (size() is in the STL vector class)
	if(pModel->pObject.size() <= 0) return;

	// Go through all of the objects stored in this model
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// Get the current object that we are displaying
		t3DObject *pObject = &pModel->pObject[i];

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
								
					// Make sure there is texture coordinates for this (%99.9 likelyhood)
					if(pObject->pTexVerts) 
					{
						// Assign the texture coordinate to this vertex
						glTexCoord2f(pObject->pTexVerts[ index ].x, 
									 pObject->pTexVerts[ index ].y);
					}
					
					// Get the vertex that we are dealing with.  This code will change
					// a bunch when we doing our key frame animation in the next .MD3 tutorial.
					CVector3 vPoint1 = pObject->pVerts[ index ];

					// Render the current vertex
					glVertex3f(vPoint1.x, vPoint1.y, vPoint1.z);
				}
			}

		// Stop drawing polygons
		glEnd();
	}
}



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
/////	This is called by the client to open the .Md3 file, read it, then clean up.
/////
///////////////////////////////// IMPORT MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD3::ImportMD3(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// This function handles the entire model loading for the .md3 models.
	// What happens is we load the file header, make sure it's a valid MD3 model,
	// then load the rest of the data, then call our CleanUp() function.

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
	
	// Now that we know the file was found and it's all cool, let's read in
	// the header of the file.  If it has the correct 4 character ID and version number,
	// we can continue to load the rest of the data, otherwise we need to print an error.

	// Read the header data and store it in our m_Header member variable
	fread(&m_Header, 1, sizeof(tMd3Header), m_FilePointer);

	// Get the 4 character ID
	char *ID = m_Header.fileID;

	// The ID MUST equal "IDP3" and the version MUST be 15, or else it isn't a valid
	// .MD3 file.  This is just the numbers ID Software chose.

	// Make sure the ID == IDP3 and the version is this crazy number '15' or else it's a bad egg
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
/////	This function reads in all of the model's data, except the animation frames.
/////
///////////////////////////////// READ MD3 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD3::ReadMD3Data(t3DModel *pModel)
{
	int i = 0;

	// This member function is the BEEF of our whole file.  This is where the
	// main data is loaded.  The frustrating part is that once the data is loaded,
	// you need to do a billion little things just to get the model loaded to the screen
	// in a correct manner.

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

	// Now comes the loading of the mesh data.  We want to use ftell() to get the current
	// position in the file.  This is then used to seek to the starting position of each of
	// the mesh data arrays.

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

	// This is function takes care of converting all of the Quake3 structures to our
	// structures that we have been using in all of our model loading tutorials.  You
	// do not need this function if you are going to be using the Quake3 structures.
	// I just wanted to make it modular with the rest of the tutorials so you (me really) 
	// can make an engine out of them with an abstract base class.  Of course, each model
	// has some different data variables inside of it, depending on each format, but that
	// is perfect for some cool inheritance.  Just like in the .MD2 tutorials, we only
	// need to load in the texture coordinates and face information for one frame
	// of the animation.  Where, the vertex information needs to be loaded for every new 
	// frame, since its vertex key frame animation used in .MD3 models.  Half-life models 
	// do NOT do this I believe.  It's just pure bone/skeletal animation.

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
	// totally new set of vertices.  This will be used in the next animation tutorial.
	currentMesh.pVerts    = new CVector3 [currentMesh.numOfVerts * meshHeader.numMeshFrames];
	currentMesh.pTexVerts = new CVector2 [currentMesh.numOfVerts];
	currentMesh.pFaces    = new tFace [currentMesh.numOfFaces];

	// Go through all of the vertices and assign them over to our structure
	for (i=0; i < currentMesh.numOfVerts * meshHeader.numMeshFrames; i++)
	{
		// For some reason, the ratio 64 is what we need to divide the vertices by,
		// otherwise the model is gargantuanly huge!  If you use another ratio, it
		// screws up the model's body part position.  I found this out by just
		// testing different numbers, and I came up with 65.  I looked at someone
		// else's code and noticed they had 64, so I changed it to that.  I have never
		// read any documentation on the model format that justifies this number, but
		// I can't get it to work without it.  Who knows....  Maybe it's different for
		// 3D Studio Max files verses other software?  You be the judge.  I just work here.. :)
		currentMesh.pVerts[i].x =  m_pVertices[i].vertex[0] / 64.0f;
		currentMesh.pVerts[i].y =  m_pVertices[i].vertex[1] / 64.0f;
		currentMesh.pVerts[i].z =  m_pVertices[i].vertex[2] / 64.0f;
	}

	// Go through all of the uv coords and assign them over to our structure
	for (i=0; i < currentMesh.numTexVertex; i++)
	{
		// Since I changed the images to bitmaps, we need to negate the V ( or y) value.
		// This is because I believe that TARGA (.tga) files, which were originally used
		// with this model, have the pixels flipped horizontally.  If you use other image
		// files and your texture mapping is crazy looking, try deleting this negative.
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

	// This function is used to load a .skin file for the .md3 model associated
	// with it.  The .skin file stores the textures that need to go with each
	// object and subject in the .md3 files.  For instance, in our elf model,
	// the upper body model links to it's texture:
	//
	// u_torso,models/players/elf/elf.bmp
	//
	// Notice the first word, then a comma.  This word is the name of the object
	// in the .md3 file.  Remember, each .md3 file can have many sub-objects.
	// The next bit of text is the Quake3 path into the .pk3 file where the 
	// texture for that model is stored.  We don't use the Quake3 path
	// because we aren't making Quake, I just grab the texture name at the
	// end of the string and disregard the rest.  of course, later this is
	// concatenated to the original MODEL_PATH that we passed into load our character.
	// So, for the torso object it's clear that elf.bmp is assigned to it.
	// What this function does is go through all the lines of the .skin file, and then
	// goes through all of the sub-objects in the .md3 file to see if their name is
	// in that line as a sub string.  We use our IsInString() function for that.
	// If it IS in that line, then we know that we need to grab it's texture file at
	// the end of the line.  I just parse backwards until I find the last '/' character,
	// then copy all the characters from that index + 1 on (i.e. "default.bmp").
	// Remember, it's important to note that I changed the texture files from .tga
	// files to .bmp files because that is what most of our tutorials use.  That way
	// you don't have to sift through tons of image loading code.  You can look at our
	// .tga loaging code if you really want to use the .tga format.

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
				// Notice that with string we can pass in the address of an index
				// and it will only pass in the characters from that point on. Cool huh?
				// So now the strFile name should hold something like ("bitmap_name.bmp")
				strcpy(texture.strFile, &strLine[textureNameStart]);
				
				// The tile or scale for the UV's is 1 to 1 
				texture.uTile = texture.vTile = 1;

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

	// This function is used to load the .shader file that is associated with
	// the weapon model.  Instead of having a .skin file, weapons use a .shader file
	// because it has it's own scripting language to describe the behavior of the
	// weapon.  There is also many other factors like environment map and sphere map
	// textures, among other things.  Since I am not trying to replicate Quake, I
	// just care about the weapon's texture.  I went through each of the blocks
	// in the shader file and deleted everything except the texture name (of course
	// I changed the .tga files to .bmp for our purposes).  All this file now includes
	// is a texture name on each line.  No parsing needs to be done.  It is important
	// to keep in mind that the order of which these texture are stored in the file
	// is in the same order each sub-object is loaded in the .md3 file.  For instance,
	// the first texture name on the first line of the shader is the texture for
	// the main gun object that is loaded, the second texture is for the second sub-object
	// loaded, and so on. I just want to make sure that you understand that I hacked
	// up the .shader file so I didn't have to parse through a whole language.  This is
	// not a normal .shader file that we are loading.  I only kept the relevant parts.

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
	// Since we free all of the member variable arrays in the same function as
	// we allocate them, we don't need to do any other clean up other than
	// closing the file pointer, which could probably also be done in the same
	// function.  I left it here so you can add more of your cleanup if you add
	// to this class. 

	// Close the current file pointer
	fclose(m_FilePointer);						
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// I hope you didn't throw up when you saw all the code :)  It is a beast to start out,
// but you should try writing it from scratch! :)  And besides, when doing character
// animation, you should expect it to be huge because this is some pretty advanced stuff.
// Since this is proprietary, all we have to work on is the file format.  If you find 
// anything wrong or just plain stupid, let me know and I will make the fix so others 
// don't have to suffer in ignorance like myself :)  Let's go over the whole shebang:
// 
// The first thing you have to understand about characters in Quake3 is that the model
// is split into multiple files.  There is a upper.md3, a head.md3 and a lower.md3.
// These hold the upper body, the head and legs models.
//
// Below, like at the bottom of main.cpp, is the explanation of how this stuff works:
// 
// Speaking of animation, so how does this stuff all work?  Is it bone animation or isn't it?
// Well, yes and no.  First, yes, there is bone animation because the legs are the parent
// node, and then the upper body attaches to the legs, then the head attaches to the upper 
// body.  Wherever the legs move, the upper body is then
// translated and rotated to the correct position that it should be, and then the head
// is in the same matrix scope so it goes where the upper body goes.  Once again, we do not
// handle rotating in this tutorial.  That requires a ton of matrix code that will be used
// in the next animation tutorial with quaternions (The room goes silent.....).
//
// And as for the second point, no the animation isn't exactly bone animation.  Each
// animation is stored in vertex key frames, just like the .MD2 files were.  The only
// difference is that we have 3 body parts to keep track of.  That way it is more realistic
// in the way of, you can have your character doing something totally different with their legs,
// than what the top part of them is doing.  Such as, when you are doing the jumping animation,
// the upper body could be doing a shooting, picking up, or dropping animation.  Make sense?
// So to the question of if this is bone animation, I can say a firm!!!..... Yes/No :)
//
// The .md3 files have skins and shader files associated with them.  It has a skin if it's
// a body part and a shader file if it's a weapon model.  These files store, for the most
// part, the textures associated with each model.  Sometimes the model has multiple textures,
// one for each sub-object.  I left the skin files the same, but I deleted a lot of the
// garbage that we didn't need in the .shader file.  Look at LoaderShader() for more info.
//
// There is also a config (.cfg) file for the animations of the model.  This stores the
// first frame, the amount of frames for that animation, the looping frame count (not used),
// and the frames per second that that animation should run.  We don't do anything with this
// file in this tutorial, but in the next tutorial we will.
//
// Finally, this brings us to the most confusing part in the .md3 model format, the tags.
// What the heck are tags?  Tags can be thought of as joints.  If you look in the .skin files
// you will see the tags on a separate line.  These are basically
// joints that we need to connect other models too.  These are the ones I have seen:
//
// tag_torso  - The joint that connects the hips and the upper body together
// tag_head   - The joing that connects the neck and the head model too
// tag_weapon - The tag that connects the hand of the upper body and the weapon too.
//
// Now, that's the easy part to understand... it's like the velcro that each body part
// sticks to right?  Well, the next part is where the confusion comes in.  Besides being
// a joint it stores the bone rotations and translations that need to be applied to the
// model that is connected to that joint.  For instance, we never rotate or translate the
// lower.md3 model.  That is all taken care of in the vertex key frame animation, but, the
// model stores tags for the "tag_torso" joint, which tells the upper body where to move and
// rotate, depending on what the legs are doing.
//
// A good example of this is in the back flip animation.  The legs animation has key frames that
// it interpolates to, to perform this back flip, but the rest of the model doesn't.  To
// compensate for that, each frame, the lower body model sends a message to the upper body
// model to move and rotate to a certain degree and position, along a certain axis.  In
// programming terms, this means that we push on a new matrix and then apply some translations
// and rotations to the rest of the body parts (upper, head and weapon parts).  You don't
// directly apply it to each one, you just push on a matrix, apply the rotations and 
// translations, then anything drawn after that will be apart of that new matrix.  Simple huh?
// The rotation is stored in a 3x3 rotation matrix.  That is why we didn't bother with it
// in this tutorial because there is no animation so no need to add more code to scare you away.
// The translation is a simple (x, y, z) position that can be easily passed in to glTranslatef().
// This can all be seen in DrawLink().  In the next tutorial, we will not use glTranslatef(),
// but we will just include the translation in our matrix that we create to rotate the model.
// Then we can just do a simple call to glMultMatrix() to do both the rotation and translation
// at the same time.
//
// As a final explanation about the tags, let me address the model linking.  This just
// attaches 2 models together at the desired tag (or joint). For example, we attach the
// the upper.md3 model to the lower.md3 model at the "tag_torso" joint.  Now, when ever
// the legs do their animation, it sends the translation and rotation messages to all of
// it's children, which in this case, is just the upper body model, which effects the
// upper body's children (The weapon and head models).  The link array has nothing to do
// with the file format being loaded in.  That is just the way that I chose to handle it.
// You can also find some code on www.planetquake.com by Lonerunner.  This guy/gal is awesome!
// He has a ton of 3D game formats that he did, which can be found at Romka's site too.  
//
// That is pretty much the crux of the .MD3 loading.  There is some bone animation to be
// loaded, but I never really figured it out.  The position was always (0, 0, 0) so I don't
// know what those were for.  I was going to display the bones, but it doesn't seem to save
// the position.  Who knows... if you know how, let me know.  Maybe I am ignoring something.
// It has min/max values, but I didn't bother to try and figure it out.
// 
// I would like to point out, this is just the solution I came up with.  There is probably
// a lot more intelligent ways to handle this, but it works great for me.  When the Quake3
// source code comes out, maybe we will all learn a few things from the mast'ehr :)
//
// Let me know if this helps you out!
//
// The Quake3 .MD3 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake, Quake2 and Quake3 are trademarks of ID Software.
// All trademarks used are properties of their respective owners. 
//
//
// ©2000-2005 GameTutorials