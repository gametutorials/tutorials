#ifndef	BITMAP_CLASS_H
#define	BITMAP_CLASS_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>

typedef unsigned char uchar; // Less typing :)

// Here is our high color bitmap class -- It will allow us to load 24 or 32 bit .bmp files
class CBitmap
{
	public:

		// Constructor() -- Zero's out CBitmap
		CBitmap():hbitmap(NULL),old_bmp(NULL),bmp_height(0),bmp_width(0),
				  bmp_channels(0),bmp_stride(0),surface_bits(NULL),bmp_hdc(NULL)
				  { GdiFlush(); /* Guarantee that writing to CBitmap is okay */ }

		
		// Data Access Functions ************
		
			inline int getWidth() const { return bmp_width; } 
			inline int getHeight() const { return bmp_height; }
			inline int getChannels() const { return bmp_channels; }
			inline int getStride() const { return bmp_stride; }
			inline HDC getHDC() const { return bmp_hdc; }
		
		// ****** End of Data Access Functions

		// This is a method that creates (initializes) our member variable "hbitmap"
		// If this method is called on a CBitmap that already has memory associated with
		// it, that memory is freed first.
		// We can call this method to create an "empty CBitmap" if we wanted :)
		// hdc is the HDC we're going to make the CBitmap compatible to
		// width and height are the width and height of the CBitmap
		// channels will specify whether this is a 24 (3 channels) or 32 (4 channels) bit image
		bool init(HDC hdc, int width, int height, int channels);

		// Loads a bmp with specified file_name, using hdc as the HDC to be compatible with
		// If loadBMP() is called on a CBitmap that already has memory associated with
		// it, that memory is freed and the .bmp is loaded
		// Returns true on success, false on failure
		bool loadBMP(char *file_name, HDC hdc);
		
		// This returns us a pointer to a "line of pixels" specified by which_line
		uchar* getLinePtr(int which_line);
		
		// Deconstructor() -- Free's up memory
		~CBitmap();
		

	private:

		HBITMAP hbitmap; // Our bitmap to be created
		HBITMAP old_bmp; // Our bitmap for holding the "old bitmap"

		int bmp_height; // Height of our CBitmap
		int bmp_width; // Width of our CBitmap
		int bmp_channels; // Number of channels in our CBitmap (either 3 or 4)
		
		int bmp_stride; // Our CBitmap's stride -- BITMAP's in windows are always DWORD
					   // aligned, so generally the actual number of bytes for a particular
					  // scan line of an CBitmap is (bmp_width * bmp_channels) bytes long
					 // but if that does not happen to be DWORD aligned (divisible evenly by 4)
					// there will be padding bytes to make it DWORD aligned 
				   // Thus bmp_stride is the TRUE number of bytes that makes up a scan line

		uchar *surface_bits; // A pointer for holding the address of our CBitmap's surface bits
							// (the actual pixel data)

		HDC bmp_hdc; // Our compatible HDC

		void freeMem(); // Frees all memory associated with CBitmap
					   // The reason we're making this private is we don't want for
					  // somebody freeing the CBitmap without us knowing
};

#endif
