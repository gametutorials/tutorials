#include "bitmap_class.h"

// Returns true for success -- false otherwise
bool CBitmap::setSize(HDC hdc, int width, int height)	
{
 	// If CBitmap has already been set -- clear it out first
	freeMem();

	mWidth = width;
	mHeight = height;

	// Set mStride -- When windows makes a bitmap, it can include up to 4 padding bytes
	// it's our responsibility to "take care of these" -- So mStride is simply the width
	// of the bitmap DWORD aligned (the width of the bitmap plus any padding bytes we got when we created it)
	mStride = mWidth;

	while((mStride % 4) != 0) // Ensure mStride is DWORD aligned
		mStride++;
		
	BITMAPINFO bmpInfo = {0};

	// Initialize the parameters that we care about
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 8; // 8 bits per pixel
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biClrUsed = 256; // 8-bit bitmaps use 256 colors

	// Create a DIBSection. Note that this returns us two
	// things - an HBITMAP handle and a memory pointer, in
	// mPixels.
	mBmp = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS,
							   (void**)&mPixels, 0, 0);
		// Error Check
		if(!mBmp)
			return false;

	// Create a copy of our DC that we can keep around
	mHDC = CreateCompatibleDC(hdc);
	
		// Error Check
		if(!mHDC)
			return false;
		
	// Select our DIBSection bitmap into our DC.
	// This allows us to draw to the bitmap using GDI functions.
	mOldBmp = (HBITMAP)SelectObject(mHDC,mBmp);

		// Error Check
		if (!mOldBmp)
			return false;

	return true;

} // end of set_size(int width, int height, int channels)

bool CBitmap::setPalette(int start_color, int num_colors, RGBQUAD *colors)
{	
	// Returns 0 on failure, number of entries set on success
	// For more on this function check it out in MSDN
	if(SetDIBColorTable(mHDC, start_color, num_colors, colors))
		return true;
	else
		return false;
}

// This is the function	that "manually" loads an 8-bit bitmap
// It will return true for success, false for failure
bool CBitmap::loadBMP(const char *fileName, HDC hdc)
{
		// Make sure we got a legal file name passed in
		if(fileName == NULL)
			return false;

	// These structures will hold our bitmap file header (top of bitmap)
	// and our bitmap info header (directly underneath bitmap file header)
	BITMAPFILEHEADER bmpFileHeader = {0};
	BITMAPINFOHEADER bmpInfoHeader = {0};

	FILE *bmpFile = fopen(fileName,"rb");

	uchar *compData = NULL; // This is a uchar*, that if we determine our bitmap is RLE (run length encoded)
					       // we're going to associate some memory with and then fill it with the compressed
					      // data of our bitmap we're loading

	bool status = false; // This the "status" of our attempt at loading the 8-bit bitmap
						 // When we have successful loaded it, we'll set this to true

		// Make sure we could open the file
		if(bmpFile == NULL)
			return false;

	// Read the file header -- Making sure the read is
	if(!fread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, bmpFile))
		goto CLEAN_UP; // Now normally "goto's" are frowned upon, but in this case
					   // it's a reasonable solution -- There are MANY things that could fail
					   // along the way, so instead of rewriting the code for each 
					   // possible failure, we'll just "goto" the "CLEAN_UP:" code and
					   // return our "status" flag of how the loading of the bitmap went :)

	// Check the type field to make sure we have a .bmp file -- This will always be "BM" for .bmps
	if(memcmp(&bmpFileHeader.bfType, "BM", 2))
		goto CLEAN_UP;

	// Read the info header -- You'll notice it's positioned directly after the 
	// BITMAPFILEHEADER.  That's roughly how a bitmap is laid out:  BITMAPFILEHEADER
	//																BITMAPINFOHEADER
	//																Pixel Data
	if(!fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, bmpFile))
		goto CLEAN_UP;

	// It's always good to double check and make sure the fread() went as planned :)
	// Obviously, these two numbers better be equal or there was a problem
	if(bmpInfoHeader.biSize != sizeof(BITMAPINFOHEADER))
		goto CLEAN_UP;

	// This had better equal one (MSDN says this will always equal one)
	if(bmpInfoHeader.biPlanes != 1)
		goto CLEAN_UP;

	// If the bitmap isn't a 8-bit bitmap, WE CAN'T LOAD IT
	// .biBitCount represents the number of bits per pixel our bitmap is
	if(bmpInfoHeader.biBitCount != 8)
		goto CLEAN_UP;	

	// Now we're creating an "empty" CBitmap for storing the 8-bit bitmap
	// If this fails we can't load the 8-bit bitmap (darn!)
	// You'll notice that the BITMAPINFOHEADER contains the width (biWidth) in pixels and
	// the height (biHeight) in pixels of the bitmap
	if(!setSize(hdc,bmpInfoHeader.biWidth, bmpInfoHeader.biHeight))
		goto CLEAN_UP;

/************* HERE WE'RE GOING TO READ THE COLOR MAP INFORMATION ***************/
	{
	
	RGBQUAD rgbQuad[256] = {0};

	// Let's determine the size of the color table (how many colors are used by the bitmap)
	uint tableSize = bmpInfoHeader.biClrUsed;
	
	// If the size equals zero, this means the number of colors used by bitmap is equal
	// to the maximum number of colors allowed for this type of bitmap (this is dependent on
	// the bits per pixel)  For 8-bit bitmaps, the maximum number of colors is 256
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
	if(fseek(bmpFile, bmpFileHeader.bfOffBits, SEEK_SET))
		goto CLEAN_UP;

	// Read in the bitmap data.
	if(bmpInfoHeader.biCompression == BI_RLE8)
	{
		// Calculate the number of bytes in the pixel data portion
		// of the .bmp file 
		int imageSize = bmpFileHeader.bfSize - bmpFileHeader.bfOffBits;
	
		// Allocate enough space for all of the compressed data.
		compData = new uchar[imageSize];
			
			// Error Check -- Make sure we could allocate the memory
			if(!compData)
				goto CLEAN_UP;
		
		// Read in all the compressed data -- This is easier than reading the compressed data
		// scan line by scan line and constantly having to worry about fread() failures
		if(!fread(compData, imageSize, 1, bmpFile))
			goto CLEAN_UP;

		// Okay now comes the fun part, decompressing the data, we'll call our private function
		// "Decompress()"  to do the task.  It returns the number of scan lines successfully decoded
		int scanLines = decompress(compData, imageSize);
		
		// If the number of scan lines decoded DOES NOT equal the height of the bitmap, there's been an error
		if(scanLines != mHeight)
			goto CLEAN_UP;

		// Otherwise, the bitmap was loaded successfully!!!!!!!!!!!!
		// let's set the status flag to reflect that 
		status = true;
	}
	else if(bmpInfoHeader.biCompression == BI_RGB) // There is NO compression
	{
		/* Okay a little explaining is in order :)
		   The total number of bytes that make a scan line (this would be one line of pixel data)
		   is equal to: (width of bitmap * bitmap channels)

		   Now a bitmap's channels is defined as "the number of bytes used per pixel"
		   So an 8-bit bitmaps channel would equal 1, an 24-bit bitmap channel would equal 3
		   Remember (8 bits == 1 byte) :)

		   Sometime bitmaps have padding bytes, if we're reading in bitmap and it has 
		   padding bytes it's our responsibility to skip over 'em
		   You'll recall we calculate the "mStride" in the set_size() method -- This can be 
		   thought us as "the actual number of bytes in scan line"
		   The mWidth would be the "the actual number of bytes in a scan line that are pixel data"

		   This of course follows our formula (mWidth * 1)
		*/

		uint padding = mStride - mWidth; // Calculate the number of 
											  // padding bytes if there is any
		
		// Okay now all we have to do is loop over all the pixel data
		// and store what we read in, in our CBitmap :)
		for(int y = 0; y < bmpInfoHeader.biHeight; y++)
		{
			// Get the current scanline
			uchar *scanLine = getLinePtr(y);

			// Read the precise number of bytes that the line requires into the bitmap.
			// This will be the width of the bitmap,  DON'T read the padding bytes.
			// Remember memory alignment requirements may vary.
			// We want our 8-bit bitmap loader to be ROBUST %)
			if(!fread(scanLine, mWidth, 1, bmpFile))
				goto CLEAN_UP; // In case you weren't aware, goto's will break out of for loops

			// Skip over the padding bytes (if there are any, if padding == 0, no harm done)
			if(fseek(bmpFile, padding, SEEK_CUR))
				goto CLEAN_UP;
		}

		// If we get here the bitmap WAS LOADED!!!!!!!!!!!!
		// So we better set status to true
		status = true;
	
	} // end of else if(bmpInfoHeader.biCompression == BI_RGB) // There is NO compression


CLEAN_UP:

		// If we allocated some memory, we better free it %)
		if(compData)
			delete[] compData;

		fclose(bmpFile); // Close the file we opened
			return status;
}


// Returns the address in memory of the specified line. This gives you a pointer to at least 
// width * channels bytes. Lines are numbered such that when the bitmap
// is displayed line zero is at the top.
uchar* CBitmap::getLinePtr(int which)
{
	return (mPixels + mStride * which);
}


// All the magic of decompressing happens in here :)
uint CBitmap::decompress(const uchar *compData, uint dataLen)
{
	/* Quickly RLE (run length encoding) works like this:

		The first byte of information contains how many times the next byte of information
		is repeated -- Make sense, now RLE can be pretty effective, but if every pixel in your
		bitmap is unique, RLE will actually double the size of your bitmap!

		Now bitmaps encoded with RLE_8 can be encoded in TWO modes, encoded and absolute
		YUCK!!!!!!!  

		Encoded is just like the description for RLE -- First byte is "repeat count" , second byte is "pixel index"
		Absolute works like this:  first byte is zero, second byte is count, third byte is "pixel index"

		The worst part is, these two style can be inter-woven in a single bitmap!!!!!  Double YUCK!
		So we're going to do our best to handle all possible scenarios :)
	*/

	const uchar *compDataEnd = compData + (dataLen - 1); // Get a pointer to the end of the data
														// We -1 from dataLen because arrays use zero
													   // based indexing
	const uchar *twoFromEnd = compDataEnd - 2;	// Calculate two positions from the end of the the data
											   // We'll use this for error checking
	
	
	// Loop for the entire CBitmap (we're filling this with the 8-bit bitmap)
	for(int y = 0; y < mHeight; y++)
	{
		int x = 0; // We'll increment this as we move along the bitmap
		
		// Get the current scan line (for our structure we are filling)
		// remember the way we have it set up, we have to "flip the bitmap upside-down"
		uchar *scanLine = getLinePtr(y);
		
		// While we haven't reached the end of the current scan line...
		while(x < mWidth)
		{
			// Make sure there's at least two bytes of data left -- If not
			// an error has occurred
			if(compData > twoFromEnd)
				return 0; // Error occurred

			uchar data = compData[0]; // Capture the first byte (which remember is actually
									  // the count of "indexes into the RGBQUAD to be repeated")
			
			// If the first byte equals zero let's check out what this could mean
			if(data == 0)
			{
				data = compData[1]; // Get the next byte (we're in "absolute" decompression mode
								   // now so this byte is the number of bytes that follow which contain
				                  // the color index of a single pixel (don't blame me I didn't make the compression scheme :)
				switch(data)
				{
					case 0: // This normally mean "the end of a scan line"
							// but we check for this elsewhere, so we don't want to 
							// get "the end of a scan line" in the middle of a scan line
						
						return 0; // Error occurred
					
					case 1: // This normally means "the end of the image" -- But we 
							// don't want the "end of the image" to appear in the middle
							// of the image
						
						return 0; // Error occurred
					
					case 2: // This normally means "skip to new location" -- However,
							// this isn't defined on bitmaps loaded from a file
							// so we're going to treat it as an error
						
						return 0; // Error occurred
					
					default:
						
						// If we've overstepped the width of the bitmap -- This is bad :(
						if((x + data) > mWidth)
							return 0; // Error occurred

						// Okay first we need to move compData to the correct location
						// (we've "dealt with" two bytes remember)
						compData += 2;

						// Now we need to copy a chunk of memory (data length long) into our scanLine (at the correct offset)
						memcpy(scanLine + x, compData, data);
						
						x += data; // We update our count (from left to right on the scan line)
						compData += data; // We update our position in the compData
						
						// If data was odd -- Move compData to the next byte -- 
						// Basically every time we "move in compData", compData MUST 
						// be WORD aligned (it says so in MSDN)
						if((data & 1) != 0)
							compData++;

						break;
				
				} // end of switch(data)
			
			}
			else // If "data" DOES NOT equal zero
			{
				// If we've overstepped the width of bitmap -- This is bad :(
				if((x + data) > mWidth)
					return 0; // Error occurred
	
				// Otherwise fill a portion of the scan line (repeat count amount of times)
				// with the "pixel index to repeat"
				memset(scanLine + x, compData[1], data);
				
				x += data; // Update our "x-offset" (how far we've traversed down the scan line)
				compData += 2; // Move past the two bytes we just "dealt with"
			
			} // end of if(data == 0)...else
		
		} // end of while(x < mWidth)

		// If we haven't reached 2 bytes from the end of the compressed data, and if the first
		// two bytes in the compData (currently) both equal zero, we've reached the end
		// of a scan line in the compressed data
		if((compData <= twoFromEnd) && (compData[0] == 0) && (compData[1] == 0))
			compData += 2;	// Skip over the end of line data.
		
		else // We better be on the last line :)
		{
			if(y != (mHeight - 1))
				return 0; // Error occurred
		}

	} // end of for(int y = 0; y < mHeight; y++)
	
	// If the current values of the compData at index 0 and index 1 are zero and one (end of image sequence)
	// we've successfully decompressed and loaded the bitmap
	if((compData[0] == 0) && (compData[1] == 1))
		return mHeight; // We were successful in loading all the scan lines
	else
		return 0; // Error occurred

} // end of decompress(char *compData, uint dataLen)

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
	mPixels = NULL;
	mWidth = mHeight = mStride = 0;
		return;
}

// Deconstructor
CBitmap::~CBitmap() { freeMem(); }


	
