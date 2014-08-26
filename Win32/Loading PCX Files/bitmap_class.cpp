// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <assert.h>
#include "bitmap_class.h"

// Some helpful defines for loading
#define PCX_HEADER_SIZE 128	// The size in bytes of a .pcx header file
#define PCX_PALETTE_TEST 769 // This is the offset from the bottom of a .pcx file
							// that (in a version 5 PCX file only) will be set to '12'
						   // if the .pcx file has a palette associated with it

// Before you can load any file, it's important to know how it's laid out.  Below 
// is a struct that defines how a PCX header is constructed						   
struct SPCXHeader
{
	char manufacturer;	// Always 10 -- 
	char version; // version information
				 // 0 = Ver2.5, 2 = Ver2.8 with palette, 3 = Ver2.8 without palette,
				// 4 = PC Paintbrush for Windows, 5 = Ver 3.0

	char encoding; // 1 = Run Length encoding

	char bitsPerPixel; // Number of bits that represent a pixel

	short xMin, yMin, xMax, yMax; // Image Dimensions

	short horDPI; // Horizontal Resolution of pixels in dots per inch
	short verDPI; // Vertical Resolution of pixels in dots per inch

	char colormap[48]; // Color palette setting

	char reserved; // Should be set to zero
	char planes; // Number of color planes
	short bytesPerLine; // Number of bytes to allocate per scanline
						// Must be an even number

	short paletteInfo; // How to interpret the palette

	short horScreenSize; // Horizontal screen size in pixels
	short verScreenSize; // Vertical screen size in pixels

	char filler[54]; // An additional 54 bytes that must be set to 0 -- 
					// Total size of a .pcx header must be 128 bytes
};

// Loads a PCX file
bool CBitmap::loadPCX(const char *fileName)
{
	FILE *filePtr = fopen(fileName,"rb"); // Open the file
	BYTE *scanLine = NULL; // A pointer to contain a row of pixel data in the PCX

	SPCXHeader header = {0}; // Create a zeroed out SPCXHeader
	
	// We want to do robust error checking so we'll wrap the loading code in a 
	// try...catch block so we can deal with errors in only place
	try
	{
		// Error Check
		if(!filePtr)
			throw "Couldn't open .pcx file";
		
		// Read in the .pcx file header
		if(!fread(&header,sizeof(SPCXHeader),1,filePtr))
			throw "Couldn't read .pcx header";

		// Error Check -- manufacturer better equal 10 otherwise this 
		// for sure isn't a .pcx file
		if((int)header.manufacturer != 10)
			throw "Not a valid .pcx file";

		// Okay we have the header.  Let's determine the width, height, and number of channels
		// that are in the .pcx file.  Notice the +1 for width and height
		int width = header.xMax - header.xMin + 1;
		int height = header.yMax - header.yMin + 1; 
		int channels = header.planes;

		// Okay let's set the size of our CBitmap
		if(!setSize(width,height,channels))
			throw "Couldn't create bitmap for loading .pcx into";

		// Now we need to test to see if there is a palette
		// If the header version equals 5, there is a chance that there is a palette
		if((int)header.version == 5)
		{
			// Since it's a version 5 .pcx it's possible that it has a palette so
			// now we must test further
			
			BYTE test_byte; // byte to check to see if it's a 8-bit .pcx file

			// Seek to where the test byte is
			if(fseek(filePtr,-PCX_PALETTE_TEST,SEEK_END))
				throw "PCX file is corrupt";
			
			// Read in the test byte
			if(!fread(&test_byte,sizeof(BYTE),1,filePtr))
				throw "PCX file is corrupt";
			
			// Check the test byte -- If it EQUALS '12' we have a palette to read in
			if(test_byte == 12)
			{
				// Load the palette
				if(!loadPCXPalette(filePtr))
					throw "Couldn't load palette";
			}
		}
		
		// Next seek past the SPCXHeader
		if(fseek(filePtr,PCX_HEADER_SIZE,SEEK_SET))
			throw "PCX file is corrupt";		
		
		// The total number of bytes in a scanline
		int totalBytes = (int)header.planes * header.bytesPerLine;

		// Allocate the scanline
		scanLine = new BYTE[totalBytes];

		// Error Check
		if(scanLine == NULL)
			throw "Couldn't allocate memory";

		// Now loop over the entire .pcx file -- We'll fill each "scanLine" and then
		// convert that scanLine to our CBitmap depending on what type of .pcx we're dealing with
		for(int lineCount = 0; lineCount < height; lineCount++)
		{
			if(!readScanLine(filePtr, scanLine, totalBytes))
				throw "Error reading scanLine";

			// If it's an 8-bit .pcx file, simply copy over the scanline
			if((int)header.planes == 1) 
				memcpy(getLinePtr(lineCount),scanLine,getStride());

			else // Converge "scanLine" into the same form as a CBitmap 24-bit scanline
			{
				uchar *pixels = getLinePtr(lineCount);
				
				// Loop through the entire line of pixels.
				for(int i = 0; i < width; ++i)
				{
					/* 
						What does this mess do?  Well lets break down what each part is doing.
						
						i * header.planes - This puts "pixels" at the current pixel
						+ RED_BITMAP_OFFSET - Offset from the current pixel to the 'R' color component
						+ GREEN_BITMAP_OFFSET - Offset from the current pixel to the 'G' color component 
						+ BLUE_BITMAP_OFFSET - Offset from the current pixel to the 'B' color component
						
						PCX stores the scanline as a sequence of 'R', then a sequence of 'G', 
						then a sequence of 'B' so "i + header.bytesPerLine" moves to the sequence
						containing the 'G' components and "i + header.bytesPerLine * 2" moves to 
						the sequence containing the 'B' components
					*/				
					pixels[i * header.planes + RED_BITMAP_OFFSET] = scanLine[i];
					pixels[i * header.planes + GREEN_BITMAP_OFFSET] = scanLine[i + header.bytesPerLine];
					pixels[i * header.planes + BLUE_BITMAP_OFFSET] = scanLine[i + header.bytesPerLine * 2];
				}
			
			}  // end of if...else ((int)header.planes == 1)

		} // end of for(int lineCount = 0; lineCount < height; lineCount++)
	}
	catch (char *str) // If an error happened, catch it here and cleanup
	{
		MessageBox(NULL, str, "ERROR", MB_OK | MB_ICONERROR);
		
		if(filePtr)
			fclose(filePtr);
			
		if(scanLine)
			delete[] scanLine;
			
		return false;
	}

	delete[] scanLine; // Free mem
	fclose(filePtr); // Close the file
		return true; // PCX was successfully loaded
}

// Sizes the bitmap to the (width x height) and number of channels passed in
// Returns true for success -- false otherwise
bool CBitmap::setSize(int width, int height, int channels)	
{
	// If CBitmap has already been set -- clear it out first
	freeMem();

	mWidth = width;
	mHeight = height;
	mChannels = channels;
	
	// Set stride
	mStride = mWidth * mChannels;

	while((mStride % 4) != 0) // Ensure mStride is dword aligned
		mStride++;
		
	// Create a DC that is compatible with the current monitor resolution
	mHDC = CreateCompatibleDC(NULL);

	// Error Check
	if(!mHDC)
		return false;
		
	BITMAPINFO bmp_info = {0};

	// Initialize the parameters that we care about
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biWidth = width;
	bmp_info.bmiHeader.biHeight = height;
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = channels * 8;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biClrUsed = (mChannels == 1? 256:0); // If it's 8-bit set to 256
															   // Otherwise set to 0

	// Create a DIBSection. Note that this returns us two
	// things - an HBITMAP handle and a memory pointer, in
	// mPixels.
	mBmp = CreateDIBSection(mHDC, &bmp_info, DIB_RGB_COLORS,
							   (void**)&mPixels, 0, 0);
		// Error Check
		if(!mBmp)
			return false;

	// Select our DIBSection bitmap into our DC.
	// This allows us to draw to the bitmap using GDI functions.
	mOldBmp = (HBITMAP)SelectObject(mHDC,mBmp);

		// Error Check
		if (!mOldBmp)
			return false;
		

	return true;

} // end of setSize(int width, int height, int channels)

bool CBitmap::setPalette(int startColor, int numColors, RGBQUAD *colors)
{	
	// If the CBitmap is not an 8-bit bitmap, CAN NOT set palette
	if(mChannels != 1)
		return false;

	// Returns 0 on failure, number of entries set on success
	if(SetDIBColorTable(mHDC, startColor, numColors, colors))
		return true;
	else
		return false;
}

// Returns the address in memory of the specified line. This gives you a pointer to at least 
// width * channels bytes. Lines are numbered such that when the bitmap
// is displayed line zero is at the top.
uchar* CBitmap::getLinePtr(int which_line)
{
	// Windows stores bitmaps upside down so (mHeight - which_line - 1)
	// flips the bitmap over

	return (mPixels + mStride * (mHeight - which_line - 1));
}

// Deconstructor
CBitmap::~CBitmap() { freeMem(); }

// *** Private Methods *** //

// Release the memory
void CBitmap::freeMem() 
{
	if(!mOldBmp)
		return;
	
	if(!SelectObject(mHDC,mOldBmp)) // get the old stuff back
		return;
	
	if(!DeleteObject(mBmp)) // delete the new stuff
		return;

	if(!DeleteDC(mHDC)) // release the device context
		return;

	// Zero out all data associated with CBitmap
	mOldBmp = mBmp = NULL;
	mHDC = NULL;
	mWidth = mHeight = mChannels = mStride = 0;
	mPixels = NULL;
		return;
}

// Loads the palette of the .pcx file
// Returns true on success, false otherwise
bool CBitmap::loadPCXPalette(FILE *filePtr)
{
	assert(filePtr != NULL); // Safety check
	
	// PCX palettes all contain 256 entries
	const int kPaletteEntries = 256;
	
	RGBQUAD paletteInfo[kPaletteEntries] = {0}; // Our array of colors in the palette
	BYTE rgb[3] = {0}; // One byte for RED, GREEN, and BLUE in that order

	// Get all the values in the palette
	for(int i = 0; i < kPaletteEntries; ++i)
	{
		// Fill "rgb" with the corresponding bytes -- Checking for errors each read
		if(!fread(rgb,sizeof(rgb),1,filePtr))
			return false;

		// Set the palette's color components
		paletteInfo[i].rgbRed = rgb[0];
		paletteInfo[i].rgbGreen = rgb[1];
		paletteInfo[i].rgbBlue = rgb[2];
	}

	// Set the palette of the bitmap -- Returning error check
	return setPalette(0,kPaletteEntries,paletteInfo);
}

bool CBitmap::readScanLine(FILE *filePtr, BYTE *scanLine, int totalBytes)
{
	BYTE byteVal = 0; // The value of a particular byte (ie a color component)
	BYTE byteCount = 0; // How many times this color is repeated
	BYTE temp = 0; // A temporary place holder to determine if a particular color is 
				  // repeated or not

	int scanLineCount = 0; // Number of bytes we've traversed from left to right in 
						  // a particular scanline

	// Loop till the end of the scanline is reached
	while(scanLineCount < totalBytes)
	{
		// Read in temp -- We'll use this to determine if the color needs to be 
		// repeated
		if(!fread(&temp,sizeof(BYTE),1,filePtr))
			return false;

		// *** NOTE ***
		/*	
			The encoding method works like this:
			For each byte X read from the file,
			if the top two bits of X are 1's then
			count = 6 lowest bits of X
			data = next byte following X
			otherwise, count = 1 and  data = X
		*/

		// If the top two bits are set
		if((temp & 0xC0) == 0xC0) 
		{
			byteCount = temp & 0x3F; // bottom six bits are count of bytes to repeat
			// of the next byte read in

			// read the byte value for repetition, check for errors
			if(!fread(&byteVal,sizeof(BYTE),1,filePtr))
				return false;

		}
		else // byte that was read in is only repeated once
		{	
			byteCount = 1;
			byteVal = temp;
		}

		// Fill the current portion of the scanline
		for(int i = 0; i < byteCount; i++, scanLineCount++)
			scanLine[scanLineCount] = byteVal;

	} // end of while(scanLineCount < totalBytes)

	return true; // "scanLine" filled successfully
}




	
