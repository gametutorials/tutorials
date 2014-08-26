#ifndef MENU_H
#define MENU_H

#include <stdio.h> // So we can use printf()

/* Here we're creating a "set of functions" that display various "menus" in our
   imaginary game.  Now it is important to note that these functions take the SAME
   parameters AND have the SAME return value (void).
*/

#define MAX_NAME_LEN 64

// This is our player struct.  Not much there eh?
typedef struct _SPlayer
{
	char name[MAX_NAME_LEN];
	int level;

} SPlayer;

// This displays the "save game menu"
void SaveMenu(SPlayer *player);

// This displays the "load game menu"
void LoadMenu(SPlayer *player);

// This displays the "options menu"
void OptionsMenu(SPlayer *player);


#endif
