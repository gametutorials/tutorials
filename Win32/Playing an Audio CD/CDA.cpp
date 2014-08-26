#include "CDA.h"

bool CAudioCD::init(HWND hwnd)
{
	if(hwnd == NULL)
		return false; // Got a bogus HWND

	// Save off the window handle
	win_hwnd = hwnd;

	MCI_OPEN_PARMS	mci_open_params; // This is a struct provided by MCI "Media Control
									// Interface".  We need to fill it with some information
								   // and then use it to see if we can open a CD audio device.

	mci_open_params.lpstrDeviceType = "cdaudio"; // We fill the device type with the name
											    // of the device we want to open.
											   // MSDN has a list of all these (if you can
										      // find it :)

	// Okay this is the function that opens our device (and is the main function for using 
	// MCI as you'll see :)
	// By parameter
	// 0 -- Needs to be zero if the second parameter is MCI_OPEN
	// MCI_OPEN -- This flag says we're initializing a device or file
	// MCI_OPEN_TYPE -- This flag says the lpstrDeviceType in "mci_open_params" is valid
	// &mci_open_params -- Address of our MCI_OPEN_PARAMS struct we filled in
	if(mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)&mci_open_params))
		return false;

	device_handle = mci_open_params.wDeviceID; // Save of the opened device handle

	// **Now we want to set the way we interrupt "time" for the tracks**

	MCI_SET_PARMS mci_set_params; // This struct will be filled with how we want the 
								 // device to hand the "time of tracks" -- We want to associate
							    // "track time" as minutes and seconds

	mci_set_params.dwCallback = (ULONG)win_hwnd; // The HWND where the MCI_NOTIFY message 
												// can be found

	mci_set_params.dwTimeFormat = MCI_FORMAT_TMSF; // This flag says we want the time in
												  // tracks, minutes, seconds and frames

	// Again we'll use the mciSendCommand(), by parameter:
	// device_handle -- Device ID that is to receive the command message
	// MCI_SET -- This flag says "this command sets device information"
	// MCI_SET_TIME_FORMAT -- This flag says the dwTimeFormat in "mci_set_params" is valid
	// &mci_set_params -- The address of the MCI_SET_PARAMS struct we need to pass in
	if(mciSendCommand(device_handle,MCI_SET,MCI_SET_TIME_FORMAT,(DWORD)&mci_set_params))
		return false;	

	// **Lastly determine how many tracks our on the CD**

	MCI_STATUS_PARMS status; // For holding status about the device

	status.dwCallback = (ULONG)win_hwnd; // Set the window handle
	status.dwItem = MCI_STATUS_NUMBER_OF_TRACKS; // Set the "dwItem" flag so 
											    // it says we are requesting
											   // the number of tracks on the CD

	// By parameter:
	// device_handle -- Device ID that is to receive the command message
	// MCI_STATUS -- This flag says "this retrieves information about an MCI device"
	// MCI_STATUS_ITEM -- This flag says "the dwItem member in "status" is valid
	// &status -- Address of the MCI_STATUS_PARMS struct we set
	if(mciSendCommand(device_handle,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status))
		return false;

	max_tracks = status.dwReturn; // Save off the maximum number of tracks

	// Lastly set the playing position for the first track
	setPlayPos(track);
	
		return true; // Everything has went well
}


// Plays the specified track -- If in the middle of a track and Play() is called
// the track will start over from the beginning
void CAudioCD::play()
{
	MCI_PLAY_PARMS play_params; // This structure stores where the MCI_NOTIFY flag is going
								// to be (in our WinProc)

	play_params.dwCallback = (DWORD)win_hwnd; // This HWND tell MCI where the "MCI_NOTIFY
											 // message" is
	
	play_params.dwFrom = pos; // Position to begin playing from

	// Now what we're doing here is saying, play the CD that is associated with "device_handle"
	// from the "position" specified in the dwFrom member of "play_params"
	MCIERROR mciError = mciSendCommand(device_handle,MCI_PLAY,
									   MCI_NOTIFY | MCI_FROM,
									   (DWORD)&play_params);

	// Will display an error message if we can't play -- Common error would be no CD in
	// the drive
	if(mciError)
		dispError(mciError);

	else // We're good to go so set "playing to true
		playing = true;

}

// Stops the current track
void CAudioCD::stop()
{
	if(playing == false)
		return; // Already stopped, no need to go on

	MCI_GENERIC_PARMS generic_params;  // We need to pass in the address of a 
									  // MCI_GENERIC_PARAMS as the last argument to the
									 // mciSendCommand()

	// This stops the CD that is playing BUT DOES NOT close the device
	mciSendCommand(device_handle,MCI_STOP,MCI_WAIT,(DWORD)&generic_params);

	playing = false; // CD has been stopped
}

// This seek to the next track (if applicable)
void CAudioCD::seekForward()
{
	// Can't go past "max_tracks"
	if(track >= max_tracks)
		return;

	// Increment the track
	track++;

	setPlayPos(track); // Set the play pos for the track

	stop(); // Stop the CD (temporarily)
	play(); // Immediately start back up at new position
}

void CAudioCD::seekBack()
{
	// Can't go past "first track"
	if(track <= 1)
		return;

	// Decrement track
	track--;

	setPlayPos(track); // Set the play pos for the track

	stop(); // Stop the CD (temporarily)
	play(); // Immediately start back up at new position
}

// Free all memory
CAudioCD::~CAudioCD()
{
	// Nothing has been initialized, we have nothing to free
	if(device_handle == NULL)
		return;

	if(playing) // If the CD is playing -- Stop it first
		stop();
	
	MCI_GENERIC_PARMS close_params; // A generic parameter to pass into mciSendCommand()

	// This just closes the device associated with "device_handle"
	// You should always close ALL devices, leaving them open can do BAD THINGS
	mciSendCommand(device_handle,MCI_CLOSE,MCI_WAIT,(DWORD)&close_params);

}

// ** Private Methods **

// Takes an MCIERROR (these are returned from calls to mciSendCommand())
// and prints out the error if one exists
void CAudioCD::dispError(MCIERROR error_code)
{
	if(error_code) // Make sure we have an error_code
	{
		char error_str[1024] = {0};

		// This function will fill "error_str" with the error message
		mciGetErrorString(error_code,error_str,1024);

		// Display a message box with the error
		MessageBox(win_hwnd,error_str,"Error",MB_OK | MB_ICONERROR);

	}

}

// Sets "pos" to point at the beginning of the specified track
void CAudioCD::setPlayPos(int track)
{
	MCI_STATUS_PARMS status; // For holding status about the device

	status.dwCallback = (ULONG)win_hwnd; // Set the window handle
	status.dwTrack = track; // Set the track number
	status.dwItem = MCI_STATUS_POSITION; // We want dwReturn to be filled with a
										// position in the track

	// All the parameters for the mciSendCommand() are "like usual" except for the 
	// dwFlags parameter.  So here's what each flag means:
	// MCI_STATUS_ITEM -- Says that "dwItem" in "status" is valid (ie is filled in)
	// MCI_TRACK -- Says the dwTrack in "status" is valid
	// So by combining these flags this will return us the starting position of the track
	// that we've specified
	mciSendCommand(device_handle, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)&status);

	// Save the start position
	pos = status.dwReturn;

}



