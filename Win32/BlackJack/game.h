#ifndef GAME_H
#define GAME_H

#include "GT_WinObj2D.h"
#include "deck.h"
#include "gamedefines.h"

enum EGameState { eNew, eBet, ePlayerTurn, eDealerTurn, eFinishHand, eBetweenHands,
				  ePlayAgain, eMaxGameStates };

enum EGameButtons { eHit, eStay, eDeal, eIncBet, eDecBet, eMaxButtons };

enum EPlayer { eHuman, eComputer };

// This struct will hold all of our Blackjack game data
struct SGameData
{
	char msg[kMsgTextLen];

	const CCard *playerCards[kMaxCardsPerHand];
	const CCard *dealerCards[kMaxCardsPerHand];

	int bet; // Holds the amount of the player's current bet
	int cash; // Holds the amount of money the player has
	int topCard; // Index to the top card of the deck
	int msgColor; // Color of the message text	

	EGameState state; // The current game state

	float idleTime; // Time in milliseconds to pause, so the player can clearly
				   // see screen updates
				   
	bool gameOver; // True when game is over, false otherwise
};

// Our game object that handles all game related tasks
class CGame
{
	public:
	
		CGame();
		
		bool init(HWND hwnd, HINSTANCE hinst); // Init the game application
		void run(); // Run the game
		
		void handleButtons(WORD buttonId);

	private:
		
		CWinObj2D win_obj;
		
		// The cards and the back of the card
		CDeck deck;
		CImage card_back;
		
		HWND buttons[eMaxButtons]; // Buttons for game input
		SGameData data; // All other game data
		
		  // ~~~~~~~~~~~~~~~ //
		 // Private Methods //
	    // ~~~~~~~~~~~~~~~ //
	    
		bool initButtons(); // Init all Win32 buttons
		void initGameState(int initialBetAmt = kMinBet, EGameState gameState = eBet);
	
		void process(float dt);
		void processDealer();
		void processStatus(); // Checks the win/lose status of all of the current hands
		
		// Switch to a new game state
		void setState(EGameState newState);
		void setMsg(const char *text, int color = ARGB(0,0,0,0)); // Set the message text
				
		int getCardValue(const CCard *card); // Returns the Blackjack value of the passed in card
		int getHandValue(EPlayer which); // Returns the current value of the player's hand
		
		bool isHandABust(EPlayer which);
		bool isHandFull(EPlayer which); // Returns true if hand is full (player has kMaxCardsPerHand
									   // in their hand), otherwise false
		bool isGameOver();
		
		void awardBet(const char *msg); // Give player their earnings and start a new Blackjack hand
		void deductBet(const char *msg); // Receive player's bet and start a new Blackjack hand
		
		void draw();
		
		void drawPlayerHand();
		void drawDealerHand();
		void drawText();
		
		void emptyHands(); // Empties the player and dealer hands
		void dealCards(EPlayer which, int num); // Deals "num" cards to "which" player
		
};

#endif
