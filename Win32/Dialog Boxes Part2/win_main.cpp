// Done by TheTutor

/*	

  This tutorial is all about "Dialog Boxes".  So before we get too much further it's 
  probably a good idea to define what exactly we mean by a "dialog box".  We look
  to M$ for our definition... "A temporary window that contains controls. You can use
  it to display status information and to get user input."  Well there you have it
  a dialog box is basically a "mini-window" for displaying info and/or getting info.

  So because of this general functionality you can imagine that there is literally
  hundreds of different style dialog boxes you can create.  Not to mention more
  than a few ways to make one in code.  What this tutorial is going to do
  is use the function CreateDialog() (hey wonder what this does :) to create a
  dialog box.  Using the DialogProc() (implementation of the dialog box) we'll create
  a SIMPLE dialog box that has a title bar, 3 check boxes, and one button.  When 
  you press the button a message box (which coincidentally is also a dialog box) will appear
  with a sentence that says which check boxes are checked.
  
  Following this introduction is a list of steps taken to create the dialog box using
  the .NET (VC++7.0) resource editor.  If you haven't used the editor yet, you really should.  It
  makes creating menus, icons, dialog boxes, etc EXTREMELY EASY.  I couldn't imagine
  using a old style template (filling in a big old structure with a whole bunch of parameters).
  Makes my typing fingers cramp-up just thinking about it.

  And with that lets end this dialogue and roll.
  
*/

/************ List of steps taken to create our dialog box

  1)  Select "Project", then "Add Resource..."
  2)  Click the plus sign (+) to the left of "Dialog" in the list of creatable resources
  3)  Select IDD_DIALOGBAR out of the list of creatable dialogs
  4)  Click New

	   **NOTE** If you CAN NOT see the "Dialog Editor" (menu bar with additional options
				when creating a dialog) Right-Click on the menu bar.  You should see a picklist
				of items either checked or non-checked (example items:  Build MiniBar, ATL, etc).
				Make sure "Dialog Editor" is checked.

  // Building the Dialog Box

  1)  Select "Check Box" from the dialog tools (also named Dialog Editor) 
	  under the Toolbox and place in the dialog box
  2)  Redo step #5 two more times
  3)  Selected the "Button" from the dialog tools and place it
	  in the dialog box
  4)  Right click on the button, then select "Properties"
  5)  Under the "Caption" tab, change button name to desired name
  8)  Rick click on the dialog box, then select "Properties"
  7)  Change the following properties:  "Caption" to "Dialog Box",
	  "Border" to Thin, "ID" renamed to "DLG_BOX", "Title Bar" set to true,
	  "System Menu" set to true, "Visible" set to true, "Tool Window" set
	  to true, and "Client Edge" set to true

************/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include "resource.h" // This is the header file VC++ .NET generously created for us
					 // containing all the #define's for our dialog box

// Width and height of window
#define WIN_WIDTH 640
#define WIN_HEIGHT 480

// String "name" of our window class
#define CLASS_NAME "GameTutorials_Dialog2"

typedef unsigned char uchar;

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// This is our dialog's callback.  If you look closely you'll notice it takes 
// the EXACT same parameters as the windows callback.  This is for good reason.
// Just like the windows callback (WinProc) this function (procedure) gets called
// whenever our dialog box receives "messages".  Thus for instance if somebody clicks
// a button in our dialog box, our DialogProc() will get "messaged".  So what
// does that mean in plain English?  Well basically the guts (implementation) of our
// dialog box will go inside of this function.
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HWND hwnd; // This will hold the handle to our window
	HWND dlgHwnd; // This will hold the handle to our dialog box
    MSG msg; // This will hold any messages (such as a mouse click) that
			 // our window might receive

    WNDCLASSEX wndclassex = {0}; // This is our "window class" -- The "EX" stands
								// for "extended style" which gives us more options
							   // when creating our window (although we're going to
							  // completely ignore 'em)

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always must be set
    wndclassex.style = CS_HREDRAW | CS_VREDRAW; // Window classes style
    wndclassex.lpfnWndProc = WinProc; // Pointer to where the WinProc() is defined
    wndclassex.hInstance = hinstance; // The handle to the instance of our window
	wndclassex.lpszClassName = CLASS_NAME; // The name of our window class
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // This sets
																   // the background color
																  // of the window to white
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // This loads the default
															 // window arrow cursor    

    RegisterClassEx(&wndclassex); // Register the window class so calls to CreateWindow()
								 // and CreateWindowEx() will know what window class to
								// use when creating the window
	
    hwnd = CreateWindowEx(NULL, // Extra window attributes, NULL means none
						  CLASS_NAME, // Name we gave our WNDCLASSEX			
						  "www.GameTutorials.com -- Dialog Box2", // Text for the title bar
						  WS_OVERLAPPEDWINDOW, // Style of window (see MSDN for full description)
						  CW_USEDEFAULT, // Upper left xPos of window (Windows picks default)
						  CW_USEDEFAULT, // Upper left yPos of window (Windows picks default)
						  WIN_WIDTH, // Width of window in pixels
						  WIN_HEIGHT, // Height of window in pixels
						  NULL, // Handle to "parent window" (we have none, this is the parent)
						  NULL, // Handle to a menu (we have none)
						  hinstance, // Handle to instance of this window (passed in by WinMain())
						  NULL); // "Extra info" to pass to the WinProc (we have none)

		// Error check
		if(!hwnd)
			return EXIT_FAILURE; // Something really bad happened!

	// Okay here we are creating our dialog box
	// By parameter:
	// hinstance -- This is the handle to the "instance" of our app.  This is the
	//				same parameter that gets passed into our CreateWindowEx() call
	// MAKEINTRESOURCE(DLG_BOX) -- Holy smokes is this bad boy long!  First it's important
	//							   to point out that MAKEINTRESOURCE() is a Win32 macro
	//							   that converts an integer value to a "resource type" that
	//							   works with Win32's resource manager.  Yeah whatever that
	//							   means %)  Basically for this parameter, you pass the 
	//							   ID of your dialog box (DLG_BOX in our case) wrapped
	//							   in this macro.  This identifies the dialog box to Windows
	// hwnd -- Parent (owner) window handle.
	// (DLGPROC)DialogProc --  A pointer to the function (procedure) that handles all
	//						   the messages for the dialog box.
	dlgHwnd = CreateDialog(hinstance,MAKEINTRESOURCE(DLG_BOX),hwnd,(DLGPROC)DialogProc);

		// Error Check
		if(!dlgHwnd)
			return EXIT_FAILURE; // Something really bad happened!

    ShowWindow(hwnd, ishow); // Show the window (make it visible)
    UpdateWindow(hwnd); // Updates the window (paint the window)

	// Loop until the app is done
    while(1)
	{
		// Get message(s) (for instance a key press) if there is any
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Do all the hardcore computational stuff here :)
		}

	}

	DestroyWindow(dlgHwnd); // Destroys (frees) up our dialog box
	UnregisterClass(CLASS_NAME,hinstance); // Free up the WNDCLASSEX

		return msg.wParam; // And we're out
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch(message)
    {
        case WM_DESTROY:
            
			PostQuitMessage(0);
             return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// This is where all the dialog magic happens.  Just like the WinProc() we use
// this function to handle all the dialog box messages.  Really the only
// difference is this returns a BOOL instead of a LRESULT.  So how do you what 
// to return form the DialogProc()?  Well it's pretty simple.  If you process
// a message you return TRUE, otherwise you return FALSE.
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// These are three static variables that will hold the state of our dialog's 
	// check boxes.  When we start we know they are all unchecked
	static uchar check1 = 0; // 0 == unchecked, 1 == checked
	static uchar check2 = 0;
	static uchar check3 = 0;

	// Depending on the message will do different stuff
    switch(message)									
    {
		case WM_INITDIALOG: // This message gets sent on creation (initialization) of the
						   // dialog box.  So if you wanted to set your dialog box to a 
						  // default state, you would do that here.

			return TRUE; // Return true 'cause we processed this baby (didn't do squat, but
						// processed it nevertheless)

			/* 			
				**NOTE** Special rule for the WM_INITDIALOG message.  If you call the 
				function SetFocus() inside the WM_INITDIALOG message, you should return
				FALSE.  Otherwise return TRUE like normal.
			*/

		// The WM_COMMAND message is sent when somebody selects a command item from a menu,
		// when a control sends a notification message to it's parent, or when an accelerator
		// keystroke is translated.  So basically when anybody clicks/types in our dialog box
		// we'll get the WM_COMMAND message
		case WM_COMMAND:									

			switch(LOWORD(wparam))						
            {    
				// If somebody has "clicked" check box #1
				case IDC_CHECK1:

					check1 ^= 1; // Toggle the checked flag
						break;

				// If somebody has "clicked" check box #2
				case IDC_CHECK2:

					check2 ^= 1; // Toggle the checked flag
						break;

				// If somebody has "clicked" check box #3
				case IDC_CHECK3:

					check3 ^= 1; // Toggle the checked flag
						break;

				// If somebody has pushed the "Apply" button
				case IDC_BUTTON1:
				{
					char buff[256] = {0};

					// If "Check 1" is currently checked, add it to our string
					// for displaying
					if(check1)
						strcat(buff,"Check1 is ON!  ");

					// If "Check 2" is currently checked, add it to our string
					// for displaying
					if(check2)
						strcat(buff,"Check2 is ON!  ");

					// If "Check 3" is currently checked, add it to our string
					// for displaying
					if(check3)
						strcat(buff,"Check3 is ON!  ");

					// Display a message box that displays a string of all the 
					// check boxes that are "checked".  If none are checked then
					// a big blank message box will be displayed.
					MessageBox(hwnd,buff,"Check This Out",MB_OK);
						break;
				}
				
            }

			return TRUE; // We processed, so we return true

        case WM_CLOSE:										
		case WM_DESTROY:

			EndDialog(hwnd,0); // EndDialog() shuts down a dialog boxes' DialogProc()
							  // The second parameter is the "return code" to send
							 // saying if the process was successful or not.  
			
			return TRUE; // We processed, so we return true

	}

	return FALSE; // When you DO NOT process a message in a DialogProc(), you return "FALSE" 
}

// Final Dialogue

// As eluded to in the tutorial, there's more than a few ways to make a dialog box.  Here's 
// a few more API's you can look up that create dialog boxes in one form or fashion:
// CreateDialogIndirect(), DialogBox(), DialogBoxIndirect(), and MessageBoxIndirect()

/*** CHALLENGE ***

  Make a dialog box that excepts an RGB color and has an "Apply" button.  When the user
  clicks "Apply", display "Hello World" in the color they've specified to the window.

*****************/

// If you have any issues you would like resolved dealing with dialog boxes, be sure
// point your browser to www.GameTutorials.com and post away

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
