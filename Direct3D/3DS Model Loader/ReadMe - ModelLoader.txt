//*********************************************************************************//
//											
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 
//                                                                       
//		$Author:	TheTutor -- thetutor@gametutorials.com		 
//										 
//		$Program:	ModelLoader
//						 
//		$Description:	Shows how to load and render a 3DS File
//										 
//*********************************************************************************//

Files:	win_main.cpp  (The Source File containing the "window" code)
	d3d_mesh.cpp (The Source File containing the mesh object code)
	d3d_texture.cpp (The Source File containing the texture mapping implementation)
	d3d_obj.cpp  (The Source File containing the "D3D Interface/Object" code)
	d3d_obj.h  (The Header File containing "D3D Interface/Object" definitions)
	d3d_texture.h (The Header File containing "D3D Texture Map" definitions)
	d3d_mesh.h (The Header File containing "D3D Mesh Object" definitions)
	vertex_types.h (Header File containing FVF vertex definitions)
	IndexBuffers.vcxproj  (The VC++ Project File holding the project info)
	IndexBuffers.sln  (The Solution File holding the solution info)
	Texture.jpg (The JPEG File to be displayed)

Libraries:	d3d9.lib (The Direct 3D v9.0 Library needed to compile the program)
		d3dx9.lib (The D3D v9.0 "Math" Library needed to compile the program)

Instructions:	If you have Microsoft Visual Studio .NET 2003 (version 7.1) just click on the
		<Program Name>.vcxproj file.  This will open up up visual c++.  You will most
		likely see the code for <Program Name>.cpp.  If you don't, press CRTL+ALT+L
		This will open the "Solution Explorer".  There you should find two folders named
		"Source Files" and "Header Files".  Double click on the "source" folder and you
		should see your source file(s) with a .cpp extension after it.  Double click on the
		file and it will open up in your main window.  Hit Control-F5 to run the program.
		You will probably see a prompt to compile/build the project.  Click OK and
		a console window should pop up with the program. :)

EULA:  	Your use of this tutorial constitutes your agreement to GameTutorials' Terms of Use found
	at:  http://www.gametutorials.com/TermsOfUse.htm

www.GameTutorials.com
©2000-2007 GameTutorials
