#include "Main.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is our new file that will hold the main code for the water.
// There are only functions, with only four of them being new.
// In past tutorials we have seen these three functions:
//
// *** These were used in our Win32 animation tutorials
// bool AnimateNextFrame(int desiredFrameRate);
// void LoadAnimTextures(UINT textureArray[], LPSTR szFileName, int startIndex, int bitmapCount);
//
// *** This was used in our OpenGL tutorial on rendering to a texture
// void CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID);
//
// The new four functions, with a brief explanation are as follows:
//
// *** This cycles through our caustic bitmaps to produce the animation
// void RenderCaustics(float waterHeight, float causticScale);
//
// *** This renders our screen to a texture that is used for reflection
// void CreateReflectionTexture(float waterHeight, int textureSize);
//
// *** This renders our screen to a texture for refraction and depth
// void CreateRefractionDepthTexture(float waterHeight, int textureSize);
//
// *** This draws our quad for the water
// void RenderWater(float waterHeight);
//
// We will not go over the first three functions because you probably
// already know and understand them, so let's focus on the new ones.
// 
// Let's get down and dirty in some green comments!
//

// This is used for the scaling of our normal map texture.
// This makes it so the water doesn't look so jittery, but
// more smooth in it's waves.
const float kNormalMapScale = 0.25f;

// Global delta time
float g_DT = 0.0f;


/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function clamps a specified frame rate for consistency
/////
/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; // Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime; // Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		g_DT = desiredFPS;
		elapsedTime -= desiredFPS; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


/////////////////////////////// LOAD ANIM TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads a list of bitmaps to be used for animation
/////
/////////////////////////////// LOAD ANIM TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadAnimTextures(UINT textureArray[], LPSTR szFileName, int startIndex, int bitmapCount)
{
	char szBuffer[255];
	int i = 0;

	// Don't load anything if a valid file name doesn't exist
	if(!szFileName) return;

	// Loop until we have loaded in all the desired bitmaps
	for(i = 0; i < bitmapCount; i++)
	{
		// Store the name of the file and the number of the bitmap (i.e. caust00.bmp)
		// The bitmaps must be named with 2 digits (i.e. caust00.bmp caust01.bmp, etc..)
		sprintf(szBuffer, "%s%d%d.bmp", szFileName, i / 10, i % 10);

		// Create a texture map from our loaded bitmap
		CreateTexture(textureArray[startIndex + i], szBuffer);
	}
}


///////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a blank texture to render too
/////
///////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	
void CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID)										
{
	// Create a pointer to store the blank image data
	unsigned int *pTexture = NULL;											

	// Allocate and init memory for the image array and point to it from pTexture
	pTexture = new unsigned int [size * size * channels];
	memset(pTexture, 0, size * size * channels * sizeof(unsigned int));	

	// Register the texture with OpenGL and bind it to the texture ID
	glGenTextures(1, &textureArray[textureID]);								
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);					
	
	// Create the texture and store it on the video card
	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_INT, pTexture);						
	
	// Set the texture quality
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Since we stored the texture space with OpenGL, we can delete the image data
	delete [] pTexture;																					
}


///////////////////////////////// RENDER CAUSTICS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the light-reflected-caustic effects underwater
/////
///////////////////////////////// RENDER CAUSTICS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderCaustics(float waterHeight, float causticScale)
{
	// How this function works is that we need to turn on a new
	// texture unit for multi-texturing the caustics over the terrain.
	// Since the first 2 units are already used for the terrain and
	// detail texture, we choose GL_TEXTURE2_ARB (unit 3).  We then
	// restrict the animation to 30 frames per second so it doesn't
	// animate too fast.  This frame rate of course would change
	// depending on the water flow.
	//
	// We then scale the texture matrix for our caustic texture,
	// right before we render the terrain again.  We use a clipping
	// plane to only render the bottom part of the terrain because
	// that is all that needs to be rendered with caustics.  In Main.cpp
	// I describe a better and faster way to do this than how we have it
	// here.

	// Activate the third texture ID and bind the caustic texture to it
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glEnable(GL_TEXTURE_2D);
		
	// Below we increase our caustic texture at a rate of 30 frames per second.
	// This simulates light from the water's surface.

	// Start the first index at 0 ("caust00") for our current caustic texture;
	static int startIndex = 0;

	// Bind the current caustic texture to our terrain
	glBindTexture(GL_TEXTURE_2D, g_Texture[startIndex + WATER_START_ID]);
	
	// We create a frame counter so that we only render the caustics every
	// 5 frames that we draw.  This way it slows the animation down.
	static int frameCount = 0;

	// If the frame count reached 5, let's increase the current caustic bitmap.
	if(frameCount == 5)
	{
		startIndex = ((startIndex + 1) % NUM_WATER_TEX);
		frameCount = 0;
	}

	// Increase the frame count every frame
	frameCount++;

	// Just like we did with the detail texture for the terrain, we
	// scale the caustic texture by our inputted scale value.
	glMatrixMode(GL_TEXTURE);

		// Reset the current matrix and apply our chosen scale value
		glLoadIdentity();
		glScalef(causticScale, causticScale, 1);

	// Leave the texture matrix and put us back in the model view matrix
	glMatrixMode(GL_MODELVIEW);

	// Next, we need to render the terrain with caustic effects, but only
	// the bottom part of the terrain (clip the top part)

	// Create the plane equation for the part we want to keep (bottom)
	double plane[4] = {0.0, -1.0, 0.0, waterHeight};

	// Turn on a clip plane and set the clipping equation
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Render the bottom of the terrain with caustics
	RenderHeightMap(g_HeightMap);

	// Turn clipping off
	glDisable(GL_CLIP_PLANE0);

	// Reset the texture matrix
	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	// Turn the third multi-texture pass off
	glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
}


/////////////////////////// CREATE REFLECTION TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders our world to a texture for our reflection
/////
/////////////////////////// CREATE REFLECTION TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateReflectionTexture(float waterHeight, int textureSize)
{
	// To create the reflection texture we just need to set the view port
	// to our texture map size, then render the current scene our camera
	// is looking at to the already allocated texture unit.  Since this
	// is a reflection of the top of the water surface we use clipping 
	// planes to only render the top of the world as a reflection.  If
	// we are below the water we don't flip the reflection but just use
	// the current view of the top as we are seeing through the water.
	// When you look through water at the surface it isn't really reflected,
	// only when looking down from above the water on the surface.

	// Change the view port to be the size of the texture we will render to
    glViewport(0,0, textureSize, textureSize);

	// Clear the color and depth bits, reset the matrix and position our camera.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    g_Camera.Look();

	// So we don't affect any other objects in the world we push on a new matrix
    glPushMatrix();

		// If our camera is above the water we will render the scene flipped upside down.
		// In order to line up the reflection nicely with the world we have to translate
		// the world to the position of our reflected surface, multiplied by two.
		if(g_Camera.Position().y > waterHeight)
		{
			// Translate the world, then flip it upside down
			glTranslatef(0.0f, waterHeight*2.0f, 0.0f);
			glScalef(1.0, -1.0, 1.0);

			// Since the world is updside down we need to change the culling to FRONT
			glCullFace(GL_FRONT);
			
			// Set our plane equation and turn clipping on
			double plane[4] = {0.0, 1.0, 0.0, -waterHeight};
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);

			// Render the world upside down and clipped (only render the top flipped).
			// If we don't turn OFF caustics for the reflection texture we get horrible
			// artifacts in the water.  That is why we set bRenderCaustics to FALSE.
			RenderWorld(false);

			// Turn clipping off
			glDisable(GL_CLIP_PLANE0);

			// Restore back-face culling
			glCullFace(GL_BACK);
		}
		else
		{
			// If the camera is below the water we don't want to flip the world,
			// but just render it clipped so only the top is drawn.
			double plane[4] = {0.0, 1.0, 0.0, waterHeight};
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld(true);
			glDisable(GL_CLIP_PLANE0);
		}

	// Restore the previous matrix
    glPopMatrix();

    // Bind the current scene to our reflection texture
	glBindTexture(GL_TEXTURE_2D, g_Texture[REFLECTION_ID]);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, textureSize, textureSize);
}


/////////////////////////// CREATE REFRACTION DEPTH TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders our world to a texture to do the reflection and depth calculations
/////
/////////////////////////// CREATE REFRACTION DEPTH TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateRefractionDepthTexture(float waterHeight, int textureSize)
{
	// To create the refraction and depth textures we do the same thing
	// we did for the reflection texture, except we don't need to turn
	// the world upside down.  We want to find the depth of the water,
	// not the depth of the sky and above water terrain.
	
 	// Change the view port to be the size of the texture we will render to
    glViewport(0,0, textureSize, textureSize);

	// Clear the color and depth bits, reset the matrix and position our camera.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    g_Camera.Look();

	// So we don't affect any other objects in the world we push on a new matrix
    glPushMatrix();

		// If our camera is above the water we will render only the parts that
		// are under the water.  If the camera is below the water we render
		// only the stuff above the water.  Like the reflection texture, we
		// incorporate clipping planes to only render what we need.

		// If the camera is above water, render the data below the water
		if(g_Camera.Position().y > waterHeight)
		{
			double plane[4] = {0.0, -1.0, 0.0, waterHeight}; 
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld(true);
			glDisable(GL_CLIP_PLANE0);
		}
		// If the camera is below the water, only render the data above the water
		else
		{
			glCullFace(GL_FRONT);
			double plane[4] = {0.0, 1.0, 0.0, -waterHeight}; 
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld(true);
			glDisable(GL_CLIP_PLANE0);
			glCullFace(GL_BACK);
		}

	// Restore the previous matrix
    glPopMatrix();

    // Bind the current scene to our refraction texture
    glBindTexture(GL_TEXTURE_2D, g_Texture[REFRACTION_ID]);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, textureSize, textureSize);

    // Bind the current scene to our depth texture
    glBindTexture(GL_TEXTURE_2D, g_Texture[DEPTH_ID]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, textureSize, textureSize, 0);
}


///////////////////////////////// RENDER WATER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the flat-water surface as a quad
/////
///////////////////////////////// RENDER WATER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderWater(float waterHeight)
{
    // You have reached the last important function of this tutorial!
	// Well, besides the shader files :)  All we are doing here is just
	// turning on all of the texture units for the water and drawing a
	// quad.  The shader code takes care of the rest.  In order for the
	// shader to know which texture is which, we need to tell our shader
	// which textures are assigned to which variables.

	// Turn on the first texture unit and bind the REFLECTION texture
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[REFLECTION_ID]);

	// Turn on the second texture unit and bind the REFRACTION texture
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_Texture[REFRACTION_ID]);

	// Turn on the third texture unit and bind the NORMAL MAP texture
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[NORMAL_ID]);

	// Turn on the fourth texture unit and bind the DUDV MAP texture
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_Texture[DUDVMAP_ID]);

	// Turn on the fifth texture unit and bind the DEPTH texture
    glActiveTextureARB(GL_TEXTURE4_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[DEPTH_ID]);

	// Set the variable "reflection" to correspond to the first texture unit
	GLint uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "reflection"); 
	glUniform1iARB(uniform, 0); //second paramter is the texture unit 

	// Set the variable "refraction" to correspond to the second texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "refraction");
	glUniform1iARB(uniform, 1); 

	// Set the variable "normalMap" to correspond to the third texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "normalMap");
	glUniform1iARB(uniform, 2);

	// Set the variable "dudvMap" to correspond to the fourth texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "dudvMap"); 
	glUniform1iARB(uniform, 3);

	// Set the variable "depthMap" to correspond to the fifth texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "depthMap");
	glUniform1iARB(uniform, 4); 

	// Give the variable "waterColor" a blue color
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "waterColor");
	glUniform4fARB(uniform, 0.1f, 0.2f, 0.4f, 1.0f); 

	// We don't use lighting in this tutorial, but we do need to calculate
	// the diffuse and specular lighting on the water to increase realism.
	// Let's position the light so it's near the light in the sky box texture.
	CVector3 lightPos(100.0f, 150.0f, 100.0f);

	// Store the camera position in a variable
	CVector3 vPosition = g_Camera.Position();

	// Give the variable "lightPos" our hard coded light position
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "lightPos");
	glUniform4fARB(uniform, lightPos.x, lightPos.y, lightPos.z, 1.0f); 

	// Give the variable "cameraPos" our camera position
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "cameraPos");
	glUniform4fARB(uniform, vPosition.x, vPosition.y, vPosition.z, 1.0f); 
	
	// Below is where we draw our quad for the water.  Before we start
	// giving out UV coordinates for each texture unit, we need to create
	// some variables to store each one.  The dudv and depth map has it's
	// UV coordinates calculated in the shader.  This is why below we set
	// the UV coordinates to 0 to show they aren't set in the code.
	// When you run this application you will notice that the water is moving.
	// To give this effect we create a static float call "move" that slowly
	// increases by our g_WaterFlow variable.  To give the water the wavy
	// effect we move the refraction map in the opposite direction
	// of the normal map.  This is very important to the effect.

	// Create a static variable for the movement of the water
	static float move = 0.0f;

	// Use this variable for the normal map and make it slower
	// than the refraction map's speed.  We want the refraction
	// map to be jittery, but not the normal map's waviness.
	float move2 = move * kNormalMapScale;

	// Set the refraction map's UV coordinates to our global g_WaterUV
	float refrUV = g_WaterUV;

	// Set our normal map's UV scale and shrink it by kNormalMapScale
	float normalUV = g_WaterUV * kNormalMapScale;

	// Move the water by our global speed
	move += g_WaterFlow;

    // Draw our huge water quad
    glBegin(GL_QUADS);

		// The back left vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, g_WaterUV);				// Reflection texture				
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, refrUV - move);			// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, normalUV + move2);		// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(0.0f, waterHeight, 0.0f);

		// The front left vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);					// Reflection texture
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f - move);			// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, 0.0f + move2);			// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(0.0f, waterHeight, 1000.0f);

		// The front right vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, 0.0f);				// Reflection texture
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, 0.0f - move);			// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, 0.0f + move2);		// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(1000.0f, waterHeight, 1000.0f);

		// The back right vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, g_WaterUV);		// Reflection texture
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, refrUV - move);		// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, normalUV + move2);	// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(1000.0f, waterHeight, 0.0f);

    glEnd();

	// Turn the first multi-texture pass off
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);

	// Turn the second multi-texture pass off
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the third multi-texture pass off
	glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the fourth multi-texture pass off
	glActiveTextureARB(GL_TEXTURE3_ARB);		
    glDisable(GL_TEXTURE_2D);

	// Turn the fifth multi-texture pass off
	glActiveTextureARB(GL_TEXTURE4_ARB);		
    glDisable(GL_TEXTURE_2D);
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This code wasn't so bad was it?  This is pretty much all the major code
// that we need to do to get the water working (besides the shader code).
// At the top of this file we give a brief description of the functions
// in this file.  Only four of them are really new.
// 
// Basically what this file has is the code to render to a texture and
// draw our water quad.  Also, not important to the water surface effect,
// is our caustics rendering code.
//
// You should be ready now to look at the shader explanations at the bottom
// of Main.cpp.  They are pretty complicated so don't get too bogged down.
//
//
// © 2000-2005 GameTutorials