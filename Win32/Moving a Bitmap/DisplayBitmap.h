#ifndef _DISPLAY_BITMAP_H
#define _DISPLAY_BITMAP_H

#include <iostream>											// Include the standard C++ file
#include <windows.h>										// We need to include windows.h
using namespace std;										// Include the standard namespace															

typedef struct _BMP
{
	BITMAPFILEHEADER	*bmFileHeader;
	BITMAPINFO		 	*bmFileInfo;
	HBITMAP				hBitmap;
	HANDLE				hFilePointer;
	HANDLE				hFileMapping;
	PUCHAR				BeginOfFile;
	PUCHAR				BeginOfImage;
	char				szFileName[48];
}BMP;

// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// This loads a bitmap and assigns the info to the BMP structure																												
bool LoadABitmap(BMP *Bmp, char *szFile);

// This displays the bitmap to the screen at a X and a Y location
void DisplayBitmap(HDC hdc, BMP *bitmap, int x, int y);

#endif
