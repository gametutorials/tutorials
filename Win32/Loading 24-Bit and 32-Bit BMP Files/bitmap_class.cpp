#include "bitmap_class.h"

// Initilizes our CBitmap to a specified width, height, and color depth
// Uses hdc for the compatible HDC
bool CBitmap::init(HDC hdc, int width, int height, int channels)	
{
	// If we're not trying to create a 24-bit or 32-bit CBitmap, there is 
	// no need to go on
	if(!(channels == 3 || channels == 4))
		return false;

	// If CBitmap has already been set -- clear it out first
	freeMem();

	// Set the width, height, and number of channels
	bmp_width = width;
	bmp_height = height;
	bmp_channels = channels;
	
	// Set the stride
	bmp_stride = bmp_width * bmp_channels;

	while((bmp_stride % 4) != 0) // Ensure bmp_stride is DWORD aligned
		bmp_stride++;

	BITMAPINFO bmp_info = {0};

	// Initialize the parameters that we care about
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biWidth = width;
	bmp_info.bmiHeader.biHeight = height;
	bmp_info.bmiHeader.biPlanes = 1; // Must be set to one
	bmp_info.bmiHeader.biBitCount = channels * 8;
	bmp_info.bmiHeader.biCompression = BI_RGB; // Bitmap is NOT compressed	

	// This will create an our hbitmap with the properties we desire -- By parameter
	// hdc -- This is HDC we want to use to determine how we're going to
	//        initialize the hbitmap's colors (ie the HDC associated with the 
	//		  window will work)
	// bmp_info -- Pointer to the BITMAPINFO that contains the various
	//             attributes of the hbitmap we want to create
	// DIB_RGB_COLORS -- Means the BITMAPINFO struct contains an array of actual colors.
	//					 In other words NO PALLETES
	// surface_bits	-- Pointer to a pointer to receive the address of hbitmap's pixel bits
	// NULL -- Handle to a file mapping object the function will use to create the hbitmap
	//		   NULL means no file mapping object
	// NULL -- An offset for the file mapping object, if the handle to the file mapping object
	//		   is NULL, this parameter is ignored
	hbitmap = CreateDIBSection(hdc, &bmp_info, DIB_RGB_COLORS,
							   (void**)&surface_bits, NULL, NULL);
		// Error Check
		if(!hbitmap)
			return false;

	// Create a copy of our DC that we can keep around
	bmp_hdc = CreateCompatibleDC(hdc);
	
		// Error Check
		if(!bmp_hdc)
			return false;
		

	// Select our DIBSection bitmap into our HDC.
	// This allows us to draw to the bitmap using GDI functions.
	old_bmp = (HBITMAP)SelectObject(bmp_hdc,hbitmap);

		// Error Check
		if (!old_bmp)
			return false;
		

	return true;

} // end of set_size(int width, int height, int channels)


// Loads a 24-bit or 32-bit .bmp file
bool CBitmap::loadBMP(char *file_name, HDC hdc)
{
	// If CBitmap has already been set -- clear it out first
	freeMem();

		// Error Check -- Make sure they passed in a valid file name
		if(!file_name)
			return false;

	FILE *bmp_file = fopen(file_name, "rb");

		// Error Check -- Make sure the file could be opened
		if(!bmp_file)
			return false;

	BITMAPFILEHEADER bmp_fileheader = {0};

	// Read the BITMAPFILEHEADER
	if(!fread(&bmp_fileheader, sizeof(BITMAPFILEHEADER), 1, bmp_file))
	{
		fclose(bmp_file);
			return false;
	}

	// Check the type field to make sure we have a .bmp file
	// bfType should contain the letters "BM"
	if(memcmp(&bmp_fileheader.bfType, "BM", 2))
	{
		fclose(bmp_file);
			return false;
	}

	BITMAPINFOHEADER bmp_infoheader = {0};
	
	// Read the BITMAPINFOHEADER.
	if(!fread(&bmp_infoheader, sizeof(BITMAPINFOHEADER), 1, bmp_file))
	{
		fclose(bmp_file);
			return false;
	}

	// Double Check to make sure we got a correct BITMAPINFOHEADER
	if(bmp_infoheader.biSize != sizeof(bmp_infoheader))
	{
		fclose(bmp_file);
			return false;
	}

	// Check for unsupported format
	// biPlanes MUST equal 1
	if(bmp_infoheader.biPlanes != 1)
	{
		fclose(bmp_file);
			return false;
	}

	// Initililize our CBitmap to be the same width, height and number of channels as
	// the .bmp we are loading
	if(init(hdc,bmp_infoheader.biWidth,bmp_infoheader.biHeight,bmp_infoheader.biBitCount / 8) == false)
	{
		fclose(bmp_file);
			return false;
	}

	// Jump to the location where the bitmap data is stored
	// **Note** fseek() returns 0 on success
	if(fseek(bmp_file, bmp_fileheader.bfOffBits, SEEK_SET))
	{
		fclose(bmp_file);
			return false;
	}

	// One more double check that we have a .bmp we can handle
	// We can only handle uncompressed .bmps
	if(bmp_infoheader.biCompression != BI_RGB)
	{
		fclose(bmp_file);
			return false;
	}

	// Calculate the number of padding bytes
	unsigned int bytesUsed = bmp_width * bmp_channels;
	unsigned int padding = bmp_stride - bytesUsed;

		// Fill our CBitmap pixel data
		for(int y = 0; y < bmp_height; y++)
		{
			// Get the current scan line in our CBitmap
			uchar *LinePtr = getLinePtr(y);

			// Read the precise number of bytes that the line requires into the bitmap.
			// Don't read the padding bytes, because the memory alignment requirements
			// may vary -- We don't want our reading code to depend on our bitmap class
			// implementation :)
			if(!fread(LinePtr, bytesUsed, 1, bmp_file))
			{
				fclose(bmp_file);
					return false;
			}

			// Skip over any padding bytes.
			if(fseek(bmp_file, padding, SEEK_CUR))
			{
				fclose(bmp_file);
					return false;
			}
	
		} // end of for(int y = 0; y < bmp_height; y++)


	fclose(bmp_file);
		return true; // If we get here .bmp was read in successfully

} // end of loadBMP(char *file_name, HDC hdc)

// Returns the address in memory of the specified line. This gives us a pointer to at least 
// width * channels bytes. Lines are numbered such that when the bitmap
// is displayed line zero is at the top.
uchar* CBitmap::getLinePtr(int which_line)
{
	return (surface_bits + bmp_stride * which_line);
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

	// Zero out member variables
	old_bmp = hbitmap = NULL;
	bmp_hdc = NULL;
	bmp_width = bmp_height = bmp_channels = bmp_stride = 0;
	surface_bits = NULL;
}

// Deconstructor -- Free the memory
CBitmap::~CBitmap() { freeMem(); }
	
