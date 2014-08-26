#include "card.h"
#include <assert.h>

// Constructor
CCard::CCard()
{
	// When suit or type equals eMaxSuits or eMaxValues, card is invalid 
	suit = eMaxSuits;
	type = eMaxTypes;
}

// Loads a card
bool CCard::load(const char *fileName)
{	
	// First try to load as .bmp file
	if(!img.loadBMP(fileName))
	{
		// If that fails, try to load as .tga file
		if(!img.loadTGA(fileName))
			return false;
	}
	
	// We got loaded
	return true;
}
		
// Draw the card to the "destDC" starting at upperleft corner (xPos, yPos)	
void CCard::draw(HDC destDC, int xPos, int yPos) const
{
	// Create destination RECT
	RECT rect = { xPos, yPos, xPos + img.getWidth(), yPos + img.getHeight() };
	
	// Blit the image
	img.transparentBlit(destDC, rect, kTRed, kTGreen, kTBlue);
}
		
// Set the card's suit and type
void CCard::setCard(ESuit s, EType v)
{
	suit = s;
	type = v;
}

// Set the card's suit and type
void CCard::setCard(int s, int v)
{
	assert(s >= 0 && s < eMaxSuits);
	assert(v >= 0 && v < eMaxTypes);
	
	suit = (ESuit)s;
	type = (EType)v;
}

  // ~~~~~~~~~~~~~~~~~~~~~ //
 // Card Helper Functions //
// ~~~~~~~~~~~~~~~~~~~~~ //

// Returns the text name of a ESuit
char* GetSuitName(CCard::ESuit suit)
{
	switch(suit)
	{
		case CCard::eDiamond: return "Diamond";
		case CCard::eSpade: return "Spade";
		case CCard::eHeart: return "Heart";
		case CCard::eClub: return "Club";
		default: return "Invalid";
	}
}
