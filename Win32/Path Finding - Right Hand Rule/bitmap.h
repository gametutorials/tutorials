#ifndef	CBitmap_H
#define	CBitmap_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>

typedef unsigned char uchar;

#define	BLUE_BITMAP_OFFSET	0
#define	GREEN_BITMAP_OFFSET	1
#define	RED_BITMAP_OFFSET	2

// A 24-bit bitmap class which supports double buffering
class CBitmap
{
	public:

		// Constructor() -- Zero's out CBitmap
		CBitmap():hbitmap(NULL),old_bmp(NULL),bmp_hdc(NULL),pixel_bits(NULL),
					 bmp_width(0),bmp_height(0),bmp_channels(0),
		             bmp_stride(0) { GdiFlush(); /* Guarentee that writing to CBitmap
												    is okay */ }

		// Data Access Functions ************
		
			inline int getWidth() const { return bmp_width; } 
			inline int getHeight() const { return bmp_height; }
			inline int getChannels() const { return bmp_channels; }
			inline int getStride() const { return bmp_stride; }
			inline HDC getDC() const { return bmp_hdc; }
		
		// ****** End of Data Access Functions

		uchar* getLinePtr(int which_line); // returns a pointer to the line passed in

		// Returns true for success -- false otherwise
		// If set_size is called on a CBitmap that already has memory associated with it
		// that memory is freed and the new size is implemented
		bool setSize(int width, int height, int channels = 3);

		// Loads a bmp with specified file_name -- Returns true on success, false otherwise
		// If loadBMP() is called on a CBitmap that already has memory associated with
		// it, that memory is freed and the the .bmp is loaded
		bool loadBMP(char *file_name);

		// Blits the CBitmap to the "dest_dc" using the "dest_rect"
		void blit(HDC dest_dc, const RECT &dest_rect);
		
		// Deconstructor();
		~CBitmap();
		

	private:

		int bmp_width;
		int bmp_height;
		int bmp_channels;
		int bmp_stride;

		HBITMAP hbitmap;
		HBITMAP old_bmp;

		HDC bmp_hdc;

		uchar *pixel_bits;

		void freeMem(); // Frees all mem associated with CBitmap
};

#endif
