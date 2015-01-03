//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera											 //
//																		 //
//		$Description:	Moves a camera in 3D space from user input		 //
//																		 //
//***********************************************************************//

#include "../Headers/Shader.h"							// References the Shader class definition and GLEW header


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
	// These will hold the shader's text file data
	std::string strVShader, strFShader;

	// Make sure the user passed in a vertex and fragment shader file
	if ( !strVertexFile.length() || !strFragmentFile.length() )
		return;

	// If any of our shader pointers are set, let's free them first
	if ( VertexShaderId || FragmentShaderId || ShaderProgramId )
		Destroy();

	// Reset the last OpenGL error so we can check if down below
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

	// Now check to see if any errors happened in this function
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

	// This returns the variable Id for a variable in a shader file
	return glGetUniformLocation(ShaderProgramId, strVariable.c_str());
}


// This function frees all of our shader data
void Shader::Destroy()
{
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
// © 2000-2015 GameTutorials
