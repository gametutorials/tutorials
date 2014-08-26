// Done by TheTutor

/*	What are conditional statements?  Well they're statements (more like a sequence of characters)
    that stand for certain logical conditions.

	For instance, let's say you had a program where you had two dice.  You wanted to test
	to see if both dice were equal.  If both dice were equal, you wanted to allow the
	player to roll again.

	To program the above scenario, conditional statements (operators) are certainly going
	to make it much easier.  So I'll list the major ones below (placing them between quotes):

	"&&"	- This is the conditional AND operator.  It follows the rules of basic logic
			  If I said, "Bob and Billy are both male."  This sentence is ONLY TRUE
			  if Bob indeed is a male AND Billy is indeed a male.

	"||"	- This is the conditional OR operator.  It follows the rules of basic logic
			  If I said, "Bob or Billy is a male." This sentence is ONLY FALSE if
			  Bob and Billy are both NOT male.  If Bob is male the sentence is true.
			  If Billy is male the sentence is true.  If Bob and Billy are male, the
			  sentence is true.

	"!"		- This is the conditional NOT operator.  Again, it follows basic rules of logic.
			  If I said, "Stop signs are not blue."  This sentence is TRUE (at least in
			  my state anyway :)  Stop signs are NOT blue, they are red and white.
			  
			** Remember any value other than zero is considered "true". **
			  

	"=="	- This is the equality conditional statement (operator).  First off, this does NOT set
			  two variables to the same value.  This tests to see if two variables are the
			  same variable.  So if I had the code:

				if(2 == 2)	// This would be true

				if(2 == 4) // This would be false

	"<"		- This is the "less than" conditional operator.  It follows basic logic
			  So here's some examples:

				if(2 < 6) // This would be true

				if(6 < 2) // This would be false

				if(2 < 2) // This would also be false.  Two IS NOT less than two (it's equal).

	">"		- This is the "greater than" conditional operator.  It follows basic logic.
			  Examples:

				if(10 > 5) // This would be true

				if(3 > 7) // This would be false

				if(4 > 4) // This would also be false.  Four IS NOT greater than four (it's equal)


	"<="	- This is the "less than or equal" conditional operator.  It works exactly like
			  the "<" operator except now there is a small difference:

				if(2 <= 2) // This is a true statement.  Two is less than OR equal to two.

	">="	- This is the "greater than or equal" conditional operator.  It works exactly like
			  the ">" operator except now there is a small difference:

				if(4 >= 4) // This statement is true.  Four is greater than OR equal to four.

	"!="	- Last but not least, we'll talk about the "does not equal" operator.  This works
			  just like basic logic.  If I were to say, "Fifty dollars does not equal two dollars."
			  This sentence would be TRUE (if not I got $2 dollars I want to trade you).
			  

  Okay, now the code below doesn't do anything too spectacular.  We're just going to use
  all of these conditional statements so you get a better feel for how they work.
		


*/

#include <stdlib.h>	// so we can use "EXIT_SUCCESS"
#include <stdio.h> // so we can use "printf()"

int main()
{
	int num1 = 20;
	int num2 = 0;

	/* I'm not going to really comment anything here in the main().  Look 
	   at what is printed out, then look back at the code.  Try changing the numbers
	   and see if you KNOW what is going to be printed

		I suggest these combinations (but make up your own if you want :)

		num1 = 0;		num1 = -20;		num1 = 22;		num1 = 5;
		num2 = 0;		num2 = 0;		num2 = 22;		num1 = 15;

  */
	   

	if(num1 && num2)
		printf("%d AND %d is a true statement\n\n",num1,num2);
	else
		printf("%d AND %d is a false statement\n\n",num1,num2);

	if(num1 || num2)
		printf("%d OR %d is a true statement\n\n",num1,num2);
	else
		printf("%d OR %d is a false statement\n\n",num1,num2);

	if(!num1)
		printf("NOT %d is a true statement\n\n",num1);
	else
		printf("NOT %d is a false statement\n\n",num1);

	if(num1 == num2)
		printf("%d EQUALS %d is a true statement\n\n",num1,num2);
	else
		printf("%d EQUALS %d is a false statement\n\n",num1,num2);

	if(num1 < num2)
		printf("%d is LESS THAN %d is a true statement\n\n",num1,num2);
	else
		printf("%d is LESS THAN %d is a false statement\n\n",num1,num2);

	if(num1 > num2)
		printf("%d is GREATER THAN %d is a true statement\n\n",num1,num2);
	else
		printf("%d is GREATER THAN %d is a false statement\n\n",num1,num2);

	if(num1 <= num2)
		printf("%d is LESS THAN OR EQUAL TO %d is a true statement\n\n",num1,num2);
	else
		printf("%d is LESS THAN OR EQUAL TO %d is a false statement\n\n",num1,num2);

	if(num1 >= num2)
		printf("%d is GREATER THAN OR EQUAL TO %d is a true statement\n\n",num1,num2);
	else
		printf("%d is GREATER THAN OR EQUAL TO %d is a false statement\n\n",num1,num2);

	if(num1 != num2)
		printf("%d DOES NOT EQUAL %d is a true statement\n\n",num1,num2);
	else
		printf("%d DOES NOT EQUAL %d is a false statement\n\n",num1,num2);

	return EXIT_SUCCESS;	// Program ended successfully
}

// The condition for the tutorial ending has been met... 
	
// Okay you hopefully now have the LOGIC CONDITIONAL STATEMENTS (operators) down pat.
// As always, any questions, comments, concerns should be posted at www.GameTutorials.com.

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
