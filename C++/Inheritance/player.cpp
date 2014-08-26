#include "player.h"

// Our constructors ---

// Our default constructor will set the player name to NULL.
// It will set life to 100 units and intellect and strength to 20 units.
// Since we a creating a player we want to start alive so we set alive to true.
CPlayer::CPlayer():mLife(100),mIntellect(20),mStrength(20),mAlive(true)
{
	// Clear the name to NULL
	mName.clear();
}


// An overloaded constructor will set the name, life, strength and intellect of the 
// player to values passed in.
CPlayer::CPlayer(const char *name, int L, int I, int S):mLife(L),mIntellect(I),mStrength(S),mAlive(true)
{
	mName.clear();

	if(name)
		mName = name;

} // end of CPlayer::CPlayer(const char *name, int L, int S, int I)


// Our CWizard class default constructor -- This set the spellPower to a value of 10.
// Everything else (like "mLife" for example) will get set by the default CPlayer constructor
// that WILL be called.
CWizard::CWizard():mSpellPower(10) { /* do nothing */ }

// Now this is our constructor that takes parameters --
// To set the CPlayer variables (such as mLife), we HAVE to EXPLICTILY call the CPlayer's constructor.
// Otherwise the default constructor for the CPlayer will get called and these variables WILL NOT get set.
CWizard::CWizard(const char *name, int L, int I, int S, int SP):CPlayer(name,L,I,S),mSpellPower(SP)
{ 
	/* do nothing */
}


/* **NOTE** WHAT IF WE WANTED TO HAVE A METHOD LIKE THIS:

void CWizard::decrementLife()
{
	mLife--;
}

  Well, if we attempted to program this, it wouldn't compile.  Remember mLife is a PRIVATE member
  of CPlayer.  ONLY CPlayer methods can directly access "mLife".  If there was a public function
  in CPlayer that let us modify the "mLife" variable, then we could modify it inside the implementation
  of CWizard methods.

*/
