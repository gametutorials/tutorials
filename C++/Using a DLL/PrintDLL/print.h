#ifndef PRINT_H
#define PRINT_H

#include <iostream>
using namespace std;

// Here we create our function prototype:

// __declspec() "declare special" -- Is a Microsoft specific keyword that allows you
//									 to "specify specific information" about a
//								     variable or function.

// __declspec(dllexport) -- By passing "dllexport" into __declspec() we're saying
//							"The function declaration that follows is to be exported
//							into a .dll file".

__declspec(dllexport) void printMessage();


/* 
   Now the "void printMessage()" is just like a normal function declaration.  The 
   return parameter is void and the function takes no parameters.
*/

#endif
