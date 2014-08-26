//***********************************************************************//
//                                                                  	 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com	 //
//									 //
//	$Program:	KeyboardInput					 //
//									 //
//	$Description:	Reads in a name and prints it to the screen      //
//									 //
//***********************************************************************//

// This tutorial shows you how to read in input from the keyboard.  For
// instance, if you want to ask the user a question and then store what they
// typed in, you will need to use the next() function.  Just as a simple
// example, we created a function that asks the user's name, then says hello
// to the person who typed in their name.  When running the application you
// will see a prompt in the Output window of NetBeans, with an "Input" field.
// The program will wait for the user to type their name in the Input field,
// then the user must press enter for the rest of the program to finish.
// Let's go over each line and review old concepts and explore new ones.

// This next line allows us to use an object called Scanner.  With this object
// we can call a function that will read in input from the user.  The function
// is called next().  The "import" keyboard is similar to the "using" and
// "#include" keyword in C# and C++.  In Java, we call other files with classes
// as "packages".  So we are including the Scanner class in the java.util package.
// A way to include all the classes in a package is by putting a '*' character like
// this:  import java.util.*;
import java.util.Scanner;

// Here we create our class for this application.  It seems fitting that
// we should probably call our class the same thing as our application.
public class Main
{   // Remember, this line starts the scope for our class.
    // This is our main() function that runs our application.
    public static void main(String[] args)
    {   // This starts the scope for our main() function.
        
        // Just like in the first Java tutorial, let's print out some text to the
        // screen.  This line asks the user a question:  
        System.out.println("What is your name?: ");

        // Now we create a "variable".  A variable is a word that we create, which
        // represents a place in memory that holds information, such as a number
        // or string of characters.  Each variable has a type.  For using whole
        // numbers there is a type called "int".  For numbers with decimal numbers
        // we can use a "float" or a "double".  A "double" is more precise than a
        // "float".  Also, if we want to store a sentence or a word, we use the
        // "String" type.  Since we want to store the user's name, we will use a
        // "String" type.  To declare a variable, declare the type, then a word for
        // the variable.  We call our variable "strName".  By putting an "str" in
        // front it tells us that the variable is a string, but you can call it
        // whatever you want, except already defined keywords in Java.
        String strName;
        
        // Next, since we want to use the "Scanner" object to read in the user's name,
        // Java requires that we create a "Scanner" object.  We do this by using the
        // the "new" keyword.  The "Scanner" class requires that we tell it where the
        // input is coming from.  To tell the class that we want to check keyboard
        // input, we pass into the Scanner class' constructor (i.e. the init function)
        // the "System.in" class.  To store the new Scanner object we create a variable
        // called "scan".  Notice that it is of type, "Scanner".  
        Scanner scan = new Scanner(System.in);
        
        // Now that we have our scan object, we can call the next() function to
        // read the input from the user.  To access the function from within the scan
        // class, we just use the '.' operator, which connects the function to the
        // object.  To store what the user types in we set the "strName" variable to 
        // the String that is returned from next().  The equals sign '=' operator 
        // works just like the logic you learned in your math classes.
        strName = scan.next();
        
        // Now the variable "strName" should equal the name that was typed into the
        // "Input" field of the "Output" window.
        
        // Finally, let's print out a greeting to the user.  To do that we will use
        // the println() function, but we add the user's name to the message.  To add
        // a string to another string you just use the '+' operator.  By adding the
        // "Hello " string to the variable strName, it concatenates the strings
        // together and passes in the final string to println().
        System.out.println("Hello " + strName);
        
    }   // This is the end of the main() function's scope
}       // This is the end of the program class' scope


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// In this tutorial we learned about a new function: next().
// next() waits until the user enters input through the keyboard and the 
// "enter" key is pressed.  The next() function only stores a since word, and
// when it reaches a space or tab in the sentence the user typed in, it stops
// and just stores the first word.  Notice that if you type in your full name
// with a space, this tutorial only says "Hello <First Name>".  If you want
// to store a whole sentence, use nextLine().  Read further below for an example.
//
// In order to use the next() function we need to import the java.util.Scanner
// class.  Then, we need to create a new instance of the Scanner object.  That
// is done by the following line:
//
// Scanner scan = new Scanner(System.in);
//
// In order for the Scanner object to listen for keys from the keyboard, we pass
// in the System.in class into the Scanner's constructor (init function).  When
// creating new classes, sometimes they require or allow input into their constructor.
// Their constructor is a function that is called in the class every time the object
// is created.  Think of it as a init function.  In the Scanner object's case, the
// Scanner object needs a "stream" of data to parse from.  If you wanted, you could
// pass in a String variable instead of System.in, but we aren't parsing a string;
// we are parsing keyboard input.
//
// Then, to store the data from the keyboard we can use a line like this:
//
// String strWord = scan.next();
//
// If you want to store a whole sentence, use this:
//
// String strSentence = scan.nextLine();
//
// If we want to store a number instead of a string, you can do this:
//
// int number = scan.nextInt();
//
// or
//
// float number = scan.nextFloat();
//
// Finally, we also learned how to add string together.  This is done by using the
// '+' operator:
//
// String strNewString = strOldString + strOtherString;
//
// The next tutorial goes over using "if/else statements" to control the flow of
// the program.  After practicing this tutorial's concepts go check it out.
//
// 
// © 2000-2006 GameTutorials