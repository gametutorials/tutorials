//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//																		 //
//		$Program:		First OpenGL Program							 //
//																		 //
//		$Description:	Init OpenGL 4 and draw a triangle to the screen	 //
//																		 //
//***********************************************************************//

#include "../Headers/Model.h"

/*
The Model class is used to manage the functionality of taking in vertex data to render an object
in 3D.  Simply, this has an Initialize(), Render() and Destroy() function.  In this tutorials, we will
just be rendering a single triangle, which has 3 points that are passed into the Initialize() function.
This will grow in the next tutorials when we start working with Matrices and more complex items.
*/

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

	// Here is where we tell OpenGL what the format each vertex is within our buffer.  In our case, we
	// have 3 floats. In this tutorial we only have vertices, but if we had colors and other things like
	// texture coordinates and normals, we would need to call this function for each attribute.  The
	// next tutorial on colors will go over this.  First we pass in the index used to refer to the
	// attribute, in our case, the vertices, and I pass in a constant value kVertexIndex which is 0.
	// I created a constant for each type of attribute we will use, defined in the Model.h file.
	// Later when we render we need to turn on this index with glEnableVertexAttribArray().  The next
	// parameters are the number of floats which is 3 for the xyz, then the format (GL_FLOAT), then
	// FALSE since we don't need OpenGL to normalize our fixed-point data, then the size in bytes each
	// attribute is, then a pointer to the location the attribute is within the Vertex3 class.  The
	// default is 0, which means that it's the first property in the class, which xyz is.  In the
	// next tutorial you will see how we need to pass in a pointer to the location of the color
	// attribute within the Vector3 class.
	glVertexAttribPointer(kVertexIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]), 0);

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

	// First we want to tell OpenGL with VAO is going to be used for rendering.  SInce there could
	// be other Model objects being rendered with their own VAO, we need to set it every time we render.
	glBindVertexArray(VertexArrayObjectId);

	// Before drawing our model, we need to specify the attribute index that will be used when drawing
	// this model, which in our case is just the vertex attribute (0).  Later we will also enable color.
	glEnableVertexAttribArray(kVertexIndex);

	// glDrawArrays() is the function that actually tells OpenGL to draw the triangles from our VBO,
	// according to the VAO which stores the state and glVertexAttribPointer() properties used when we 
	// created the VBO in our Initialize() function.  We first pass in GL_TRIANGLES since we are rendering
	// triangles, then a starting position of 0 within the array since we want to render all of the
	// vertices from the the begiining of the array, and then finally the amount of vertices to draw.
	glDrawArrays(GL_TRIANGLES, 0, VerticesLength);

	// Now we do our cleanup, which is to disable the current vertex attribute for our VAO/VBO.
	glDisableVertexAttribArray(kVertexIndex);

	// Next, let's reset the current VAO binding to 0, which clears out any binding. This isn't necessary
	// since the next time a glBindVertexArray() would be called from another Model it will override 
	// the current one, but just to show how to clear a VAO binding I added this for those interested.
	glBindVertexArray(0);

	// This turns off the shader so that other objects rendered after this won't be using this shader.
	// The idea is that this is how you would turn on and off shaders for different objects being drawn
	// with their own shaders. Say for instance, one character might have a cartoon-rendered look and
	// another would have a more realistic rendering that requires completely separate shaders.
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
// Now you can see how to initialize and draw with a Vertex Buffer Object (VBO) 
// and a Vertex Array Object (VAO), which replaces the old way of using glBegin()
// and glEnd() in OpenGL 2.0 and below.  OpenGL 3.0 is the version where it changed.
// Instead of using glBegin()/glEnd(), we now need to use glDrawArrays() which draws
// from the buffers stored on the video card.
// 
// We also went over in this file about how to use our Shader class to initialize,
// then turn on and off our shader when rendering.  Checkout Shader.cpp for more
// details on how the shaders are loaded, compiled and assigned.
//
// The next tutorial will go over adding the ability to pass in colors to the 
// shaders, which will require us to use an extra glVertexAttribPointer() call to
// tell OpenGL where the color information is within our buffer.
// 
// © 2000-2014 GameTutorials