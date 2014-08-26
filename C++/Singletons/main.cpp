// Done by TheTutor

// If you've been programming in C++ for any length of time, you've probably heard
// somebody talking about "singletons".  So the first question that really needs to 
// be answered is "What in the heck is a singleton?".

// Well a singleton is quite simply an object (a class) that you can have one and
// only one instance of.  So say you had a class CVector.  If it was a singleton,
// then you could only declare one variable of CVector for the entire program.
// For a CVector class, this would be a pretty dumb thing to do 'cause chances are good that
// you will want more than one unique vector at a time in your program, but there are classes
// that you might want to ensure there is only one instance of it at all times.
//
// As we get going you'll see that singletons are really pretty easy to implement.
// So let's get this single party started.

#include "errorLog.h"

// Here's our simple main just to show, "hey, this really does compile" %)
int main()
{
	// Create the CErrLog
	// **NOTE** the :: (scope resolution operator).  You
	// call a static function in a class using this syntax
	CErrLog::create();
									
	CErrLog::getInstance()->openLog("log.txt"); // Open our log file
	CErrLog::getInstance()->logError("Beware of spiders"); // Print out an error message for fun
	CErrLog::getInstance()->closeLog(); // Close the log file, we're done with it

	// Last but not least, free up the singleton
	CErrLog::destroy();
	return 0;
}

// You've been singled out...

/*
	Singletons certainly have their uses especially in large programs, but if you are
	making smaller programs you probably won't need them.  However, using a singleton or
	two will certainly get you brownie points with the academia crowd %)

	Also you'll probably see slightly different implementations of a singleton class when looking
	at other's code.  This particular implementation, with the "create" and "destroy" methods, was
	chosen because it makes it easier to make your singleton thread safe in multi-threaded
	applications.
*/

// Questions/Comments ???  If so direct 'em to the forums at www.GameTutorials.com

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
