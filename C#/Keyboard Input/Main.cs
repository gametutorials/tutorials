//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		KeyboardInput									 //
//																		 //
//		$Description:	Reads in a name and prints it to the screen      //
//																		 //
//***********************************************************************//

// This tutorial shows you how to read in input from the keyboard.  For
// instance, if you want to ask the user a question and then store what they
// typed in, you will need to use the ReadLine() function.  Just as a simple
// example, we created a function that asks the user's name, then says hello
// to the person who typed in their name.  Let's go over each line and review 
// old concepts and explore new ones.

// Remember, this allows us to use the "Console" object that holds the
// text input and output functions like WriteLine(), Write() & ReadLine().
using System;

// Here we create our class for this application.  It seems fitting that
// we should probably call our class the same thing as our application.
class KeyboardInput
{   // Remember, this line starts the scope for our class.
    // This is our Main() function that runs our application.
    static void Main()
    {   // This starts the scope for our Main() function.
        
        // Now, the next couple lines are a bit different from the previous
        // tutorial.  Notice that we use Write() instead of WriteLine().
        // What's the difference?  Well, WriteLine() actually writes a line
        // of text and moves the cursor to the next line, whereas Write() just
        // prints text to the screen and does NOT move the cursor to the next
        // line.  The first line asks the user a question:  
        Console.Write("What is your name?: ");

        // This next line does something tricky.  We want to now read in the
        // user's name.  We do this with the ReadLine() function.  Notice that
        // we use Console.ReadLine() to call the function below, since ReadLine(),
        // like Write() and WriteLine(), is a part of the "Console" object.  
        // Also, you might be confused at how we call ReadLine() below, but we
        // actually pass in the result from ReadLine() into the WriteLine()
        // function in the second parameter.  To separate parameters you use a
        // comma.  See how the first parameter into the function is the string
        // that says "Hello, {0}!"?  Then there is a ',' character after the string,
        // telling us we want to pass in another parameter.  In WriteLine() you don't
        // have to pass in more than one parameter, but we want the text to say
        // "Hello YourName!" with your name in there.  To do this we want to insert
        // the name the user types in into our string of characters.  This is done
        // by putting a "{0}" in our string where we want the name to appear, then
        // passing in the string that we want to appear in the second parameter.
        // ReadLine() waits until the user hits "enter" on the keyboard, then stores
        // all the characters entered and returns a string.  That string is then
        // passed into WriteLine() as the second parameter.  So basically, this 
        // WriteLine() function never really starts until ReadLine() finishes.
        Console.WriteLine("Hello, {0}!", Console.ReadLine());
    }   // This is the end of the Main() function's scope
}       // This is the end of the program class' scope


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// In this tutorial we learned about a couple new functions: Write() and ReadLine().
// Write() does exactly the same thing as WriteLine(), except the cursor stays
// on the same line as the text.  ReadLine() waits until the user enters input
// through the keyboard and the "enter" key is pressed.
//
// Just so you understand, these lines below do the same thing:
//
// Console.WriteLine("Hello!");
// Console.Write("Hello!\n");
//
// In C# if you have the '\n' characters in a string (between quotes) it tells
// the cursor to go to the next line.  It stands for "new line".  So if you did
// this it would move the cursor down two lines:
//
// Console.WriteLine("Hello!\n");
//
// We also learned in this tutorial how to insert strings into other strings
// when using WriteLine() or the Write() function.  You want to put a special
// set of characters {0} into the first parameter string.  The number between
// the curly brackets will correspond with the parameter number passed in, minus
// two.  For example, we use {0}, which matches up with the data that was passed
// into the second parameter.  Where, if we wanted a first and last name typed
// separately we would do this:
//
// Console.Write("What is your name?: ");
// Console.WriteLine("Hello, {0} {1}!", Console.ReadLine(), Console.ReadLine());
//
// Notice that the user can now type in a first name, hit enter, then type in a
// last name and hit enter, which will then print out "Hello FirstName LastName!".
// In the first parameter string we see a {0} and {1}, which is where the string
// for the second and third parameter will be inserted.  If we would have just put
// two {0} characters in the string it would just print out the first name twice,
// like: "Hello Tommy Tommy!", even though the user typed in "Tommy" and "Delora".
// Test yourself with the code doing your own questions and obtaining input from
// the user.
//
// The next tutorial goes over using "if/else statements" to control the flow of
// the program.  After practicing this tutorial's concepts go check it out.
//
// 
// © 2000-2006 GameTutorials