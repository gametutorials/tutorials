#ifndef GAMEDEFINES_H
#define GAMEDEFINES_H

// Commit this in to see FPS
#define DRAW_FPS

  // ~~~~~~~~~~~~~~~~~~~~~~~ //
 // Screen Position Defines //
// ~~~~~~~~~~~~~~~~~~~~~~~ //

// Starting upper-left (x,y) to draw player cards
const int kPlayerX = 25;
const int kPlayerY = 250;

// Starting upper-left (x,y) to draw dealer cards
const int kDealerX = 25;
const int kDealerY = 75;

// Amount of space in pixels that should be between each card drawn
const int kCardSpacing = 10;

// Starting upper-left (x,y) to draw the misc. messages (ie who won the hand)
const int kMsgTextX = 500;
const int kMsgTextY = 125;

// Starting upper-left (x,y) to draw player's current cash text
const int kCashTextX = 500;
const int kCashTextY = 150;

// Starting upper-left (x,y) to draw player's current bet text
const int kBetTextX = 500;
const int kBetTextY = 175;

// Starting upper-left (x,y) to draw "Dealer" above dealer's cards
const int kDealerTextX = 25;
const int kDealerTextY = 50;

// Starting upper-left (x,y) to draw "Player" above player's cards
const int kPlayerTextX = 25;
const int kPlayerTextY = 225;

  // ~~~~~~~~~~~~~~~~~ //
 // Dimension Defines //
// ~~~~~~~~~~~~~~~~~ //

// Width/Height of "Hit", "Deal", and "Stay" buttons
const int kButtonWid = 75;
const int kButtonHgt = 40;

// Width/Height of bet increment and decrement buttons 
const int kIncDecButtonsWid = 45;
const int kIncDecButtonsHgt = 30;

  // ~~~~~~~~~~~~~~~~~ //
 // Misc Game Defines //
// ~~~~~~~~~~~~~~~~~ //

const int kMsgTextLen = 256; // Length of in game messages
const int kMaxCardsPerHand = 5; // Maximum number of cards per hand of Blackjack
const int kMaxHandValue = 21; // Maximum value for any hand of Blackjack
const int kMinBet = 100; // Minimum bet amount per hand of Blackjack
const int kStartingCash = 5000; // Starting amount of money for the player
const float kIdleTime = 1.5f; // Whenever in an idle state, do so for kIdleTime seconds 

#endif
