//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Dialog Boxes									 //
//																		 //
//		$Description:	Create a simple window with a dialog box		 //
//																		 //
//***********************************************************************//
#include <iostream>											// Include the standard C++ file
#include <windows.h>										// We need to include windows.h
#include "resource.h"
using namespace std;										// Include the standard namespace															
															
bool gCorrectPassword = false;								// Create a flag that holds if the user tped in the right password or not 
																													
															
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
																												

///////////////////////////////// CREATE MY WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a window, but doesn't have a message loop
/////
///////////////////////////////// CREATE MY WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

HWND CreateMyWindow(LPSTR szWindowName, int width, int height, DWORD dwStyle, HINSTANCE hInstance)
{
	HWND        hwnd;										// HWND - a HWND is a handle to a window.  
    WNDCLASSEX  wndclass;									// WNDCLASSEX - This variable will hold all the information about the window.

    wndclass.cbSize        = sizeof (wndclass);				// Here we set the size of the wndclass. 
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// The style we want is Verticle-Redraw and Horizontal-Redraw
    wndclass.lpfnWndProc   = WndProc;						// Here is where we assign our CALLBACK function.
    wndclass.cbClsExtra    = 0;								// We don't want to allocate any extra bytes for a class (useless for us)
    wndclass.cbWndExtra    = 0;								// Another useless thing for us.  
    wndclass.hInstance     = hInstance;						// We assign our hInstance to our window.  
    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	// We call a function called LoadIcon that returns information about what icon we want.  
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);	// We call a function called LoadCursor that returns information about what cursor we want.
															// Here we set the background color.
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;							// We set the menu ID to NULL because we don't have a menu

    wndclass.lpszClassName = szWindowName;					// Here we set a name for our class, we use the name passed in to the function.
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// We want the icon to be the windows logo.

	RegisterClassEx (&wndclass);							// We need to register our windows class with the windows OS.

	if(!dwStyle) dwStyle = WS_OVERLAPPEDWINDOW;				// If not specified a style, do the default one.	
											
															// *Now we will fill in the data from the function's parameter to create the window.*

    hwnd = CreateWindow (szWindowName,						// window class name - This tells CreateWindow() to use our class up above.
						 szWindowName,			  			// window's Title    - This will be the name on the title bar of the window.
						 dwStyle,							// window style		 - This flag tells windows to create a typical window
						 CW_USEDEFAULT,						// initial x position- The top left corner X value of the window in screen coordinates.  
						 CW_USEDEFAULT,						// initial y position- The top left corner Y value of the window in screen coordinates.  
						 width,								// initial x size	 - The bottom right corner X value of the window in screen coordinates.
						 height,						    // initial y size	 - The bottom right corner Y value of the window in screen coordinates.
						 NULL,								// This is the parent window handle.  Since we don't have a parent window, we set this to NULL
						 NULL,								// This is the window menu handle, but since we don't have a menu, we set this to NULL.
						 hInstance,						    // This is the programs instance handle.  We just pass it our hInstance from our WinMain().  
						 NULL);								// If we wanted to pass in a variable or structure to the "WndProc", we would do it here.

	if(!hwnd) return NULL;									// If CreateWindow() didn't give us a valid window handle, return NULL.

	ShowWindow(hwnd, SW_SHOWDEFAULT);						// Show the window
	UpdateWindow(hwnd);										// Draw the window

	SetFocus(hwnd);											// Sets Keyboard Focus To The Window	
	return hwnd;											// Return the window handle
}

///////////////////////////////// DLG PASSWORD PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Here is our Dialog Procedure that handles the dialog's messages
/////
///////////////////////////////// DLG PASSWORD PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
															
LRESULT CALLBACK DlgPasswordProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char szPassword[25]={0};								// This will be used to hold the text they user types in.

    switch( message )										// Switch on all of the dialogs messages
    {
		case WM_INITDIALOG:									// If Initilizing the dialog box		

			// Do initialization here	(like WM_CREATE)

            return TRUE;

		case WM_COMMAND:									// If we clicked on anything in the dialog box
            
			switch( LOWORD( wParam ) )						// Check the LOWORD of the wParam (Which holds the ID of what was clicked on)
            {    
				case IDOK:									// Check if the OK button was clicked																			                    
															// This gets what the user typed into the password field.
															// It takes the hWnd, the ID of the dialog box control, a string to hold what they typed in,
															// and how many characters you want to retrieve from the field.
					GetDlgItemText(hWnd, IDC_PASSWORD, szPassword, 25);
															
															// Check if they typed in the right password
					if(!strcmp(szPassword, "GameTutorials") || !strcmp(szPassword, "gametutorials"))
					{						
						gCorrectPassword = true;			// Set the global flag to true
						EndDialog( hWnd, FALSE );			// Close the dialog box	
					}
					else 
					{										// Display a message box that tells the user they entered the incorrect password
						MessageBox(hWnd, "Incorrect password! (""GameTutorials"")", "Error!", MB_OK);
					}										// MessageBox takes (the window handle, the string of text, the title, and extra flags - Look in msdn).

					return TRUE;							// Return from the dialog proc

				case IDCANCEL:								// Check if the cancel button was pressed
															// Display a message box saying we clicked cancel.  (MB_OK stands for message box with a OK button)
					MessageBox(hWnd, "You must enter the correct password! (""GameTutorials"")", "Error!", MB_OK);
					EndDialog( hWnd, FALSE );				// Close the dialog box

					return TRUE;							// Quit from this function
            }
            break;

        case WM_CLOSE:										// If we close the dialog box

			EndDialog( hWnd, FALSE );						// Close the dialog box
            
			break;

		case WM_DESTROY:									// This message happens when the dialog box closes          
        
			// If we need to free anything, do it here

			break;											// Break from the loop
    }

    return FALSE;											// Return a default false
}

///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function is a MUST have.  It's the equivelant console function main().
/////
///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
															
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)			
{
	MSG msg;
	HWND hWnd;

	// *** Here we display the dialog box, and then check if we type in the correct password. 
	// *** If they didn't, then quit.  If we return from WinMain() the program is over. Like Main(). 
									
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_PASSWORD), NULL, (DLGPROC)DlgPasswordProc );

	if(!gCorrectPassword) return false;						// Check if we got an incorrect password
															// Create our window using our own function (This adds to resuability)
	hWnd = CreateMyWindow("Dialog Boxes", 800, 600, 0, hInstance);
														
	while (GetMessage (&msg, NULL, 0, 0))					// Here is our main loop.
    {														
		TranslateMessage (&msg);							// "The TranslateMessage() function translates virtual-key messages into character messages."
		DispatchMessage (&msg);								// "The DispatchMessage() function dispatches a message to a window procedure."
    }

	UnregisterClass("Dialog Boxes",hInstance);				// We need to unregister our windows class to make sure we free up memory
    
	return msg.wParam ;										// We return the wParam of the structure "msg".
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Here is the WndProc that handles all the messages															
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
														
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)											// This checks what the message is.  Below is some of the message that windows might return.  There of course is HUNDREDS.
    {

		case WM_DESTROY:									// This message is sent when the user closes the window.
			
			PostQuitMessage(0);								// You must call this function or else you will need to do control-alt-delete and manually close the program from the programs queue.  0 = WM_QUIT.
															// This function actually puts a message on the message queue.  
			break;				
	}													

	return DefWindowProc (hwnd, iMsg, wParam, lParam);		// The DefWindowProc function calls the default window procedure to provide default processing for any window messages that an application does not process. 
															// This function ensures that every message is processed. DefWindowProc is called with the same parameters received by the WndProc. 
}															// End of the WndProc



////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// In order to have a dialog box, we need to have a resource file, which
// we create in visual studio.  We go to the "Insert" menu, and click on "Resource"	.
// We then click on dialog and then OK.  We can make our dialog box there.
// So, in order to use it, we have to call:
// DialogBox( The HINSTANCE, The resource ID, NULL (parent window), The dialog procedure (like WndProc) );
// The second parameter uses a macro called MAKEINTSOURCE() which converts the ID to a string.
// The dialog procedure is a callback function that is called everytime a message happens
// with the dialog box.  SO when I click it, or put my cursor over it, or press a key,
// it calls that function that we give it.  Every dialog box must have one.
// That's pretty much it for a dialog box.  There are tons of other things though... EXPERIMENT!
// 
// We created our own function now that we can use again and again for creating
// a window.  Just cut and past the function in every program here after, then call it.
// Cool huh?!?!?!
//
// *Vocabulary*
//
//	- Field :  A rectangle where you can type text into from the keyboard
//
//	- Control : A control is anything from a button, a field, a picture, scroll bar...
//				Basically, anything that can be in a dialog box.
//
//  - Callback Function : A function that handles message (Ie, the WndProc, and DialogProc)
//
// *Dialog Box Controls*
//
//	We have 2 static text controls.  Static text is text that DOESN'T change, at least not by	
//  the user.  We also have an edit box.  This allows the user to type in text.
//  There is also 2 default buttons, OK and Cancel.  If you double click on the edit box
//  there is an option to have the password hidden with asterixs.
//  Click on the "Password" check box to enable this option.  There are many other option as well.
//
// © 2000-2005 GameTutorials     
