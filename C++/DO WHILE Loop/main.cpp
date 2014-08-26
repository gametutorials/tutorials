// Done by TheTutor

#include <conio.h> // So we can use _kbhit()
#include <iostream>
using namespace std;


/*	This tutorial is going to cover the do while loop.  Do while loops are very similar to
	while loops, the ONLY real difference is that do while loops are guaranteed to execute
	at least once.

	Do while loops look like this:

	do
	{
		// Execute lines of code
	
	} while(Some expression is true);	NOTE:  An example "while" would be 
												   while(x > 0);

	
	NOTE:  the command break, goto or return
		   will prematurely exit (stop the loop and begin at the first line of code
		   following the do/while loop) a do while loop.

*/

int main()
{
	// All we're going to do is repeatedly print out a line of text until 
	// the user presses a key.

	do
	{
		cout << "Still in the do...while loop" << endl;
	
	} while(!_kbhit());       /*	For starters what does _kbhit() do?  This is a function
								that returns TRUE if any key on the keyboard has been pressed
								and FALSE otherwise -- Because this is in a do/while loop
								the cout statement will execute at least once.

								The check to see if the loop should terminate happens at
								the END of the loop -- Unlike a normal while() loop where the
								check to see if the loop should terminate happens at the
								beginning of the loop */  

	return EXIT_SUCCESS; // The program terminated correctly

} // end of main()

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
