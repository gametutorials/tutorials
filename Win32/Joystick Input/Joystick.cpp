//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Joystick										 //
//																		 //
//		$Description:	This shows how to use joysticks in your game	 //
//																		 //
//***********************************************************************//

// * IMPORTANT NOTE * 
//
// BE SURE TO INCLUDE winmm.lib IN ORDER TO USE THE JOYSTICK API
#pragma comment(lib,"winmm.lib")

#include <windows.h>										// We need to include windows.h
#include <stdio.h>											// Include stdio.h for the basics

#define CURSOR_SPEED 20										// This will be the speed the cursor moves with the joystick control

RECT windowRect = {100, 100, 600, 500};						// Let's create a rectangle that defines our desired window position and size.

///////////////////////////////// INIT JOYSTICK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This initializes the joystick and returns a failure or a success
/////
///////////////////////////////// INIT JOYSTICK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

BOOL InitJoystick(HWND hWnd)
{
	DWORD dwResult = 0;

	// This function below return the number of joysticks the driver supports.
	// If it returns 0 then there is no joystick driver installed.

	if (!joyGetNumDevs())
    {
		// Display the error message there there is no driver and return FALSE (Unsuccessful)
		// The MB_OK or'd with the MB_ICONEXCLAMATION puts a OK button with a ! icon in the message box.
        MessageBox(NULL, "There are no joystick devices installed.", "Error", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

	// This function below initializes a joystick.  We have the option of JOYSTICKID1 or JOYSTICKID2.
	// Since most people only have 1 joystick, we just use JOYSTICKID1.  The first parameter is
	// is the handle to the window that will be receiving the joystick messages.  We will need
	// to check for these messages down below in our WndProc().  The second parameter is the
	// joystick we want to use, we want the primary joystick.  The next parameter is the time
	// in milliseconds that we want to send messages about the joystick.  We want to update the
	// joystick's condition as fast as possible so we give it 0, which does NOT give a delay.
	// The last parameter allows us to only send a message when the joystick's position changes.
	// Like the last parameter, we want it to ALWAYS update so we say FALSE. 

	// You will notice that we get the return value.  We will use this to handle any errors.

	dwResult = joySetCapture(hWnd, JOYSTICKID1, 0, FALSE);

	// Let's check what the return value was from joySetCapture()
	// - If the joystick is unplugged, say so.
	// - If there is no driver installed, say so
	// - If we can access the joystick for some strange reason, say so
	// Otherwise, let's return a SUCCESS!
	// You don't need to do this, but it helps the user solve the problem if there is one.
	// Error checking is VERY important, and employers want to see you use it.

	switch (dwResult) 
	{
		case JOYERR_UNPLUGGED:								// The joystick is unplugged
			MessageBox(hWnd, "Please plug in the joystick first.", NULL, MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return failure
		case MMSYSERR_NODRIVER:								// There is no driver for a joystick
			MessageBox(hWnd, "No valid joystick driver.", NULL, MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return failure
		case JOYERR_NOCANDO:								// Unknown error, try restarting
			MessageBox(hWnd, "Couldn't capture joystick input, try restarting.", NULL, MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// return failure
	}

	return TRUE;											// It worked!!  return SUCCESS!
}


///////////////////////////////// MOVE CURSOR WITH JOYSTICK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This moves the cursor depending on the movement of the joystick
/////
///////////////////////////////// MOVE CURSOR WITH JOYSTICK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void MoveCursorWithJoystick(LPARAM lParam)
{
	POINT cursorPos;										// This will hold the cursor position
	int x = 0, y = 0;										// These will hold the joystick coordinates

	// Here we store the current cursor position in our variable.
	// Since it changes the variable, we have to pass in the address.

	GetCursorPos(&cursorPos);								// This gets the current cursor position

	// Here comes the weird part.  Joystick coordinates range from 0 to 65535, with the
	// origin being the uper left corner.  So it works just like the window, where (0,0)
	// is the top left corner of the joystick coordinate system.  Instead of dealing
	// with really high numbers, we can bit shift the X and Y position down to a range of
	// 0 to 15.  In this case, we would then bit shift to the right to reduce the number.
	// Basically, for every bit shift to the right it adds a zero in front of the number,
	// and takes the very right most bit, whether it be 1 or zero.  Bit shifting to the right
	// once divides the number in half.  Likewise, bit shifting the number to the left once
	// doubles it.  Below, are dividing the number by 2 essentially 12 times.  Hopefully there
	// should be a graphical binary tutorial up on the site www.GameTutorials.com by now.

	// There is a windows macro called LOWORD() and HIWORD() which grabs the bottom bits
	// and top bits of 32-bit data type, which WPARAM and LPARAM are.  The X value is stored
	// in the LO bits of the lParam and the Y value is stored in the HI bits.  This allows
	// you to store numerous information in one variable.  Cool huh?

	x = LOWORD(lParam) >> 12;
    y = HIWORD(lParam) >> 12;

	// Now that we have the x and y value of the joystick movement from 0 to 15,
	// we can start moving the cursor position.  Generally, the X and Y value
	// will be around 7 and 8 when we aren't moving the joystick.  But, when we
	// move the joystick to the left the X value turns to 0.  When we move the
	// joystick to the right the X value goes to 15 (The highest possible).
	// It's the same for the Y value, if we move up on the joystick, the Y value,
	// goes to 0, where down makes the Y value go to 15.  In this simple case, 
	// we just test those cases, but when you want more precision for movement,
	// you might want to make the range bigger than 0 to 15.  That way you can
	// test certain ranges instead of FULL LEFT or FULL RIGHT.  Sometimes you
	// can just move it slightly, so you would want to move slower than a full movement.

	// If we are holding LEFT, decrease the cursor X position by our acceleration
	// Else If we are holding RIGHT, increase the cursor X position by our acceleration
	if(x == 0)
		cursorPos.x -= CURSOR_SPEED;
	else if(x == 15)
		cursorPos.x += CURSOR_SPEED;

	// If we are holding UP, decrease the cursor Y position by our acceleration
	// Else If we are holding DOWN, increase the cursor Y position by our acceleration
	if(y == 0)
		cursorPos.y -= CURSOR_SPEED;
	else if(y == 15)
		cursorPos.y += CURSOR_SPEED;

	// If we aren't touching the movement, don't change the position of the cursor.

	// Here we set the new cursor position
	SetCursorPos(cursorPos.x, cursorPos.y);
	
}

// Create a function prototype for the wndProc.
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		
															// Here is our "main()" equivalent in windows, WinMain().	
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			
{														
    HWND        hwnd;										// Create a variable that holds a handle to our window.
    MSG         msg;										// Create a variable to hold the message information
    WNDCLASSEX  wndclass;									// This variable will hold all the information about the window (The name, icon, cursor, color, menu bar...)

    wndclass.cbSize        = sizeof (wndclass);				// Here we set the size of the wndclass. 
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// The style we want is Verticle-Redraw and Horizontal-Redraw
    wndclass.lpfnWndProc   = WndProc;						// Here is where we assing our CALLBACK function. (The function to handle the messages)
    wndclass.cbClsExtra    = 0;								// We want zero extra bytes
    wndclass.cbWndExtra    = 0;								// Init this useless thing to 0
    wndclass.hInstance     = hInstance;						// We assign our hInstance to our window.  
    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	// Load a logo for our window
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);	// Load a arrow cursor for our window.
															// Set the background color for the window
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;							// We pass NULL because we don't want a menu.
    wndclass.lpszClassName = "Joystick";					// Create a name that we identify this window class with.
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// Create an icon for the top left of the window.
			
	RegisterClassEx (&wndclass);							// We need to register the wndclass with the operating system
															// Here we create the window returning our handle to the window.
    hwnd = CreateWindow ("Joystick",						// window class name 
						 "Joystick",			  			// window's Title    
						 WS_OVERLAPPEDWINDOW,				// window style		 
						 windowRect.left,					// initial x position
						 windowRect.top,					// initial y position
						 windowRect.right,					// initial x size	 
						 windowRect.bottom,				    // initial y size	 
						 NULL,								// Pass NULL for the parent window
						 NULL,								// Pass NULL for a menu
						 hInstance,						    // Pass in our hInstance
						 NULL);								// Pass NULL to the wndProc, we don't want to pass it any variables address's.

    ShowWindow (hwnd, iCmdShow);							// Show the window
    UpdateWindow (hwnd);									// Update our window and paint it to the screen
															
															// Start our main loop
	while (GetMessage (&msg, NULL, 0, 0))					// Create our message loop, Get the message, then translate it and handle it.
    {														
		TranslateMessage (&msg);							// Translate the message recieved from the user
		DispatchMessage (&msg);								// Handle the message recieved from the user
    }
    
	UnregisterClass("Joystick",hInstance);					// We need to unregister the wndclass to make sure we free up memory

	return msg.wParam;										// Quit the program
}
															// Start our window procedure (wndProc).  This handles the messages
LRESULT CALLBACK WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	
	PAINTSTRUCT paintStruct;								// Create a paint struct
	HDC hdc;												// Create our HDC (Handle to a Device Context = The graphics card)

    switch (iMsg)											// This checks what the message is.  Below is some of the message that windows might return.  There of course is HUNDREDS.
    {
		case WM_CREATE:										// This message is sent when the window is created.

			if(!InitJoystick(hWnd))							// Call our init joystick function and check if it was a success
				PostQuitMessage(0);							// If it return FALSE, close the program

			break;											// We break from the switch statement.

		case MM_JOY1BUTTONDOWN:								// This message is sent when a joystick button is pressed

			// When ever we press a joystick button, this message will be called.
			// I was thinking of doing a cool, yet simple drawing program with
			// the buttons, but I wanted to make it as simple as possible.
			// If you want to check which button was pressed you need to AND ('&')
			// the button define with the wParam.  The wParam holds which button
			// was pressed, but there needs to be a binary '&' to retrieve the info.
			// If the button was pressed, then the statement will evaluate to TRUE
			// and will display a dialog box saying so.  You can make it do what
			// ever you want.  This is annoying, yet simple and direct :)

			if (wParam & JOY_BUTTON1)						// If we pressed button 1
				MessageBox(NULL, "Button 1 was pressed!", "Message", MB_OK);

			if (wParam & JOY_BUTTON2)						// If we pressed button 2
				MessageBox(NULL, "Button 2 was pressed!", "Message", MB_OK);

			if (wParam & JOY_BUTTON3)						// If we pressed button 3
				MessageBox(NULL, "Button 3 was pressed!", "Message", MB_OK);

			if (wParam & JOY_BUTTON4)						// If we pressed button 4
				MessageBox(NULL, "Button 4 was pressed!", "Message", MB_OK);

			// Another important message is MM_JOY1BUTTONUP.  This will tell you
			// when a button is released.  You will want to keep track of this
			// by setting a flag when the button is pressed and setting it to false
			// when the button is released.  That way you can do checks like:
			// if(bButton1Down) { // etc.. }   or   if(inputArray[BUTTON1]) { // etc.. }
			break;

		case MM_JOY1MOVE:									// This message gets sent when we move the joystick
															
			// The MM_JOY1MOVE message is sent when the joystick moves, but in our case,
			// we wanted it to send a message regarless if it moves or not.
			// The joystick is actually always moving... so the message is always being sent.
			// If you look at the cross hair when you calibrate your joystick in the control
			// panel, you will notice it will twitch ever so slightly.  We only want to
			// move the cursor when it is a very obvious move, done within this function below.
			// We pass in the lParam because it has the value of how much the joystick moved.

			MoveCursorWithJoystick(lParam);					// Move the cursor if it moved noticably.			
			break;

		case WM_PAINT:										// This message is sent to the WndProc when the window needs to be repainted.  This might be if we moved the window, resized it, or maximized it, or another window was over it.
			hdc = BeginPaint(hWnd, &paintStruct);			// Get our painting HDC.					
			EndPaint(hWnd, &paintStruct);					// Now we must free our memory that was allocated for the paintStruct.
			DeleteDC(hdc);									// Free the device context
		    break;											// Break from the switch statement

		case WM_DESTROY:									// This message is sent when the user closes the window.
			PostQuitMessage(0);								// Post a WM_QUIT to our message loop to quit the application
			break;											// Break from the switch statement
	}													

	return DefWindowProc (hWnd, iMsg, wParam, lParam);		// Return generic information to our message loop
}															// End of the WndProc

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// BE SURE TO INCLUDE winmm.lib TO YOUR PROJECT IN ORDER TO USE THE JOYSTICK API
//
// So, now we know how to do some simple things with they joystick.
// This program is very simple, but I wanted to not cloud the basics
// with a neat program.  See if you can make it so when you hit the
// first button it draws a pixel where the cursor is in the window, then
// when you press the second button it draws a line between the 2 points.
// That is what I wanted to do, but simple is better :)
// 
// Also, when we speak of "joystick" that could mean gamepad too. That is
// what I use, a 4 button Gravis GamePad.
//
// The steps to allowing joystick support is easy:
// 
// 1) Call joyGetNumDevs() to see if we have at least 1 device.
// 
// 2) Call joySetCapture() to initialize the desired joystick we are using.
// 
// 3) Check the MM_JOY1BUTTONDOWN and MM_JOY1MOVE messages.  The lParam is
//    the X and Y coordinates and the wParam is the buttons pressed.
// 
// That's pretty much it!  COoOOoOoOl huh?  Let us know at GameTutorials
// if this tutorial was helpfull to you.  Your suggestions are always welcome.
// 
// DigiBen
// 
// © 2000-2005 GameTutorials 
