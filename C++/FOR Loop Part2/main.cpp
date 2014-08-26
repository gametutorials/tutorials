// Done by TheTutor

/*
	So what exactly is a for loop?  Well a for loop is nothing more than a way to tell
	the computer to do something for a specific number of times.  For loops are great for 
	doing something in a program "count" number of times.

    So lets say you wanted to print out "Hello World" 10 times.  Well, you could do the
	following:

	int main()
	{
		// Display to the screen the text "Hello World" then carriage return (go down to the 
		// next line) -- "endl" is what "carriage returns"
		
		cout << "Hello World" << endl; 
	    cout << "Hello World" << endl;
		cout << "Hello World" << endl;
		cout << "Hello World" << endl;
		cout << "Hello World" << endl;
		cout << "Hello World" << endl;
		cout << "Hello World" << endl;
		cout << "Hello World" << endl;
		cout << "Hello World" << endl;
		cout << "Hello World" << endl;

			return 0;
	}

	But what if you wanted to print it 100 times???  That would be a lot of "copy 'n pasting".
	This is but one example of where a for loop would come in handy.

	So lets get on with the show :)

*/

// First we include this file "iostream" -- This is a "header file" that includes a bunch
// of "function definitions" that some smart people wrote for us to use in our
// programs.  Now USUALLY "header files" end in a .h -- But this is A NEW type of header file
// that is why afterward you have to include the funny line of code "using namespace std;"
// For the time being this doesn't really need to make any sense (it will later, I swear :)
// Just KNOW that you need to include the following two lines of code to be able to print
// to the screen.

#include <iostream>  // So we can use "cout"
using namespace std;

// **NOTE** in case you're using an older compiler the above two lines CAN be replaced by this:
// #include <iostream.h>  -- This is the older version of the "iostream header file" but 
//							 it will do what we want it to do just fine :) 


/* 
   This is the main function (the core of ALL "console" programs) -- Here we are going
   to return an int (integer) from it -- The rule is, if your program encounters an error
   you should return the integer 1, otherwise you should return the integer 0 to signify
   that everything went great!
*/
int main()
{
	// So what we want to do is print to the screen "Hello World" 10 times using a for loop

	int count; // This is an "integer" variable -- It will be used to count from 0 to 10 in
			  // our for loop
	
	// Below is the code that prints "Hello World" 10 times to the screen
	for(count = 0; count < 10; count++)
	{
		cout << "Hello World" << endl;
	}

	/* Okay now that you've seen it, lets break in down piece by piece :)
	
	for(count = 0; count < 10; count++)
	 |
	 |__ The keyword "for" signifies the start of a for loop.  You will start ALL for 
		 loops with this keyword

    for(count = 0; count < 10; count++)
		    |
			|__ This is the "initialization part" of the for loop.  Here you will initialize
				any variables that you want to use in the for loop.  For this program 
				"count = 0;" sets the variable count to the value of zero.  You CAN HAVE
				for loops that don't initialize anything.  For example:

				for(; count < 10; count++) // Notice we still HAVE TO put a semi-colon

				So again this "first part of the for loop" is used to initialize variables.
				Since we are using C++ we could declare AND initialize a variable right in the
				for loop.  Example:

				for(int x = 45; x < 50; x++) // Here we declare an int 'x' and sets it's
											// initial value to 45 -- So the first time
										   // through the for loop, x will equal 45

    for(count = 0; count < 10; count++)
					    |
						|__ This is the "condition part" of the for loop -- A for loop will
							keep executing AS LONG AS this condition remains TRUE -- So for
							our example, as long as count is LESS THAN 10, all the code in
							the for loop (everything between the two {}'s) will keep executing

    for(count = 0; count < 10; count++)
								 |
								 |__ This is the "increment part" of the for loop -- Remember
									 for loops are designed to do "something" a certain number
									 of times, well this is where we update the count of how
									 many times we've done "something".  Here "count++" (Notice
									 NO semi-colon) adds one to count every time we reach the
									 BOTTOM (the } bracket) of the for loop.

	Now lets go through what happens when this program is run:

	First the variable "count" is created.
	Then we begin the for loop.  First AND ONLY ONCE count is set to zero.
	Then the comparison (count < 10) is made -- If count is LESS THAN 10 the lines
	of code in the for loop are executed (the code between the {}'s is executed).
	After the last piece of code INSIDE the for loop (between the {}'s) is executed,
	the line of code "count++" (which adds one to count) is executed.

	Then we go back to the top of the for loop and check the comparison (count < 10)
	If count is LESS THAN 10 (which it is because count only now equals 1) the code between 
	the {}'s is executed.  When ALL the code between the {}'s is executed, "count++" is 
	executed (we add one to count, thus it now equals 2).

	And the cycle keeps repeating itself until count equals 10 and therefore 
	IS NOT LESS THAN 10.

*/	

	return 0; // Return 0 to say "Our program terminated without any errors"
}

/*

  Well I hope that clears up for loops a little bit -- If you're still having trouble
  post your question at www.GameTutorials.com

*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/

  


