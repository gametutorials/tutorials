//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Color											 //
//																		 //
//		$Description:	Draws a colored triangle in OpenGL 4			 //
//																		 //
//***********************************************************************//

#include "../Headers/Model.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
In this file we added the ability to pass in color attributes with our VBO and VAOs.
This is done with an extra call to glVertexAttribPointer().  We then just need to enable
it when rendering with glEnableVertexAttribArray(kColorIndex) and then disable it afterwards.
*/

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// This takes in an array of Vertex3's and the length of the array, which generates the Vertex Buffer
// Object and Vertex Array Object for the model.  It also passes in the text file locations for the
// shaders that will be loaded and compiled to render the triangle.
void Model::Initialize(Vertex3 vertices[], GLint length, std::string strVertex, std::string strFragment)
{
	// Create and compile our GLSL vertex and fragment shaders from the shader text files passed
	// in.  Please refer to Shader.cpp to see how these shaders are loaded, compiled then assigned.
	Shader.Initialize(strVertex, strFragment);

	// Store the list of vertices and the length in our member variables
	Vertices = vertices;
	VerticesLength = length;

	// This function gets the last error that happened from OpenGL, after the last time this
	// function was called.  So by calling it now, we reset the errors and then call it below again.
	GLenum ErrorCheckValue = glGetError();
	
	// This is where we create our Vertex Array Object (VAO), which saves the state and property information
	// about the Vertex Buffer Object (which stores the vertices in a buffer to pass to the GPU).
	// glGenVertexArrays() sends back a new unique unsigned number that represents the VAO.  Once we
	// receive the newly created VAO index we need to bind it to tell OpenGL we are using is right away.
	// This can generate many VAO's, but we just need one so we just pass in a single GLuint and just say
	// that we want to 1 returned.
	glGenVertexArrays(1, &VertexArrayObjectId);
	glBindVertexArray(VertexArrayObjectId);

	// Now we need to create our Vertex Buffer Object (VBO) which stores a buffer of all the vertices,
	// which can then be quickly passed to our video card's GPU to render them every frame.  Similar to
	// generating the VAO, we need to also get a unique number for the VBO with glGenBuffers, which we
	// tell it that we want 1 number back.  We then immediately bind the buffer to memory to tell OpenGL
	// we want to be working with that newly created buffer going forward.  GL_ARRAY_BUFFER tells OpenGL
	// that this buffer is a standard array of data instead of an indexed array with GL_ELEMENT_ARRAY_BUFFER.
	// Indexed arrays works as long as you aren't doing texture coordinates, which can't be shared so
	// you end up having to pass all the duplicated vertices in anyways if you want texture mapping.
	glGenBuffers(1, &VertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId);

	// The next steps is actually storing the list of vertices in our buffer.  We use the same target
	// GL_ARRAY_BUFFER as what we bound to above, and then the length of the buffer, then the actual
	// buffer which is the vertices passed in, then a "hint" to OpenGL to tell it if it will be a static
	// buffer or will be changing (DYNAMIC).  We will always just choose STATIC in our case.  This "hint"
	// just helps OpenGL know if the data will be changing often so it knows how to optimize the usage.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * length, Vertices, GL_STATIC_DRAW);

	// Here is where we tell OpenGL what the format each vertex is within our buffer.  
	glVertexAttribPointer(kVertexIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]), 0);


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// We now need to add the color attributes to our Vertex Array Object and Vertex Buffer Object.
	// This takes an index to be referenced in the vertex shader(kColorIndex = 1), the number of
	// floats(4 for r, g, b, a), the type which is a GL_FLOAT, we don't want it normalized so GL_FALSE,
	// the size of a single Vertex3 object, and then a void pointer to the start of the location
	// within the Vertex3 class to the rgba property, which is right after the xyz variable.  By
	// giving a pointer to the size of the xyz variable, we give the offset in bytes into the class.
	glVertexAttribPointer(kColorIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]), (GLvoid*)sizeof(Vertices[0].xyz));

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Now check to see if any errors happened in this function, which saves us tons of time instead
	// of trying to figure out why our screen is black and we would have no idea why.
	ErrorCheckValue = glGetError();

	// If there was an error found, print it to the screen and close the OpenGL window.
	if ( ErrorCheckValue != GL_NO_ERROR )
	{
		// Use gluErrorString(ErrorCheckValue) if you want to get the string value of the
		// error, but I removed this since Mac OSX 10.9 (Mavericks) deprecated this function.
		fprintf(stderr, "ERROR: Could not create a VAO and VBO with error Id: %d\n", ErrorCheckValue);
		exit(-1);
	}
}


// This is the function that will actually render the model to the backbuffer, usually every frame
// if called in the GameLoop() function in Main.cpp.  
void Model::Render()
{
	// This turns our shader on before we tell OpenGL to render the triangle.
	Shader.TurnOn();

	// First we want to tell OpenGL with VAO is going to be used for rendering
	glBindVertexArray(VertexArrayObjectId);

	// Tell OpenGL to enable our vertex information within the shaders
	glEnableVertexAttribArray(kVertexIndex);


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Tell OpenGL to enable our color information within the shaders
	glEnableVertexAttribArray(kColorIndex);

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Tell OpenGL to draw the triangles from our VBO
	glDrawArrays(GL_TRIANGLES, 0, VerticesLength);


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Disable the color index
	glDisableVertexAttribArray(kColorIndex);

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Disable the vertex index
	glDisableVertexAttribArray(kVertexIndex);

	// Next, let's reset the current VAO binding to 0, which clears out any binding
	glBindVertexArray(0);

	// This turns off the shader so that other objects rendered after this won't be using this shader.
	Shader.TurnOff();
}


// This handles all the cleanup for our model, like the VBO/VAO buffers and shaders.
void Model::Destroy()
{
	// If we have a valid vertex buffer, reset it's binding, delete it and clear the property
	if ( VertexBufferId )
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VertexBufferId);
		VertexBufferId = 0;
	}

	// If we have a valid vertex array, reset it's binding, delete it and clear the property
	if ( VertexArrayObjectId )
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &VertexArrayObjectId);
		VertexArrayObjectId = 0;
	}

	// Release the memory for the compiled shaders
	Shader.Destroy();
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this file we added a few function calls to be able to pass in our color
// attributes to the shaders, glVertexAttribPointer(), glEnableVertexAttribArray()
// and glDisableVertexAttribArray().
//
// 
// © 2000-2014 GameTutorials