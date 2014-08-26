// Done by TheTutor

/*
	Well for starters, what the heck is a DLL?  DLL stands for Dynamic Link Library.
	That means that the "dll executable code" isn't linked to until it's needed by your
	"main executable code".

	DLL's are used for two main reasons:

	1)  They give programmers the ability to have "a piece of code" shared by many
		applications easily and effectively.

    2)  They make updating an existing .exe (that uses DLL's) easy.  Simply pop in a new
		DLL and new things will happen.  Take Winamp as example.  DLL's can also hold
		things like bitmaps, so you could use them to update "player skins" like in
		Unreal Tournament.

	Okay, so enough with what a .dll is and it's uses.  Lets get to the meat and potatoes.
*/

/*	
	Project creation steps with Visual Studio 8.0

	1)  Started up the compiler
	2)  Select File -> New -> Project (or Crtl + Shift + N)
	3)  Select the Win32 project and name it
	4)  Click on "Application Settings"
	5)  Check "DLL" and "Empty Project"
*/


#include "print.h"

// __declspec	This is a Microsoft specific keyword (declare special).
//				Here's what MSDN has to say about it:
//				"The extended attribute syntax for qualifying storage-class information
//			    simplifies and standardizes Microsoft-specific extensions to the C and C++ 
//				languages".

// So basically what is going on is by passing "dllexport" into __declspec() we're
// saying that this function is to be EXPORTED to a .dll.

__declspec(dllexport) void printMessage()
{
	// Print out a message
	cout << "You just used a .dll" << endl << endl;
}

// Again, the "void printMessage()" is just like a "normal" function implementation.
// This function returns nothing (void) and takes no parameters.

/*
	So if you were to compile this, it would create two files we care about:

	PrintDLL.dll and PrintDLL.lib

	Now remember, depending on how you have the "view" for your "Window Folder
	Options" you may or may not see the file "PrintDLL.dll".

	To make it visible, go to "My Computer", select "View", then "Folder Options".
	Then select the "View" tab, then select "Show all files" .
*/	

// **NOTE**
/*
		When people make .dll's, a lot of the time you see them
		do something like the sample code below:
		
		// This #define would be in the .cpp file (this file for instance).
		// This gets done before you #include any header files.
		#ifndef DLL_API
		#define DLL_API __declspec(dllexport) 
		#endif

		// This #define would be in the .h file (print.h for instance)
		#ifndef DLL_API
		#define DLL_API __declspec(dllimport)
		#endif

		The reason this is done, is if you do it this way you WON'T have to change 
		the header file (print.h in this case) when you use your .dll with your 
		"main program".

		So what happens is FIRST the preprocessor comes along and DLL_API gets replaced 
		with __declspec(dllexport).  Then the header file gets processed but since
		DLL_API has already been #defined IT WILL NOT be #defined to __declspec(dllimport).
		So for the "DLL project" (this one)	DLL_API would equal __declspec(dllexport)
		in all instances.

		But when you just use the header file in another project, DLL_API would equal
		__declspec(dllimport) (which is what we want).
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/

