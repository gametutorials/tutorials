#ifndef GT_INPUT_H
#define GT_INPUT_H

#include <windows.h>

const int kMaxKeys = 256;

class CWinInput
{
	public:
	
		CWinInput();
		
		void poll(HWND hwnd); // Polls the mouse and keyboard for the HWND passed in
		
		void setKey(int key, bool onOrOff);
		void setKey(WPARAM key, bool onOrOff);
		void setMousePos(int x, int y);
		void setMousePos(const POINT &p) { mPos = p; }
		
		// Sets the state of the mouse buttons
		void setLeftMB(bool onOrOff) { mKeys[VK_LBUTTON] = onOrOff; }
		void setRightMB(bool onOrOff) { mKeys[VK_RBUTTON] = onOrOff; }
		void setMiddleMB(bool onOrOff) { mKeys[VK_MBUTTON] = onOrOff; }
		
		bool isKeyDown(int key);
		bool isLeftMBDown() { return mKeys[VK_LBUTTON]; }
		bool isRightMBDown() { return mKeys[VK_RBUTTON]; }
		bool isMiddleMBDown() { return mKeys[VK_MBUTTON]; }	
		
		// Data Access ***
		
			POINT getMousePos() { return mPos; }
			
		// *** End Data Access
		
	private:
	
		bool mKeys[kMaxKeys]; // Array of virtual keys whether they are down (true) or up (false)
		
		// Mouse position
		POINT mPos;
};

#endif
