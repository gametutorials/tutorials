#include "menu.h"

// Display the "save game menu"
void SaveMenu(SPlayer *player)
{
	// Ask if they want to save their game
	cout << "Save game for " << player->name << " at level " << player->level << "?";
	cout << " Yes or No"; // Display the choice of "Yes" or "No" for saving

	/*

	Now remember all we're doing is DISPLAYING THE MENU -- WE ARE NOT IMPLEMENTING IT

	*/

}

// Display the "load game menu"
void LoadMenu(SPlayer *player)
{
	// Ask which game they want to load
	cout << "Load which game:" << endl << endl;

	cout << "1)  Game1" << endl;
	cout << "2)  Final_Level" << endl;
	cout << "3)  SecretPlace" << endl;

	// Display who the current player is
	cout << endl << "Current player is: " << player->name << endl;

	// Prompt them to enter which game they want to load
	cout << endl << "Enter game to load: __";

	/*

	Now remember all we're doing is DISPLAYING THE MENU -- WE ARE NOT IMPLEMENTING IT

	*/

}

// Display the "options menu"
void OptionsMenu(SPlayer *player)
{
	// Display options
	cout << "Select Option:" << endl << endl;

	cout << "1)  Sound" << endl;
	cout << "2)  Music" << endl;
	cout << "3)  Controls" << endl;

	// Prompt them to enter which game they want to load
	cout << endl << "Enter Option: __";

	// NOW NOTICE WE DON'T EVEN USE "player", but when dealing with arrays of function
	// pointers it's necessary to have all of the function's signatures take the
	// same parameters.

	/*

	Now remember all we're doing is DISPLAYING THE MENU -- WE ARE NOT IMPLEMENTING IT

	*/

}
