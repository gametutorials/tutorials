// Done by TheTutor

/*  This tutorial is to show you how to play music off a CD.
	We'll utilize MCI to play our music.  All of the "hardcore" explaining is
	in CDA.h and CDA.cpp

	So on with the show :)
*/

#pragma comment(lib,"winmm.lib") // Load the Window library necessary to hear lovely sound

#include "CDA.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 1

// Some pound defines for our buttons
#define PLAY_BUTTON 0x00A1
#define STOP_BUTTON 0x00A2
#define SEEK_FORWARD_BUTTON 0x00A3
#define SEEK_BACKWARD_BUTTON 0x00A4

#define class_name "GameTutorials_PlayCD"

CAudioCD audio; // This is our global CAudioCD variable -- We're going to play some
			   // kickin' CD tuneage with it (Be sure to look over CDA.cpp and CDA.h to 
			  // see CAudioCD's implementation)

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	LOGBRUSH b_ground;

    // Set up background image
	b_ground.lbStyle = BS_PATTERN;
	b_ground.lbColor = DIB_RGB_COLORS;
	b_ground.lbHatch = (long)LoadImage(NULL,"win_bkgrd.bmp", IMAGE_BITMAP, 0, 0,
		                               LR_LOADFROMFILE);
    
	// Init the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);
	
	// If LoadImage() cannot open "win_bkgrnd.bmp" it will return NULL
	// We don't want to try and call CreateBrushIndirect() if we don't have a valid
	// logbrush to pass in so we check first
	if(b_ground.lbHatch)
		wndclassex.hbrBackground = CreateBrushIndirect(&b_ground);
	else
		wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // Make it white

    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(NULL, // No extended style
						  class_name,
						  "www.GameTutorials.com -- CD-Player",
						  WS_SYSMENU | WS_CAPTION, // We won't be able to resize the window
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  WINDOW_WIDTH,
						  WINDOW_HEIGHT,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	while(1)
	{
		// If we get a Window Message -- Handle it appropriately
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass(class_name,hinstance); // Free up WNDCLASSEX
		return msg.wParam; // And we're out :)
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// By making these static, they won't "disappear" when the function returns
	static HWND PlayButton = NULL;
	static HWND StopButton = NULL;
	static HWND SeekForward = NULL;
	static HWND SeekBackward = NULL;
	
    switch (message)
    {
        case WM_CREATE:

			// -- Create all necessary button's -- //

			PlayButton = CreateWindow("Button","Play",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									  CW_USEDEFAULT, hwnd, (HMENU)PLAY_BUTTON,
									  ((LPCREATESTRUCT)lparam)->hInstance, NULL);

			StopButton = CreateWindow("Button","Stop",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									  CW_USEDEFAULT, hwnd, (HMENU)STOP_BUTTON,
									  ((LPCREATESTRUCT)lparam)->hInstance, NULL);

			SeekForward = CreateWindow("Button",">>>",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									   CW_USEDEFAULT, hwnd, (HMENU)SEEK_FORWARD_BUTTON,
									   ((LPCREATESTRUCT)lparam)->hInstance, NULL);

			SeekBackward = CreateWindow("Button","<<<",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									    CW_USEDEFAULT, hwnd, (HMENU)SEEK_BACKWARD_BUTTON,
									    ((LPCREATESTRUCT)lparam)->hInstance, NULL);

			// Initilize our CAudioCD class -- If we can't, we better give up.
			if(audio.init(hwnd) == false)
			{
				MessageBox(hwnd,"Couldn't init CAudioCD!  I can't go on!",
					       "ERROR", MB_OK | MB_ICONERROR);
				
				return -1; // **NOTE** by return -1 in the WM_CREATE message you tell Windows
						   // to return NULL with the CreateWindow() call thus terminating
						   // the window before it was ever created.
			}

			return 0; // Created successfully

        case WM_SIZE:
		  
			// Now by using the MoveWindow command we can place our buttons we made 
			// anywhere within our window and give them any size we see fitting
			MoveWindow(PlayButton,75,50,80,35,true);
			MoveWindow(StopButton,250,50,80,35,true);
			MoveWindow(SeekBackward,75,95,40,25,true);
			MoveWindow(SeekForward,115,95,40,25,true);
				return 0;

        case WM_COMMAND:

			// This if statement below will be true every time a button is clicked on
			if(HIWORD(wparam) == BN_CLICKED)
			{
				// If the user hit the "Play" button, start playing the CD			
				if(LOWORD(wparam) == PLAY_BUTTON)
					audio.play();
				
				// If the user hit the "Stop" button, stop the CD
				else if(LOWORD(wparam) == STOP_BUTTON)
					audio.stop();

				// If the user hit the "Seek Forward" button, skip to the next song
				else if(LOWORD(wparam) == SEEK_FORWARD_BUTTON)
					audio.seekForward();

				// If the user hit the "Seek Backward" button, skip back a song
				else if(LOWORD(wparam) == SEEK_BACKWARD_BUTTON)
					audio.seekBack();				

			}

			return 0;

		case MM_MCINOTIFY:  // This is a message that gets sent when
						    // "an MCI device has completed an operation"

			// We could check stuff if in here if wanted -- Maybe display the 
			// current track, and time remaining in it, but we're not going to 
			// do anything :)

			return 0;
		
        case WM_DESTROY:
            
			PostQuitMessage(0);
				return 0;
    
	}// end of switch statement

    return(DefWindowProc(hwnd, message, wparam, lparam));
}

/*
	Now hear this...

	The known "short-comings" -- If you have multiple CD players this will only read CD's 
	from the "default CD ROM" (system dependent) -- If you don't have a CD in the drive you'll 
	get a nasty error message when you try to run the app or when you press the "Play" button.
	
	There could be problems if you switch CD's while playing them (since the new
	CD would think it's max tracks were equal to the old CD's max tracks) -- So if you wanted
	to make this REAL robust you'd do a check and anytime the CD drawer was opened/closed
	you do some reinitialization of variables

	As always let us know if you have any questions at www.GameTutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
