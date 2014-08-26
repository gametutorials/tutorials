#ifndef	BITMAP_CLASS_H
#define	BITMAP_CLASS_H

/* 
	Here is our Win32 image class.  Windows stores images a bitmaps, so we will convert
	the .pcx file as we load it to a bitmap so Windows can then explain it.  Most of the
	specifics to parsing/loading the .pcx can be found in the "loadPCX" method. 
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>

typedef unsigned char uchar;

// RGB offsets, per pixel, for Win32 bitmaps
#define	BLUE_BITMAP_OFFSET	0
#define	GREEN_BITMAP_OFFSET	1
#define	RED_BITMAP_OFFSET	2

// A bitmap class which for Win32 bitmaps
class CBitmap
{
	public:

		// Constructor() -- Zero's out CBitmap
		CBitmap() :	mBmp(NULL),mOldBmp(NULL),mHDC(NULL),mPixels(NULL),
					mWidth(0),mHeight(0),mChannels(0),
					mStride(0) { GdiFlush(); /* Make sure that writing to CBitmap is okay */ }

		// Data Access Functions ***
		
			inline int getWidth() const { return mWidth; } 
			inline int getHeight() const { return mHeight; }
			inline int getChannels() const { return mChannels; }
			inline int getStride() const { return mStride; }
			inline HDC getHDC() const { return mHDC; }

		// *** End of Data Access Functions
		
		// This is the file we care most about, it loads a .pcx file
		bool loadPCX(const char *fileName);

		// Returns true for success -- false otherwise
		// If set_size is called on a CBitmap that already has memory associated with it
		// that memory is freed and the new size is implemented
		bool setSize(int width, int height, int channels);

		// If we're dealing with an 8-bit CBitmap, this function will set the 
		// palette to what is passed in
		// Returns true for success -- false otherwise
		bool setPalette(int startColor, int numColors, RGBQUAD *colors);

		uchar* getLinePtr(int which); // Returns a pointer to the row of pixels passed in
		
		// Deconstructor();
		~CBitmap();
		
	private:

		int mWidth; // Width of the bitmap in pixels
		int mHeight; // Height of the bitmap in pixels
		int mChannels; // Number of bytes per pixel (ie 24-bit .bmp would have 3 channels)
		int mStride; // Number of bytes in any row of pixels

		HBITMAP mBmp; // Handle to the bitmap
		HBITMAP mOldBmp; // Old handle to the bitmap, we use this to properly free up memory

		HDC mHDC; // Device context associated with our bitmap

		uchar *mPixels; // Pointer to the individual pixels in our bitmap

		void freeMem(); // Frees all mem associated with CBitmap
		
		// ** PCX Helper functions **
		
		// Loads the .pcx palette if one is present
		// Returns true on success, false otherwise
		bool loadPCXPalette(FILE *filePtr);
		
		// Reads in pixel data from the .pcx file and places it in 
		bool readScanLine(FILE *filePtr, BYTE *scanLine, int totalBytes);
		
};

#endif
