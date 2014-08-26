#ifndef WIN_MAIN_H
#define WIN_MAIN_H

#include <windows.h>
#include <time.h>

#define FRAME_RATE 30	// 30 sounds good to me %)

struct WIN_OBJ
{
	HWND win_hwnd;

	HDC win_dc;		 // The window's HDC
	HDC back_dc;	// The "back buffering" HDC

	HBITMAP hbitmap; // The double buffering bitmap
	HBITMAP old_bmp; // Used to store the "original" bitmap (before double buffering)
					 // We need this to free up ALL memory at the end of the program 

	int screen_width;
	int screen_height;

};



// Sets up double buffering -- Returns true on success, false on failure
bool DoubleBufferInit(WIN_OBJ &win_obj);

void FreeWinObj(WIN_OBJ &win_obj); // Frees WIN_OBJ

#endif
