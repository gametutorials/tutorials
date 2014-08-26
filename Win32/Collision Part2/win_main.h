#ifndef WIN_MAIN_H
#define WIN_MAIN_H

#include <windows.h>
#include <time.h>



struct WIN_OBJ
{
	// Constructor -- Init the win_obj
	WIN_OBJ(int scr_wdth = 640, int scr_hgt = 480):win_hwnd(NULL),hpen(NULL),old_pen(NULL),
												   win_dc(NULL),back_dc(NULL),bmp_dc(NULL),
												   old_bmp(NULL),screen_width(scr_wdth),
												   screen_height(scr_hgt)  { /* do nothing */ }
	
	HWND win_hwnd;

	HPEN hpen;
	HPEN old_pen;

	HDC win_dc;		 // The window's HDC
	HDC back_dc;	// The "back buffering" HDC
	HDC bmp_dc;    // HDC used for blitting bitmaps

	HBITMAP hbitmap; // The double buffering bitmap
	HBITMAP old_bmp; // Used to store the "original" bitmap (before double buffering)
					 // We need this to free up ALL memory at the end of the program 

	int screen_width;
	int screen_height;

};



void free_win_obj(); // Frees WIN_OBJ

#endif
