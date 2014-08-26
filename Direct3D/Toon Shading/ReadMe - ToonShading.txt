//*********************************************************************************//
//											
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 
//                                                                       
//		$Author:	TheTutor -- thetutor@gametutorials.com		 
//										 
//		$Program:	ToonShading
//						 
//		$Description:	Goes over a technique for cartoon shading
//											 
//*********************************************************************************//

Files:		win_main.cpp  (Source file containing the "window" code)
		d3d_obj.cpp  (Source file containing the "D3D Interface/Object" code)
		d3d_texture.cpp (Source file containng the "D3D texture object" code)
		d3d_texture.h (Header file containing the D3D texture object definition)
		d3d_obj.h  (Header file containing "D3D Interface/Object" definitions)
		vertex_types.h (Header file containing FVF vertex definitions)
		Shader.fx (Effect file containing shader and effect implementation)
		ToonShading.vcxproj  (The VC++ project file holding the project info)
		ToonShading.sln  (The solution file holding the solution info)
		shade.bmp (A BMP File used as a texture)
		edge.bmp (A BMP File used as a texture)

Libraries:	d3d9.lib (The Direct 3D v9.0 Library needed to compile the program)
		d3dx9.lib (The D3D v9.0 "Math" Library needed to compile the program)

Instructions:	If you have Microsoft Visual Studio 2005 just click on the
		<Program Name>.vcxproj file.  This will open up up visual c++.  You will most
		likely see the code for <Program Name>.cpp.  If you don't, press CRTL+ALT+L
		This will open the "Solution Explorer".  There you should find two folders named
		"Source Files" and "Header Files".  Double click on the "source" folder and you
		should see your source file(s) with a .cpp extension after it.  Double click on the
		file and it will open up in your main window.  Hit Control-F5 to run the program.
		You will probably see a prompt to compile/build the project.  Click OK and
		a console window should pop up with the program. :)

EULA:  		Your use of this tutorial constitutes your agreement to GameTutorials' Terms of Use
		found at:  http://www.gametutorials.com/TermsOfUse.htm

www.GameTutorials.com
©2000-2006 GameTutorials