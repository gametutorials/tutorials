//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Exporter Part 2								 //
//																		 //
//		$Description:	3DS Max weighted-animation exporter	series		 //
//																		 //
//***********************************************************************//


// Below are the needed libraries to compile a max exporter
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "core.lib") 
#pragma comment(lib, "geom.lib") 
#pragma comment(lib, "mesh.lib") 
#pragma comment(lib, "maxutil.lib") 
#pragma comment(lib, "paramblk2.lib") 

// Include our main header file for the exporter
#include "Exporter.h"

// This store a global hInstance of the DLL
HINSTANCE g_hInstance;

// This tells us if the controls for the DLL have been initialized yet
bool g_bControlsInitialized = false;

// This is our 
static ExporterClassDesc g_ExporterDesc;


///////////////////////////////// GetExporterDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the class instance of our exporter information
/////
///////////////////////////////// GetExporterDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

ClassDesc2* GetExporterDesc() 
{ 
	return &g_ExporterDesc; 
}


///////////////////////////////// DllMain \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the DLL Main function that is needed for our DLL 
/////
///////////////////////////////// DllMain \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

BOOL WINAPI DllMain(HINSTANCE hInstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	// Store the DLL's hInstance
	g_hInstance = hInstDLL;

	// Make sure we only initialize our common controls once
	if (!g_bControlsInitialized) 
	{
		// We initialized our common controls
		g_bControlsInitialized = true;

		// Initialize Max's custom controls
		InitCustomControls(g_hInstance);

		// Initialize Win95 controls
		InitCommonControls();			
	}
			
	// Everything went well
	return (TRUE);
}


///////////////////////////////// LibDescription \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the library description
/////
///////////////////////////////// LibDescription \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

__declspec( dllexport ) const TCHAR* LibDescription()
{
	// We have nothing to return
	return NULL;
}


///////////////////////////////// LibNumberClasses \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns how many classes we have for our library
/////
///////////////////////////////// LibNumberClasses \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

__declspec( dllexport ) int LibNumberClasses()
{
	// We only create one extra class, Exporter
	return 1;
}


///////////////////////////////// LibClassDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns our class description for Exporter
/////
///////////////////////////////// LibClassDesc \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	// For each class we added, return it's class description
	switch(i) 
	{
		// The first and only class is our Exporter class
		case 0: 
			return GetExporterDesc();
		
		// Bad class ID; not a valid class
		default: 
			return 0;
	}
}


///////////////////////////////// LibVersion \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This tells Max which version of the SDK we are using
/////
///////////////////////////////// LibVersion \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

__declspec( dllexport ) ULONG LibVersion()
{
	// Return the current version of the SDK we are compiling with
	return VERSION_3DSMAX;
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Nothing new was added to this file since the last tutorial.
//
//
// © 2000-2006 GameTutorials