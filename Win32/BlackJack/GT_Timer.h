#ifndef GT_TIMER_H
#define GT_TIMER_H

#include <windows.h>

// A Win32 timer class
class CTimer
{
	public:
		
		CTimer();
		
		bool init(); // Init the timer, returns true on success, false otherwise
		
		void process(); // Updates delta time
		
		float getTimeElapsed(); // Returns the amount of time elapsed since init() was called
							   
		// Data Access ***
		
			float getDT() { return mDT; }
			
		// *** End Data Access
		
	private:
	
		LONGLONG mStart; // Time when CWinTimer was started
		LONGLONG mCurrent; // Current running time
		
		double mRes; // Resolution of timer (1.0 / clock ticks per second) 	
		float mDT; // Delta Time, A time slice between each call of process()
};

#endif

// © 2000-2005 GameTutorials
