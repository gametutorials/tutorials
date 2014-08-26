#include "winobj.h"

// Constructor
CWinObj::CWinObj()
{
	win_hwnd = NULL; // Set window handle to NULL
	win_hdc = back_hdc = NULL; // Set HDCs to NULL
	hbitmap = old_bmp = NULL; // Set HBITMAPs to NULL
	
	memset(&win_rect,0,sizeof(RECT)); // Zero out window RECT
	memset(&client_rect,0,sizeof(RECT)); // Zero out client RECT
}

bool CWinObj::init(HWND hwnd)
{
	// First free up anything that may be allocated
	freeObj();

		// Error Check
		if(!hwnd)
			return false; // Not a valid HWND passed in
	
	win_hwnd = hwnd; // Set window handle

	// Set the window RECT making sure we can
	if(!GetWindowRect(win_hwnd,&win_rect))
		return false;

	// Set the client RECT making sure we can
	if(!GetClientRect(win_hwnd,&client_rect))
		return false;

	// Capture window's HDC
	win_hdc = GetDC(win_hwnd);

		// Error Check
		if(win_hdc == NULL)
			return false;

	// Set up the double buffering ***

	back_hdc = CreateCompatibleDC(win_hdc); // Create compatible HDC

		// Error Check
		if(back_hdc == NULL)
			return false;

	hbitmap = CreateCompatibleBitmap(win_hdc,getClientWid(),getClientHgt());

		// Error Check
		if(hbitmap == NULL)
			return false;

	// Select the created back buffer into our window's device context
	old_bmp = (HBITMAP)SelectObject(back_hdc,hbitmap);

		// Error Check
		if(old_bmp == NULL)
			return false;

	// *** Finish setting up double buffering

	return true; // Initialization phase completed successfully
}

// Deconstructor
CWinObj::~CWinObj() { freeObj(); }

// -- Private methods --
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

void CWinObj::freeObj()
{
	if(old_bmp)
	{
		SelectObject(back_hdc,old_bmp); // Select back original HBITMAP
		old_bmp = NULL;
	}

	if(hbitmap)
	{
		DeleteObject(hbitmap);
		hbitmap = NULL;
	}

	if(back_hdc)
	{
		DeleteDC(back_hdc);
		back_hdc = NULL;
	}

	if(win_hdc)
	{
		ReleaseDC(win_hwnd,win_hdc);
		win_hdc = NULL;
	}
}



