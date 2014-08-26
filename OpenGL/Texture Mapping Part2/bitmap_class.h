#ifndef	BITMAP_CLASS_H
#define	BITMAP_CLASS_H

#include <windows.h>
#include <stdio.h>

typedef unsigned char uchar; // We're lazy so typedef "unsigned char" as "uchar"

// We will use this class to load 24 and 32-bit .bmp for us
class CBitmap
{
	public:

		// Constructor() -- Zero's out CBitmap
		CBitmap():hbitmap(NULL),pixels(NULL),bmp_width(0),bmp_height(0),
					 bmp_channels(0),bmp_stride(0) { GdiFlush(); /* Guarantee that writing to
																 CBitmap is okay */ }

		// Data Access Functions ************
		
			inline int getWidth() const { return bmp_width; } 
			inline int getHeight() const { return bmp_height; }
			inline int getChannels() const { return bmp_channels; }
			inline int getStride() const { return bmp_stride; }
					
		// ****** End of Data Access Functions

		// Creates a "empty" CBitmap with the "traits" of the parameters passed in
		// Returns true for success -- false otherwise
		// If set_size is called on a CBitmap that already has memory associated with it
		// that memory is freed and the new size is implemented
		bool setSize(int width, int height, int channels);

		// Loads a bmp with specified file_name -- Returns true on success, false otherwise
		// If loadBMP() is called on a CBitmap that already has memory associated with
		// it, that memory is freed and the .bmp is loaded
		bool loadBMP(const char *file_name);

		uchar* getLinePtr(int which_line); // returns a pointer to the line passed in
		
		// Deconstructor();
		~CBitmap();
		

	private:

		int bmp_width; // The width of the bitmap
		int bmp_height; // The height of the bitmap
		int bmp_channels; // How many channels is the bitmap (3 == 24-bit, 4 == 32-bit)
		int bmp_stride; // The TRUE number of bytes in a scan line (in a line of pixels
					   // in memory)

		HBITMAP hbitmap; // This will be the handle to our bitmap
		
		uchar *pixels; // This is a pointer to the actual pixels of the bitmap

		void freeMem(); // Frees all memory associated with CBitmap
};

#endif
