// Done by TheTutor

/*	
	This tutorial is about "loading files" using "dialog boxes" -- We're don't use a
	"true dialog box" (ie we DO NOT have a DialogProc()) but instead we use a windows API
	to do the dirty work for us :)

	So lets get loaded :)
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include "resource.h"

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

#define BUFF_MAX 256 // The size of the buffers that have to do with opening a file

#define class_name "GameTutorials_OpenFileUseDialog"

void FillOpenParams(OPENFILENAME&, HWND, char*, char*); // This function will fill the 
													   // OPENFILENAME structure which we'll
													  // need to create the "default open
													 // dialog box"

// Loads and blits the .bmp file passed in to the screen
bool BlitImage(char*, HWND);

void ClearWindow(HWND); // This "clears" the window to all white


// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    HMENU hmenu;
	MSG msg;
    WNDCLASSEX wndclassex = {0};

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclassex.lpszClassName = class_name;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

	// Load the menu
	hmenu = LoadMenu(hinstance,MAKEINTRESOURCE(IDR_MENU1));

		// Error check
		if(!hmenu)
			return EXIT_FAILURE; // Something bad happened :(

    RegisterClassEx(&wndclassex);	// Register the window
    
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, // Window will have a "sunken edge"
						  class_name,
						  "GameTutorials -- Opening Files with a Dialog Box",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, // Window will receive a default x pos on screen
						  CW_USEDEFAULT, // Window will receive a default y pos on screen
						  WIN_WIDTH,
						  WIN_HEIGHT,
						  NULL,
						  hmenu,
						  hinstance,
						  NULL);

		// Error check
		if(!hwnd)
			return EXIT_FAILURE; // Something really bad happened!

    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

   
    while(1)
	{
		// Get message(s) if there is one
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Do all the hardcore computational stuff here ;)
		}

	}

	DestroyMenu(hmenu); // Free up menu
	UnregisterClass(class_name,hinstance); // Unregister the WNDCLASSEX
		return (int)msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
	OPENFILENAME open_params = {0}; // This structure is used by the either the
								   // GetOpenFileName() function (for opening files)
								  // or the GetSaveFileName() function (for saving files)
								 // We're going to use it for opening a file -- Info about
								// the file we try to open WILL be stored in this struct

	char filter[BUFF_MAX] = {0}; // This will be filled in with a "filter", explained later :)
	char file_name[BUFF_MAX] = {0}; // This will be used as the "default filename" for the
							        // dialog box
 
	// Depending on the message -- we'll do different stuff
    switch(message)
    {
		case WM_PAINT:
            
			BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
				return 0;

		case WM_COMMAND:

			// If the user selected "Quit" from the menu
			if(LOWORD(wparam) == ID_FILE_QUIT)
				SendMessage(hwnd, WM_CLOSE, 0, 0); // We'll do as they ask and close the program

			// If the user selected "Open" from the menu
			if(LOWORD(wparam) == ID_BMP_FILE_OPEN)
			{
				// Okay we're going to set the "open_params" members we need to but first
				// we have to set up the "filter" member -- This is pointer to a buffer 
				// containing pairs of null-terminated strings. The last string in the buffer
				// must be terminated by two NULL characters.  The first "string" contains a 
				// description for the type of file that will be displayed -- The second 
				// "string" is that files extension(s) -- Multiple extensions get separated
				// by a semi-colon -- What a mouthful :)

				// So this is kinda confusing so we'll look at filter every step of the way
				// Since we are defining filter like this:
				// 
				// char filter[BUF_MAX] = {0};
				//
				// We can be certain that every character in the char array filter will equal
				// NULL when we start out
				
				strcat(filter,"BMP Files"); // Tack on "BMP Files" to filter

				// Now filter looks like this:
				//
				// filter[0] = 'B'
				// filter[1] = 'M'
				// filter[2] = 'P'
				// filter[3] = ' '
				// filter[4] = 'F'
				// filter[5] = 'i'
				// filter[6] = 'l'
				// filter[7] = 'e'
				// filter[8] = 's'
				// filter[9] = NULL // All the remaining characters equal NULL also

				int index = strlen(filter) + 1; // index now equals the first open
												// char one past the first NULL in filter

				// The "second half" of filter is filled with "*.bmp"
				filter[index++] = '*';
				filter[index++] = '.';
				filter[index++] = 'b';
				filter[index++] = 'm';
				filter[index++] = 'p';
				
				// So when it's all said and done filter looks like this:
				//
				// filter[0] = 'B'			filter[10] = '*'
				// filter[1] = 'M'			filter[11] = '.'
				// filter[2] = 'P'			filter[12] = 'b'
				// filter[3] = ' '			filter[13] = 'm'
				// filter[4] = 'F'			filter[14] = 'p'
				// filter[5] = 'i'			filter[15] = NULL
				// filter[6] = 'l'			filter[16] = NULL // All the rest of the 
				// filter[7] = 'e'							  // the characters in filter 
				// filter[8] = 's'							  // equal NULL as well
				// filter[9] = NULL 


				FillOpenParams(open_params,hwnd,filter,file_name);

				// If GetOpenFileName DOES NOT FAIL (it could be returning an error code
				// so it would pass the "if test")
				if(GetOpenFileName(&open_params))
				{
					// Notice how were passing in "open_params.lpstrFile" -- With our call
					// to GetOpenFileName(), this has been filled with the FULL PATH NAME
					// of the file the user selected -- We'll use this to load the file, then
					// blit it (assuming all goes well :)
					if(BlitImage(open_params.lpstrFile,hwnd) == false)
						MessageBox(hwnd,"Failed to blit the image","ERROR",MB_OK | MB_ICONERROR);

				}

			}

			return 0;
				
        case WM_DESTROY:
            
			PostQuitMessage(0);
             return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

void FillOpenParams(OPENFILENAME &open_params, HWND hwnd, char *filter, char *file_name)
{
	/** Initialize the "open_params" fields we need to for opening a .bmp file **/

	open_params.lStructSize = sizeof(OPENFILENAME); // The size of the struct
	open_params.hwndOwner = hwnd; // The "window owner" of this dialog box
	open_params.lpstrFilter = filter;
	
	open_params.lpstrFile = file_name; // This is a pointer to a filename used
								      // to initialize the dialog box -- Since our
								     // "file_name" equals NULL, No "default filename"
							        // will be used
				
	open_params.nMaxFile = BUFF_MAX; // The size of the "file_name" buffer
				
	open_params.lpstrInitialDir = NULL; // Now this already equals NULL, but I'm setting
									   // it again just so I can comment it -- This means
									  // (for Windows NT 5.0/Windows 98 and later)
									 // that if the "current directory" contains any files of 
									// the specified filter type(s) this directory will be the 
								   // initial directory.  Otherwise it will be the "personal
								  // files directory" for whoever's running the program 

	open_params.lpstrFileTitle = NULL; // Again it's already NULL but what this is, is the 
									  // title of the dialog box -- Since we're setting it
									 // to NULL it will use the default title ("Open" in 
									// our case)

	// As you can imagine, there is a plethora of flags that can be set for the dialog box
	// I'll break down what these four mean
	// OFN_FILEMUSTEXIST -- Only allows the typing of existing files in the File Name 
	//						entry field
	// OFN_PATHMUSTEXIST -- Allows the user to ONLY type valid paths and filenames
	// OFN_NOCHANGEDIR -- Changes the current directory back to it's original value
	//					  if the user changes the directory while searching for files
	// OFN_HIDEREADONLY -- This hides the "read only" check box (the ability to open files
	//					   as "read only" is possible)					
	open_params.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR
						| OFN_HIDEREADONLY;

	// Okay we've filled what we wanted to
}

bool BlitImage(char *file_name, HWND hwnd)
{
	ClearWindow(hwnd); // First we'll clear out the window (make it all white)

	// Load the bitmap
	HBITMAP hbitmap = (HBITMAP)LoadImage(NULL,file_name,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

		// Error Check
		if(!hbitmap)
			return false;

	HDC hdc = GetDC(hwnd); // Capture the windows device context

		// Error Check
		if(!hdc)
			return false;

	HDC bmp_hdc = CreateCompatibleDC(hdc); // Make a compatible device context

		// Error Check
		if(!bmp_hdc)
			return false;

	HBITMAP old_bmp = (HBITMAP)SelectObject(bmp_hdc,hbitmap); // Select the bitmap we loaded
															 // into the compatible device
															// context

		// Error Check
		if(!old_bmp)
			return false;

	// Okay if we get here we can blit the pretty bitmap we loaded :)
	BitBlt(hdc,0,0,WIN_WIDTH,WIN_HEIGHT,bmp_hdc,0,0,SRCCOPY);

	/*** Free Memory ***/
	SelectObject(bmp_hdc,old_bmp); // Always have to select back the original HGDIOBJ 
	DeleteObject(hbitmap);

	DeleteDC(bmp_hdc);
	ReleaseDC(hwnd,hdc);

	return true; // Image has been loaded and displayed

} // end of bool BlitImage(char *file_name, HWND hwnd)

// Clears the window background to all white
void ClearWindow(HWND hwnd)
{
	HDC hdc = GetDC(hwnd); // Get the windows device context

	RECT rect = {0}; // A RECT that will define the "client area" of the window

	GetClientRect(hwnd,&rect); // Set "rect" to the client area of the window

	// This will fill the window with all white
	FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

	// Free Memory
	ReleaseDC(hwnd,hdc);

}

/*	Some last minute dialog --

	For our buffer size we used BUFF_MAX.  Windows has their own #define which is
	MAX_PATH -- For any app you make, you probably want to use this for the "file name"
	buffer size.  I didn't use it because I thought this way was a little more clear.

	**CHALLENGE**

	We used the windows API GetOpenFileName() to "put up a dialog box that allowed the
	user to select a file to load" -- Windows also has a similar API, GetSaveFileName()
	that gets a file name for saving files.  See if you can write a small app that allows
	a user to type in a text (just like NotePad) and then have them save their file
	using GetSaveFileName().
*/

//	As always, any questions, comments, rants or raves, post 'em at www.GameTutorials.com	

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
