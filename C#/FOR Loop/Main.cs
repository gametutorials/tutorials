//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		ForLoop     									 //
//																		 //
//		$Description:	Shows how to use a "for loop" to cut down code   //
//																		 //
//***********************************************************************//
  
// This lesson explores the functionality of loops, particularly the "for loop".
// We all know what looping means right?  No, it's different from what your
// significant other does when they are lost trying to find a party :)
// Basically, in code it just means repeating a process.  For instance,
// what if you wanted to read in a bitmap image and display all the pixels
// to the screen?  What if the image was a huge image that had thousands of
// pixels in it, would you really write a line of code displaying every single
// pixel individually?  No, you would read the image header, find out the
// width and height of the image, then do a loop that would go through every
// pixel in the file and display it to the screen.
//
// A "for loop" would be good for doing things like handling tons of data
// like display an image, since we know how many loops we need to do with
// a number (like width*height).  Below we show something a lot simpler. We
// just ask the user to enter a minimum number to start at, then a maximum
// number to end at.  We then use a "for loop" to display all the numbers
// between the min and max numbers to the screen on a new line.
//
// Check out the code below:

// Use the System namespace so that we can use the "Console" object
using System;

// Create our class entitled "ForLoop" for our program
class ForLoop
{
    // Start our Main() function for our program
    static void Main()
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
        Console.Write("Input your first number to count from: ");
       
        // Then we wait until they press <enter> and read what they typed in.
        // Remember like we did in the previous tutorial, we first need to
        // convert the string into an integer since ReadLine() returns a string.
        string strInput = Console.ReadLine();
        min = Int32.Parse(strInput);					

        // Now we prompt the user to input a number for the max
        Console.Write("Input your last number to count to: ");
        
        // Then read in the maximum number to count to and store it in the variable "max".
        strInput = Console.ReadLine();
        max = Int32.Parse(strInput);

        // Now here comes the loop:
        // This is called a "For Loop".  You will use these a million times.
        // Keep in mind that there are essentially three lines of code between "for(" and ")".
        // Each line is separated by the semicolon.  This is normal for "for loops".
        // Ok, here is what it does, it gets a starting point:
        //                "i=min"     "i" is used as a counter
        // "i" now equals min, let's say we typed in 10.
        // This is the same thing as saying "i=10", but we don't
        // know WHAT the number is so we hold it in a variable: min
        //				  "i <= max"	This says, keep looping until this condition is false. 
        // In this case, "Keep looping until i is greater than max."
        // Let's pretend that max is 15.  The loop will quit when i = 16 or over.
        // You might be thinking, why would i = 16 or over?  "i" equals "min" (let's say 10).
        // Well, the next parameter passed into the "for loop" is where we say what happens to 'i'.
        //				   "i++"		This tells the compiler to add 1 to "i" each time through.
        // This is the same thing as saying : "i = i + 1", It's just shorthand.  The loop in words:
        //
        // for(start; condition for the loop; after we go through the loop each time - do this)
        //
        // You'll also notice we don't have a ";" after the "for" loop...
        // Good 'i'! :)  if we put a semicolon after this, the loop would never run.
        // That is because a ";" says we are done with the line, but we ARENT... 
        // We never put a ";" after anything with a scope after it .. look at "int main()".
        // So, everything in our scope {} will get executed each iteration of the loop.
        for (i = min; i <= max; i++)	
        {
            // Here we print out "i" each time through the loop and go to the next line.
			// Just like we did in previous tutorials, the "{0}" characters in a string
			// tell the function like Write() and WriteLine() that we are inserting a
	        // variable's value into the string, so it will replace the "{0}" with the value.
            Console.WriteLine("{0}", i);		
        }							

        // You might be wondering what this does?  If so.. let's go through the loop:
        // Let's stick to our previous example of min=10 and max=15:
        //
        //		for(i=10, 10 <= 16, 10 + 1 (only if middle condition is not met, so 'i' is still 10) )
        //		{
        //          // i is 10 right now (the first iteration of the loop)
        //			Console.Write("{0}\n", i);		
        //		}
        //
        // That is one time through the loop.  When it hits "}" .. the loop executes the 3rd statement,
        // which happens to be "i++".  Now, i = 11 right? we just added 1 to i which was 10.
        // The compiler never goes back to the first parameter which was "i=min". That was just to start "i" off.
        // So, after the first loop, and we add 1 to "i", the compiler goes and checks the middle statement
        // to see if the loop should continue.. So:
        //				"11 <= 16"  ... It's still true, so we go through the loop again.
        // Now we print out 11, then "12 <= 16" , then print 12, etc...
        // until we print out "16".  After the loop ends, 16 gets 1 added to it.  Now i = 17.
        //				"17 <= 16"  ... This is false, so the loop quits and the code 
        // goes past the loop and past the "}".  Since there is nothing else in the program, the program is over.
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// Pretty simple at first right?  Well if not, the for loop syntax is unlike 
// most other things in C/C++/C# so it's a weird site at first.  Basically, what
// a for loop does it just loop through some code until it reaches a false
// statement.
//
// for( <initialize a variable>; <give a logical statement>; <change the looping variable>)
// {
//      // Do some stuff in the loop
// }
//
// For example:
//
// for( int i = 0; i < 10; i++)
// {
//      Console.WriteLine("The variable i = {0}", i);
// }
//
// That "for loop" above will print out 0 through 9 to the screen, with each
// number on a new line.
//
// You want to use "for loops" when you know a beginning and end to your loop;
// however, when you want to do a loop that is dependent on something that isn't
// so quantatative like increasing or decreassing numbers you will want to use
// a "while loop".  For instance, you want to loop a certain song playing in your
// program or game until the user presses the <ESCAPE> key.  A for loop isn't
// appropriate, since there is not counting or anything involved, just waiting
// for input from the user.
//
// The next tutorial goes over how to use "while loops".  Check it out after
// you feel confident in using the basics of a "for loop".
//
// 
// © 2000-2006 GameTutorials
