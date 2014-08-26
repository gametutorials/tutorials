#ifndef ERRORLOG_H
#define ERRORLOG_H

/*
	For our singleton we'll make a VERY limited error logger class.  This type of a class
	is a good candidate for a singleton 'cause you typically do not want multiple 
	instances of an error logger class floating around in your project.
*/

#include <fstream>
#include <iostream>
using namespace std;

// Here's our error logger
class CErrLog
{
	public:

		// This is the first singleton specific function.  We use this to create
		// the only instance of our class
		static void create();

		void openLog(const char *fileName); // This will open up our error log file
		void closeLog(); // This closes our error log file

		void logError(const char *err); // This adds "error text" to our error log file
		
		// This is the second singleton specific function.  It returns the
		// only instance of our CErrLog class.
		static CErrLog* getInstance() { return mInstance; }
		
		// This is the third "singleton specific" function.  It destroys
		// the instance of our class
		static void destroy();

	private:

		ofstream mFileOut; // This is our output stream.  We'll use this to 
						   // output text to a file.

		static CErrLog *mInstance; // Here we will hold the one true instance of our 
							   	   // class.  This is the ONLY instance of the CErrLog that
							       // will be allowed for the entire program.	

		/*
			Notice here we are doing something a bit odd.  We've made the constructor,
			copy constructor, assignment operator, and deconstructor all PRIVATE member functions.

			Have we lost our minds!  Nope, not at all.  You will see this in ALL singleton
			implementations, and this is a big part of what makes a singleton a singleton.

			Since all four of these are private, that means they can't get called by 
			any other file.  So for instance, if we were in main.cpp and tried to write this
			code:

			int main()
			{
			    CErrLog errLog; // The compiler would puke 'cause you are trying to call
							    // a private method (the constructor).
				return 0;
			}

			Thus, for our singleton we'll make the constructor, destructor, copy constructor
			and assignment operator private to ensure no other instances of the class
			are created.
		*/
		CErrLog() { /* do nothing */ }
		CErrLog(const CErrLog&) { /* do nothing */ }
		CErrLog& operator =(const CErrLog&) { return *this; }
		~CErrLog() { /* do nothing */ }
};


	
#endif
