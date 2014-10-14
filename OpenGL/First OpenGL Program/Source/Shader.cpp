//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//																		 //
//		$Program:		First OpenGL Program							 //
//																		 //
//		$Description:	Init OpenGL 4 and draw a triangle to the screen	 //
//																		 //
//***********************************************************************//

#include "../Headers/Shader.h"							// References the Shader class definition and GLEW header

/*
This file has all of our main shader management code and implements the Shader class
methods.  For the actual shader code you will need to look in the .vertex and .fragment 
files to find the details of each line explained.  Keep in mind, the .vertex and .fragment
extensions are arbitrary, they can be anything you want since it's just a text file to load.

*How to create and use shaders*
In order to create a shader you need to do the following:

1) first call glCreateShader() function with a given type passed in, either GL_VERTEX_SHADER 
or GL_FRAGMENT_SHADER depending on the type desired (you will want to do one for each).  This
will return an Id for you to load and compile the shader with, as well as associate it with
a Shader Program so it can get turned on and off and then release it when you are done with it. 

2) The next step is to use that newly created Id to pass it into the glShaderSource() function,
along with the char array that has the loaded shader code in it (from the .vertex/.fragment files).
You call this for each shader file, in our case the vertex and fragment shader.  There is also
a geometry and tesselation shader which we will go over in a future tutorial.

3) Next you need to compile the shader with glCompileShader( shaderId ).  This internally
takes the shader code you passed into glCreateShader which is referenced by the Id you pass
in, and then compiles it to a language that the video card's GPU can read.

4) Next you need to create a Shader Program which you associate each shader you compiled with.
The Shader Program is created by glCreateProgram(), which returns a unique Id that is used
to turn the shader on and off when rendering.

5) Now that you have a Shader Program, you want to associate each compiled shader with it.
This is done by calling glAttachShader( programId, shaderId ).

6) Finally, you need to link the shader program with OpenGL by calling glLinkProgram( programId ).
According to the OpenGL documentation, this actually creates an executable for each vertex and
fragment shader that is run on a vertex and fragment processor on the GPU.

*Turning on and off a shader*
Once the Shader Program is created with the associated shader Ids, you need to call the
glUseProgram( programId ) function to turn it on.  Then glUseProgram(0) to turn it off.
This is done right before you render an object and right after you finish rendering it.
Refer to the Model::Render() function in Model.cpp for an example of this.
*/

// This function loads a text file and returns the code for our shaders
std::string Shader::LoadShaderFile(std::string strFile)
{
	// Open the file passed in
	std::ifstream fin(strFile.c_str());

	// Make sure we opened the file correctly
	if ( !fin )
		return "";

	std::string strLine = "";
	std::string strText = "";

	// Go through and store each line in the text file within a "string" object
	while ( getline(fin, strLine) )
	{
		strText = strText + "\n" + strLine;
	}

	// Close our file
	fin.close();

	// Return the text file's data
	return strText;
}


// This function loads a vertex and fragment shader file and creates them
void Shader::Initialize(std::string strVertexFile, std::string strFragmentFile)
{
	// This function actually loads, compiles and links our shaders.  Basically,
	// we create some pointers for shaders, then load in text files for
	// each shader, then compile the shader.  We also need to create a
	// program object that represents all of our shaders.  We link the
	// loaded shaders to our program object and then can turn it on when needed.

	// These will hold the shader's text file data
	std::string strVShader, strFShader;

	// Make sure the user passed in a vertex and fragment shader file
	if ( !strVertexFile.length() || !strFragmentFile.length() )
		return;

	// If any of our shader pointers are set, let's free them first
	if ( VertexShaderId || FragmentShaderId || ShaderProgramId )
		Destroy();

	// This function gets the last error that happened from OpenGL, after the last time this
	// function was called.  So by calling it now, we reset the errors and then call it below again.
	GLenum ErrorCheckValue = glGetError();

	// Here we get an Id to our vertex and fragment shaders
	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Now we load the shaders from the respective files and store it in a string
	strVShader = LoadShaderFile(strVertexFile.c_str());
	strFShader = LoadShaderFile(strFragmentFile.c_str());

	// Do a quick switch so we can do a double pointer below
	const char *szVShader = strVShader.c_str();
	const char *szFShader = strFShader.c_str();

	// Now this assigns the shader text file to each shader pointer
	glShaderSource(VertexShaderId, 1, &szVShader, nullptr);
	glShaderSource(FragmentShaderId, 1, &szFShader, nullptr);

	// Now we actually compile the shader code
	glCompileShader(VertexShaderId);
	glCompileShader(FragmentShaderId);

	// Next we create a program object to represent our shaders
	ShaderProgramId = glCreateProgram();

	// We attach each shader we just loaded to our program object
	glAttachShader(ShaderProgramId, VertexShaderId);
	glAttachShader(ShaderProgramId, FragmentShaderId);

	// Our last init function is to link our program object with OpenGL
	glLinkProgram(ShaderProgramId);

	// Now check to see if any errors happened in this function, which saves us tons of time instead
	// of trying to figure out why our screen is black and we would have no idea why.
	ErrorCheckValue = glGetError();

	// If there was an error found, print it to the screen and close the OpenGL window.
	if ( ErrorCheckValue != GL_NO_ERROR )
	{
		// Use gluErrorString(ErrorCheckValue) if you want to get the string value of the
		// error, but I removed this since Mac OSX 10.9 (Mavericks) deprecated this function.
		fprintf(stderr, "ERROR: Could not create the shader program with error Id: %d\n", ErrorCheckValue);
		exit(-1);
	}
}

// This function returns a variable Id for a shader variable.  This is used
// when we need to pass in variables into a shader like numbers and matrices.
GLint Shader::GetVariable(std::string strVariable)
{
	// If we don't have an active program object, let's return -1
	if(!ShaderProgramId)
		return -1;

	// This returns the variable Id for a variable that is used to find
	// the address of that variable in memory.
	return glGetUniformLocation(ShaderProgramId, strVariable.c_str());
}


// This function frees all of our shader data
void Shader::Destroy()
{
	// To free a shader we need to detach the vertex and fragment
	// shader pointers from the program object, then free each one.
	// Once that is done we can finally delete the program object.

	// If our vertex shader pointer is valid, free it
	if(VertexShaderId)
	{
		glDetachShader(ShaderProgramId, VertexShaderId);
		glDeleteShader(VertexShaderId);
		VertexShaderId = 0;
	}

	// If our fragment shader pointer is valid, free it
	if(FragmentShaderId)
	{
		glDetachShader(ShaderProgramId, FragmentShaderId);
		glDeleteShader(FragmentShaderId);
		FragmentShaderId = 0;
	}

	// If our program object pointer is valid, free it
	if(ShaderProgramId)
	{
		glDeleteShader(ShaderProgramId);
		ShaderProgramId = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this file we define our Shader class functions.  As a quick overview, to
// initialize a shader you need to use the following functions:
//
// glCreateShader()
// glShaderSource()
// glCompileShader()
// glAttachShader()
// glCreateProgram()
// glLinkProgram()
// 
// Then to turn on and off a shader, you call glUseProgram().
//
// Please review the Shader.vertex and Shader.fragment text files to see how our
// GLSL shaders are created.
//
//
// © 2000-2014 GameTutorials