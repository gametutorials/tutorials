//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Window											 //
//																		 //
//		$Description:	Create a simple window							 //
//																		 //
//***********************************************************************//
#include <windows.h>	// We need to include windows.h

// This is the "Hello World" of Windows programming.  If you've never programmed 
// using the Win32 API before, this is the place for you.  First up lets talk in 
// about how a Windows application is set up.  In general, there are two main
// functions, the WinMain which is the core of the program and the WndProc which
// is a function the operating system sends information, typically called messages,
// that get received by the application and the application does stuff with.

// Lets start by looking at the function prototype for the WndProc.  Looking below 
// at the code, you'll notice it returns a LRESULT.  Well in English, a LRESULT is a
// 32-bit number.  If Windows ever comes out with a 64-bit operating system, this 
// will most likely change to a 64-bit number.  The next piece of the syntax is "CALLBACK".
// If we look in HELP (pressing F1 to launch MSDN), it says CALLBACK is a calling 
// convention for callback functions.  Well, that sounds redundant... But what CALLBACK
// does, is specify how in assembly parameters will get passed and returned from the
// function.  This is one of those things you don't have to fully understand to begin
// Windows programming.  Next lets look at the parameters to the function HWND, UINT,
// WPARAM, and LPARAM.  HWND is the handle to the window that is receiving a message from
// the operating system.  UINT is flag that says what type of message is being sent.
// WPARAM and LPARAM are flags, that depending on the message being sent, will have 
// different meanings.  So say the WndProc received a message that the mouse moved, WPARAM and
// LPARAM might contain the X and Y position of the mouse cursor.

// Remember "main()" ?  In a Win32 application it is called WinMain().  Take a peek
// below to see the signature of the function.  The return type is an int, and it is
// used to return any problems, if any.  If we lookup WINAPI in the help, it says:  
// "Calling convention for the Win32 API."  Again this means how in assembly language the
// parameters get passed by the function.  The first parameter passed to the WinMain is
// an HINSTANCE.  An HINSTANCE is a handle to the an instance, in this case, it is 
// a handle to the specific instance of the program we're creating.  The second
// parameter is also an HINSTANCE, on older Windows operating systems, it returned
// a the previous HINSTANCE of the application.  However, now it is always NULL, so 
// we'll simply ignore it.  Next is passed a PSTR.  This is optional command 
// line arguments to the program.  Lastly, is a int.  This is a flag that
// states how the window should be displayed.  For instance, should it be maximized, minimized
// in the task bar, etc.

// There are a lot of other new variable types in a Win32 app.  Keep reading and 
// each one will be explained.  Don't worry if this feels overwhelming.  After you 
// dive in a do some Win32 coding, all of this will start to click. 															
						
// The Window callback procedure															
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		
															
// The main window function (equavilent to main() in a console application)	
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)			// Here is our "main()" for windows.  We must have this for a windows application.
    {														
    
    // HWND - a HWND is a handle to a window.  This is used to keep track of a certain window.
    // Programs can have multiple windows.  We only have one though.
    HWND        hwnd;										
    
    // MSG  - A MeSsaGe variable to hold what messages are being sent to the window.
    // For instance, if the window was clicked, closed, moved, etc...
    MSG         msg;										
    
	// Windows have two set of attributes that define how they work.  The first is a WNDCLASSEX
	// it contains information that can apply to multiple windows such as icons, cursors, background
	// color, menu bar etc...  We'll fill only the fields that we need to get a window up and running. 
    WNDCLASSEX  wndclass = {0};
    
    // *** Begin filling the fields of the WNDCLASSEX ***							

	// Here we set the size of the wndclass. You will see this a lot in windows, it's kinda odd.
	// We use the "sizeof()" function to tell windows the size of our struct.
    wndclass.cbSize        = sizeof(wndclass);				
    
	// The style we want is vertical redraw and horizontal redraw.  These styles say that if the window
	// is resized or moved and the client size of the window changes then the entire window will be drawn.
	// What's the client size of a window you ask?  Well that's the inner rectangle region of a window
	// which does not include the title bar, minimize button, maximize button, or the thin border
	// surrounding the window.
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		
    
    // Here we assign our WndProc.  Remember the WndProc is a function that will intercept and deal
    // with any messages the operating system sends our window. 
    wndclass.lpfnWndProc   = WndProc;
    
    // We assign our hInstance to our window.  Once again, You can have several instances of a
    // program, so this keeps track of the current one.						
	wndclass.hInstance     = hInstance;						
	
	// Here we set the background color.  The window will get repainted with what ever color we 
	// specify, which for us, will be white.
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    
    // Here we set a name for our class, to distinguish it against others.  
    // We need to use this name later when we create the window.
	wndclass.lpszClassName = "GameTutorials";
	
	// *** Whew!  We're finally finished filling the fields of the WNDCLASSEX ***							
   
	// We need to register our windows class with the Windows OS.  This tells Windows that we have
	// a type of window we'd like to create and here are the basic parameters that define that window.
	RegisterClassEx(&wndclass);								
															
	// Now we actually create the window.  CreateWindowEx() returns a handle to the window, which we
	// store in our HWND "hwnd".  Notice the "EX" on the create window function?  Well Win32 actually
	// has two functions for creating a window, CreateWindow() and CreateWindowEx().  The "EX"
	// stands for "extended version".  If you look at the program closely, you'll notice the "EX" 
	// popping up on other functions and variables as well.  Lets begin breaking down each 
	// variable that we're passing to CreateWindowEx().  First we pass NULL, this is for
	// any extended window style.  Your going to want to consult MSDN to see all the tons of 
	// styles you can set for a window.  Next, is the string "GameTutorials".  If you notice
	// we used the same string as one of the variables in WNDCLASSEX.  We pass it it to CreateWindow()
	// to identify which WNDCLASSEX we want to use when creating the window.  Next, is the
	// text string "My First Window".  This is the text that will appear in the title bar of the
	// window.  Moving on, is the parameter WS_OVERLAPPEDWINDOW.  This describes the style of
	// the window.  Specifically it means the window will have a title bar, minimize and maximize box,
	// a system menu (click the top-left of a window to see it's system menu), and a thick frame
	// surrounding the window.  One thing you'll learn quickly when doing Windows programming is that
	// MSDN (MicroSoft Developer's Network) is your friend.  You read up on additional window 
	// styles there.  The next 2 parameters are CW_USEDEFAULT, in general, CW_USEDEFAULT tells
	// Windows to pick a default value for that parameter.  These two parameters specify the 
	// upper-left hand corner (x,y) of where the window should be located on the screen.  So if
	// we changed these to (0,0) the window would be created in the upper-left hand corner of
	// the monitor.  The next pair of variables is 320 and 200.  These are the width and height
	// of the window in pixels.  So if we changed these to 640 by 480, we'd get a window 640 pixels
	// in width and 480 pixels in height.  Moving on the next parameter is NULL.  This is for 
	// a handle to a parent window.  Windows allow you to attach windows together through a parent/child
	// relationship.  Since this is the parent window, we pass NULL.  Next is another NULL parameter,
	// it is used to pass a pointer to an optional menu for the window.  Since we don't have a menu
	// for the window, we pass NULL.  Next up, we must pass the HINSTANCE we received from WinMain.
	// This instantiates our created window with the operating system.  Last but not least, we
	// pass a NULL pointer.  Optionally we could pass a pointer to some object we created and retrieve
	// that pointer in the WndProc.  Why would we want to do that?  Well if we wanted to have zero
	// global variables we could use this parameter to pass an object that contained all the data for
	// our application, then our WinMain and WndProc could reference the same data without the use
	// of globals.  But creating an application without global variables is more of an exercise in
	// software engineering than this beginner Win32 tutorial is gonna try and tackle. 											
	hwnd = CreateWindowEx(NULL,	"GameTutorials", "My First Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
						  CW_USEDEFAULT, 320, 200, NULL, NULL, hInstance, NULL);							

    // Now that the window is created, lets show it!  The ShowWindow() function shows the 
    // window as specified by the iCmdShow parameter.  We could choose to show the 
    // window minimized on the task bar, maximized, or other options.  Typically iCmdShow equals
    // SW_SHOW which shows the window using the size and location specified in CreateWindowEx() 
    ShowWindow(hwnd, iCmdShow);																
    
    // This tells the window to draw itself so we can see it
    UpdateWindow(hwnd);									
	
	// All right! Finally our window is created and visible.  Now it's time to 
	// have it do things.  There are multiple ways to write what's referred to 
	// as the "window loop", but below is a typical way to go about it.  Lets 
	// digest the window loop
														
	while(1) // Loop forever
	{	
		// PeekMessage() peeks at the operating systems message queue and returns true for
		// every message it finds.  Lets look at each variable passed to the function in 
		// greater detail.  The first parameter is a MSG struct, it will get 
		// filled with whatever the current message the operating system has to tell us.
		// Next is NULL, if we wanted to check the messages going to a specific window, 
		// we could pass the handle of that window here.  By passing NULL, it will get
		// all the message for the current running thread.  Next is a pair of big fat 
		// zeros.  These are flags that specify the range of messages we want to 
		// check on.  So for instance, we could pass some flags to only check on keyboard 
		// input messages.  By passing the the zeros, we're saying we want to examine
		// all messages received.  Lastly, the final parameter says what to do with the 
		// messages once we've received them.  The PM_REMOVE flags says to discard the messages
		// once it's been received. 											
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If we receive a message, PeekMessage() will return true and will 
			// enter this portion of the code
		
			// If the message says to quit, we quit and break out the while(1) loop
			if(msg.message == WM_QUIT)
				break;
		
			// Translate message takes windows virtual key codes and translates 
			// them into character messages.  So what does that do for us?  Well in 
			// this tutorial nothing, but lets dig a little deeper to see what's happening.
			// For example, when you press 'A' on the keyboard, Windows assigns a virtual 
			// key code to the keystroke and PeekMessage() receives this message.  By calling
			// TranslateMessage() a new message is sent that converts the Windows code into
			// the character representation of 'A'.  This way we can use standard C++ syntax
			// to deal with alphanumerics instead of having to convert back and forth between
			// Windows representation of the keyboard.  If this sounds confusing, well it is, but
			// if you are dealing with keyboard input your gonna most likely want this 
			// function.
			TranslateMessage(&msg);
			
			// The DispatchMesage() function sends the message we recieved from PeekMessage()
			// to the WndProc.							
			DispatchMessage(&msg);
		}							
    }

	// When we done with a particular WNDCLASSEX, we can tell Windows we don't need 
	// it anymore and free up memory.  That's what UnregisterClass() does.  You'll notice
	// we also pass the HINSTANCE of the window.  Why do we do that?  Well more than one 
	// window can use the same WNDCLASSEX, so UnregisterClass() will only completely
	// remove the WNDCLASSEX if no windows are currently using it. 
	UnregisterClass("GameTutorials",hInstance);		
    
    // Return the exit code for the application.  This exit code was
    // set by the PostQuitMessage() function that we call in the WndProc()
    return msg.wParam; 
}

// Here is our WndProc (Window Procedure).  We'll send it messages 
// we receive and have it deal with them. 														
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// This checks what the message is.  Below is some of the message that windows might return.
	// There of course are HUNDREDS of potential messages.
    switch(iMsg)											
    {	
		// The only message that you NEED to deal with, is the WM_DESTROY.  The rest you don't have to check for.  
		// Below are just some examples on how to check for a message.
								
		case WM_CREATE: // This message is sent when the window is created.									
			// We would want to put our initialization code here...
			break;

		case WM_SIZE: // This message happens when we resize the window.
			// We would put code to resize text or a picture here...
			break;										

		case WM_PAINT: // This message is sent to the WndProc when the window needs to be repainted.  
			// Put code here to paint what should be in the window.	
		    break;										

		case WM_DESTROY: // This message is sent when it's time to destroy the window.
			// Here you would handle deinitialization, freeing memory, etc..
			
			// If you look back up top at our window loop you notice a piece of code like this:
			//	if(msg.message == WM_QUIT)
			//		break;
			// PostQuitMessage() sends the WM_QUIT message.  The parameter we pass is 
			// the exit code we want to report to the operating system.  By 
			// passing zero we're saying "Zero errors occured in the application." 
			PostQuitMessage(0);								
															
			break;
		
		// So what if we get a message that we don't explicitly handle?  This is where 
		// the default window procedure comes in.  All messages we don't handle ourselves,
		// should get sent to the DefWindowProc().  It will apply a default Windows behavior
		// to the message.  For example, lets say somebody clicks the 'X' in the upper-right
		// hand corner of the window.  We could handle that message ourselves and set a quit 
		// message to the window or we could simply let the default window procedure handle it
		// for us.
		default:
		
			return DefWindowProc(hwnd, iMsg, wParam, lParam);											
	}													

	// Typically if you handle a message in the WndProc, you return zero.
	return 0;  
				
}		

// And that's all it takes to make a windows program :)
// Yes it's a lot of front, but once you go through it a few times, it will 
// become much clearer.  

// So lets recap with the 5 steps to win32 programming:

// 1) Initialize a class
// 2) Register the class
// 3) Create the window
// 4) Get the messages from the operating system
// 5) Translate and dispatch the messages to the WndProc

// What's that you say?  Haven't had enough Win32 yet?  Well here's a look at 
// the variables that we didn't set in our WNDCLASSEX
/*
	WNDCLASSEX  wndclass;
	
	// These two variables are used for allocating "extra space" and their value is the
	// number of extra bytes to allocate.  Chances are you'll always set these to zero. 
	wndclass.cbClsExtra
	wndclass.cbWndExtra
	
	// This is the icon that is associated with the executable. 
	wndclass.hIcon
	
	// This is the cursor that is associated with the executable.
	wndclass.hCursor
	
	// This is the name of a menu to associate with the window class
	// If a menu is associated, all windows created using this window class will have
	// the associated menu 
	wndclass.lpszMenuName
	
	// The small icon for the executable.  This is the icon that appears in the upper-left
	// hand corner of the window's title bar.
	wndclass.hIconSm	
*/

// That's it.  Be sure to check out the www.GameTutorials.com message boards if you
// have any Win32 questions.

// © 2000-2005 GameTutorials
