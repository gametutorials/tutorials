#include "input.h"

// This is an array of bytes that will store the state of the keyboard
BYTE keyboard[256] = {0};

// We're going to have to check the high bit and/or low bit of the bytes in our 
// "keyboard" array.  So we'll make these handy dandy defines to make it easier on us.
#define HIGH_BIT 0x80 // High bit is on (ie 10000000 in binary)
#define LOW_BIT 0x01 // Low bit is on (ie 00000001 in binary)

// This returns true if "key" (which MUST be a letter) is being pressed, false otherwise
bool KeyPressed(char key)
{
	// If it's NOT a letter (if it's not part of the alphabet), return false 'cause
	// we don't care about it
	if(isalpha(key) == false)
		return false;

	/*	
		This kickin' little function fills our array of bytes "keyboard" with
		the status of ALL the keyboard keys.  Each index into this array corresponds 
		to a virtual key code (a list of virtual key codes is at the bottom of this file)
	
		When the function returns, if the high-order bit of any of the bytes is 1,
		then that key is down.  Otherwise the key is up. 
	
		Now for "toggle keys" (such as the caps lock key) if the low-order bit is 1, the key
		is toggled (ie it's currently on).  The key is off and untoggled (ie NOT on) if the 
		low-order bit is 0.
	*/
	
	GetKeyboardState(keyboard); // Fill "keyboard" with the state of all the keys

	// All right we have our filled keyboard status array, now what?  Well we can't necessarily
	// just index directly into it.  First we need to determine if the "key" that was
	// passed in is a LOWERCASE key
	
	if(islower(key)) // If the key is lowercase
	{
		// If the shift key is down OR the caps lock is on -- They can't be pressing
		// the LOWERCASE version of "key"
		if((keyboard[VK_SHIFT] & HIGH_BIT) || (keyboard[VK_CAPITAL] & LOW_BIT))
			return false;

		// Now it just so happens that the virtual key codes and the ASCII value
		// for CAPITAL LETTERS (that's A - Z) are EQUAL.  So if we want to check
		// if this lowercase letter was pressed all we have to do is make it uppercase
		// and use it as an index into our keyboard array

		key = toupper(key); // Make it uppercase
			
		// Now this may look a bit (no pun intended) tricky -- But really it's not.
		// Here's what we are doing expanded out a bit (I can hear the groans...)
		//
		//	if(keyboard[key] & HIGH_BIT) // If the HIGH_BIT is set
		//		return true;
		//	else
		//		return false; // the HIGH_BIT is not set
		// 
		return ((keyboard[key] & HIGH_BIT) == HIGH_BIT);
	}
	else
	{
		// If we get here we know that "key" is an UPPERCASE LETTER.  We also know
		// that there's only two ways somebody can type a uppercase letter; Either
		// they are holding down the shift key or caps lock is on.

		// Here we'll check to see if indeed the key was pressed AND if the
		// shift key is down
		if((keyboard[key] & HIGH_BIT) && (keyboard[VK_SHIFT] & HIGH_BIT))
			return true; // They pressed the key and the shift key is down

		// Here we'll check to see if the key has been pressed AND if 
		// caps lock is currently on
		else if((keyboard[key] & HIGH_BIT) && (keyboard[VK_CAPITAL] & LOW_BIT))
			return true; // They pressed the key and caps lock is on

	}		

	return false; // "key" WAS NOT pressed
	
}

// And a list of the important virtual codes just for you
// "Missing codes" are most likely OEM (original equipment manufacture) specific so check
// out the website of the keyboard in question or MSDN for a full virtual code list

/*	
	Virtual Code	Hex Value			"Key"
	============	=========			=====

	 VK_LBUTTON			01			left mouse button  
	 VK_RBUTTON			02			right mouse button  
	 VK_CANCEL			03			control-break processing  
	 VK_MBUTTON			04			middle mouse button (three-button mouse)  
 
	 VK_BACK			08				backspace  
	 VK_TAB				09				tab  
	
	 VK_CLEAR			0C				clear  
	 VK_RETURN			0D				enter  

	 VK_SHIFT			10				shift  
	 VK_CONTROL			11				ctrl   
	 VK_MENU			12				alt 
	 VK_PAUSE			13				pause   
	 VK_CAPITAL			14				caps lock  

	 VK_ESCAPE			1B				esc  
	 VK_SPACE			20				spacebar  
	 VK_PRIOR			21				page up   
	 VK_NEXT			22				page down  
	 VK_END				23				end key  
	 VK_HOME			24				home key  
	 VK_LEFT			25				left arrow  
	 VK_UP				26				up arrow  
	 VK_RIGHT			27				right arrow  
	 VK_DOWN			28				down arrow  
	 VK_SELECT			29				select key  

	 VK_EXECUTE			2B				execute key  
	 VK_SNAPSHOT		2C				print screen key  
	 VK_INSERT			2D				insert key  
	 VK_DELETE			2E				delete key  
	 VK_HELP			2F				help key  
	 VK_0				30					0   
	 VK_1				31					1   
	 VK_2				32					2   
	 VK_3				33					3   
	 VK_4				34					4   
	 VK_5				35					5   
	 VK_6				36					6   
	 VK_7				37					7   
	 VK_8				38					8   
	 VK_9				39					9   
											
	 VK_A				41					a   
	 VK_B				42					b   
	 VK_C				43					c   
	 VK_D				44					d   
	 VK_E				45					e   
	 VK_F				46					f   
	 VK_G				47					g   
	 VK_H				48					h   
	 VK_I				49					i   
	 VK_J				4A					j   
	 VK_K				4B					k   
	 VK_L				4C					l   
	 VK_M				4D					m   
	 VK_N				4E					n   
	 VK_O				4F					o   
	 VK_P				50					p   
	 VK_Q				51					q   
	 VK_R				52					r   
	 VK_S				53					s   
	 VK_T				54					t   
	 VK_U				55					u   
	 VK_V				56					v   
	 VK_W				57					w   
	 VK_X				58					x   
	 VK_Y				59					y   
	 VK_Z				5A					z   
 	 VK_NUMPAD0			60			Numeric keypad 0 key  
	 VK_NUMPAD1			61			Numeric keypad 1 key  
	 VK_NUMPAD2			62			Numeric keypad 2 key  
	 VK_NUMPAD3			63			Numeric keypad 3 key  
	 VK_NUMPAD4			64			Numeric keypad 4 key  
	 VK_NUMPAD5			65			Numeric keypad 5 key  
	 VK_NUMPAD6			66			Numeric keypad 6 key  
	 VK_NUMPAD7			67			Numeric keypad 7 key  
	 VK_NUMPAD8			68			Numeric keypad 8 key  
	 VK_NUMPAD9			69			Numeric keypad 9 key  
	 VK_MULTIPLY		6A				Multiply key  
	 VK_ADD				6B				Add key  
	 VK_SEPARATOR		6C				Separator key  
	 VK_SUBTRACT		6D				Subtract key  
	 VK_DECIMAL			6E				Decimal key  
	 VK_DIVIDE			6F				Divide key  
	 VK_F1				70				f1 key  
	 VK_F2				71				f2 key  
	 VK_F3				72				f3 key  
	 VK_F4				73				f4 key  
	 VK_F5				74				f5 key  
	 VK_F6				75				f6 key  
	 VK_F7				76				f7 key  
	 VK_F8				77				f8 key  
	 VK_F9				78				f9 key  
	 VK_F10				79				f10 key  
	 VK_F11				7A				f11 key  
	 VK_F12				7B				f12 key  
	 	   
	 VK_NUMLOCK			90			num lock key  
	 VK_SCROLL			91			scroll lock key  
*/
