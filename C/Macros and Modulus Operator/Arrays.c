//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Arrays										 	 //
//																		 //
//		$Description:	Print out random numbers with arrays and macros. //
//																		 //
//***********************************************************************//


// Welcome to a very important tutorial.  We will be discussing the topic of
// "arrays".  An array is just a list of data.  When you think of the word
// "array", think of how people say "what a beautiful array of colors".  For
// instance, a rainbow has an array of colors.  Or in other words, a list or
// group of colors.  When you think of a word or a phrase, you are actually
// looking at an array of characters right?  The same goes for programming.
// In this tutorial we create a random array of numbers and print them out
// to the screen.  An array's data is stored in memory right next to each other,
// so it isn't very useful for dynamic lists.  We will show you later how
// to work with types of lists.  Go through the comments and code below to
// see how to use arrays.


#include <windows.h>						// The windows header information
#include <stdio.h>							// We must include this file to use certain functions like printf(), etc..
#include <time.h>							// We need to include this to call the time() function

#define MAX_NUMBERS 15						// This is called a define.  A little review: usually the #defines go after the includes.
											// Typically, with defines.. they are ALL_CAPS and use underscores between words.
											// This let's the coder know that this is a define.  You might be asking, why use define?
											// How are they difference from variables?  Well, variables allocate memory.
											// The line "int i;" asks the operating system (OS) for some memory so that it could hold a number.
											// Defines on the other hand, don't take any memory, but instead REPLACE where ever it's typed with it's definition.
											// In this case, you notice we use MAX_NUMBERS in our for loop.  Before the compiler compiles the code, it "secretly"
											// goes and replaces the MAX_NUMBERS with 15.  Does that make sense?  It's literally as if we typed it our self.
											// In games and most any other coding, these are used a lot.  They are useful because ...
											// What if you use MAX_NUMBERS 100 times in your code?  What if want to change the number later?
											// All you have to do is go to the top of this file and change the 15 to what ever number you want.
											// If you had just typed 15 100 times throughout your code, you would have to change them one by one.
											// Even though there is a find and replace in most applications, trust me... it doesn't always work out
											// like you think it does.  If you have 50 different files for your project.. are you going to go and
											// find and replace in ALL 50 files?  It's SOoOoOooO time consuming a tedious.  Don't learn the hard way :)
											// You can also use defines like this:
											// "#define PRINT_SCORE(score) printf("Your score is %d.\n", score);
											// If I type PRINT_SCORE(<a variable>) anywhere, it will substitute the printf function and print the value of the
											// variable passed in.  Like this: PRINT_SCORE(<a integer>) printf("Your score is %d.\n", <a integer>);  
											// Do you get it?  It's like your own personalized printf function.  
											// The bad thing with defines are if you have a syntax error (you didn't follow the compiler's rules),
											// they are HARD to trace.  It could give you an error that you have no idea where it came from.
											// In fact, let's do a #define of PRINT_NUM();

#define PRINT_NUM(num) printf("%d\n", num)	// See what I mean?  All this does is print the number passed into as "num" (num isn't the name, just the word to replace in the printf function).
											// And go to the next line.  Simple huh?  You notice there isn't an "=" sign,...
											// This is because #defines are just replacing text, not assigning values.  #define are also called MACROS.
											// We have successfully made our first macro.
											// Don't put a ; after the macro, you will do that when you call (in other words, use) the macro.
									

int main()									// The beginning of our program
{											// This starts the main function
	int list[MAX_NUMBERS]={0}, i=0;			// Initialize the array of integers (numbers) and a counter "i".
											// This line says that we now have storage for MAX_NUMBERS (which is 15) integers.
											// If we didn't have arrays, we would have to do things like:
											// int i, i2, i3, i4, i5, i6, i7, i8, i9, etc... and who wants to do that?? (besides the freak who just raised his hand :) )
	printf("\nThe not so random list:\n\n");

	for(i=0; i < MAX_NUMBERS; i++)			// We call our for loop that will continue to print out MAX_NUMBERS (15) random numbers.
	{										// Start the cycle if the expression in the middle is true.
		list[i] = rand();					// list[i] means, the slot "i" (which will start out as 0) in list gets a random number.
											// Say, the random number came out to be 2200.  In the beginning, since i = 0,  list[0] would equal 2200;
		PRINT_NUM(list[i]);					// If list[i] was 2200, then PRINT_NUM() would print out 2200 to the screen.	
	}										// The end of the cycle, now check at the top to see if we need to continue.
		
											// The for loop will continue to go until i == 15.  Then it will stop.
											// Run this program again and you will notice that the numbers are the same every time.
	printf("\nThe TRULY random list:\n\n");	// They are different from each other, but the same every time.
											// We need to "seed" the random number generator. To do this, we use time.
											// The computers internal clock will always be changing, so we start the random number on our system's time.
	srand( (unsigned int)time(NULL) );		// srand() stands for "seed randomizer"  ... or something like that :)
											// We pass the function time() to srand() to seed the random generator. We pass in NULL to time() because we
											// don't care what format is returned.  However, we need to cast the return value to an unsigned int because
											// that is what srand() wants!

	Sleep(3000);							// This functions causes a delay for about 3 seconds.  1000 is ~ 1 second.  (~ means approximately)

											// Let's see that again...
	for(i=0; i < MAX_NUMBERS; i++)			// Now you'll notice the differences in numbers EVERYTIME ... or at least when the clock changes :)
	{											
		list[i] = rand();					// Randomize the new list
											
		PRINT_NUM(list[i]);					// Print out the randomized number.									
	}
											// Now we will restrict the random numbers so that we don't get numbers in the 10's of thousands.
											// Print the new title of numbers.
	printf("\nThe *restricted* TRULY random list:\n\n");

	Sleep(3000);							// Sleep for 3 seconds

	for(i=0; i < MAX_NUMBERS; i++)			// Now you'll notice the differences in numbers EVERYTIME ... or at least when the clock changes :)
	{											
		list[i] = rand() % 100;				// Randomize the new list with numbers less than 100.
											// using the modulus operator "%" we can restrict the random numbers to being less than 100.
											// Here is another review on how modulus works.  It's kinda like divide.
											// "  15 % 5 = 0,      15 % 7 = 1,    15 % 26 = 15 "
											// Can you see why?  It's the remainder.  15 / 5 = 3 with NO remainder, so 15 % 5 = 0
											// 15 / 7 has a remainder of 1.
											
		PRINT_NUM(list[i]);					// Print out the randomized number.									
	}

	return 0;								// The end of the program 
}											

// © 2000-2005 GameTutorials
