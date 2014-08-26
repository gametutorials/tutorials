// Done by TheTutor

/*  What this tutorial is about is how to load and play midi files --
 
	This app is set up to only play the midi file associated with it, but with
	not to much trouble we could set it up to load ANY midi file and act a little more like
	WinAmp.

	So lets get going so we can experience and remember the sweet, sweet sound of MIDI
*/

#pragma comment(lib,"winmm.lib") // We have to include this library to play .midi files

#include "Midi.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 200

// Define our button ID's
// I'm tired of #defining stuff with 1,2,3,etc. so I used some hex numbers.
#define PLAY_BUTTON 0x00CC
#define STOP_BUTTON 0x00DD
#define LOOP_BUTTON 0x00EE

#define class_name "GameTutorials_MIDI"

MIDI gMidiPlayer; // This is our global MIDI variable -- We're going to play some
			  // kickin' midi goodness with it (Be sure to look over Midi.cpp and Midi.h 
			  // to see MIDI's guts and how they work)

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
	
	// If LoadImage() cannot open "win_bkgrnd.bmp" it will retun NULL
	// We don't want to try and call CreateBrushIndirect() if we don't have a valid
	// logbrush to pass in so we check first
	if(b_ground.lbHatch)
		wndclassex.hbrBackground = CreateBrushIndirect(&b_ground);
	else
		wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // If we couldn't fill
																	   // the window's background	
																      // with a bitmap, just
																     // fill it white

    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(NULL, // No extra style,
						  class_name,
						  "www.GameTutorials.com -- Midi-Player",
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
	    return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// By making these static, they won't "disappear" when the function returns
	static HWND PlayButton = NULL;
	static HWND StopButton = NULL;
	static HWND LoopButton = NULL;
	
    switch (message)
    {
        case WM_CREATE:

			// -- Create all necessary button's -- //

			/* What is going on here???  Okay step by step:
			1) First parameter says that we are creating a predefined type of a window.
			   A button in this case.
			2) Second parameter says the name of the button that will be displayed
			3) 3rd parameter says this "button window" will belong to a parent window
			   which means they can share a WinProc() same idea how children share a house
			   with their parents.  At anytime the parents can decide to redecorate and the
			   kids are just going to have to except it, OR ELSE!
			   WS_VISIBLE means we can see the button and BS_PUSHBUTTON means it's a push button (when you click it, it will look as if you pushed it in)
			4) The first two CW_USEDEFAULT's say place this button at a default X and Y location in my window
			5) The second two CW_USEDEFAULT's say give this button a default width and default height, in that order
			6) Specifies who the parent is -- It's answering the question "Who's your daddy?"
			7) A way to specify (just a number) our button from other buttons -- It's the button's ID
			8) Got no clue... No just kidding -- First you should look at the CreateWindow()
			   in WinMain(),  see how we passed in hinstance there.  Well we need to pass that
			   in again here.  But if you look at the WinProc() you'll see that we're not
			   passing in that varialbe, so we DON'T have access to it.  Well this is only partially true.
			   If you just tried to pass in "hinstance" it would not compile.  But we do have
			   the "hinstance", it's hidden within the "lparam" -- By typecasting the "lparam" to
			   a LPCREATESTRUCT we can extract the "hinstance"
			9) NULL -- don't care, this parameter is used to pass "extra information" but we don't have any to pass
			*/

			PlayButton = CreateWindow("Button","Play",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									  CW_USEDEFAULT, hwnd, (HMENU)PLAY_BUTTON,
									  ((LPCREATESTRUCT)lparam)->hInstance, NULL);

			StopButton = CreateWindow("Button","Stop",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									  CW_USEDEFAULT, hwnd, (HMENU)STOP_BUTTON,
									  ((LPCREATESTRUCT)lparam)->hInstance, NULL);

			LoopButton = CreateWindow("Button","Loop",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									  CW_USEDEFAULT, hwnd, (HMENU)LOOP_BUTTON,
									  ((LPCREATESTRUCT)lparam)->hInstance, NULL);

			// Load our Midi file -- Make sure it loaded if not there's no point on running
			// the program so quit
			if(gMidiPlayer.OpenMidi("MidiMusic.mid") == false)
			{
				MessageBox(hwnd,"Couldn't open MidiMusic.mid!  Did you do something to it?  Well the program is over folks.",
					       "ERROR", MB_OK | MB_ICONERROR);
				
				return -1; // Don't even create the window and abort the application
			}

			return 0; // Success
		  

        case WM_SIZE:
		  
			// Now by using the MoveWindow command we can place our buttons we made 
			// anywhere within our window and give them any size we see fitting
			// Try changing the '75' to '175' and see what I mean
			MoveWindow(PlayButton,75,50,75,35,true);
			MoveWindow(StopButton,275,50,75,35,true);
			MoveWindow(LoopButton,175,100,75,35,true);
				return 0;

        case WM_COMMAND:

			// This if statement below will be true every time a button is clicked on
			if (HIWORD(wparam) == BN_CLICKED)
			{
				/* 	But we have 3 buttons!  How do we know which one was clicked?
					The LOWORD(wparam) gets us the "button's ID" -- This was set as the 7th parameter
					in CreateWindow() when were making the buttons.
					So depending on the "button's ID" we know what button has been clicked  
				*/
				if(LOWORD(wparam) == PLAY_BUTTON)
					gMidiPlayer.PlayMidi(hwnd,false);
				
				else if(LOWORD(wparam) == STOP_BUTTON)
					gMidiPlayer.StopMidi();

				else if(LOWORD(wparam) == LOOP_BUTTON)
				{
					// If we're not playing -- Start playing and pass in "repeat" as true
					if(gMidiPlayer.GetPlayStatus() == false)
						gMidiPlayer.PlayMidi(hwnd,true);
					else
						gMidiPlayer.GetLoopStatus() = true; // The midi is already playing
															// so just set "loop" to true
															// so the midi we'll keep playing
				}

			}

			return 0;

		case MM_MCINOTIFY:  // This is a message that gets sent when
						    // "an MCI device has completed an operation"

			if(wparam == MCI_NOTIFY_SUCCESSFUL) // Check make sure the MCI device
			{									// "completed operation" was successful

				// Midi has been played from start to finish -- but we want it to
				// Loop so let's "rewind" the midi and replay it
				if(gMidiPlayer.GetLoopStatus())
					gMidiPlayer.PlayMidi(hwnd,true); // The ".mid" file is done but
													// since we're looping we'll just replay it
				else
					gMidiPlayer.GetPlayStatus() = false; // The midi is finished so set
														// "playing" to false
			}

			return 0;
		
        case WM_DESTROY:
            
			PostQuitMessage(0);
				return 0;
    
	}// end of switch statement

    return(DefWindowProc(hwnd, message, wparam, lparam));
}

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
