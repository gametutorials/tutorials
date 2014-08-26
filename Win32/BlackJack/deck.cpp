#include "deck.h"
#include <assert.h>

// Load the deck
bool CDeck::load(const char *path)
{
	char fileName[MAX_PATH] = {0};
	
	// Loop over the four different suits of cards we have
	for(int i = 0; i < CCard::eMaxSuits; ++i)
	{
		// Loop over the thirteen cards per suit
		for(int j = 0; j < CCard::eMaxTypes; ++j)
		{	
			char fileName[MAX_PATH] = {0}; // The full path name of file
			char cardNum[3] = {0};	// The character string that holds the current card number
			
			// If we were passed a path, append it
			if(path)
			{
				// Append the path
				strcat(fileName, path);
				
				// Make sure the path name ends with a slash
				if(path[strlen(path) - 1] != '\\')
					strcat(fileName, "\\");
			}

			// Set the name of the suit
			strcat(fileName, GetSuitName((CCard::ESuit)i));
					
			_itoa_s((j+1),cardNum,10); // Get ASCII representation of card number
								   // +1 is because file names begin counting at one instead of
								  // zero

			strcat(fileName,cardNum); // Tack on card number
			strcat(fileName,".bmp"); // Tack on file extension

			int index = i * CCard::eMaxTypes + j; // This will give us the correct index into
												  // our array of cards images
												        
			assert(index >= 0 && index < kMaxCards);

			// Load the bitmap for the card
			if(!cards[index].load(fileName))
				return false;
			
			cards[index].setCard(i, j);

		} // end of for(int j = 0; j < MAX_TYPES; ++j)	

	} // end of for(int i = 0; i < MAX_SUITS; ++i)

	return true; // We got loaded %)
}

// Shuffle the deck
void CDeck::shuffle()
{
	int count = 0; // Current count of shuffled cards (doubles as an index into our deck)

	int temp[kMaxCards] = {0}; // A "temp" deck of cards -- We'll use this to determine what
							  // cards have been shuffled and what cards need to be shuffled

	while(count < kMaxCards) // Keep looping until all cards are shuffled
	{
		int suit = rand()%CCard::eMaxSuits; // Randomly select a suit
		int val = rand()%CCard::eMaxTypes; // Randomly select a type

		int which = suit * CCard::eMaxTypes + val; // Compute index into "temp" deck of cards
							  
		if(temp[which] == 0) // If this card HAS NOT been placed into the deck 
		{								
			indices[count] = which;
			temp[which] = 1; // This card has now been set so mark it off

			// Increment the count
			++count; 
		}

	} // end of while(count < kMaxCards)

} // end of shuffle()

// Get "which" card from the deck
CCard* CDeck::getCard(int which)
{
	assert(which >= 0 && which < kMaxCards);
	return &cards[indices[which]];
}

// Get "which" card's suit
CCard::ESuit CDeck::getCardSuit(int which)
{
	assert(which >= 0 && which < kMaxCards);
	return cards[indices[which]].getSuit();
}

// Get "which" card's value
CCard::EType CDeck::getCardType(int which)
{
	assert(which >= 0 && which < kMaxCards);
	return cards[indices[which]].getType();
}
	
	
	
