#ifndef CARD_H
#define CARD_H

#include "GT_Image.h"

// Transparency (R,G,B) for cards
const uchar kTRed = 0;
const uchar kTGreen = 168;
const uchar kTBlue = 0;

class CCard
{
	public:
	
		enum ESuit { eDiamond, eSpade, eHeart, eClub, eMaxSuits };
		enum EType { eAce, eTwo, eThree, eFour, eFive, eSix, eSeven, eEight, eNine,
					 eTen, eJack, eQueen, eKing, eMaxTypes };
	
		CCard();
		
		// Loads the card image specified by "fileName"
		bool load(const char *fileName);
		
		// Draw the card to the "destDC" starting at upperleft corner (xPos, yPos)
		void draw(HDC destDC, int xPos, int yPos) const;
		
		// Set the card's suit and value
		void setCard(ESuit s, EType t);
		void setCard(int suit, int type);
		
		// Data Acess ***
		
			ESuit getSuit() const { return suit; }
			EType getType() const { return type; }
			
			// Return width/height of card image
			int getWidth() const { return img.getWidth(); }
			int getHeight() const { return img.getHeight(); }
		
		// *** End Data Access
		
	private:
	
		// Copy constructor and assignment operator are private because 
		// we do not want copies of this object
		CCard(const CCard &card);
		CCard& operator =(const CCard &card) { return *this; }
		
		ESuit suit;
		EType type;
		
		CImage img; // Card image
};

// Returns the text name of a suit
char* GetSuitName(CCard::ESuit suit);

#endif
