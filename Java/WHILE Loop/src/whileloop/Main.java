//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com	 //
//									 //
//	$Program:	WhileLoop     					 //
//									 //
//	$Description:	Shows how to use a "for loop" to cut down code   //
//									 //
//***********************************************************************//

// This tutorial works from the ForLoop tutorial and does exactly the same
// thing, but just shows how to do it using a "while loop".  Check out the
// code below:

// Import the Scanner class so we can use nextInt()
import java.util.Scanner;

// Create our class entitled "WhileLoop" for our program
public class Main
{
    // Start our main() function for our program
    public static void main(String[] args)
    {
        // We initialize 3 integer (number) variables to store
        // our range.  We set them equal to zero or else they
        // will be some crazy number like -858993460.  We could
        // also have said:
        //					int min=0;
        //					int max=0;
        //					int i=0;
        // These are the same thing--it is just cleaner the other way.
        int min = 0, max = 0, i = 0;
        
        // Now we prompt the user to input a number for the min
        System.out.println("Input your first number to count from: ");
       
        // In order to read in an integer, let's create a Scanner object
        Scanner scan = new Scanner(System.in);
        
        // Read in an integer from the user
        min = scan.nextInt();					

        // Now we prompt the user to input a number for the max
        System.out.println("Input your last number to count to: ");
        
        // Then read in the maximum number to count to and store it in the variable "max".
        max = scan.nextInt();

        // Now here comes the loop.
        // This is called a "while loop".  You will use these a tremendous amount.
        // A "while loop" is very similar to a "for loop", except there is less going on.
        // "while ( <expression is true> ) do what ever is inside my brackets { }".
        // That is what the "while loop" is doing.  So in this case,
        // "while ( min is less than or equal to max ) { print out the value of min, then add 1 to min }
        // Let's say we chose 5 for min and 10 for max.
        // "while ( 5 is less than or equal to 10 ) etc....
        // This will print out 5, then 6, then 7, then 8, then 9, then 10 (it will print 10 because 10 
        // is less than or equal to 10 ).  Then when min becomes 11, 11 isn't <= (less than or equal to) 10, 
        // so it stops and doesn't do what's in the brackets, it skips them--no more looping
        while (min <= max)
        {
            // We print out the current value of min and skip down to a new line				        
            System.out.println(min);

            // Increment min by 1.  (remember, this is the same thing as :  "min = min + 1", just shorter.
            min++;		
			
            // Now the code will jump back up to the while() statement and check if the
            // expression is still true.  If it is it will continue running the code in
            // between its scope, otherwise jump down past this last bracket below.
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// Doesn't a "while loop" seem easier than a for loop?  I think so, but each
// loop has its own benefits.  Quickly reviewing, the demonstration we did in
// this program is actually more for "for loops"; however, just to show the
// difference of doing the same thing as the "for loop" tutorial we did it this
// way.  Ideally you want to use "while loops" for loops that aren't necessarily
// dealing with numbers, but for "flags" and "states" checks.  For instance,
// let's say you want to create a game and want to keep looping the game until
// the user hits 'Q' for "Quit".  You could create a "flag" that is of type
// "boolean".  That means that it can only be a "true" or "false" value, being
// a 1 for "true" and a 0 for "false".  Something like this:
//
// boolean bNotGameOver = true;
//
// while(bNotGameOver)
// {
//      bNotGameOver = HandleMainGameCode();
// }
// 
// So, let's say we created a function called HandleMainGameCode() that handles
// the main game and checks input from the user.  This function returns a boolean 
// that tells us if the game is still going over not (e.g. the user hit 'Q').
// Initially, we set the flag "bNotGameOver" to true, telling the "while loop" to
// keep looping the game.  So the HandleMainGameCode() function will keep returning
// "true" until the user hits 'Q', then it will return a "false".  When the "while
// loop" checks the flag the next time it will be "false" so it will stop looping.
// See how that works?  This is a more appropriate use of the "while loop".
//
// "boolean" is another useful type that you will want to use.  Usually you will see
// a 'b' in front of the variable so that you know that it's type "boolean".  You
// can either use the keywords "false" and "true", or just use a 1 or 0.
// Both of these are acceptable and the same meaning:
//
// boolean bFlag = 1;
// boolean bFlag = true;
//
// 1 = true and 0 = false.  
//
// Try creating a program that keeps asking the user for a password and if the password
// doesn't equal the secret password keep printing an error message to the screen.
//
//
// © 2000-2006 GameTutorials