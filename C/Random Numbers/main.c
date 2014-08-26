// Done by TheTutor

// Random numbers are used in countless ways in all types of games abroad.  Luckily C gives
// a painless way to generate random numbers.  This tutorial will show you how to make random
// numbers (integers) and how to "seed the random number generator".  So enough of this random
// drivel, let's get coding.

#include <windows.h> // We include this header file so we can call the function GetTickCount()
#include <stdio.h> // Here we include "stdio.h" so we can use printf()

int main()
{
	int num1 = rand(); // Here is the mystical function that makes random numbers for us.
					   // The rand() function returns a random int.  It doesn't take any
					   // parameters and returns a "random" number between 0 and RAND_MAX (which
					   // happens to equal 32767).

	printf("A call to rand() gave us: %d\n\n", num1); // We'll print the int to the screen

	// So the next question is:  Okay that's great, I get a random int.  Now what if I want it
	// say, between 0 and 10?  How do I do that?  Well here is where we introduce a VERY HANDY math 
	// operation called the "mod operator".  This is the mod (modulus) operator: %
	// The way the % operator works is that it returns the remainder (fractional part) of a
	// division.  Example:

	// 5 % 2 = 1		5 divided by 2 is 2 and 1/2.  So as you can see 5 % 2 gives you 1,
	//					which is the NUMERATOR of the fractional part of the division (the 
	//					remainder part).

	// Let's do another example:

	// 39 % 6 = 3		So let's see what is 39 / 6.  Well, 6 goes into 39 six times, giving 
	//					you 36 with a remainder of 3.  Again, the remainder part (3) is what
	//					the mod operator gives you.

	// So how does this all tie into the rand() function?  Well it's really quite simple.  
	// To get a random number in a range (lets say 0 - 9) you mod rand() by the number 
	// ONE GREATER THAN the LARGEST number in the range.  That's quite wordy lets do an example
	// or two.

	num1 = rand()%15; // Here num1 will equal a number between (and including) 0 - 14

	// Print num1 to the screen to verify
	printf("rand()%15 = %d\n\n",num1);
	
	num1 = rand()%35; // Here num1 will equal a number between (and including) 0 - 34

	// Print num1 to the screen to verify
	printf("rand()%35 = %d\n\n",num1);

	// Okay cool, we can generate random numbers.  One problem... If you run this app 
	// you'll notice that the "random numbers" created are always the same!  This is because
	// you can think of rand() as going into a giant pool of numbers (array of numbers) and
	// giving you the first number in the pool the first time you call it, the second number
	// the second time, etc, etc.  What we need is a way to say, "Hey don't start at the beginning
	// of the pool, start here", so we get much more random numbers.  The function srand() gives
	// us that ability.  srand() "seeds the random number generator", giving us "truly random
	// numbers" EVERY TIME we run the application.  srand() essentially says, "Okay start
	// HERE in our pool of random numbers".  The problem is if we are always starting at
	// the same spot in our pool, it's no better than if we were just starting at the beginning.

	// So there are two main ways to specify a different starting spot (a different seed) and 
	// they both involve one thing that is constantly changing:  time.

	srand(GetTickCount()); // srand() will seed our random number generator.  The seed (offset)
						   // we pass it is specified by GetTickCount().  The GetTickCount 
						   // function retrieves the number of milliseconds that have elapsed 
						   // since the system was started (which is obviously always going to
						   // be different, which means our seed will always be different, which
						   // means we'll get some random numbers, yeah!)

	num1 = rand()%100; // Here num1 will equal a number between (and including) 0 - 99.
					  // If you keep running this app, this will be the ONLY number that
					 // will change, the others will stay the same (even though they were
					// technically randomly generated.  It's just they use the SAME algorithm
				   // each time they are created).

	// Print num1 to the screen to verify
	printf("rand()%100 = %d\n",num1);

	return 0; // Return 0 to say "The program exited successfully"
}

// Okay so a quick recap:

// To truly get random numbers be sure to call srand().  For the input (seed) to srand() you
// can use GetTickCount().  To get a random number between 0 and 9, you % (mod) the call
// to rand() with 10.  So in general to get a random number between 0 and X you do this:
//
// int rand_number = rand()%(X + 1);

// If you DO NOT want to use GetTickCount() there is another way to seed the random
// number generator (however GetTickCount() works the best).  Here's a sample program that does
// "seeding" another way:

/*

  #include <stdio.h>
  #include <time.h>
  using namespace std;

  int main()
  {
	srand(time(NULL));

	int x = rand()%255;

	printf("%d",x);

	return 0;
  }

*/


// As always, post any and all of your questions at www.GameTutorials.com.

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/