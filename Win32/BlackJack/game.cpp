#include "game.h"
#include <assert.h>
#include <stdio.h>

// Constructor
CGame::CGame()
{
	/* Constructors init:  win_obj, deck, and card_back */
	
	memset(buttons, 0, sizeof(buttons));
	memset(data.msg, 0, sizeof(data.msg));
	
	data.bet = 0;
	data.cash = 0;
	data.topCard = 0;
	data.msgColor = 0;
		
	data.state = eNew;
	data.idleTime = 0.0f;
	data.gameOver = false;
	
	emptyHands(); // Set player and dealer hands to empty
}

// Init the game application -- Return true on success, false otherwise
bool CGame::init(HWND hwnd, HINSTANCE hinst)
{
	if(hwnd == NULL || hinst == NULL)
		return false;
	
	// Init window object	
	if(!win_obj.init(hinst, hwnd))
		return false;
	
	// Load the deck of cards
	if(!deck.load("Imgs\\Cards\\"))
		return false;
		
	// Load back of card
	if(!card_back.loadBMP("Imgs\\CardBack.bmp"))
		return false;
	
	// Init all of the game's buttons
	if(!initButtons())
		return false;
	
	initGameState(kMinBet, eNew);
		return true;
}

// Run the game -- This is the game loop
void CGame::run()
{
	MSG msg = {0};

	while(data.gameOver == false)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(win_obj.lockToFrameRate())
		{
			win_obj.clear();
			
			process(win_obj.getDT());
			draw();
						
			// Draw frame rate
			#ifdef DRAW_FPS
				win_obj.drawFPS();
			#endif
			
			win_obj.draw(); // Draw the back buffer to the screen 
			
			SleepEx(1, true);  // Give OS a little time.  This makes the Task Manager CPU Usage not
							  // spike at 100%.  **NOTE** This lowers overall frame rate and shouldn't
							 // be done in a speed critical application
		}
	}
}

// Handle the Win32 button input
void CGame::handleButtons(WORD buttonId)
{
	// Depending on which button was pushed do different things
	switch(buttonId)
	{
		case eHit:
		
			if(data.state == ePlayerTurn) // If the player's turn
			{
				// If they choose to hit and have room in their hand, deal the
				// player a card
				if(isHandFull(eHuman) == false)
					dealCards(eHuman, 1); // Deal the player one card
			}
			
			break;

		case eStay:

			setState(eDealerTurn);
				break;

		case eDeal:

			if(data.bet > 0) // If they placed a bet
			{
				dealCards(eHuman, 2); // Deal two cards to player
				dealCards(eComputer, 2); // Deal two cards to dealer
	
				setState(ePlayerTurn);
			}
			
			break;

		case eIncBet:

			if(data.state == eBet)
				data.bet += 100; // Raise bet by 100 dollars

			if(data.bet > data.cash)
				data.bet = data.cash; // Can't bet more than what the player has

			break;

		case eDecBet:

			if(data.state == eBet)
				data.bet -= 100; // Lower bet by 100 dollars

			if(data.bet < kMinBet) 
				data.bet = kMinBet; // Our minimum bet is $100 dollars so make sure we
									// bet at least $100
			break;
	}	
}

  // ~~~~~~~~~~~~~~~ //
 // Private Methods //
// ~~~~~~~~~~~~~~~ //

// Create all the Win32 buttons for the game
bool CGame::initButtons()
{
	buttons[eHit] = CreateWindow("Button","Hit",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									CW_USEDEFAULT, win_obj.getHWND(), (HMENU)eHit,
									win_obj.getHINST(), NULL);

	buttons[eStay] = CreateWindow("Button","Stay",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									CW_USEDEFAULT, win_obj.getHWND(), (HMENU)eStay,
									win_obj.getHINST(), NULL);

	buttons[eDeal] = CreateWindow("Button","Deal",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									CW_USEDEFAULT, win_obj.getHWND(), (HMENU)eDeal,
									win_obj.getHINST(), NULL);

	buttons[eIncBet] = CreateWindow("Button","Inc +",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									CW_USEDEFAULT, win_obj.getHWND(), (HMENU)eIncBet,
									win_obj.getHINST(), NULL);

	buttons[eDecBet] = CreateWindow("Button","Dec -",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									CW_USEDEFAULT, win_obj.getHWND(), (HMENU)eDecBet,
									win_obj.getHINST(), NULL);
									
	// If any of the buttons were not created there's been an error
	for(int i = 0; i < eMaxButtons; ++i)
	{
		if(buttons[i] == NULL)
			return false;
	}									

	// Position our buttons and size them to our needs 
	// **NOTE** There was no equation used to derive these numbers.  They are 
	//			all arbitrary and were picked by the process of empirical analysis.
	MoveWindow(buttons[eHit],500,300,kButtonWid,kButtonHgt,true);
	MoveWindow(buttons[eStay],500,350,kButtonWid,kButtonHgt,true);
	MoveWindow(buttons[eDeal],500,300,kButtonWid,kButtonHgt,true);
	MoveWindow(buttons[eIncBet],500,200,kIncDecButtonsWid,kIncDecButtonsHgt,true);
	MoveWindow(buttons[eDecBet],545,200,kIncDecButtonsWid,kIncDecButtonsHgt,true);
		return true;
}

// Init the game data
void CGame::initGameState(int initialBetAmt, EGameState gameState)
{
	deck.shuffle(); // Shuffle all the cards
	emptyHands(); // Clear out all the cards in the player's and dealer's hand
	
	data.topCard = 0; // Start back at the top of the deck
	data.bet = initialBetAmt; // Set player's starting bet amount
	setState(gameState);
}

// Processes the game every frame, updating the game state as apporpriate
void CGame::process(float dt)
{
	// Process the status of the game every frame
	processStatus();
	
	// Process the keys -- We only have to see if they want to quit
	if(win_obj.isKeyDown(VK_ESCAPE))
		data.gameOver = true;

	// Check to see if we're idle... 
	if(data.idleTime > 0.0f)
	{
		data.idleTime -= dt;
			return;
	}

	// Process game based on current game state
	switch(data.state)
	{
		case eNew:
			
			data.cash = kStartingCash;
			setState(eBet);
				break;

		case ePlayerTurn:

			// If they have hit five times, move to dealer's turn
			if(isHandFull(eHuman) == true)
				setState(eDealerTurn);
						
			break;

		case eDealerTurn:
			
			processDealer();
				break;
				
		case eBetweenHands: // Idle state occurs at the conclusion of each hand
			
			initGameState(); // Get ready for next hand
				break;
				
		case ePlayAgain:
		
			if(MessageBox(win_obj.getHWND(), 
						  "You lost all your money.  Want to play again?",
						  "GAME OVER", MB_YESNO) == IDYES)
			{
				data.idleTime = 0.0f; // Erase any remaining idle time 
				initGameState(kMinBet, eNew); // Start the game over
			}
			else
				data.gameOver = true; // Quit the application
						
			break;					  

	} // end of switch(data.state)
}

// Decides the dealer's course of action
void CGame::processDealer()
{
	int dealerVal = getHandValue(eComputer);
	int playerVal = getHandValue(eHuman);
	
	// This shouldn't happen since we check every frame for "end of hand" condition
	assert(dealerVal <= kMaxHandValue);
	
	// If the dealer has given themselves five cards, it's time to finish the hand
	if(isHandFull(eComputer) == true)
	{
		setState(eFinishHand); // Dealer's stays, see who has won
			return;	
	}	
	else if(dealerVal == 16) // If the dealer has a hand of 16, they have to hit
	{ 
		dealCards(eComputer, 1);
			return;
	}	
	else if(dealerVal == 17) // If the dealer has a hand of 17, they have to stay
	{
		setState(eFinishHand); // Dealer's stays, see who has won
			return;		
	}
	else // We need to make a decision
	{
		// Well since our dealer ONLY has to worry about beating the player
		// our decision isn't too difficult.  If we have a higher score, we've 
		// won.  If we are tied, we'll push and simply restart the round.  Otherwise
		// we must have a lower score so we'll hit
		if(dealerVal >= playerVal) // If the dealer has at least tied the player the dealer is done
		{
			setState(eFinishHand); // Dealer's stays, see who has won
				return;
		}
		else // If the dealer hasn't busted he must be lower than the player so hit
			dealCards(eComputer, 1);
	}
}

// This determines the winner/loser and updates all game data accordingly
void CGame::processStatus()
{
	// Only need to process status of game while in an active Blackjack hand state
	if((data.state < eBet) || (data.state > eFinishHand))
		return;

	// If the player busted, he loses
	if(isHandABust(eHuman))
		deductBet("You busted");
	else if(isHandABust(eComputer)) // If the computer busts, the player wins
		awardBet("You won");
	else if(data.state == eFinishHand) // If the dealer if finished, end the hand
	{
		int dealerVal = getHandValue(eComputer);
		int playerVal = getHandValue(eHuman);
		
		if(dealerVal > playerVal)
			deductBet("You lost");
		else if(playerVal > dealerVal)
			awardBet("You won");
		else // Must be a push -- Hand is discarded, allow player to rebet
		{
			data.idleTime = kIdleTime;
			setMsg("Push", ARGB(255, 0, 255, 0));
			setState(eBetweenHands);
		}
	}
	
	// If the game is over
	if(isGameOver() == true)
		setState(ePlayAgain);
}	

// Sets the state of the game
void CGame::setState(EGameState newState)
{
	switch(newState)
	{
		case eBet:
		
			setMsg(NULL, RGB(0,0,0));
			
			// We need to hide the "hit and "stay" button
			ShowWindow(buttons[eHit],SW_HIDE);
			ShowWindow(buttons[eStay],SW_HIDE);

			// Show the "Deal", bet increment and bet decrement buttons
			ShowWindow(buttons[eDeal],SW_SHOW);
			ShowWindow(buttons[eIncBet], SW_SHOW);
			ShowWindow(buttons[eDecBet], SW_SHOW);
				break;
				
		case ePlayerTurn:
			
			// Hide the "Deal", bet increment and bet decrement buttons
			ShowWindow(buttons[eDeal],SW_HIDE);
			ShowWindow(buttons[eIncBet], SW_HIDE);
			ShowWindow(buttons[eDecBet], SW_HIDE);

			// Show the "hit" and "stay" buttons
			ShowWindow(buttons[eHit],SW_SHOW);
			ShowWindow(buttons[eStay],SW_SHOW);
				break;
				
		case eDealerTurn:
		
			// Hide all buttons
			ShowWindow(buttons[eDeal],SW_HIDE);
			ShowWindow(buttons[eIncBet], SW_HIDE);
			ShowWindow(buttons[eDecBet], SW_HIDE);
			ShowWindow(buttons[eHit],SW_HIDE);
			ShowWindow(buttons[eStay],SW_HIDE);
			
			data.idleTime = kIdleTime;
				break;
								
	}
	
	data.state = newState; // Set the new state
}

// Set the game message, passing NULL clears out the game message
void CGame::setMsg(const char *text, int color)
{
	if(text == NULL)
		memset(data.msg, 0, sizeof(data.msg));
	else
		strcpy(data.msg, text);
		
	data.msgColor = color;
}

// Returns the point value of a card
int CGame::getCardValue(const CCard *card)
{
	assert(card != NULL);
	
	switch(card->getType())
	{
		case CCard::eAce: return 1; // Assume always 1
		case CCard::eTwo: return 2;
		case CCard::eThree: return 3;
		case CCard::eFour: return 4;
		case CCard::eFive: return 5;
		case CCard::eSix: return 6;
		case CCard::eSeven: return 7;
		case CCard::eEight: return 8;
		case CCard::eNine: return 9;
		
		case CCard::eTen: 
		case CCard::eJack:
		case CCard::eQueen:
		case CCard::eKing:
			return 10;
			
		default:
			assert(!"Invalid card type!");
				return -1;
	}
}

// Returns the value of the specified player's hand
int CGame::getHandValue(EPlayer which)
{
	const CCard **curHand = NULL;

	switch(which)
	{
		case eHuman:
			curHand = data.playerCards;
				break;
				
		case eComputer:
			curHand = data.dealerCards;
				break;				
	}
	
	int value = 0; // The value (point total) of the hand
	bool hasAce = false; // We'll set this true if the hand contains an ACE

	for(int i = 0; i < kMaxCardsPerHand; ++i)
	{
		// If we reach the end of the hand, break 
		if(curHand[i] == NULL)
			break;

		value += getCardValue(curHand[i]);

		if(curHand[i]->getType() == CCard::eAce)
			hasAce = true;
	}

	if(hasAce) 
	{
		// So we know that an ACE can either equal 11 or 1.  We also know the 
		// max value for a hand is 21.  This means that no matter what, the maximum
		// number of ACE's in a hand that can equal 11 is one.
		//
		// Here we'll exchange one ACE of value 1 for one ACE of value 11.  If the
		// total value is still less than or equal to 21, that's what'll we'll return.
		// Otherwise we'll just return the value treating all the ACE's as value 1.
		
		int savedValue = value;

		value--; // Subtract off one ACE
		value += 11; // Add in 11 in it's place

		if(value > kMaxHandValue) // If having one of the ace's equal 11 would
			value = savedValue;	  // cause the hand to bust, set it back to 
								 // value calculated above
	}

	return value;
}

// Returns true if "which" hand is a bust, false otherwise
bool CGame::isHandABust(EPlayer which)
{
	int val = getHandValue(which);
	return (val > kMaxHandValue);
}

// Returns true if "which" hand contains 5 cards, false otherwise
bool CGame::isHandFull(EPlayer which)
{
	const CCard **curHand = NULL;
	int i = 0;

	switch(which)
	{
		case eHuman:
			curHand = data.playerCards;
				break;
				
		case eComputer:
			curHand = data.dealerCards;
				break;				
	}
	
	// Count the number of cards in their hand
	for(; i < kMaxCardsPerHand; ++i)
	{
		if(curHand[i] == NULL)
			break;
	}
	
	// Hand is full if i == kMaxCardsPerHand, otherwise the hand is not full
	return (i == kMaxCardsPerHand);
}

// Returns true if the game is over, false otherwise
bool CGame::isGameOver()
{
	return (data.cash == 0);
}

// Awards the bet to the player
void CGame::awardBet(const char *msg)
{
	data.cash += data.bet;
	data.idleTime = kIdleTime;
	
	setMsg(msg, ARGB(255, 0, 0, 255));
	setState(eBetweenHands);
}

// Deducts the bet from the player
void CGame::deductBet(const char *msg)
{
	data.cash -= data.bet;
	data.idleTime = kIdleTime;
	
	setMsg(msg, ARGB(255, 255, 0, 0));
	setState(eBetweenHands);
}

// Draws the entire game scene	 
void CGame::draw()
{
	drawPlayerHand();
	drawDealerHand();
	drawText(); // Displays all game text
}

// Draws the player's cards to the screen
void CGame::drawPlayerHand()
{
	// Starting upper-left corner of where to draw the cards at
	int xPos = kPlayerX;
	int yPos = kPlayerY;
	
	for(int i = 0; i < kMaxCardsPerHand; ++i)
	{
		// If we come across an "empty" card, we're done drawing
		if(data.playerCards[i] == NULL)
			break;
			
		data.playerCards[i]->draw(win_obj.getBackDC(), xPos, yPos);

		// Move over the appropriate spacing based on if it's the first - fifth card
		xPos += data.playerCards[i]->getWidth() + kCardSpacing;
	}
}

// Draws the dealer's cards to the screen
void CGame::drawDealerHand()
{
	// Starting upper-left corner of where to draw the cards at
	int xPos = kDealerX;
	int yPos = kDealerY;

	int index = 0; // Index into dealer's hand of cards

	for(; index < kMaxCardsPerHand; ++index)
	{
		// Once we hit an "empty" card we're done
		if(data.dealerCards[index] == NULL)
			break;
		
		// Only draw the dealer's first card when we've made it to 
		// the dealer's turn
		if(index == 0 && ((data.state < eDealerTurn) || (isHandABust(eHuman) == true)))
		{
			// Create a RECT for where to draw the card
			RECT rect = { xPos,
						yPos,
						xPos + card_back.getWidth(),
						yPos + card_back.getHeight() }; 

			card_back.blit(win_obj.getBackDC(), rect); // Draw the back of the card
		}
		else
		{
			// Draw the card
			data.dealerCards[index]->draw(win_obj.getBackDC(), xPos, yPos);
		}
		
		// Move over the appropriate spacing based on if it's the first - fifth card
		xPos += data.dealerCards[index]->getWidth() + kCardSpacing;
	}
}

// Draws any active game text to the screen
void CGame::drawText()
{
	char buff[64] = {0};

	// Display the player's cash
	sprintf(buff, "Cash = %d", data.cash);
	TextOut(win_obj.getBackDC(), kCashTextX, kCashTextY, buff, (int)strlen(buff));
	
	// Display the player's bet
	sprintf(buff, "Bet = %d", data.bet);
	TextOut(win_obj.getBackDC(), kBetTextX, kBetTextY, buff, (int)strlen(buff));

	// Draw "Dealer" over the dealer's cards
	sprintf(buff, "Dealer");
	TextOut(win_obj.getBackDC(), 25, 50, buff, (int)strlen(buff));

	// Draw "Player" over the player's cards
	sprintf(buff, "Player");
	TextOut(win_obj.getBackDC(), 25, 225, buff, (int)strlen(buff));
		
	if(data.msg[0] != NULL)
	{
		COLORREF oldTextColor = GetTextColor(win_obj.getBackDC());
		int c = data.msgColor; // Less typing %)
		
		SetTextColor(win_obj.getBackDC(), RGB(GET_R(c), GET_G(c), GET_B(c)));
		TextOut(win_obj.getBackDC(), kMsgTextX, kMsgTextY, data.msg, (int)strlen(data.msg));
		SetTextColor(win_obj.getBackDC(), oldTextColor);
	}	
}

// Clears out the player's and dealer's hands
void CGame::emptyHands()
{
	memset(data.playerCards, 0, sizeof(data.playerCards));
	memset(data.dealerCards, 0, sizeof(data.dealerCards));
}

// Deals "num" cards to "which" player
void CGame::dealCards(EPlayer which, int num)
{
	const CCard **curHand = NULL;

	switch(which)
	{
		case eHuman:
			curHand = data.playerCards;
				break;
				
		case eComputer:
			curHand = data.dealerCards;
				break;				
	}
	
	int index = 0; // Index into the hand of cards
	
	// Determine the next open slot for a card
	while((curHand[index] != NULL) && (index < kMaxCardsPerHand))
		++index;
		
	assert(index != kMaxCardsPerHand);
	assert(index + num <= kMaxCardsPerHand);

	// Add "num" cards to the hand
	for(int i = 0; i < num; ++i)
	{
		curHand[index] = deck.getCard(data.topCard);
		
		data.topCard++; // Move to next card in deck
		index++; // Move to next card in hand
	}
	
	data.idleTime = kIdleTime; // Always wait a few seconds after giving a card
}

