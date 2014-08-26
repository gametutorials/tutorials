#ifndef PRINT_H
#define PRINT_H

#include <iostream>
using namespace std;

// Here is our slightly modified "print.h" from the "PrintDLL" project

// __declspec() "declare special" -- Is a Microsoft specific keyword that allows you
//									 to "specify specific information" about a
//								     variable or function

// __declspec(dllimport) -- By passing "dllimport" into __declspec() we are saying
//							"The function declaration that follows is to be IMPORTED
//							from a .dll file".

__declspec(dllimport) void printMessage();


/* 
   Now the "void printMessage()" is just like a normal function declaration.  The 
   return parameter is void and the function takes no parameters.
*/

#endif
