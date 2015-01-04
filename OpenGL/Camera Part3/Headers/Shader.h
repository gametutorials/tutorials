#ifndef _Shader_H
#define _Shader_H

#include <string>										// Used for our STL string objects
#include <fstream>										// Used for our ifstream object to load text files
#include "GL\glew.h"									// Used for the OpenGL types like GLuint, GLfloat, etc.


// This is our very basic shader class that we will use to load and turn on/off our shaders
class Shader
{
public:

	// Create an empty constructor and have the deconstructor release our memory.
	Shader()	{ }
	~Shader()	{ Destroy(); }

	// This loads our text file for each shader and returns it in a string
	std::string LoadShaderFile(std::string strFile);

	// This loads a vertex and fragment shader from a text file (relative or full path)
	void Initialize(std::string strVertexFile, std::string strFragmentFile );
	
	// This returns an ID for a variable in our shader, to be used with a Set*() function
	GLint GetVariable(std::string strVariable);

	// Below are functions to set an integer, a set of floats or a matrix (float[16])
	void SetInt(GLint id, int newValue)											{ glUniform1i(id, newValue);		}
	void SetFloat(GLint id, GLfloat newValue)									{ glUniform1f(id, newValue);		}
	void SetFloat2(GLint id, GLfloat v0, GLfloat v1)							{ glUniform2f(id, v0, v1);			}
	void SetFloat3(GLint id, GLfloat v0, GLfloat v1, GLfloat v2)				{ glUniform3f(id, v0, v1, v2);		}
	void SetFloat4(GLint id, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)	{ glUniform4f(id, v0, v1, v2, v3);	}
	
	// This allows us to pass in 4x4 matrix (float array of 16) to a shader by the Id received from GetVariable()
	void SetMatrix4(GLint id, GLsizei count, GLboolean transpose, const GLfloat *value)
	{ 
		glUniformMatrix4fv(id, count, transpose, value);
	}

	// These 2 functions turn on and off our shader, which uses the OpenGL glUseProgram() function
	void TurnOn()		{	glUseProgram(ShaderProgramId);  }
	void TurnOff()		{	glUseProgram(0);				}
	
	// This releases our memory for our shader
	void Destroy();

private:

	// This Id stores our vertex shader information
	GLuint VertexShaderId;

	// This Id stores our fragment shader information
	GLuint FragmentShaderId;

	// This Id stores our program information which encompasses our shaders
	GLuint ShaderProgramId;
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this file we have our Shader class definition, which handles the loading,
// compiling, attaching and using of shaders.
// 
// 
// © 2000-2015 GameTutorials
