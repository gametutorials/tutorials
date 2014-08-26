#ifndef	BITMAP_CLASS_H
#define	BITMAP_CLASS_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>

#define TGA_RGB 2 // This is the identifier in the "data type field" of a .tga file that says
				  // "this .tga file is an uncompressed RGB file"

typedef unsigned char uchar; // Less typing :)

// Here is our high color bitmap class -- It will allow us to load 24 or 32 bit .tga files
class CTarga
{
	public:

		// Constructor() -- Zero's out CTarga
		CTarga():hbitmap(NULL),old_bmp(NULL),height(0),width(0),
				 channels(0),stride(0),surface_bits(NULL),hdc(NULL)
				 { GdiFlush(); /* Guarantee that writing to CTarga is okay */ }

		
		// Data Access Functions ************
		
			inline int getWidth() const { return width; } 
			inline int getHeight() const { return height; }
			inline int getChannels() const { return channels; }
			inline int getStride() const { return stride; }
			inline HDC getHDC() const { return hdc; }
		
		// ****** End of Data Access Functions

		// This is a method that creates (initializes) our member variable "hbitmap"
		// If this method is called on a CTarga that already has memory associated with
		// it, that memory is freed first.
		// We can call this method to create an "empty CTarga" if we wanted :)
		// width and height are the width and height of the CTarga
		// channels will specify whether this is a 24 (3 channels) or 32 (4 channles) bit image
		bool init(int width, int height, int channels);

		// Loads a .tga with specified file_name
		// If loadTGA() is called on a CTarga that already has memory associated with
		// it, that memory is freed and the .tga is loaded
		// Returns true on success, false on failure
		bool loadTGA(char *file_name);
		
		// This returns us a pointer to a "line of pixels" specified by which_line
		uchar* getLinePtr(int which_line);
		
		// Deconstructor() -- Free's up memory
		~CTarga();
		

	private:

		HBITMAP hbitmap; // Our bitmap to be created
		HBITMAP old_bmp; // Our bitmap for holding the "old bitmap"

		int height; // Height of our CTarga
		int width; // Width of our CTarga
		int channels; // Number of channels in our CTarga (either 3 or 4)
		
		int stride; // Our CTarga's stride -- BITMAP's in windows are always DWORD
				   // aligned, so generally the actual number of bytes for a particular
				  // scan line of an CTarga is (width * channels) bytes long
				 // but if that does not happen to be DWORD aligned (divisible evenly by 4)
				// there will be padding bytes to make it DWORD aligned 
			   // Thus stride is the TRUE number of bytes that makes up a scan line

		uchar *surface_bits; // A pointer for holding the address of our CTarga's surface bits
							// (the actual pixel data)

		HDC hdc; // Our compatible HDC

		void freeMem(); // Frees all memory associated with CTarga
					   // The reason we're making this private is we don't want for
					  // somebody freeing the CTarga without us knowing
};

#endif
