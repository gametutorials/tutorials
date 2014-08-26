#include "bitmap_class.h"

// Initilizes our CTarga to a specified width, height, and color depth
bool CTarga::init(int w, int h, int c)	
{
	// If we're not trying to create a 24-bit or 32-bit CTarga, there is 
	// no need to go on
	if(!(c == 3 || c == 4))
		return false;
	
	// If CTarga has already been set -- clear it out first
	freeMem();
	
	// Set the width, height, and number of channels
	width = w;
	height = h;
	channels = c;
	
	// Set the stride
	stride = width * channels;

	while((stride % 4) != 0) // Ensure stride is DWORD aligned
		stride++;

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
	//        initialize the hbitmap's colors
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

	// Create a compatible DC with whatever the current display mode is
	hdc = CreateCompatibleDC(NULL);
	
		// Error Check
		if(!hdc)
			return false;

	// Select our DIBSection bitmap into our HDC.
	// This allows us to draw to the bitmap using GDI functions.
	old_bmp = (HBITMAP)SelectObject(hdc,hbitmap);

		// Error Check
		if (!old_bmp)
			return false;		

	return true;

} // end of init(int width, int height, int channels)


// Loads a 24-bit or 32-bit .tga file
bool CTarga::loadTGA(char *file_name)
{
	// Make sure we have a legal file name
	if(!file_name)
		return false;

	FILE *tga_file = fopen(file_name,"rb");

		// Error Check
		if(!tga_file)
			return false;

	BYTE idLength; // Length of the identification field in the tga
				  // After we read the tga header this is the length in bytes to
				 // the pixels

	// Read in the idLength
	if(!fread(&idLength, sizeof(BYTE), 1, tga_file))
	{
		fclose(tga_file);
			return false;
	}

	fseek(tga_file,1,SEEK_CUR); // Increment our file pointer to the next byte
								// in the .tga file

	BYTE imageID; // This variable will hold the "images ID" -- This will tell us 
				 // what type of image we have (ie compressed, color-mapped image)

	// Read in the imageID
	if(!fread(&imageID, sizeof(BYTE), 1, tga_file))
	{
		fclose(tga_file);
			return false;
	}

	// Only support RGB uncompressed .tga files
	if(imageID != TGA_RGB)
		return false;

	/*** HERE WE'RE SKIPPING SOME BYTES OF INFORMATION THAT DESCRIBE THE ORIENTATION OF
	     THE IMAGE -- WE ARE ASSUMING THE IMAGE'S TOP LEFT CORNER IS AT (0,0) ***/

	fseek(tga_file,9,SEEK_CUR); // Seek to width of image
							   // When you look at the file format of a .tga (I tell you
							  // where I got the documentation at the bottom of win_main.cpp)
							 // why we move ahead 9 bytes will become apparent :)

	WORD wid, hgt; // These variables will hold the width and height of the image
	BYTE chn; // This will hold the images channels (3 == 24-bit, 4 == 32-bit)

	// Read the width
	if(!fread(&wid, sizeof(WORD), 1, tga_file))
	{
		fclose(tga_file);
			return false;
	}

	// Read the height
	if(!fread(&hgt, sizeof(WORD), 1, tga_file))
	{
		fclose(tga_file);
			return false;
	}

	// Read the channels
	if(!fread(&chn, sizeof(BYTE), 1, tga_file))
	{
		fclose(tga_file);
			return false;
	}

	// We're not supporting 16-bit .tga files, so if we're tying to load a
	// 16-bit .tga file, just return false
	if(chn == 16)
		return false;

	// Otherwise it is either a 24 or 32-bit .tga file
	// So we set our CTarga's size
	if(!init(wid,hgt,chn / 8))
		return false;

	fseek(tga_file,idLength + 1,SEEK_CUR); // Seek to where the image bytes should begin
									      // + 1 is for the image descriptor byte

	// This equals the number of bytes in one scan line
	int bytesUsed = width * channels;

	// Load in all the pixel data
	for(int y = 0; y < height; y++)
	{
		uchar *line_ptr = getLinePtr(y);

		if(!fread(line_ptr,bytesUsed,1,tga_file))
		{
			fclose(tga_file);
				return false;
		}

	}

	// Close the file and we're done
	fclose(tga_file);
		return true; // Everything went well :)


} // end of loadTGA(char *file_name, HDC hdc)

// Returns the address in memory of the specified line. This gives us a pointer to at least 
// width * channels bytes. Lines are numbered such that when the bitmap
// is displayed line zero is at the top.
uchar* CTarga::getLinePtr(int which_line)
{
	return (surface_bits + stride * which_line);
}

// Release the memory
void CTarga::freeMem() 
{
	if(!old_bmp)
		return;
	
	if(!SelectObject(hdc,old_bmp)) // get the old stuff back
		return;
	
	if(!DeleteObject(hbitmap)) // delete the new stuff
		return;

	if(!DeleteDC(hdc)) // release the device context
		return;

	// Zero out member variables
	old_bmp = hbitmap = NULL;
	hdc = NULL;
	width = height = channels = stride = 0;
	surface_bits = NULL;
}

// Deconstructor -- freeMem the memory
CTarga::~CTarga() { freeMem(); }
	
