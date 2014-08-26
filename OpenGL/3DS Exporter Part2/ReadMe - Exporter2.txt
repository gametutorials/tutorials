//***********************************************************************
//	
//	- "Talk to me like a 3 year old!" Programming Lessons -			 
//                                                                       
//	$Author:	DigiBen	DigiBen@GameTutorials.com		 
//		
//	$Program:	Exporter Part 2							
//
//	$Description:	3DS Max weighted-animation exporter series
//
//***********************************************************************

Files:  	Exporter.cpp (The Source File containing the main exporter code)
		DLLEntry.cpp (The Source File containing the Win32 and OpenGL initialization code)
		Exporter.h (The Header File containing definitions for the main program)
		Exporter.def  (The VC++ Project File holding the project info)
		Exporter.sln  (The Solution File holding the solution info)
		Exporter.vcxproj  (The Project File that keeps track of the files)
		Vick.max (3DS Model)
		crabarmor.bmp (BMP File that is the texture for the model)
	
Libraries:	comctl32.lib (The library to do com for our DLL)
		core.lib (The core library for 3DS Max)
		geom.lib (The library for the basic 3DS Max library for geometry)
		mesh.lib (The library for the 3DS Max mesh class)
		maxutil.lib (The library for 3DS max utility functions)
		paramblk2.lib (The library for the ClassDesc2 class)


Instructions:	If you have Microsoft Visual Studio 2005 just click on the
		<Program Name>.sln file.  This will open up visual c++.  You will most
		likely see the code for <Program Name>.cpp.  If you don't, press CRTL+ALT+L
		This will open the "Solution Explorer".  There you should find two folders named
		"Source Files" and "Header Files".  Double click on the "source" folder and you
		should see your source file(s) with a .cpp extension after it.  Double click on the
		file and it will open up in your main window.  Hit Control-F5 to run the program.
		You will probably see a prompt to compile/build the project.  Click OK and
		the Win32 program should pop up. :)

EULA:  		Your use of this tutorial constitutes your agreement to GameTutorials' Terms of Use found
		at:  http://www.gametutorials.com/TermsOfUse.htm

www.GameTutorials.com
©2000-2006 GameTutorials