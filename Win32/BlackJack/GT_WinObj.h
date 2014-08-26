#ifndef WINOBJ_H
#define WINOBJ_H

#include <windows.h>

// This is a base window object that handles basic Win32 functionality for us
class CWinObj
{
	public:
		
		CWinObj(); // Constructor

		// Inits a CWinObj -- Returns true on success, false on failure
		virtual bool init(HINSTANCE hinstance, HWND hwnd);
		
		// Sets window dimensions to the specified WINDOW width/height
		// Returns true for success, false otherwise
		virtual bool setWindowSize(int wid, int hgt);
		
		// Sets window dimensions to the specified CLIENT width/height
		// Returns true for success, false otherwise
		virtual bool setClientSize(int wid, int hgt);
		
		// Sets the style and extended style of the window to the values passed in
		// Returns true on success, false otherwise
		virtual bool setStyle(DWORD style = 0, DWORD exStyle = 0);
		
		virtual bool toggleFullScreen(); // Toggles between window mode and full screen mode
										// Returns true for success, false otherwise
		
		// Returns status of being full screen or not						
		bool isFullScreen() { return mFullScreen; }

		// Data Access ***

			HINSTANCE getHINST() { return mHinst; }
			HWND getHWND() { return mHwnd; }
			HDC getDC() { return mHdc; }

			RECT getClientRect() { return mRect; }
			
			int getClientWid() { return mRect.right; }
			int getClientHgt() { return mRect.bottom; }
			
			DWORD getStyle() { return mStyle; }
			DWORD getStyleEx() { return mExStyle; }

		// *** End Data Access

		// Deconstructor
		virtual ~CWinObj();

	protected: // -- The data --

		HINSTANCE mHinst; // A copy of the HINSTANCE that was used to make the window
		HWND mHwnd; // A copy of the window's HWND
	
		HDC mHdc; // The window's device context
		
		RECT mRect;
		
		DWORD mStyle;
		DWORD mExStyle;

		bool mFullScreen; // True if in full screen mode, false otherwise
		
	private: // -- Private methods --
			
		void freeMem(); // Free's memory
		void zeroVars(); // "Zeros" out all the variables
		
		// Private copy constructor and assignment operator so 
		// no accidental copies are made
		CWinObj(const CWinObj &obj) { }
		CWinObj& operator =(const CWinObj &obj) { return *this; }
};

#endif

extern CWinObj *gWinObj;

// © 2000-2005 GameTutorials
