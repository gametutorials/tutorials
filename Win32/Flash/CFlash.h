#ifndef _CFLASH_H
#define _CFLASH_H


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// Here we define the classes for Flash.  We first define a GUID for the Flash
// interface (which stores all the functions for Flash), then create a class
// to hold all the functions.  Finally, we create our own class to handling
// the initialization and loading of our Flash movie.
//

// Define this so we don't have to see string warnings for Visual Studio 2005
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>		// Include basic C++ definitions
#include <atlbase.h>	// Add this to use ATL
#include <atlwin.h>		// Use this for creating an ATL window
using namespace std;	// Use the standard namespace


// This is the GUID for the Flash class.  We use this to access the Flash interface through COM.
const IID IID_IShockwaveFlash = {0xD27CDB6C,0xAE6D,0x11CF,{0x96,0xB8,0x44,0x45,0x53,0x54,0x00,0x00}};

// This is the Flash interface class that has all of the Flash functions we can call.
// This class in inherited from an IDispatch class, which is a COM class.
class IShockwaveFlash : public IDispatch
{
public:

	// All our functions are virtual because they will be defined when
	// we use COM to connect to the Flash DLLs.  The functions below
	// are the main functions that you most likely use.  The function
	// names are pretty self-explanatory.
	virtual HRESULT STDMETHODCALLTYPE StopPlay(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Play(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Back(void) = 0;\
	virtual HRESULT STDMETHODCALLTYPE Forward(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Rewind(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE GotoFrame(long FrameNum) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_TotalFrames(long __RPC_FAR *numframes) = 0;
	virtual HRESULT STDMETHODCALLTYPE CurrentFrame(long __RPC_FAR *FrameNum) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Loop(bool Loop) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Loop(bool __RPC_FAR *Loop) = 0;
	virtual HRESULT STDMETHODCALLTYPE IsPlaying(bool __RPC_FAR *Playing) = 0;

	// The rest of these functions are available as well.
	virtual HRESULT STDMETHODCALLTYPE get_ReadyState(long __RPC_FAR *thestate) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Playing(bool __RPC_FAR *Playing) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Playing(bool Playing) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Quality(int __RPC_FAR *Quality) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Quality(int Quality) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ScaleMode(int __RPC_FAR *scale) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ScaleMode(int scale) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AlignMode(int __RPC_FAR *align) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_AlignMode(int align) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BackgroundColor(long __RPC_FAR *color) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BackgroundColor(long color) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Movie(BSTR __RPC_FAR *path) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Movie(BSTR path) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_FrameNum(long __RPC_FAR *FrameNum) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_FrameNum(long FrameNum) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetZoomRect(long left, long top, long right, long bottom) = 0;
	virtual HRESULT STDMETHODCALLTYPE Zoom(int factor) = 0;
	virtual HRESULT STDMETHODCALLTYPE Pan(long x, long y, int mode) = 0;
	virtual HRESULT STDMETHODCALLTYPE PercentLoaded(long __RPC_FAR *__MIDL_0015) = 0;
	virtual HRESULT STDMETHODCALLTYPE FrameLoaded(long FrameNum, bool __RPC_FAR *loaded) = 0;
	virtual HRESULT STDMETHODCALLTYPE FlashVersion(long __RPC_FAR *version) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_WMode(BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_WMode(BSTR pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_SAlign(BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_SAlign(BSTR pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Menu(bool __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Menu(bool pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Base(BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Base(BSTR pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_scale(BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_scale(BSTR pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DeviceFont(bool __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_DeviceFont(bool pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_EmbedMovie(bool __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_EmbedMovie(bool pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BGColor(BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BGColor(BSTR pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Quality2(BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Quality2(BSTR pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE LoadMovie(int layer, BSTR url) = 0;
	virtual HRESULT STDMETHODCALLTYPE TGotoFrame(BSTR target, long FrameNum) = 0;
	virtual HRESULT STDMETHODCALLTYPE TGotoLabel(BSTR target, BSTR label) = 0;
	virtual HRESULT STDMETHODCALLTYPE TCurrentFrame(BSTR target, long __RPC_FAR *FrameNum) = 0;
	virtual HRESULT STDMETHODCALLTYPE TCurrentLabel(BSTR target, BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE TPlay(BSTR target) = 0;
	virtual HRESULT STDMETHODCALLTYPE TStopPlay(BSTR target) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetVariable(BSTR name, BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetVariable(BSTR name, BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE TSetProperty(BSTR target, int property, BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE TGetProperty(BSTR target, int property, BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE TCallFrame(BSTR target, int FrameNum) = 0;
	virtual HRESULT STDMETHODCALLTYPE TCallLabel(BSTR target, BSTR label) = 0;
	virtual HRESULT STDMETHODCALLTYPE TSetPropertyNum(BSTR target, int property, double value) = 0;
	virtual HRESULT STDMETHODCALLTYPE TGetPropertyNum(BSTR target, int property, double __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_SWRemote(BSTR __RPC_FAR *pVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_SWRemote(BSTR pVal) = 0;  
};


// This is our main Flash class
class CFlash 
{
public:
  
	// The constructor and deconstructor
	CFlash();
	~CFlash();
  
	// A constructor to init and load a movie
	CFlash(HWND hWnd, LPSTR szFilePath, LPRECT pRect, bool bLoop, bool bPlay);  

	// This loads a movie from a full path
	void LoadMovie(HWND hWnd, char *szMovie, LPRECT pRect, bool bPlay);

	// This turns pause on and off
	void TogglePause();
  
	// This stops the Flash movie
	void Stop();

	// This releases all of our COM pointers
	void CleanUp();

	// This returns the name of our movie
	string GetMovie() { return m_strMovie; }

	// This returns the total frames of the movie
	int GetTotalFrames() { return (int)m_totalFrames; }

	// This tells us if the movie is paused or not
	bool IsPaused()		 { return m_bPaused; }
	
	// This tells us if we are looping the movie or not
	bool IsLooping()	 { return m_bLoop; }


private:

	HWND m_hWnd;							// The window handle
	bool m_bPaused;							// The flag that says if we are paused or not
	bool m_bLoop;							// The flag that says if we are looping or not
	IShockwaveFlash *m_pFlashInterface;		// Our Flash interface pointer
	IUnknown *m_pControlInterface;			// The pointer for the Flash control
	CAxWindow m_flashWindow;				// The ATL window that will run our Flash movie
	string m_strMovie;						// The name of the movie
	long m_totalFrames;						// The amount of frames of animation in the movie
};


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


#endif


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// In this file we created our two classes to work Flash.  We could have made
// the CFlash more robust with more data access functions, but just to keep
// it simple I omitted more code.  Feel free to explore and add more 
// functionality.
//
// Flash is a registered trademark of Macromedia.  All rights reserved.
//
//
// © 2000-2006 GameTutorials	