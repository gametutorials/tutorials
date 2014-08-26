//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -      //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com	 //
//									 //
//	$Program:	SwitchStatement 				 //
//									 //
//	$Description:	Shows how to use a switch statement              //
//									 //
//***********************************************************************//

// In this lesson we show what a switch statement does and why we want
// to use it.  Basically, a switch statement is just a cleaner way to do
// a lot of if/else statements; however with some restrictions.  Let's say
// you want to check if a number is 12 different values, and depending on
// the value you do something different.  Well, you COULD do that with an
// if/else statement, but as you will see, a switch statement is much cleaner.
// In our demonstration of switch statements below we grab an integer from
// the user and then display different messages depending on what they
// type in.  We also do the same thing for a single character, like 'a'.
//
// Check out the code below:

// Import the Scanner class so we can use nextInt() to read in user input
import java.util.Scanner;

// Create our class for the Main program
public class Main
{
    // Start our main() function for our program
    public static void main(String[] args)
    {
        // Prompt the user to enter a number from 1 to 3
        System.out.println("Input a number between 1 and 3: ");

        // Below we create a Scanner object and read in an integer from the user
        Scanner scan = new Scanner(System.in);
        int number = scan.nextInt();

        // The code below is a switch statement.  Basically, what it does
        // is do a bunch of if() statements inside.  In English, the code below is
        // saying, "Check the value of number, if it's a 1, say so, if it's
        // a 2, say so, if it's a 3, also say so.  However, if it's none of these,
        // then tell the user they didn't follow directions."

        // You put the variable you want to evaluate between the parenthesis (<here>).
        // Then you create a new scope (The { } ).  The "case" keyword is the same
        // thing as saying, "If number's value equals 1".  Or, relating to an if() statement:
        // "if(number == 1)". We put a colon after the case.  Now, if the value of "number:
        // is 1, it will do everything after the case 1: and between the "break";
        // In this case, we just print to the screen that they typed the number 1, and then
        // we "break" out of this switch statement.  What the "break" does is leave the
        // scope of the switch statement.  So, it will go to the next line AFTER the ending '}'.
        // If we didn't have a "break" it would go to the next line, which happens to be "case 2:".
        // Once one case is evaluated to be true, then it continues to do the ones after that
        // unless there is a break. This is strange, but useful, as seen in an example below this one.

        // If none of the statements are evaluated to true, then it goes down to the "default".
        // You don't have to have a default, but it's usually a good idea.

        switch (number)							// Check the value of "number"
        {								// The beginning of the switch statements scope
            case 1:							// If number equals 1, say so
                System.out.println("You typed in the number 1!");
                break;							// Stop this and leave the switch statement

            case 2:							// If number equals 2, say so
                System.out.println("You typed in the number 2!");
                break;							// break out of the switch statement

            case 3:							// If number equals 3, say so
                System.out.println("You typed in the number 3!");
                break;							// break from this switch statement

            default:							// If the number isn't 1, 2 or 3, complain :)
                System.out.println("You didn't follow directions!");
                break;            
        }								// This is the END of the switch statements scope

        // We didn't need the last break in the default section, but I like to for consistency.
        // The code would continue past the switch statement anyway since it was the last section.
        
        // Below, we will show you an example of checking characters...

        // Now let's ask the user for a character.  Notice that we put
	// the "\n" characters twice in the beginning of our string.  This 
        // stands for "new line" and it will move the cursor down two lines from 
        // the previous text printed out to the Output window.								
        System.out.println("\n\nInput a character: ");

        // Now we will do something a bit different to grab a character from the user.
        // First, we will use the next() function to grab a string from the user.
        // But, instead of wanting a word or a sentence, we just want the a single 
        // character.  Since there isn't a nextChar() function in the Scanner object,
        // we need to just grab a String, then use the charAt() function to get the
        // first character of the string.  The charAt() function takes a parameter that
        // is the index into the string that we want to grab.  The String object is
        // an array of characters, and arrays have an index for ever element in the
        // array.  For instance, say we have a String with the value:  "Hello!".
        // The 'H' character is actually index 0, 'e' is index 1, 'l' is 2, etc...
        // The first index in an array is actually 0, not 1, so remember that.  It's
        // kinda tricky.  So, in order to grab the first index of the array, we use
        // this syntax:  strInput.charAt(0);  That returns the first character in the string.
        
        // Grab the string from the user (most likely just one character)
        String strInput = scan.next();		
        
        // Now we grab just one character, the first index into the string (0).
        char ch = strInput.charAt(0);

        // Now let's do a switch statement on a single character.
        switch (ch)							// Check the value of "ch"
        {								// The beginning of the switch statements scope
            case 'a':							// If ch equals 'a', say so
                System.out.println("\nYou typed in an A!");
                break;							// Stop this and leave the switch statement

            case 'b':							// If ch equals 'b', say so
                System.out.println("\nYou typed in a B!");
                break;							// break out of the switch statement

            case 'c':							// If ch equals 'c', say so
                System.out.println("\nYou typed in a C!");
                break;							// break from this switch statement

            default:							// If ch isn't 'a', 'b' or 'c', let us know
                System.out.println("\nYou did not type an a, b or c!");
                break;							// Once again we don't need this break, but it doesn't hurt.											
        }

        // Well, you will notice that the last example only checks for LOWER CASE characters.
        // We don't want to have to make a whole new case for lower and upper case characters,
        // so here is what we do: we just add another case right after the first one, separated
        // by a colon ':'.   We can do as many as we want.  We can also do it on each line, like this:
        // case 'a':
        // case 'A':
        //	System.out.println( etc... );

        // Let's try this again--prompt the user and grab a key.
        System.out.println("\n\nInput a lowercase or uppercase character: ");
        
        // Just like the first time above, let's grab a string from the user
        // and then just grab the first index into the string.
        strInput = scan.next();		
        ch = strInput.charAt(0);

        switch (ch)						// Check the value of "ch"
        {							// The beginning of the switch statements scope
            case 'a':
            case 'A':				        	// If ch equals 'a' or 'A', say so
                System.out.println("\nYou typed in an A!");
                break;						// Stop this and leave the switch statement

            case 'b':
            case 'B':					        // If ch equals 'b' or 'B', say so
                System.out.println("\nYou typed in a B!");
                break;						// break out of the switch statement

            case 'c':
            case 'C':					        // If ch equals 'c' or 'C', say so
                System.out.println("\nYou typed in a C!");
                break;						// break from this switch statement

            default:						// If ch isn't a, b or c, let us know
                System.out.println("\nYou did not type an a, b or c!");
                break;						// Once again we don't need this break, but it doesn't hurt.											
        }	
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// Switch statements are a LOT cleaner than if() statements when is comes to checking
// more than 2 or 3 values.  It is customary to use a switch statement when you have
// over 2 values to check, otherwise if() statements can get too messy.  There is some
// restrictions on switch statements though.  One of them is that it can't do strings.
// You can not do :  switch("Test") or switch(strName).  The reason is because a
// String is an array, and switch() statements just check a constant value.  Switch()
// statements can't do floating point numbers either, so no: switch(22.2).  This is
// unfortunate too.  The last thing that restricts switch statements is that you can't
// use expressions with the "case", such as:  case (number != 2):
//
// Otherwise, switch statements are great!  Try some for yourself.  I take
// mine with sugar ;)
//
// One last note on what we did with grabbing a character.  We did this:
//
// String strInput = scan.next();
// char ch = scan.charAt(0);
//
// That grabs a string from the user, then only takes the first character they typed
// in.  Most likely, since we only ask for a character the user will only type one
// character, but that character will still be stored into the String object and
// we can grab it by using charAt(), which takes an index into the String to grab.
// We need to do this because the Scanner class doesn't have a function called nextChar().
// It seems to have almost everything else, strange huh?  It has a nextByte(), but that
// gives warnings when you use it.
//
//
// © 2000-2006 GameTutorials
