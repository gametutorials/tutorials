#ifndef _ModelManager_H
#define _ModelManager_H

#include <stdio.h>										// Used for fprintf and stderr
#include <stdlib.h>										// Used for the exit() function
#include "GL\glew.h"									// Used for the OpenGL types like GLuint
#include "Shader.h"										// Used for loading and running our shaders

// This is our custom class to store attributes for a vertex like the x,y,z values.
class Vertex3
{
public:
	
	float xyz[3];										// The x,y,z position


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	float rgba[4];										// The r,g,b,a (Red, Green, Blue, Alpha) color values

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


};


// The Model class to handle initializing, rendering and destroying of a 3D object
class Model
{
public:

	// These are created to standardize the index properties used for vertex properties.
	static const GLuint kVertexIndex = 0;				// This is for the x,y,z vertices
	static const GLuint kColorIndex = 1;				// This is for the r,g,b,a colors
	static const GLuint kTextureIndex = 2;				// This is for the u,v texture coordinates
	static const GLuint kNormalIndex = 3;				// This is for the x,y,z normals

	// Create an empty constructor and have the deconstructor release our memory.
	Model()		{ }
	~Model()	{ Destroy(); }

	// Takes in a list of x,y,z vertices and vertex length to create the VBO and VAO's from,
	// as well as the text file locations for the vertex and fragment shaders to load.
	void Initialize(Vertex3 vertices[], int length, std::string strVertex, std::string strFragment);
	
	// This draws the Model to the backbuffer
	void Render();

	// This cleans up the VBO and VAO's
	void Destroy();

private:

	Vertex3 *Vertices;									// The list of the vertices
	GLint VerticesLength;								// The length of the vertices
	GLuint VertexBufferId;								// The VBO Id stored by the VAO
	GLuint VertexArrayObjectId;							// The VAO Id to bind when drawing

	// Our shader object to initialize and run the shaders
	Shader Shader;						
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file stores our definitions for our Model and Vertex3 classes.  Please
// look at Shader.cpp to get more information on how we create shaders in OpenGL.
// We added the rgba color values to our Vertex3 class for coloring our triangles.
// 
//
// © 2000-2014 GameTutorials