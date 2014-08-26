#ifndef WIN_MAIN_H
#define WIN_MAIN_H

#include "winobj.h"

#define MAX_LENGTH 20 // This is the max number of characters that can be in a sentence

// These are the width and height of every letter
#define LTR_WID 64
#define LTR_HGT 64

#define ALPHA_CONST 65 // This is the decimal ASCII representations of the ASCII entry
					  // immediately before the ASCII entry for the capital letter 'A'
					 // We'll use this to determine offsets into our Alphabet.bmp we 
					// want to blit from

// This struct will hold all the things we need for a text blitter
struct SText
{
	HBITMAP hbitmap; // An hbitmap (for the alphabet bitmap)
	HBITMAP old_bitmap; // Any time we select a hbitmap into an HDC, we need to save
						// the "bitmap that was there" so we can select back into the HDC
						// when we are done with it

	HDC hdc; // An HDC to draw with
};


// This is where all the magic happens -- Okay by parameter
// win_obj -- A reference to a CWinObj, the CWinObj contains the windows HDC
// text -- A reference to a SText, this contains all the information we need to "text blit"
// sentence -- A char array with the "sentence" to print
// x -- Upper left x-coord of where to begin printing
// y -- Upper left y-ccord of where to begin printing 
void PrintText(CWinObj &win_obj, SText &text, char sentence[MAX_LENGTH], int x, int y);

#endif
