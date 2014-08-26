// Done by TheTutor

/*
	Well what we're going to do is make a VERY WATERED DOWN version of NotePad --
	The concept we're going to focus on is the "Edit Box" and saving/loading text
	in a window -- We're going to hard code the save/load file as "OurFile.txt"

	So I'll cut the babble short and get going :)

*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "resource.h"

#define classname "GameTutorials_WDVnotepad" // GameTutorials watered down version of NotePad :)

#define EDIT_BOX 0x00EB // Handle to edit box

// Window width and height
#define WIN_WIDTH 640
#define WIN_HEIGHT 480	

// Globals
HWND gEditBox = NULL; // This is a handle to the "edit box" we're going to make

// Standard WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0}; // Our extended window class
	HMENU menu = NULL; // A handle for our menu

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclassex.lpszClassName = classname;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow cursor

	// Load our menu
	menu = LoadMenu(hinstance,MAKEINTRESOURCE(IDR_MENU1));

		// Error Check
		if(!menu)
			return EXIT_FAILURE; // We must have our menu!!!!!!

    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  classname,
						  "EditBoxes",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  WIN_WIDTH,
						  WIN_HEIGHT,
						  NULL,
						  menu, // Make sure we create the window with the menu
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
		{
			DestroyMenu(menu); // Free up the "menu" we made
				return EXIT_FAILURE; 
		}

	// Show and update the window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	// Message Loop
    while(1)
	{
		// If there's a message, deal with it
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			// If this it true -- "We be done"
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Do all the heavy computational stuff 
		}
	}

	DestroyMenu(menu); // Free up the "menu" we made
	UnregisterClass(classname,hinstance); // Unregister the WNDCLASSEX
	    return (int)msg.wParam; // Return code (0 == Program exited properly)
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
   
	// Do stuff depending on the "message"
    switch(message)
    {
        case WM_CREATE:
		{

			RECT rect;

			// First we get the client rect of our window -- We'll use this
			// to create our edit window
			GetClientRect(hwnd,&rect);
         
			// -- Create the edit box -- //

			/* What is going on here???  Okay step by step:
			1) First parameter says that we are creating a predefined type of a window,
			   an edit box.
			2) Second parameter says the "default text" of the edit box (NULL means no default text)
			3) 3rd parameter (WS_CHILD) says this "edit box" will belong to a parent window
			   which means they can share a WinProc() same idea how children share a house
			   with their parents.  At anytime the parents can decide to redecorate and the
			   kids are just going to have to except it, OR ELSE!
			   Meaning of other parameters:
			   WS_VISIBLE -- Window is visible, ES_AUTOHSCROLL -- automatically scrolls to
							 right when user reaches end of a line
			   ES_AUTOVSCROLL -- automatically scrolls text up one page when user presses
								 "return" at end of a page
			   ES_LEFT -- left aligns text, ES_MULTILINE -- Allows for multiple lines
			   ES_NOHIDESEL -- negates the "default behavior" of an edit box (won't hide it 
							   when it's no in focus)
			   ES_WANTRETURN -- when user presses "enter", it will function as a "carriage return"
			   WS_VSCROLL - vertical scroll bar, WSHSCROLL -- horizontal scroll bar
			   WS_BORDER -- edit box will have a thin border
			4) 0, 0 -- The placement of the upper left corner of the edit box, relative to 
					   the parent window
			5) rect.right -- The width of the edit box
			6) rect.bottom -- The height of the edit box
			6) hwnd -- Handle to the parent window
			7) (HMENU)EDIT_BOX -- A way to specify (just a number) our edit box
			8) Got no clue... No just kidding -- First you should look at the CreateWindow()
			   in WinMain(),  see how we passed in hinstance there.  Well we need to pass that
			   in again here.  But if you look at the WinProc() you'll see that we're not
			   passing in that variable, so we DON'T have access to it.  Well this is only
			   partially true.  If you just tried to pass in "hinstance" it would not compile.
			   But we do have the "hinstance", it's hidden within the "lparam" -- By typecasting
			   the "lparam" to a LPCREATESTRUCT we can extract the "hinstance"
			9) NULL -- Don't care.  This parameter is used to pass "extra information" but we 
					   don't have any to pass
			*/
			gEditBox = CreateWindow("EDIT",NULL,WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL |
									ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL |
									WS_VSCROLL | WS_HSCROLL | WS_BORDER | ES_WANTRETURN,
									0, 0, rect.right, rect.bottom, hwnd, (HMENU)EDIT_BOX,
									((LPCREATESTRUCT)lparam)->hInstance, NULL);

			return 0;
		}

       case WM_COMMAND:
	   {
		
		   // If the clicked the "Load menu button"
		   if(LOWORD(wparam) == LoadFile)
		   {
			   FILE *file = NULL;
			   fpos_t length = 0; // Used to record the length of the file
			   char *buff = NULL; // A character buffer, used to hold ALL the text loaded in

			   file = fopen("OurFile.txt","rb");

					// Error check
					if(!file)
					{
						MessageBox(hwnd,"Couldn't load OurFile.txt","Error",MB_OK | MB_ICONERROR);
						PostQuitMessage(0);
							return 0;
					}

				// Seek to the end of the file
				fseek(file,0,SEEK_END);

				// This will get the file pointer position -- We'll use this to allocate
				// enough space to load all of the file
				fgetpos(file,&length);

				buff = new char[(int)length + 1]; // Allocate memory
												 // The "+ 1" is for the NULL terminator
												// we want to make sure buff ends with NULL

					// Error check
					if(!buff)
					{
						MessageBox(hwnd,"Couldn't load OurFile.txt","Error",MB_OK | MB_ICONERROR);
						PostQuitMessage(0);
							return 0;
					}

				// Move the file pointer back to the beginning of the file
				fseek(file,0,SEEK_SET);

				// Read in the ENTIRE file, into buff
				fread(buff,(int)length,1,file);
				
				buff[(int)length] = NULL; // Be sure to end buff with a NULL terminator
										  // (Remember we added "+ 1" to buff's length
										  // so "length" is the last index into our buff array)

				// Close the file pointer -- We're done with it :)
				fclose(file);

				// This is where all the magic happens -- This function will fill 
				// the edit box with all the text we just read in
				SetWindowText(gEditBox,buff);

				delete[] buff; // Free up "buff"
					return 0;
		   
		   } // end of if(LOWORD(wparam) == LoadFile)

		   // Otherwise if we are saving a file...
		   else if(LOWORD(wparam) == SaveFile)
		   {
			   // Create a char-string for the directory path to write the file out to
			   char dir_path[256] = {0};
			   char *buff = NULL; // A character buffer, used to hold ALL the text typed in the edit box
			
			   // This will get the current directory the .exe is running in
			   GetCurrentDirectory(256,dir_path);

			   // Append the file name -- Remember we need two "\\" to represent one "\"
			   strcat(dir_path,"\\OurFile.txt");

			   // Get the "length" (count of characters) in the "gEditBox"
			   int length = GetWindowTextLength(gEditBox);

			   length++; // This accounts for a NULL terminator character
			
			   // Make a buffer big enough to hold all the text
			   buff = new char[length];

					// Error check -- If we couldn't allocate the memory
					if(!buff)
					{
						MessageBox(hwnd,"Couldn't load OurFile.txt","Error",MB_OK | MB_ICONERROR);
						PostQuitMessage(0);
							return 0;
					}
			   
				// Fill buff with the text in the "edit box"
				GetWindowText(gEditBox, buff, length);
				
				buff[length - 1] = NULL; // Be sure that "buff" is NULL terminated
				
				// Open up the file for writing
				FILE *file_pointer = fopen(dir_path, "wb");
				
				// Write the contents of the "edit box" to the file
				fwrite(buff, length, 1, file_pointer);
				
				fclose(file_pointer); // Be sure and close the file
				
				delete[] buff; // Free up "buff"
					return 0;
		   }

		   return 0;

	   } // end of case WM_COMMAND:
			
		case WM_DESTROY:
            
			PostQuitMessage(0);
				return 0;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

/*  Extra, Extra -- Read all about it

  Now of course if you were making a "robust" application, you WOULD NOT hard-code the 
  name of the .txt file -- You would give the user the ability to name it when saving AND
  the ability to load different text files

  But this wasn't the point of this tutorial :)

  Maybe in the future I'll take this tutorial "to the next level" :)

  If you have questions post 'em on the board at www.GameTutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
