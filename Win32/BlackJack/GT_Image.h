#ifndef	GT_IMAGE_H
#define	GT_IMAGE_H

#include <windows.h>

typedef unsigned char uchar;

// Windows bitmaps are in BGR format instead of RGB.
// These defines will help let us index into a "color" more clearly
enum {
	eBlue = 0,
	eGreen = 1,
	eRed = 2,
	eAlpha = 3,
};

// Creates an ARGB color -- A, R, G, B are expected to be of type unsigned char
#define ARGB(A, R, G, B) ( (int)((A & 0xFF) << 24 | \
								 (R & 0xFF) << 16 | \
								 (G & 0xFF) << 8 | \
								 (B & 0xFF)) )
								 

#define GET_A(c) ((c >> 24) & 0xFF)
#define GET_R(c) ((c >> 16) & 0xFF)
#define GET_G(c) ((c >> 8) & 0xFF)
#define GET_B(c) (c & 0xFF)

// The only type of .tga files this library supports
#define TGA_RGB 2

// An image class which supports 24-bit images
// Minimal error checking so optimized for speed
class CImage
{
	public:
		
		CImage(); // Constructor() -- Zero's out CImage
		CImage(const CImage &image); // Copy constructor

		CImage& operator =(const CImage& image); // Assignment operator

		// Data Access Functions ************
		
			inline int getWidth() const { return mWidth; } 
			inline int getHeight() const { return mHeight; }
			inline int getChannels() const { return mChannels; }
			inline int getStride() const { return mStride; }
			inline HDC getDC() const { return mHDC; }
		
		// ****** End of Data Access Functions

		float getVersion() const; // Returns the version number of the CImage lib

		RECT getRect() const; // Returns a RECT of CImage with upper-left corner being (0,0)

		// Returns a pointer to the line passed in
		// ---------- Line (0) Top of image
		// |		|
		// |		|
		// |		|
		// ---------- Line (Height - 1) Bottom of image
		uchar* getLinePtr(int which); 
		uchar* getLinePtr(int which) const;

		// Returns true for success -- false otherwise
		// If set_size is called on a CImage that already has memory associated with it
		// that memory is freed and the new size is implemented
		bool setSize(int width, int height, int channels = 3);

		// Loads a bmp with specified fileName -- Returns true on success, false otherwise
		// If loadBMP() is called on a CImage that already has memory associated with
		// it, that memory is freed and the the .bmp is loaded
		bool loadBMP(const char *fileName);

		// Loads a .tga with specified fileName -- Returns true on success, false otherwise
		// Free's existing CImage if already allocated
		bool loadTGA(const char *fileName);

		// Sets the pixel (x,y) to the color passed in
		void setPixel(int x, int y, uchar R, uchar G, uchar B);
		void setPixel(int x, int y, int argb);
												   
		// Sets the CImage to the specified color
		void setToColor(uchar R, uchar G, uchar B);
		void setToColor(int argb);
		
		// Fills the rectangle area in the CImage to the color passed in
		void fillRect(const RECT &rect, uchar R, uchar G, uchar B);
		void fillRect(const RECT &rect, int argb);

		// Blits CImage to "dest" starting at upper-left coordinates (x,y)
		void blit(CImage &dest, int x, int y) const;
		
		// Blits CImage to "destDC" using dimensions in "destRect"
		void blit(HDC destDC, const RECT &destRect) const;
		
		// Transparent blits to "destDC" using dimensions in "destRect" with specified
		// transparency color
		void transparentBlit(HDC destDC, const RECT &destRect, uchar R, uchar G, uchar B) const;
		void transparentBlit(HDC destDC, const RECT &destRect, int color = ARGB(0,0,0,0)) const;

		// Blits to "dest" using alpha starting at upper-left coordinates (x,y)
		// **NOTE** CImage must be 32-bit or unexpected results will occur
		void alphaBlit(CImage &dest, int x, int y) const;

		// Fills the CImage with "src" by the dimensions specified in "srcRect"
		// starting at upper-left corner (destX, destY) using transparency color (R,G,B)
		// Does not alter alpha in source or destination
		void transparentFill(CImage &src, const RECT &srcRect, int destX, int destY,
							 uchar R, uchar G, uchar B);

		// Fills the CImage with "src" by the dimensions specified in "srcRect"
		// starting at upper-left corner (destX, destY)
		// Achieves "opaqueness" by only drawing every other pixel
		// Does not alter alpha in source or destination
		void opaqueFill(CImage &src, const RECT &srcRect, int destX, int destY);
		
		// Fills the CImage with "src" by the dimensions specified in "srcRect"
		// starting at upper-left corner (destX, destY) using per pixel alpha in "src"
		// Does not alter alpha in source or destination
		// **NOTE** "src" must be a 32-bit CImage or this method fails
		void alphaFill(CImage &src, const RECT &srcRect, int destX, int destY);
				
		void blendDown(); // Averages all lines together from top to bottom
		void blendUp(); // Averages all lines together from bottom to top

		void decrement(int amount); // Decrements CImage by "amount"
		void increment(int amount); // Increments CImage by "amount"
		
		// Deconstructor();
		~CImage();
		
	private:

		int mWidth;
		int mHeight;
		int mChannels;
		int mStride;

		HBITMAP mBmp;
		HBITMAP mOldBmp;

		HDC mHDC;

		uchar *mPixels;

		void freeMem(); // Frees all mem associated with CImage
};

#endif

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
