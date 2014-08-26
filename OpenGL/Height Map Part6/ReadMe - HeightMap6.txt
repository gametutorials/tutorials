//***********************************************************************
//	
//	- "Talk to me like a 3 year old!" Programming Lessons -			 
//                                                                       
//	$Author:	DigiBen	DigiBen@GameTutorials.com		 
//		
//	$Program:	HeightMap6							
//
//	$Description:	Using shaders we show realistic-refracted water
//
//***********************************************************************

Files: 		Main.cpp (The Source File containing the main program code)
		Init.cpp (The Source File containing the Win32 and OpenGL initialization code)
		Camera.cpp (The Source File containing the camera code)
		CShader.cpp (The Source File containing the shader code)
		Water.cpp (The Source File containing the water code)
		Terrain.cpp (The Source File containing the terrain rendering code)
		Terrain.h (The Header File containing the "terrain" function definitions)
		Water.h (The Header File cotnaining the water functions)
		CShader.h (The Header File cotnaining the shader class definition)
		Camera.h (The Header File cotnaining the camera class definition)
		main.h (The Header File containing definitions for the main program)
		HeightMap6.vcxproj  (The VC++ Project File holding the project info)
		HeightMap6.sln  (The Solution File holding the solution info)
		Terrain.raw (The map file)
		Terrain.bmp (The Bitmap File used for terrain)
		Back, Bottom, Front, Right, Top.bmp (The Bitmap Files used for the sky box) 
		NormalMap.bmp, DUDVMap.bmp and Caust00.bmp-Caust31.bmp (Bump/Refraction/Caustic maps)
		Water.vert (The vertex shader text file)
		Water.frag (The pixel/fragment shader text file)

CONTROLS:	The controls are just like the last tutorials.  The camera is moved by
		the mouse and the arrow keys (also w, a, s, d).  The mouse buttons
		turn detail texturing on/off as well as switch to wireframe.  The fog in
		the water can be changed by the + and minus keys.  We also added these keys:

		F1 - Makes the water appear farther away by shrinking the water texture
		F2 - Makes the water appear closer by enlarging the water texture
		F3 - Makes the water move faster
		F4 - Makes the water slower

Libraries:	opengl32.lib (The OpenGL Interface Library needed to compile the program)
		glu32.lib (The OpenGL Utility Library needed to compile the program)
		glaux.lib (The OpenGL Auxiliary Library needed to compile the program)

Instructions:	If you have Microsoft Visual Studio 2005 just click on the
		<Program Name>.vcxproj file.  This will open up visual c++.  You will most
		likely see the code for <Program Name>.cpp.  If you don't, press CRTL+ALT+L
		This will open the "Solution Explorer".  There you should find two folders named
		"Source Files" and "Header Files".  Double click on the "source" folder and you
		should see your source file(s) with a .cpp extension after it.  Double click on the
		file and it
 will open up in your main window.  Hit Control-F5 to run the program.
		You will probably see a prompt to compile/build the project.  Click OK and
		the Win32 program should pop up. :)

EULA:  		Your use of this tutorial constitutes your agreement to GameTutorials' Terms of Use found
		at:  http://www.gametutorials.com/TermsOfUse.htm

www.GameTutorials.com
©2000-2006 GameTutorials