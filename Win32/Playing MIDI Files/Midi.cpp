#include "Midi.h"

bool MIDI::OpenMidi(char file_name[])
{
	if(!file_name)		// Verify we have a valid file name
		return false;

	MCI_OPEN_PARMS	mci_open_params;  /* Okay this is a struct that we're going to try and 
										 fill with information we need to open an .mid file

										 MCI stands for "Media Control Interface" and functions
										 or variables that have an MCI in the name most likely are
										 for playing multimedia devices or "recording" using 
										 multimedia devices */
	
	// Here we are initializing the fields we care about
	
	// There's different Device's that can be filled in here -- For a "decent" explanation of
	// all the different devices I suggest looking them up in MSDN (Microsoft Software Development Network)
	// If you have MSDN installed on your machine, place your cursor over the MCI_OPEN_PARAMS a few lines up
	// then press F1 -- From that page you should find a link to all the different devices

	mci_open_params.lpstrDeviceType = "sequencer"; // This was a little difficult to track down -- Like I said 
												  // above there is a list of them in MSDN but I'll take a second and list a couple
												 // of other devices we could put here: cdaudio, digitalvideo, scanner, etc.
											    // Anyway if you're familiar with the term "Midi Sequencer", the "sequencer" part
											   // should make a little bit of sense -- Okay so now by setting it to "sequencer" we're 
											  // stating we want to open a midi sequencer -- Let's move on

	mci_open_params.lpstrElementName = file_name; // Which .mid we want to open
	
	/* mciSendCommand() is the "main" way to send information/commands about an MCI device
	   there are tons of different flags and such that you can pass in.  What we are saying
	   here is that we're trying to open an MCI (Media Control Interface) which is a 
	   "midi sequencer" using the file name we passed in -- if mciSendCommand() is successful the
	   return value is zero, otherwise it will return the error code */
	if(mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)&mci_open_params))
		return false;

	// Alright if we got here than we were able to open our midi file, YEAH!
	// Let's save off the "handle" that got filled in for us
	device_handle = mci_open_params.wDeviceID;
		return true; // We were successful

} // end of OpenMidi(char file_name[])

bool MIDI::PlayMidi(HWND hwnd, bool repeat)
{
	if(!hwnd) // Since we're good programmers we'll make sure we didn't get a NULL hwnd
		return false; // If we did -- There's been an error

	// Otherwise we want to start playing a midi
	// Lets set the loop variable to what was passed in
	loop = repeat;

	// Okay now lets send the message to play midi! //
	
	MCI_PLAY_PARMS play_params; // This structure stores where the MCI_NOTIFY flag is going
								// to be (in our WinProc)
	MCI_SEEK_PARMS seek_params; // This structure is necessary for passing into the 
								// mciSendCommand() when doing a "seek"

	// Make sure we're always at the beginning of the file
	mciSendCommand(device_handle,MCI_SEEK,MCI_SEEK_TO_START,(DWORD)&seek_params);

	play_params.dwCallback = (DWORD)hwnd; // We know where the "MCI_NOTIFY message" is taken care of so we'll fill that in

	// Now what we're doing here is saying, play the midi that is associated with "device_handle"
	// The return code is zero for success, some number for failure
	if(mciSendCommand(device_handle,MCI_PLAY,MCI_NOTIFY,(DWORD)&play_params))
		return false;

	playing = true; // We're playing so set the flag
		
		return true; // All right we're playing midi -- Return success

} // end of PlayMidi(HWND hwnd, bool repeat)

bool MIDI::StopMidi()
{
	if(!playing) // Nothing is playing -- And that's what we want so we're done
		return true;

	MCI_GENERIC_PARMS generic_params;  // This structure stores where the MCI_NOTIFY flag is going
									  // to be (in our WinProc) -- You'll notice that's the same thing
									 // I put for "play_params", MCI_PLAY_PARMS has two other members in
								    // structure that we just didn't bother to fill
	
	MCI_SEEK_PARMS  seek_params; // This is the structure that mciSendCommand() takes when we're going to be
								// "positioning a file pointer"

	// This stops the midi that is playing BUT DOES NOT close the midi --
	// So we could restart playing it if we wanted
	if(mciSendCommand(device_handle,MCI_STOP,MCI_WAIT,(DWORD)&generic_params))
		return false;

	// This puts the "file pointer" of the midi back to the beginning of the midi
	mciSendCommand(device_handle,MCI_SEEK,MCI_SEEK_TO_START,(DWORD)&seek_params);
	
	// We're no longer playing midi -- and if we were looping we're no longer looping
	playing = false;
	loop = false;

		return true; // We stopped the midi successfully

	/* Again I reiterate there are LOTS of commands you can pass into mciSendCommand()
	   I certainly don't have them all memorized -- If there is something I need to do
	   I look it up in MSDN %)  */
}

// Close's a "midi"
bool MIDI::CloseMidi()
{
	// Nothing has been opened or is currently open so we're done
	if(device_handle == NULL)
		return true;
	
	MCI_GENERIC_PARMS close_params; // A generic parameter to pass into mciSendCommand()

	// This is pretty simple really -- it just closes the device associated with "device_handle"
	// You should always close ALL devices, leaving them open can do BAD THINGS
	if(mciSendCommand(device_handle,MCI_CLOSE,MCI_WAIT,(DWORD)&close_params))
		return false; // Again mciSendCommand() returns zero for success -- So if get anything besides zero, there's been an error
	
	device_handle = NULL; // Nothing is associated with it now

		return true;
}	

// Deconstructor
MIDI::~MIDI()
{
	CloseMidi(); // See CloseMidi()
}
