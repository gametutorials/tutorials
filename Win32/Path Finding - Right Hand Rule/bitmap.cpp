#include "bitmap.h"

// Returns the address in memory of the specified line. This gives you a pointer to at least 
// width * channels bytes. Lines are numbered such that when the bitmap
// is displayed line zero is at the top.
uchar* CBitmap::getLinePtr(int which_line)
{
	// Windows stores bitmaps upside down so (bmp_height - which_line - 1)
	// flips the bitmap over

	return (pixel_bits + bmp_stride * (bmp_height - which_line - 1));
}

// Returns true for success -- false otherwise
bool CBitmap::setSize(int width, int height, int channels)	
{
 
	// If CBitmap has already been set -- clear it out first
	freeMem();

	// Create a compatible HDC
	bmp_hdc = CreateCompatibleDC(NULL);
	
		// Error Check
		if(!bmp_hdc)
			return false;

	bmp_width = width;
	bmp_height = height;
	bmp_channels = channels;
	
	// Set stride
	bmp_stride = bmp_width * bmp_channels;

	while((bmp_stride % 4) != 0) // Ensure bmp_stride is dword aligned
		bmp_stride++;
		
	BITMAPINFO bmp_info = {0};

	// Initialize the parameters that we care about
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biWidth = width;
	bmp_info.bmiHeader.biHeight = height;
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = channels * 8;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biClrUsed = 0;

	// Create a DIBSection. Note that this returns us two
	// things - an HBITMAP handle and a memory pointer, in
	// pixel_bits.
	hbitmap = CreateDIBSection(bmp_hdc, &bmp_info, DIB_RGB_COLORS,
							   (void**)&pixel_bits, 0, 0);
		// Error Check
		if(!hbitmap)
			return false;		

	// Select our DIBSection bitmap into our DC.
	// This allows us to draw to the bitmap using GDI functions.
	old_bmp = (HBITMAP)SelectObject(bmp_hdc,hbitmap);

		// Error Check
		if (!old_bmp)
			return false;

	return true;

} // end of setSize(int width, int height, int channels)

bool CBitmap::loadBMP(char *file_name)
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

	bool status = false; // Status of our load attempt

	BITMAPFILEHEADER bmp_fileheader;

	// Read the BITMAPFILEHEADER
	if(!fread(&bmp_fileheader, sizeof(BITMAPFILEHEADER), 1, bmp_file))
		goto BMP_FINISH_LOAD;

	// Check the type field to make sure we have a .bmp file
	if(memcmp(&bmp_fileheader.bfType, "BM", 2))
		goto BMP_FINISH_LOAD;

	BITMAPINFOHEADER bmp_infoheader;
	
	// Read the BITMAPINFOHEADER.
	if(!fread(&bmp_infoheader, sizeof(BITMAPINFOHEADER), 1, bmp_file))
		goto BMP_FINISH_LOAD;

	// Double Check to make sure we got a correct BITMAPINFOHEADER
	if(bmp_infoheader.biSize != sizeof(bmp_infoheader))
		goto BMP_FINISH_LOAD;

	// Check for unsupported format
	if(bmp_infoheader.biPlanes != 1)
		goto BMP_FINISH_LOAD;

	// CBitmap only supporst 24-bit and 32-bit .bmps
	if((bmp_infoheader.biBitCount != 24) && (bmp_infoheader.biBitCount != 32))
		goto BMP_FINISH_LOAD;

	if(setSize(bmp_infoheader.biWidth,bmp_infoheader.biHeight,bmp_infoheader.biBitCount / 8) == false)
		goto BMP_FINISH_LOAD;

	// Jump to the location where the bitmap data is stored
	// fseek() returns 0 on success
	if(fseek(bmp_file, bmp_fileheader.bfOffBits, SEEK_SET))
		goto BMP_FINISH_LOAD;

	// One more double check that we have a .bmp we can handle
	if(bmp_infoheader.biCompression != BI_RGB)
		goto BMP_FINISH_LOAD;

	{ // Now read in the pixel bits

		unsigned int bytesUsed = bmp_width * bmp_channels;
		unsigned int padding = bmp_stride - bytesUsed;

			for(int y = 0; y < bmp_height; y++)
			{
				// We have to flip the y-coordinate because our bitmap class
				// puts line zero at the top of the screen, and the .bmp format
				// on disk puts the first bytes at the bottom of the screen
				uchar *line_ptr = getLinePtr(bmp_height - 1 - y);

				// Read the precise number of bytes that the line requires into the bitmap.
				// Don't read the padding bytes, because the in memory alignment requirements
				// may vary
				if(!fread(line_ptr, bytesUsed, 1, bmp_file))
					goto BMP_FINISH_LOAD;

				// Skip over any padding bytes.
				if(fseek(bmp_file, padding, SEEK_CUR))
					goto BMP_FINISH_LOAD;
		
			} // end of for(int y = 0; y < bmp_height; y++)

		status = true; // Load was successful
	}

BMP_FINISH_LOAD:

	fclose(bmp_file);
		return status; // Return the status of our load attempt

} // end of loadBMP(char *file_name, HDC hdc)

// Blit CBitmap to "dest_dc" using coordinates specified by "rect"
void CBitmap::blit(HDC dest_dc, const RECT &rect)
{
	BitBlt(dest_dc,rect.left,rect.top,(rect.right - rect.left),(rect.bottom - rect.top),
		   bmp_hdc,0,0,SRCCOPY);
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
	bmp_width = bmp_height = bmp_channels = bmp_stride = 0;
	pixel_bits = NULL;
		return;
}

// Deconstructor
CBitmap::~CBitmap() { freeMem(); }

	
