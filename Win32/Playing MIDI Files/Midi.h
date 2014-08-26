#ifndef MIDI_H
#define MIDI_H

#include <windows.h>

class MIDI
{
	public:
	
		// Constructor
		MIDI():device_handle(NULL),playing(false),loop(false) { /* do nothing */ }

		// Opens a .mid file with name "file_name"  -- If successful returns true
		// otherwise, returns false
		bool OpenMidi(char file_name[] = NULL);

		// Plays the midi to the window associated with "hwnd",
		// If repeat equals true we'll continue playing midi
		// If there's already a midi playing, nothing happens
		// Returns false if an error occurs, true otherwise
		bool PlayMidi(HWND hwnd, bool repeat); 

		bool StopMidi(); // Stops the midi that is playing --
						//  If no midi is playing does noting --
					   //   returns false if an error occurrs, true otherwise

		bool CloseMidi(); // Close the midi that was opened with "OpenMidi()" --
						 // Returns false on failure, true otherwise

		// DATA ACCESS
		inline MCIDEVICEID GetHandle() { return device_handle; }
		inline bool& GetPlayStatus() { return playing; }
		inline bool& GetLoopStatus() { return loop; }

		/* Okay I'll explain why we're returning the reference to a bool --
		   By doing this, we don't have to have a separate function to set the
		   playing status or set the looping status

			In our main() we can do something like this:

			MIDI midi;

			midi.GetLoopStatus() = true;  // Now we've changed the loop status to TRUE
										  // Remember our constructor sets it to false initially

		*/

		// Deconstructor -- Frees all memory
		~MIDI();


	private:

		MCIDEVICEID	device_handle; // Handle to a "media device" (for our case midi)
		
		bool playing; // If set to true the midi "song" is playing, otherwise
					  // the midi "song" is stopped
		bool loop; // If this is true the midi "song" will loop forever, otherwise
				   // it will stop when it reaches it's end.

};

#endif
