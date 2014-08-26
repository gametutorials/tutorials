// Done by TheTutor

/*  
	This purpose of this program is demonstrate how pointers are used.  This program
	consists of a one function that swaps the value of two integers passed into it.
	This "swapping" will be accomplished through the use of pointers.
*/

#include <stdio.h>
#include <stdlib.h> // Defines EXIT_SUCCESS

// We want to write a function that takes in two integers and swaps their values.
// As you can see there is a right and a wrong way to go about accomplishing this.

void SwapWrong(int num1, int num2); 
void SwapCorrect(int *num1, int *num2);

// Prints the values of num1 and num2 to the screen on separate lines
void PrintNumbers(int num1, int num2);

int main()
{
	int num1 = 52;
	int num2 = 44;

	printf("Initial values of num1 and num2\n");
	PrintNumbers(num1,num2);

	// This doesn't actually swap the numbers 
	SwapWrong(num1,num2);
	printf("\nAfter calling SwapWrong() on num1 and num2...\n");
	PrintNumbers(num1,num2);

	// This correctly swaps the numbers
	SwapCorrect(&num1,&num2);				/* What is with the & in front of num1 and num2?
											   Remember our function declaration looked like
											   this: void SwapCorrect(int *num1, int *num2);
											   That means you MUST pass pointers into this 
											   function.  But num1 and num2 are NOT pointers
											   Therefore, we put the & before them which
											   means "The address of".  Thus what is passed
											   into the function is the address in memory
											   of num1 and num2 AND addresses in memory ARE
											   pointers. */

	printf("\nAfter calling SwapCorrect() on num1 and num2...\n");
	PrintNumbers(num1,num2);

	return EXIT_SUCCESS; // Tells the OS that the program exited without any problems
}

void SwapWrong(int num1, int num2)
{
	// The intent of this function is to switch the values of num1 and num2.
	// Therefore, first we'll make a temporary number to store num1
	int temp = num1;

	// Now we'll store the value of num2 into num1
	num1 = num2;

	// Finally we'll put num1's value (which is saved in temp) into num2
	num2 = temp;

	/* That should do it.  Except you see when this function returns, the values have 
	   remained unchanged.  This is because of a very important aspect of functions.
	   Functions make local copies of the variables they are passed.  When num1 and num2
	   are created by the lines of code:

	   int num1 = 52;
	   int num2 = 44;

	   The program sets aside memory for these two variables.  When the function SwapWrong()
	   gets called a new and different chunk of memory is set aside for temporary copies
	   of num1 and num2.  You can change the values of num1 and num2 anyway you want.  When
	   the function looses scope (returns from the function) those temporary copies of num1
	   and num2 are destroyed.

	   But what if you wanted to change them?  This will be accomplished by SwapCorrect() */

} // end of SwapWrong(int num1, int num2)

void SwapCorrect(int *num1, int *num2)
{
	int temp;

	// The intent of this function is to switch the values of num1 and num2
	// Therefore, first we'll make a temporary number to store num1.
	// What does the * do???
	// If we were to printf() num1 you would see it's a long crazy looking number.

	// Try it by removing the comment from this printf():
	// printf("\nnum1 = %d\n",num1);

	// This is because num1 (inside of this function) is an address in memory.
	// We don't want the address of memory (which happens to be represented by a long number),
	// we want what is stored at that address of memory.  This is what the * accomplishes.
	// The * gets us what is stored at that address of memory.
	temp = *num1;

	// Now we'll store the value of num2 into num1
	*num1 = *num2;

	// Finally we'll put num1's value (which is saved in temp) into num2
	*num2 = temp;
}

// This is a function that prints out the values of num1 and num2 on separate lines.
// I knew I'd be printing out the values of these numbers more than once, so instead
// of typing the two printf() statements everywhere I can now just call this function.
void PrintNumbers(int num1, int num2)
{
	printf("num1 = %d\n",num1);
	printf("num2 = %d\n",num2);
}

// If you still feel like you could use a few more pointers (yes bad pun intended :) head to
// www.GameTutorials.com and jump on the forums.

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
