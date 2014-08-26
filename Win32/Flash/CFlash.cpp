//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Flash											 //
//																		 //
//		$Description:	Load a Macromedia Flash movie in Win32			 //
//																		 //
//***********************************************************************//

// Include our CFlash class file
#include "CFlash.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this file we define our CFlash class.  You will notice that we define a
// lot of GUID IDs in our CFlash.cpp and CFlash.h files.  This is how we tell
// COM which classes to extract from the Flash DLLs.  The GUIDS should be
// stored in the registry, which is why it's important to have the Flash player
// installed correctly on your system.
// 
// Since we are using an external media library (Flash), we will want to use
// Microsoft's ATL (Active Template Library) to create a window where the Flash
// object can run in.  ATL allows us to work well with COM objects.  If you are
// unfamiliar with COM (Common Object Model), basically it is used to tap into
// DLLs and use their functions.  For example, if we had Microsoft Word
// installed, we can use COM to access the Word DLLs and call the dll functions.
// We are doing the same thing with Flash by accessing the special functions in
// the Flash DLL file(s).
//
// Let go over the code needed to initialize an ATL window with a Flash object
// running in it.
// 


// This class is needed in order to access the COM objects in run-time.
// If you don't include this the program will still compile, but will crash
// when the program is run.
CComModule _Module;

// This is the GUID ID for the main Flash class.  This number is very important
// and must stay the same in order to use COM with Flash.  A GUID is a Globally
// Unique Identifier", which is used to define things like programs and DLLs.
char *Flash_ClassID = "{D27CDB6E-AE6D-11CF-96B8-444553540000}";


// This is our constructor for our class.  It takes a window handle, full path to
// the file being loaded (.swf), the size of the desired Flash window, and two flags
// that say if we want to loop the file and play it upon loading.
CFlash::CFlash(HWND hWnd, LPSTR szFilePath, LPRECT pRect, bool bLoop, bool bPlay)
{
	// Init all of our variables
	m_hWnd = NULL;
	m_bPaused = false;
	m_pFlashInterface = NULL;
	m_pControlInterface = NULL;
	m_strMovie = "";
	m_totalFrames = 0;
	m_bLoop = bLoop;

	// Load the Flash file
	LoadMovie(hWnd, szFilePath, pRect, bPlay);
}


// If we don't want to load the movie upon creation use this constructor
CFlash::CFlash()
{
	// Init all of our variables
	m_hWnd = NULL;
	m_bPaused = false;
	m_pFlashInterface = NULL;
	m_pControlInterface = NULL;
	m_strMovie = "";
	m_totalFrames = 0;
	m_bLoop = true;
}


// This is the deconstructor that frees all the memory
CFlash::~CFlash()
{
	// Cleanup our allocated COM pointers
	CleanUp();
}


// This loads our movie with a window handle, movie name, window rect and play flag.
void CFlash::LoadMovie(HWND hWnd, char *szMovie, LPRECT pRect, bool bPlay)
{
	// If we already have Flash allocated, let's free it and start over.
	if(m_pFlashInterface)
		CleanUp();

	// We call this function to initialize ATL for our ATL window
	AtlAxWinInit();

	// Set the window handle for our member variable
	m_hWnd = hWnd;
	
	// If an ATL window is already open, destroy it to start over and free memory.
	if (m_flashWindow.IsWindow())
		m_flashWindow.DestroyWindow();

	// Create an ATL window for the Flash object and show it
	m_flashWindow.Create(m_hWnd, pRect, Flash_ClassID, WS_CHILD);
	m_flashWindow.ShowWindow(SW_SHOW);

	// If we have a valid rectangle for our Flash window
	if (pRect != NULL)
	{
		// Here we adjust the rect according to the ATL window's style (no menu = false).
		AdjustWindowRect(pRect, m_flashWindow.GetWindowLong(GWL_STYLE),FALSE);

		// Now we can set the ATL window's position for our Flash screen.  We don't want
		// to insert the window after another window so we pass in NULL, position the window
		// at 0, 0 according to our rect, then we pass in the width and height, then a bunch
		// of flags that make sure we can't move the window, ignore the WM_ERASEBKGND message,
		// make sure it doesn't activate, and a bunch of flags that make sure we let the Flash
		// object handle the drawing and updating.
		m_flashWindow.SetWindowPos(NULL, 0, 0, pRect->right - pRect->left, pRect->bottom - pRect->top,
								   SWP_NOMOVE | SWP_DEFERERASE | SWP_NOACTIVATE | SWP_NOCOPYBITS |
								   SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOZORDER);
	}

	// Now we actually start working with Flash.  This grabs a pointer to a COM Flash control.
	m_flashWindow.QueryControl(&m_pControlInterface);
	
	// Make sure we were able to find the COM object for Flash and allocate memory for it.
	if (!m_pControlInterface) 
		MessageBox(hWnd, "NULL unknown", "Error", MB_OK);

	// Now that we have a pointer to the control, let's get a pointer to the Flash interface
	// that has many of the Flash functions we will need.  The IID_IShockwaveFlash is our
	// GUID for the Flash interface.  This is how it knows which class to give us access to.
	HRESULT hr = m_pControlInterface->QueryInterface(IID_IShockwaveFlash,(void **)&m_pFlashInterface);
	
	// Make sure we retrieved a good pointer for our Flash class
	if (!m_pFlashInterface)
	{
		MessageBox(hWnd, "Unable to query IFlash", "Error", MB_OK);
		return;
	}

	// Set our member variable for the movie
	m_strMovie = szMovie;

	// Now we need to do some tricky conversions, since the LoadMovie()
	// function in the Flash interface takes a character type BSTR.  COM
	// uses this type of "basic string" so we need to convert to it.
	
	// Here we need to create a WCHAR array to input in MultiByteToWideChar()
	WCHAR* wszRealName = new WCHAR[MAX_PATH]; 

	// Convert the char string to a wide-character string.  CP_ACP is for Windows ANSI.
	MultiByteToWideChar(CP_ACP, 0, szMovie, -1, wszRealName, MAX_PATH);
	
	// Now we can pass in the file name to our LoadMovie() function
	m_pFlashInterface->LoadMovie(0, wszRealName);

	// Delete the allocated memory for the wide character string
	delete [] wszRealName;

	// If we didn't want it to play immediately, pause it.
	if(!bPlay)
		TogglePause();

	// If we didn't want to loop the movie, set looping to false
	if(!m_bLoop)
		m_pFlashInterface->put_Loop(FALSE);
	
	// Grab the frames in the animation/movie
	m_pFlashInterface->get_TotalFrames(&m_totalFrames);
}


// This turns pause on and off
void CFlash::TogglePause()
{
	// If we don't have a Flash interface, return.
	if(!m_pFlashInterface) return;

	// Set the pause on/off
	m_bPaused = !m_bPaused;

	// If we want to pause the animation
	if(m_bPaused)
	{
		// Hide the flash window and stop the play
		m_flashWindow.ShowWindow(SW_HIDE);
		m_pFlashInterface->StopPlay();
	}
	else
	{
		// If we want to start the animation again show the window and play
		m_flashWindow.ShowWindow(SW_SHOW);
		m_pFlashInterface->Play();
	}
}


// This stops the Flash movie
void CFlash::Stop()
{ 
	// Stop the playing if we have a valid pointer
	if(m_pFlashInterface)
		m_pFlashInterface->StopPlay();
}


// This release all our COM pointers
void CFlash::CleanUp()
{ 
	// If we have our control interface allocated, free it.
	if (m_pControlInterface) 
	{
		m_pControlInterface->Release();
		m_pControlInterface = NULL;
	}

	// If we have our Flash interface allocated, free it.
	if (m_pFlashInterface)
	{
		m_pFlashInterface->Release();
		m_pFlashInterface = NULL;
	}

	// If we have our ATL window allocated, destroy it.
	if (m_flashWindow.IsWindow())
		m_flashWindow.DestroyWindow();
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// Pretty simple to load a Flash movie isn't it?  The only hard part is
// converting the full path of the movie to a WCHAR, and obtaining the
// pointers for the Flash interface and control.
//
// Check out CFlash.h to see our classes that we created.
//
// Flash is a registered trademark of Macromedia.  All rights reserved.
//
//
// © 2000-2006 GameTutorials	