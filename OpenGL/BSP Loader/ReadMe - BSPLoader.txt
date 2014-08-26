//*****************************************************************************//
//										 
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 
//                                                                       
//		$Author:	DigiBen	digiben@gametutorials.com
//			
//		$Program:	BSPLoader2
//
//		$Description:	Loads faces and textures from a Quake3 BSP file	 
//											
//****************************************************************************//

Files:  	Main.cpp (The Source File containing the main program code)
		Init.cpp (The Source File containing the Win32 and OpenGL initialization code)
		Camera.cpp (The Source File containing the camera implementation)
		Image.cpp (The Source File containing the image loading implementation)
		Quake3Bsp.cpp (The Source File containing the Quake 3 BSP implementation)
		Quake3Bsp.h (The Header File containing the Quake 3 BSP definitions)
		Image.h (The Header File containing the image loading definitions)
		Camera.h (The Header File containing the camera definitions)
		main.h (The Header File containing definitions for the main program)
		jpeglib.h (Independent JPEG Group's Header File containg .jpeg loading definitions)
		BspLoader2.vcxproj  (The VC++ Project File holding the project info)
		BspLoader2.sln  (The Solution File holding the solution info)
		config.ini (The INI File containing settings about the map)
		maps Directory (Contains the BSP file for loading)
		textures Directory (Contains the texture maps for the level)
		Quake3Format.htm (A HTML document that goes over the "unofficial" BSP file format)
		
Libraries:	opengl32.lib (The OpenGL Interface Library needed to compile the program)
		glu32.lib (The OpenGL Utility Library needed to compile the program)
		glaux.lib (The OpenGL Auxiliary Library needed to compile the program)
		jpeg.lib (Independent JPEG Group's JPEG Library needed to compile the program)
		winmm.lib (The Windows Multimedia Library needed to compile the program)

Instructions:	If you have Microsoft Visual Studio 2005 just click on the
		<Program Name>.vcxproj file.  This will open up visual c++.  You will most
		likely see the code for <Program Name>.cpp.  If you don't, press CRTL+ALT+L
		This will open the "Solution Explorer".  There you should find two folders named
		"Source Files" and "Header Files".  Double click on the "source" folder and you
		should see your source file(s) with a .cpp extension after it.  Double click on the
		file and it will open up in your main window.  Hit Control-F5 to run the program.
		You will probably see a prompt to compile/build the project.  Click OK and
		the Win32 program should pop up. :)

EULA:  		Your use of this tutorial constitutes your agreement to GameTutorials' Terms of Use
		found at:  http://www.gametutorials.com/TermsOfUse.htm

www.GameTutorials.com
©2000-2006 GameTutorials
