#ifndef DECK_H
#define DECK_H

#include "card.h"

const int kMaxCards = 52;

class CDeck
{
	public:
		
		CDeck() { /* do nothing */ } 
		
		// Loads the deck -- Assumes card names are like such:
		// Club1 = Ace of clubs
		// Club2 = Two of clubs
		// ...
		// Club12 = Queen of clubs
		// Club13 = King of clubs
		// Replace "Club" with appropriate suit name for other suits
		// Uses "path" as the search path for opening files
		// Returns true on success, false otherwise
		bool load(const char *path);
		
		void shuffle(); // Shuffles the deck
		
		// Data Access ***
		
			CCard* getCard(int which); // Returns "which" card
			CCard::ESuit getCardSuit(int which); // Returns suit of "which" card
			CCard::EType getCardType(int which); // Returns type of "which" card
			
		// *** End Data Access
		
	private:
	
		// Copy constructor and assignment operator are private because 
		// we do not want copies of this object
		CDeck(const CDeck &deck);
		CDeck& operator =(const CDeck &deck) { return *this; }
		
		int indices[kMaxCards]; // Stores the shuffled order 
		CCard cards[kMaxCards];	
};

#endif
