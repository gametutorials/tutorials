// Done by TheTutor

/*	
	This tutorial is an introduction to the many GDI (Graphical Device Interface) functions
	Windows has for drawing shapes.  Using the Win32 API, we will generate the following shapes:
	
		Ellipse
		Arc
		Polygon
		Line
	
	So with that brief introduction lets head to the code and get drawing. 
*/

#include <windows.h>
#include <assert.h>

////////////////
// Constants //
//////////////

const int kWinWid = 640; // Width of window
const int kWinHgt = 480; // Height of window

const int kMaxShapes = 4; // Maximum number of shapes drawn by this tutorial

const char kClassName[] = "GameTutorials_DrawingShapes"; // WNDCLASSEXs name

//////////////
// Globals //
////////////

HDC gHDC = NULL; // The window's device context
RECT gRect = {0}; // The window's client area

// Our global pens
HPEN gPen = NULL;
HPEN gOldPen = NULL;

// Our global brushes
HBRUSH gBrush = NULL;
HBRUSH gOldBrush = NULL;

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Selects a pen and brush into our window's HDC
void SelectRandomPenAndBrush();

// Unselect and frees up the currently selected pen and brush in
// our window's HDC
void UnselectPenAndBrush();

// These four functions draw a random [ fill in the name of the shape ] to our window
void DrawRandomEllipse();
void DrawRandomArc();
void DrawRandomLine();
void DrawRandomPolygon();

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
 	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclassex.lpszClassName = kClassName;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow cursor
    
    RegisterClassEx(&wndclassex); // Register the WNDCLASSEX
 
	// Create our window
    hwnd = CreateWindow(kClassName,
						"www.GameTutorials.com -- GDI Shape Drawing",
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, // Window will receive a default X-Pos on screen
						CW_USEDEFAULT, // Window will receive a default Y-Pos on screen
						kWinWid,
						kWinHgt,
						NULL,
						NULL,
						hinstance,
						NULL);

		// Error check
		if(!hwnd)
			return EXIT_FAILURE; // Something really bad happened!

	srand( GetTickCount() ); // Seed the random number generator

	gHDC = GetDC(hwnd); // Get the window's device context
		
		//Error check
		if(!gHDC)
			return EXIT_FAILURE; // Couldn't get window's HDC
			
	// Get the client area of the window
	GetClientRect(hwnd, &gRect);

	// Make the window visible and draw it once to the screen
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
			// Select a new pen and brush
			SelectRandomPenAndBrush();
			
			// Randomly select one of our four shapes to draw
			switch(rand()%kMaxShapes)
			{
				case 0:
					DrawRandomEllipse();
						break;
					
				case 1:
					DrawRandomArc();
						break;
						
				case 2:
					DrawRandomLine();	
						break;
						
				case 3:
					DrawRandomPolygon();
						break;
			}
			
			// Unselect the pen and brush so we can select a new pen and brush
			// the next time we draw
			UnselectPenAndBrush();
			Sleep(1000); // Wait for a second, then draw another random shape
			
		}
	}
	
	// Free up the WNDCLASSEX that was registered
	UnregisterClass(kClassName, hinstance);
		return (int)msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// Handle the messages we care about
    switch(message)
    {
		case WM_DESTROY: // Destroy the window
                       
			ReleaseDC(hwnd,gHDC); // Free up global HDC
			PostQuitMessage(0); // Send WM_QUIT message
				return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

void SelectRandomPenAndBrush()
{
	// Calculate a random color for the pen and brush 
	// each time we select a new one
	COLORREF penColor = RGB(rand()%256, rand()%256, rand()%256);
	COLORREF brushColor = RGB(rand()%256, rand()%256, rand()%256);
	
	// Create a pen and brush for using
	gPen = CreatePen(PS_SOLID, 2, penColor); // Solid pen 2 units wide
	gBrush = CreateSolidBrush(brushColor); // Solid colored brush
	assert((gPen != NULL) && (gBrush != NULL)); // Safety check
	
	// Select the pen and brush into the window's device context so
	// that we can draw with them
	gOldPen = (HPEN)SelectObject(gHDC, gPen);
	gOldBrush = (HBRUSH)SelectObject(gHDC, gBrush);
	assert((gOldPen != NULL) && (gOldBrush != NULL)); // Safety check
}

void UnselectPenAndBrush()
{
	// Put back the original pen and brush
	SelectObject(gHDC, gOldPen);
	SelectObject(gHDC, gOldBrush);
	
	// Free up the pen and brush
	DeleteObject(gPen);
	DeleteObject(gBrush);
}

// Draw an ellipse using Win32 GDI
void DrawRandomEllipse()
{
	// Calculate the center of the client area of the window
	int cenX = gRect.right / 2;
	int cenY = gRect.bottom / 2;
	
	// Calculate two random offsets in the X and Y direction
	// The first is a number between 0 and (cenX - 1)
	// The second is a number between 0 and (cenY - 1)
	int x = rand()%cenX;
	int y = rand()%cenY;
	
	// Draw a random ellipse.  You pass the GDI function a bounding rectangle
	// that you want the ellipse to be contained in.  The center of the rectangle
	// is the center of the ellipse and the outer boundaries of the rectangle will
	// contain the ellipse.  The ellipse will be outlined with the current pen and
	// filled in with the current brush.  
	// Lets break down each parameter:
	// gHDC -- The device context to draw the ellipse to.  Here gHDC is our 
	//		   window's HDC so the ellipse will be drawn to our window.
	// cenX - x -- The upper-left X-position of the bounding rectangle of the ellipse
	// cenY - y -- The upper-left Y-position of the bounding rectangle of the ellipse
	// cenX + x -- The lower-right X-position of the bounding rectangle of the ellipse
	// cenY + y -- The lower-right Y-position of the bounding rectangle of the ellipse
	Ellipse(gHDC, cenX - x, cenY - y, cenX + x, cenY + y);
}

// Draw an arc using Win32 GDI
void DrawRandomArc()
{
	// Calculate the center of the client area of the window
	int cenX = gRect.right / 2;
	int cenY = gRect.bottom / 2;

	// Calculate two random offsets in the X and Y direction
	// The first is a number between 0 and (cenX - 1)
	// The second is a number between 0 and (cenY - 1)
	int x = rand()%cenX;
	int y = rand()%cenY;

	// Draw the arc.  You pass GDI a bounding rectangle which will contain the arc.
	// Then you pass GDI the starting position and ending position of the arc.  The
	// arc will be drawn with the currently selected pen.  The currently selected
	// brush is ignored.
	// By parameter:
	// gHDC -- The device context to use for drawing the arc
	// cenX - x -- Upper-left X-position of bounding rectangle of arc
	// cenY - y -- Upper-left Y-position of bounding rectangle of arc
	// cenX + x -- Lower-right X-position of bounding rectangle of arc
	// cenY + y -- Lower-right Y-position of bounding rectangle of arc
	// cenX - (x / 2) -- Starting X-position of arc
	// cenY - (y / 2) -- Starting Y-position of arc
	// cenX + (x / 2) -- Ending X-position of arc
	// cenY + (y / 2) -- Ending Y-position of arc
	Arc(gHDC, cenX - x, cenY - y, cenX + x, cenY + y,
		cenX - (x / 2), cenY - (y / 2), cenX + (x / 2), cenY + (y / 2));
}

// Draw a line using Win32 GDI
void DrawRandomLine()
{
	// Calculate a random starting (x,y) inside the client area of our window
	int startX = rand()%gRect.right;
	int startY = rand()%gRect.bottom;
	
	// Calculate a random ending (x,y) inside the client area of our window
	int endX = rand()%gRect.right;
	int endY = rand()%gRect.bottom;

	// First we position the tip of the pen to the location we want to 
	// begin drawing a line from.
	// By parameter:
	// gHDC -- The device context to move the virtual tip of the pen on
	// startX -- X-position of where to begin drawing from
	// startY -- Y-position of where to begin drawing from
	// NULL -- This is an optional pointer to a POINT struct.  If a valid 
	//		   pointer was passed, it would be filled with the previous position
	//		   of the virtual tip of the pen
	MoveToEx(gHDC,startX,startY,NULL);

	// Draw the line.  The line is drawn from where the tip of the pen is
	// (set by MoveToEx) up to, but not including, (endX, endY).  The pen
	// will be drawn with whatever pen is currently selected.
	// By parameter:
	// gHDC -- The device context for the line to be drawn on
	// endX -- The ending X-position of the line
	// endY -- The ending Y-position of the line 
	LineTo(gHDC,endX,endY); 
}

// Draw a polygon using Win32 GDI
void DrawRandomPolygon()
{
	// Maximum number of points in our random polygon
	const int kMaxPoints = 4;

	// Create an array of four totally random points contained
	// within the client area of our window
	POINT points[kMaxPoints] = 
	{	
		{rand()%gRect.right, rand()%gRect.bottom},
		{rand()%gRect.right, rand()%gRect.bottom},
		{rand()%gRect.right, rand()%gRect.bottom},
		{rand()%gRect.right, rand()%gRect.bottom}
	};

	// Draw the polygon.  You pass a list of points that define the
	// outside dimesion of the polygon and the number of points that define
	// the polygon.  The polygon is then outlined with the current pen and
	// filled in with the current brush.
	// By parameter:
	// gHDC -- The device context we want to draw the polygon to
	// points -- An array of points that define the polygon's shape
	// kMaxPoints -- The number of points in the "points" array passed to the function
	Polygon(gHDC,points,kMaxPoints);
}

// Shape up or ship out!

/*
	While not the fastest way to draw anything, GDI does provide a bunch of well tested
	functionality for drawing shapes.  If it's a not a speed critical task, the Win32 GDI
	shape drawing functions should serve you well.
	
	Got a question?  Be sure to post it at the forums on www.GameTutorials.com.

*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
