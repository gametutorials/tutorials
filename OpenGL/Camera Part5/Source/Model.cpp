//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 5									 //
//																		 //
//		$Description:	Camera Strafing									 //
//																		 //
//***********************************************************************//

#include "../Headers/Model.h"

// Initialize the model with vertices, length, and the vertex and fragment shader files
void Model::Initialize(Vertex3 vertices[], int length, std::string strVertex, std::string strFragment)
{
	// Create and compile our GLSL vertex and fragment shaders from the shader text files
	Shader.Initialize(strVertex, strFragment);

	// Store the list of vertices and the length in our member variables
	Vertices = vertices;
	VerticesLength = length;

	// Reset the errors before we check again below
	GLenum ErrorCheckValue = glGetError();
	
	// Create the Vertex Array Object (VAO)
	glGenVertexArrays(1, &VertexArrayObjectId);
	glBindVertexArray(VertexArrayObjectId);

	// Create the Vertex Buffer Object (VBO)
	glGenBuffers(1, &VertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId);

	// Store the vertices in the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * length, Vertices, GL_STATIC_DRAW);

	// Here is where we tell OpenGL what the format each vertex is within our buffer.  
	glVertexAttribPointer(kVertexIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]), 0);

	// Add the color attributes to our Vertex Array Object and Vertex Buffer Object
	glVertexAttribPointer(kColorIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]), (GLvoid*)sizeof(Vertices[0].xyz));

	// Now check to see if any errors happened in this function
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

	// Grab the view and projection matrices from the current camera
	mat4 projectionMatrix = Camera->GetProjectionMatrix();
	mat4 viewMatrix = Camera->GetViewMatrix();

	// Create the Model Matrix with the current position
	mat4 modelMatrix = translate(mat4(1.0f), Position);

	// Similar to the Position above, we do the same for the Scale
	modelMatrix = scale(modelMatrix, Scale);

	// Add any rotation to the Model Matrix for each axis
	modelMatrix = rotate(modelMatrix, Rotation.x, vec3(1, 0, 0));	// x-axis
	modelMatrix = rotate(modelMatrix, Rotation.y, vec3(0, 1, 0));	// y-axis
	modelMatrix = rotate(modelMatrix, Rotation.z, vec3(0, 0, 1));	// z-axis

	// Get the variables from the shader for each matrix
	GLint modelMatrixId = Shader.GetVariable("modelMatrix");
	GLint viewMatrixId = Shader.GetVariable("viewMatrix");
	GLint projectionMatrixId = Shader.GetVariable("projectionMatrix");
	
	// Set the model, view and projection matrix for the uniform matrices in the Vertex Shader.
	// To get at a GLM mat4 value, we use the address of the first item [0][0].
	Shader.SetMatrix4(modelMatrixId, 1, false, &modelMatrix[0][0]);
	Shader.SetMatrix4(viewMatrixId, 1, false, &viewMatrix[0][0]);
	Shader.SetMatrix4(projectionMatrixId, 1, false, &projectionMatrix[0][0]);

	// First we want to tell OpenGL with VAO is going to be used for rendering
	glBindVertexArray(VertexArrayObjectId);

	// Tell OpenGL to enable our vertex information within the shaders
	glEnableVertexAttribArray(kVertexIndex);

	// Tell OpenGL to enable our color information within the shaders
	glEnableVertexAttribArray(kColorIndex);

	// Tell OpenGL to draw the triangles from our VBO
	glDrawArrays(GL_TRIANGLES, 0, VerticesLength);

	// Disable the color index
	glDisableVertexAttribArray(kColorIndex);

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

	// Be sure to stop pointing to the camera object
	Camera = nullptr;

	// Release the memory for the compiled shaders
	Shader.Destroy();
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file handles the creating and rendering of a Model.
// 
// © 2000-2015 GameTutorials