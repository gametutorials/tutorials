#ifndef GL_OBJ_H
#define GL_OBJ_H

#include <windows.h>
#include <gl\gl.h> // Header File For The OpenGL32 Library
#include <gl\glu.h>	// Header File For The GLu32 Library

// This is our OpenGL object that just sets up OpenGL for us in a Win32 environment
class CGLObj
{
	public:

		// Constructor -- Just set everything to NULL
		CGLObj():hdc(NULL),hglrc(NULL) { /* do nothing */ }
	
		// Inits "OpenGL" with passed in parameters
		bool init(HDC hdc, GLint colorBits, GLint depthBits);

		// Sets the view port and field of view
		// Returns true on success, false on failure
		bool setViewPort(GLint wid, GLint hgt, GLdouble fov);

		void begin(); // Begins rendering
		void end(); // Ends rendering

		~CGLObj(); // Deconstructor

	private:

		HDC hdc; // The window's HDC
		HGLRC hglrc; // OpenGL render context

		// Private data for creating the OpenGL object -- 
		// These are private because we only want them called once
		bool setPixelFormat(GLint colorBits, GLint depthBits);
		bool setRenderContext();

};

#endif


