#ifndef GT_WINOBJ2D_H
#define GT_WINOBJ2D_H

#include "GT_WinObj.h"
#include "GT_Image.h"
#include "GT_Timer.h"
#include "GT_Input.h"
	
const int kDefaultFPS = 60;

// A 2D Window object
// Supports:  Internal clock (time based movement), double buffering, mouse/KB input
class CWinObj2D : public CWinObj
{
	public:
	
		CWinObj2D();
		
		// Init CWinObj2D
		virtual bool init(HINSTANCE hinstance, HWND hwnd);
		
		void process();  // Process the timer and polls KB/Mouse (call once a frame)
		void draw(); // Draws the back buffer to the window
		
		// Sets window dimensions to the specified WINDOW width/height
		// Returns true for success, false otherwise
		virtual bool setWindowSize(int wid, int hgt);

		// Sets window dimensions to the specified CLIENT width/height
		// Returns true for success, false otherwise
		virtual bool setClientSize(int wid, int hgt);

		// Processes the window and calculates the frame rate
		// Returns true when it's time to draw determined by the frames per second for 
		// the CWinObj2D, otherwise it returns false
		// **NOTE** You should NOT call process() or calcFPS() if using this function
		bool lockToFrameRate(); 	

		// Draws the window's FPS to the upper left corner of the window 
		void drawFPS(int color = ARGB(255, 0, 0, 255));

		// Sets the desired frames per second
		void setFPS(unsigned int desiredFPS = kDefaultFPS);

		void calcFPS(float dt); // Calculates the FPS of the window	

		// Clears the back buff to the passed in color
		void clear(int color = ARGB(255, 255, 255, 255));
		
		// Data Access ***

			HDC getBackDC() { return mBackBuff.getDC(); } // Get back buffer HDC
			float getDT() { return mDeltaTime; } // Get time slice for current frame
			int getFPS() { return mFPS; } // Get current frames per second
			
			bool isKeyDown(int key) { return mInput.isKeyDown(key); }
			bool isLeftMBDown() { return mInput.isLeftMBDown(); }
			bool isRightMBDown() { return mInput.isRightMBDown(); }
			bool isMiddleMBDown() { return mInput.isMiddleMBDown(); }	
			
		// *** End Data Access
		
		virtual ~CWinObj2D() {} // Nothing to explicitly free
		
	protected:
	
		float mDesiredFPS; // 1.0f / DESIRED_FRAMES_PER_SECOND
		int mFPS; // The frames per second
		float mDeltaTime;

		CTimer mTimer; // Timer for the application
		CImage mBackBuff; // A double buffer for the window
		CWinInput mInput; // Input handler
		
	private: // -- Private methods --

		bool createDoubleBuffer(); // Create's the double buffer
		
		// Private copy constructor and assignment operator so 
		// no accidental copies are made
		CWinObj2D(const CWinObj2D &obj) { }
		CWinObj2D& operator =(const CWinObj2D &obj) { return *this; }
};

#endif
