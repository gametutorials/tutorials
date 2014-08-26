#ifndef _IMAGE_H
#define _IMAGE_H

#include "jpeglib.h"


// These defines are used to tell us about the type of TARGA file it is
#define TGA_RGB		 2		// This tells us it's a normal RGB (really BGR) file
#define TGA_A		 3		// This tells us it's a ALPHA file
#define TGA_RLE		10		// This tells us that the targa is Run-Length Encoded (RLE)

// This is our image structure for our targa data
struct tImage
{
	int channels;			// The channels in the image (3 = RGB : 4 = RGBA)
	int sizeX;				// The width of the image in pixels
	int sizeY;				// The height of the image in pixels
	unsigned char *data;	// The image pixel data
};


// This loads and returns the BMP data
tImage *LoadBMP(const char *strFileName);

// This loads and returns the TGA data
tImage *LoadTGA(const char *strFileName);

// This loads and returns the JGP data
tImage *LoadJPG(const char *strFileName);

// This decompresses the JPEG and fills in the image data
void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData);


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this file we have all of our image loading code and made it so they
// each pass back a tImage object.  We use the windows LoadImage function to load the
// bitmap like we did before in all the previous tutorials, but convert the
// data to a tImage structure which is passed back.
//
// I might note that I got rid of the tImageJPG structure out of jpeglib.h
// since we didn't need it anymore.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
