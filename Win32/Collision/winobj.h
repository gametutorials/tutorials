#ifndef WINOBJ_H
#define WINOBJ_H

#include <windows.h>

// This is our window object class -- Once we've created our window it handles 
// all of other "window related" tasks for us
class CWinObj
{
	public:
		
		CWinObj(); // Constructor

		bool init(HWND win_hwnd); // Inits a CWinObj -- Returns true on success, false on failure

		// Data Access ***

			HWND getHWND() { return win_hwnd; }

			HDC getHDC() { return win_hdc; }
			HDC getBackHDC() { return back_hdc; }

			RECT getWindowRect() { return win_rect; }
			RECT getClientRect() { return client_rect; }

			int getClientWid() { return client_rect.right; }
			int getClientHgt() { return client_rect.bottom; }

			int getWindowWid() { return win_rect.left - win_rect.right; }
			int getWindowHgt() { return win_rect.top - win_rect.bottom; }

		// *** End Data Access

		// Deconstructor
		~CWinObj();

	private:

		HWND win_hwnd; // A copy of the window's HWND

		HDC win_hdc; // The window's device context
		HDC back_hdc; // The back buffer's device context

		HBITMAP hbitmap; // Bitmap or "back buffer" to draw to for blitting to the window
		HBITMAP old_bmp; // Holds the "old" HBITMAP so we can select it back to free up memory

		RECT win_rect; // The rectangle (in screen coordinates) of the window
		RECT client_rect; // The rectangle (in client coordinates) of the window

		// *** Private methods ***
		
		void freeObj(); // Frees the CWinObj 
};


#endif
