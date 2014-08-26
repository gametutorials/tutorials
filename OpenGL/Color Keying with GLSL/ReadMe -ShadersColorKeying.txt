//*******************************************************************************
//											
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 
//                                                                       
//		$Author:	DigiBen	digiben@gametutorials.com	
//											
//		$Program:	ShadersColorKeying
//						
//		$Description:	Using GLSL we create transparencies in textures
//											
//*******************************************************************************

Files:  	Main.cpp (The source file containing the main program code)
		Init.cpp (The source file containing the Win32 and OpenGL initialization code)
		Main.h (The header file containing definitions for the main program)
		CShader.cpp (The source file containing the code for the CShader class)
		CShader.h (The header file containing definitions for the CShader class)
		ShadersColorKeying.vcxproj  (The VC++ project file holding the project info)
		ShadersColorKeying.sln  (The solution file holding the solution info)
		ColorKeying.vert (The vertex shader text file)
		ColorKeying.frag (The pixel/fragment shader text file)
		Tree and Picture.bmp (The bitmap files used by this tutorial)

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