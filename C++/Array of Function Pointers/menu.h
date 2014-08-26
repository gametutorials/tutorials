#ifndef MENU_H
#define MENU_H

#include <string>		// So we can use "string"
#include <iostream>		// So we can use "cout"
using namespace std;

/* Here we're creating a "set of functions" that display various "menus" in our
   imaginary game.  Now it is important to note that these functions take the SAME
   parameters AND have the SAME return value (void).
*/

// This is our player struct -- Not much there eh?
struct SPlayer
{
	string name;
	int level;
};

// This displays the "save game menu"
void SaveMenu(SPlayer *player);

// This displays the "load game menu"
void LoadMenu(SPlayer *player);

// This displays the "options menu"
void OptionsMenu(SPlayer *player);

#endif
