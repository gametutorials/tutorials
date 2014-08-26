//***********************************************************************
//	
//	- "Talk to me like a 3 year old!" Programming Lessons -			 
//                                                                       
//	$Author:	DigiBen	DigiBen@GameTutorials.com		 
//		
//	$Program:	Loader							
//
//	$Description:	Loads the GT Skeletal Animation format .GTF
//
//***********************************************************************

Files:  	Main.cpp (The Source File containing the main program code)
		Init.cpp (The Source File containing the Win32 and OpenGL initialization code)
		Gtf.cpp (The Source File containing the .gtf load and rendering code)
		Gtf.h (The Header File containing the gtf class definitions)
		main.h (The Header File containing definitions for the main program)
		Loader.vcxproj  (The VC++ Project File holding the project info)
		Loader.sln  (The Solution File holding the solution info)
		Loader.exe (The executable file)
		Vick.gtf (The .gtf model file)
		CrabArmor.bmp (The Bitmap File used for the model) 

Libraries:	opengl32.lib (The OpenGL Interface Library needed to compile the program)
		glu32.lib (The OpenGL Utility Library needed to compile the program)
		glaux.lib (The OpenGL Auxiliary Library needed to compile the program)
		Winmm.lib (The library so we can use timeGetTime())

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