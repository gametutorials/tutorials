#include "bitmap_class.h"

// Returns true for success -- false otherwise
bool CBitmap::setSize(HDC hdc, int width, int height)	
{
 
	// If CBitmap has already been set -- clear it out first
	freeMem();

	bmp_width = width;
	bmp_height = height;

	// Set bmp_stride -- When windows makes a bitmap, it can include up to 4 padding bytes
	// it's our responsibility to "take care of these" -- So bmp_stride is simply the width
	// of the bitmap DWORD aligned (the width of the bitmap plus any padding bytes we got when we created it)
	bmp_stride = bmp_width;

	while((bmp_stride % 4) != 0) // Ensure bmp_stride is dword aligned
		bmp_stride++;
		
	BITMAPINFO bmp_info = {0};

	// Initialize the parameters that we care about
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biWidth = width;
	bmp_info.bmiHeader.biHeight = height;
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = 8; // 8 bits per pixel
	bmp_info.bmiHeader.biCompression = BI_RGB; // No compression
	bmp_info.bmiHeader.biClrUsed = 256; // 8-bit bitmaps use 256 colors

	// Create a DIBSection. Note that this returns us two
	// things - an HBITMAP handle and a memory pointer, in
	// surface_bits.
	hbitmap = CreateDIBSection(hdc, &bmp_info, DIB_RGB_COLORS,
							   (void**)&surface_bits, 0, 0);
		// Error Check
		if(!hbitmap)
			return false;

	// Create a copy of our DC that we can keep around
	bmp_hdc = CreateCompatibleDC(hdc);
	
		// Error Check
		if(!bmp_hdc)
			return false;
		

	// Select our DIBSection bitmap into our DC.
	// This allows us to draw to the bitmap using GDI functions.
	old_bmp = (HBITMAP)SelectObject(bmp_hdc,hbitmap);

		// Error Check
		if (!old_bmp)
			return false;
		

	return true;

} // end of set_size(int width, int height, int channels)

bool CBitmap::setPalette(int start_color, int num_colors, RGBQUAD *colors)
{	
	// Returns 0 on failure, number of entries set on success
	// For more on this function check it out in MSDN
	if(SetDIBColorTable(bmp_hdc, start_color, num_colors, colors))
		return true;
	else
		return false;
}

// This is the function	that "manually" loads an 8-bit bitmap
// It will return true for success, false for failure
bool CBitmap::loadBMP(char *file_name, HDC hdc)
{
		// Make sure we got a legal file name passed in
		if(file_name == NULL)
			return false;

	// These structures will hold our bitmap file header (top of bitmap)
	// and our bitmap info header (directly underneath bitmap file header)
	BITMAPFILEHEADER bmp_fileheader = {0};
	BITMAPINFOHEADER bmp_infoheader = {0};

	FILE *bmpFile = fopen(file_name,"rb");

	bool status = false; // This the "status" of our attempt at loading the 8-bit bitmap
						 // When we have successful loaded it, we'll set this to true

		// Make sure we could open the file
		if(bmpFile == NULL)
			return false;

	// Read the file header -- Making sure the read is
	if(!fread(&bmp_fileheader, sizeof(BITMAPFILEHEADER), 1, bmpFile))
		goto CLEAN_UP; // Now normally "goto's" are frowned upon, but in this case
					   // it's a reasonable solution -- There are MANY things that could fail
					   // along the way, so instead of rewriting the code for each 
					   // possible failure, we'll just "goto" the "CLEAN_UP:" code and
					   // return our "status" flag of how the loading of the bitmap went :)

	// Check the type field to make sure we have a .bmp file -- This will always be "BM" for .bmps
	if(memcmp(&bmp_fileheader.bfType, "BM", 2))
		goto CLEAN_UP;

	// Read the info header -- You'll notice it's positioned directly after the 
	// BITMAPFILEHEADER.  That's roughly how a bitmap is laid out:  BITMAPFILEHEADER
	//																BITMAPINFOHEADER
	//																Pixel Data
	if(!fread(&bmp_infoheader, sizeof(BITMAPINFOHEADER), 1, bmpFile))
		goto CLEAN_UP;

	// It's always good to double check and make sure the fread() went as planned :)
	// Obviously, these two numbers better be equal or there was a problem
	if(bmp_infoheader.biSize != sizeof(BITMAPINFOHEADER))
		goto CLEAN_UP;

	// This had better equal one (MSDN says this will always equal one)
	if(bmp_infoheader.biPlanes != 1)
		goto CLEAN_UP;

	// If the bitmap isn't a 8-bit bitmap, WE CAN'T LOAD IT
	// .biBitCount represents the number of bits per pixel our bitmap is
	if(bmp_infoheader.biBitCount != 8)
		goto CLEAN_UP;	

	// Now we're creating an "empty" CBitmap for storing the 8-bit bitmap
	// If this fails we can't load the 8-bit bitmap (darn!)
	// You'll notice that the BITMAPINFOHEADER contains the width (biWidth) in pixels and
	// the height (biHeight) in pixels of the bitmap
	if(!setSize(hdc,bmp_infoheader.biWidth, bmp_infoheader.biHeight))
		goto CLEAN_UP;

/************* HERE WE'RE GOING TO READ THE COLOR MAP INFORMATION ***************/
	{
	
	RGBQUAD rgbQuad[256] = {0};

	// Let's determine the size of the color table (how many colors are used by the bitmap)
	uint tableSize = bmp_infoheader.biClrUsed;
	
	// If the size equals zero, this means the number of colors used by bitmap is equal
	// to the maximum number of colors allowed for this type of bitmap (this is dependent on the bits per pixel)
	// For 8-bit bitmaps, the maximum number of colors is 256
	if(tableSize == 0)
		tableSize = 256;

	// 8-bit bitmaps can't have more than 256 colors
	// Also 8-bit bitmaps can't have a negative number of colors
	// Make sure there isn't an error :)
	if((tableSize > 256) || (tableSize < 0))
		goto CLEAN_UP;

	// Okay we're ready to read the RGBQUAD (the color table (a.k.a. color map) )
	if(!fread(rgbQuad, sizeof(RGBQUAD) * tableSize, 1, bmpFile))
		goto CLEAN_UP;

	// Set the palette in the bitmap
	if(!setPalette(0,tableSize,rgbQuad))
		goto CLEAN_UP;
	
	} // If you noticed the {} it's because I wanted to make a "local scope" to separate
	  // the color table reading for clarity sake (also makes the compiler not puke)
	
/*************** DONE READING THE COLOR MAP INFORMATION *************************/

	// Jump to the location where the bitmap data is stored
	// bfOffBits is the number of bytes from the beginning of the file to where the bitmaps bits reside 
	if(fseek(bmpFile, bmp_fileheader.bfOffBits, SEEK_SET))
		goto CLEAN_UP;

	// Read in the bitmap is compressed (has been saved with Run Length Encoding)
	// We can't load it
	if(bmp_infoheader.biCompression == BI_RLE8)
		goto CLEAN_UP;

	else if(bmp_infoheader.biCompression == BI_RGB) // There is NO compression
	{
		/* Okay a little explaining is in order :)
		   The total number of bytes that make a scan line (this would be one line of pixel data)
		   is equal to: (width of bitmap * bitmap channels)

		   Now a bitmap's channels is defined as "the number of bytes used per pixel"
		   So an 8-bit bitmaps channel would equal 1, an 24-bit bitmap channel would equal 3  -- Remember (8 bits == 1 byte) :)

		   Sometime bitmaps have padding bytes, if we're reading in bitmap and it has 
		   padding bytes it's our responsibility to skip over 'em
		   You'll recall we calculate the "bmp_stride" in the set_size() method -- This can be 
		   thought us as "the actual number of bytes in scan line"
		   The bmp_width would be the "the actual number of bytes in a scan line that are pixel data"

		   This of course follows our formula (bmp_width * 1)
		*/

		uint padding = bmp_stride - bmp_width; // Calculate the number of 
											  // padding bytes if there is any
		
		// Okay now all we have to do is loop over all the pixel data
		// and store what we read in, in our CBitmap :)
		for(int y = 0; y < bmp_infoheader.biHeight; y++)
		{
			// We have to flip the y-coordinate because our CBitmap
			// puts line zero at the top of the screen, and the .bmp format
			// on disk puts the first bytes at the bottom of the screen.
			uchar *scanLine = getLinePtr(bmp_height - 1 - y);

			// Read the precise number of bytes that the line requires into the bitmap.
			// This will be the width of the bitmap,  DON'T read the padding bytes.
			// Remember memory alignment requirements may vary.
			// We want our 8-bit bitmap loader to be ROBUST %)
			if(!fread(scanLine, bmp_width, 1, bmpFile))
				goto CLEAN_UP; // In case you weren't aware, goto's will break out of for loops

			// Skip over the padding bytes (if there are any, if padding == 0, no harm done)
			if(fseek(bmpFile, padding, SEEK_CUR))
				goto CLEAN_UP;
		}

		// If we get here the bitmap WAS LOADED!!!!!!!!!!!!
		// So we better set status to true
		status = true;
	
	} // end of else if(bmp_infoheader.biCompression == BI_RGB) // There is NO compression

CLEAN_UP:
		
		fclose(bmpFile); // Close the file we opened
			return status;
}


// Returns the address in memory of the specified line. This gives you a pointer to at least 
// width * channels bytes. Lines are numbered such that when the bitmap
// is displayed line zero is at the top.
uchar* CBitmap::getLinePtr(int which_line)
{
	// Windows stores bitmaps upside down so (bmp_height - which_line - 1)
	// flips the bitmap over

	return (surface_bits + bmp_stride * (bmp_height - which_line - 1));
}


// Release the memory
void CBitmap::freeMem() 
{
	if(!old_bmp)
		return;
	
	if(!SelectObject(bmp_hdc,old_bmp)) // get the old stuff back
		return;
	
	if(!DeleteObject(hbitmap)) // delete the new stuff
		return;

	if(!DeleteDC(bmp_hdc)) // release the device context
		return;

	// Zero out all data associated with CBitmap
	old_bmp = hbitmap = NULL;
	bmp_hdc = NULL;
	surface_bits = NULL;
	bmp_width = bmp_height = bmp_stride = 0;
		return;
}

// Deconstructor
CBitmap::~CBitmap() { freeMem(); }


	
