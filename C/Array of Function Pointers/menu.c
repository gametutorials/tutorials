#include "menu.h"

// Display the "save game menu"
void SaveMenu(SPlayer *player)
{
	// Ask if they want to save their game
	printf("Save game for %s at level %d?",player->name,player->level);
	printf(" Yes or No");

	/*

	Now remember all we're doing is DISPLAYING THE MENU -- WE ARE NOT IMPLEMENTING IT

	*/

}

// Display the "load game menu"
void LoadMenu(SPlayer *player)
{
	// Ask which game they want to load
	printf("Load which game:\n\n");

	printf("1) Game#1\n");
	printf("2) Game#2\n");
	printf("3) Game#3\n");

	// Display who the current player is
	printf("\nCurrent player is: %s\n",player->name);

	// Prompt them to enter which game they want to load
	printf("\nEnter game to load: __");

	/*

	Now remember all we're doing is DISPLAYING THE MENU -- WE ARE NOT IMPLEMENTING IT

	*/

}

// Display the "options menu"
void OptionsMenu(SPlayer *player)
{
	// Display options
	printf("Select Option:\n\n");

	printf("1) Sound\n");
	printf("2) Music\n");
	printf("3) Controls\n");

	// Prompt them to enter which game they want to load
	printf("\nEnter Option: __");

	// NOW NOTICE WE DON'T EVEN USE "player", but when dealing with arrays of function
	// pointers it's necessary to have all of the functions signatures take the
	// same parameters.

	/*

	Now remember all we're doing is DISPLAYING THE MENU -- WE ARE NOT IMPLEMENTING IT

	*/

}
