#ifndef CDA_H
#define CDA_H

#include <windows.h>

class CAudioCD
{
	public:

		// Our default constructor
		// We set max to -1 to signify "device not initialized"
		CAudioCD():device_handle(NULL),playing(false),track(1),max_tracks(-1),
				   pos(0),win_hwnd(NULL) { /* do nothing */ }

		bool init(HWND); // Initializes our CAudioCD so it's ready to play CD's

		void play(); // Plays the CD from the current position it is pointing at

		void stop(); // Stops the current track

		void seekForward(); // Seeks to the next track (if applicable)
		void seekBack(); // Seeks back a track (if applicable)

		// Data access ***

		bool isPlaying() { return playing; }
		int getTrack() { return track; }
		int getMaxTracks() { return max_tracks; }

		// *** End of data access

		~CAudioCD(); // The deconstructor

	private:

		MCIDEVICEID	device_handle; // Handle to a "media device" (for our case CD audio player)
		
		bool playing; // If set to true the current track is playing, otherwise it's not

		int track; // Holds which track are we playing
		int max_tracks; // Holds the maximum number of tracks on the CD

		DWORD pos; // This will be used to hold the position of where to start playing from

		HWND win_hwnd; // Keep around a copy of the window handle

		void dispError(MCIERROR error_code); // Display a message box with 
											// a message of what error has occurred

		void setPlayPos(int track); // Sets the "pos" variable to point to the 
								   // track passed in

};


#endif
