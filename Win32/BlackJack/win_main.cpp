// Done by TheTutor

/*
	In this tutorial, we are going to create the game Blackjack using good ole' Win32
	programming.  As always when making any game, it's very important to have as clear
	as vision as possible of what you are attempting to make.  So lets first go over the
	rules of our version of Blackjack.

	***BLACKJACK RULES***
		
		The Player will begin with $5000.  They'll be able to place bets in $100 increments.
		A minimum bet is $100 and a maximum bet is the amount of cash the player has.
		When the player has no more money OR presses the ESC key, the game will end.

		The goal of the game is to get a hand worth 21 points.  If you go over 21 points
		you automatically lose.  For each hand, the player is given two cards face up and the
		dealer is given one card face up and one card face down.  At this time the player
		will have to determine if they want to "hit" (ie receive another card, up to five total)
		or stay.  Once the player stays, the dealer flips over their card and tries to 
		tally more points in their hand then the player.

		Whoever has more points at the end of a hand wins.  If there is a push (ie if 
		both the dealer and the player score the same hand) then no money is awarded and
		the current bet is disregarded.

		Point totals are tallied by adding all of your cards together in your hand.
		Below is the point value for each card:

		2 - 10 == Numeric cards equal the value of the card (ie 3 Clubs = 3 points)
		J, Q, K == 10 points
		Ace == 1 or 11 points -- Depending on the surrounding cards, an Ace either equals
								 1 point or 11 points.  It will always equal the higher of
								 the two, provided this doesn't cause the player to "bust" (ie
								 go over 21 total points)

  ***END OF RULES***

  Well there you have it, the finest set of Blackjack rules known to man :)  If you are still
  unsure of how to play, a quick search on the Internet should help you out.

  Now it's time to consider some of the technical hurdles we'll have to overcome to make this 
  game.

	1)  We'll need a way to manage cards (load them, shuffle them, deal them, display them, etc)
	2)  We'll need some sort of button or mechanism to allow the player to hit, stay and bet.
	3)  Last but not least we'll need a snazzy interface so it's easy for the player
		to tell what they are doing.

  Other than that, it should mostly be game logic.  So without further ado, lets get going.

*/

#include "game.h"
#pragma comment(lib, "GT_WINOBJ2D.lib")

// Lesson #1 -- Using preexisting "engines".  Here we are including a GT made "2D Engine" that
// handles:  general window functionality, keyboard/mouse input, clock functions, double buffering, 
// image loading, and image displaying .  You should not reinvent the wheel every time
// you want to make a game.  Tons of things that are needed by games can be made generic
// and reused multiple times.  This gives the programmers the chance to focus on the
// actual GAME code.  The gaming industry has increased it's use of preexisting	engines quite
// a bit over the last few years.  A game like Grand Theft Auto 3 a former game of the year,
// is an example of a title that used a preexisting "3D Engine" and focused on the game play
// aspect of the game instead of how they were going to draw a triangle to the screen.
// More and more development houses are separating engines and game development into
// two separate entities.  So the moral of the story is, we've wrapped all most of the engine
// code into a handy dandy library we will use to make our game.

#define WIN_WID 640
#define WIN_HGT 480

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	CGame theGame; // Our game object that will handle everything
	HWND hwnd;
    WNDCLASSEX wndclassex = {0};
	char class_name[] = "GameTutorials_BlackJack";
														
    wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 
											0, 0, LR_SHARED);

    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  class_name,
						  "www.GameTutorials.com -- Blackjack",
						  WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN | WS_VISIBLE,
						  CW_USEDEFAULT, 
						  CW_USEDEFAULT, 
						  WIN_WID,
						  WIN_HGT,
						  NULL,
						  NULL,
						  hinstance,
						  &theGame);

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE;
		
	srand(GetTickCount()); // Seed random timer
	
	// Init the game
	if(!theGame.init(hwnd, hinstance))
	{
		MessageBox(NULL, "Couldn't load game!", "FATAL ERROR", MB_OK | MB_ICONERROR);
			return EXIT_FAILURE;
	}

	theGame.run(); // Run the game until we quit

	UnregisterClass(class_name,hinstance); // Unregister the WNDCLASSEX
		return 0;	

/*
	Doesn't this WinMain() look a bit different?  Have we lost our bloody mind?  Nope not
	at all.  It is extremely common to create games with these three basic steps:

		Initialization -- This is where you load all the levels, models, bitmaps/textures,
						  sounds, and any other resource you need to run the game

		Game Loop -- This is the loop that keeps on running while you're in the game.
					 It handles all game related activities and won't stop until the 
					 player quits the game.

		De-Initialization -- When the player quits the game it's time to free up all the memory
							 that was allocated and return the computer to it's original
							 settings.

	For any game you make you should follow this simple three step pattern.  You don't 
	have to do it exactly as it's presented in this tutorial, but the overall flow should 
	follow the init, run, de-init pattern.
*/

}

// We use the WinProc to check when a button has button been pushed 
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static CGame *gamePtr = NULL;

	switch(message)
	{
		case WM_CREATE:
		{
			// Get a pointer to the CGame object to use in the WinProc
			gamePtr = (CGame*)(((LPCREATESTRUCT)lparam)->lpCreateParams);
			
			if(gamePtr == NULL)
				return -1; // An error has occurred
			else
				return 0;
		}
		
		case WM_COMMAND:

			// If a button was clicked
			if(HIWORD(wparam) == BN_CLICKED)
			{
				gamePtr->handleButtons(LOWORD(wparam));
			} 
		
			return 0;
	
		case WM_DESTROY:			
	
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// You think this game is over?  Don't bet on it...

// Lesson #2 -- Making the Magic Numbers

/*
	If you take a few minutes to look through the code, you'll find some "magic numbers"
	all over the place.  As an example lets look at these lines of code:

	MoveWindow(buttons[eHit],500,300,kButtonWid,kButtonHgt,true);
	MoveWindow(buttons[eStay],500,350,kButtonWid,kButtonHgt,true);

	Incase you aren't 100% sure of what the signature of the MoveWindow() API look like,
	here it is:

		MoveWindow( HWND hwndToWindow, int upperLeftX, int upperLeftY,
					int widthOfWindow, int heightOfWindow, bool repaintWindow );

	So anyway... Where did we get those (upperLeftX,upperLeftY) to move the windows to?  Well the
	answer is actually pretty simple:  Guessing.  That's right those number are totally a
	result of empirical analysis.  So you may be thinking... "Man that's gonna take forever
	to compile code, switch a number, see if I like it... Recompile the code, change a number,
	see if I like it..."

	Well there's tricks to greatly speed up this otherwise tedious process.  One way to
	greatly speed up your "magic number" time is to initially make them static.  If 
	the variables are static, any decent compiler will allow you to change them at run time.
	This way you can get IMMEDIATE feed back as to what "magic numbers" you like.  Once you
	find it, just go back into the code and change it (you can also make the number not static
	then)

	If you've never done the above, I recommend trying it with this game.  Try rearranging
	the interface.  The more comfortable you are with a debugger, the better coder you'll
	be in the long run.
*/

// This is obviously the BARE BONE version of Blackjack.  If you are new to making 
// games and/or Win32 programming, an excellent exercise would be sprucing this game up.
// See if you can add multi-player same keyboard support.
// Questions, comments, suggestions ???  Let us know at www.GameTutorials.com

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/

  

			
