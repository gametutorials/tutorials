#ifndef INPUT_H
#define INPUT_H

#include <windows.h>

// This is the function where all the magic happens.  This function will take any LETTER
// (that's a - z and A - Z) and return true if it is being typed, false otherwise
bool KeyPressed(char key);

#endif
