
//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		IfStatement    									 //
//																		 //
//		$Description:	Displays different messages depending on an age  //
//																		 //
//***********************************************************************//

// This tutorial shows us how to use "if/else statements" to control the
// flow of our program.  For instance, if you wanted to ask someone's age
// and then display different messages to the screen depending on their
// age, you would need an if and else statement.  We do exactly that in this
// lesson.  We also go over a new concept called "variables".  Essentially,
// a variable is a way to store a value so that we can use or check it later.
// For example, we first want to ask the user's age and store the age in
// a "variable".  We then use the variable to check if the age is under or
// over a certain age so that we can display custom messages depending on age.
// Each variable has a type.  If you want to store a string of character, a
// whole number, a floating point number (a number with a decimal), etc.,
// then you need to specify the variable type BEFORE you use it.  Below are
// some of the basic variables that you will probably want to use:
//
// int     - whole numbers like 1, 2, 200, etc...
// float   - decimal numbers like 1.543, 354.6789, etc...
// double  - larger, more precise decimal numbers like 3.1598473234, etc...
// string  - character strings like "Don't call me Shirley, my name is Ed!"
//
// When using the ReadLine() function it returns a "string" of characters so
// we need to create a "string" variable to store the input.  However, if we
// want to get an integer value (int) like 65, then we need to convert the
// string to an integer.  We will show you how to do this in this tutorial. 
//
//Check out the if statements in the code and learn some new concepts.

// Use the System namespace so that we can use the "Console" object
using System;

// Create our class entitled "IfStatement" for our program
class IfStatement
{
    // Start our Main() function for our program
    static void Main()
    {
        // This is how we create a variable.  You first specify the type in
        // front, then a space, then the unique variable name.  So below we
        // create a variable named "age" of type "int".  Notice that we also
        // initialize the variable to "0" by saying "= 0" with a ';' at the end.
        // Remember that variable types and names are case sensitive so be careful.
        int age = 0;

        // Print to the screen a question about the users age
        Console.Write("How old are you? ");			

        // Next we want to read in the answer after the user hits return, but
        // ReadLine() returns a type of "string" so we need to create a string
        // variable to hold the user's age.  Just like when we created the "age"
        // variable up above, we specify a new variable called "strInput" of type
        // string.  When using some variable types it's a good idea to prefix
        // the variable name with the type, like "str" in "strInput".  That is
        // a common coding style.  You will see more styles for variables later.
        string strInput = Console.ReadLine();

        // Before we can get the integer value we need to convert our string
        // to an integer by using the Parse() function in the "Int32" object.
        // Just like the "Console" object, we use a '.' to access the object's
        // Parse() function.  We just need to pass in the "strInput" variable
        // that holds our input, which will then allow Parse() to return an
        // integer value that will be stored in our "age" variable.  Just like
        // in math class, you see a variable's values by using an equals sign.
        age = Int32.Parse(strInput);	

        // Now we want to start checking the value of age to see which messages
        // that we should display.  The syntax for an if statement is: if(<expression>)
        // Below we check if the age is greater than 20.  If so we display a
        // message commenting on the age.  Notice that we create a new scope.
        // If the statement is true, the code in the new scope is executed,
        // otherwise it is ignored and nothing happens.
        if(age > 20)							
        {										// Start of the if statement's scope
            // Print a message to the screen about being over 20 years old
            Console.WriteLine("You're over 20 huh?");
        }										// End the if statement's scope

        // The next if statement shows some different syntax when checking if
        // the age is over 30 years old.  See how we don't use a scope for this
        // if statement?  Technically, you only need to use a scope if the
        // code is over one line of code (or more than one semicolon statement).
        // It is customary to always indent with the tab key like shown below,
        // even though there isn't an expressed scope, since it is a new scope,
        // but you just can't see it :)

        // If the age is also over 30 then display another message too
        if(age > 30)							
            Console.WriteLine("You're over 30!?");	

        // If the user typed 35 as their age the program would print out both messages
        // above, But if they entered something like 25, it would only print the first one.

        // Below is another coding style...  people who put their '{' after the 
        // conditional statement.  This doesn't do anything different, it's just 
        // that some people prefer it.  There are some languages that REQUIRE it, 
        // but C/C++/C# are VERY flexible in this regard.  

        // If the user is less than 20 print a different message
        if (age < 20) {  							
            Console.WriteLine("You're a young'n!");
        }

        // Next we have something new.  The "&&" means "and".  So the statement is saying, 
        // "if ( age is less than 20 AND greater than 12)".  You will use this type of 
        // expression a lot.  There is also this: "!=" which means "does not equal" and "||"
        // which means "or".  So instead of checking if both things are true, you can check if 
        // one or the other are true by using "||"; the OR operator.  If you are familiar with 
        // the binary operators, "&", and "|" .... these are similar, but different.  "&&" and 
        // "||" check conditions between two statements.  
        
        // If the user is a teenager print a new message to them
        if(age < 20 && age > 12)				
        {										
            Console.WriteLine("Being in your teens can be tough...");
        }

        // This checks if "age equals 100".  This is different than "age = 100".  The
        // "==" is a boolean check that returns a "true" or "false", where the '=' is
        // an assigning operator just like you would expect in math class.
        if(age == 100)							
            Console.WriteLine("WOW!  What's your secret!?");
        
        // Now that we have gone over if statements, let's show how to do else statements.
        // If you want to check if the age is greater than 50, then print a message, but
        // print another message if the age 50 and under you can do the following:
        if(age > 50)							
            Console.WriteLine("Life after 50 can be great!");
        else									
            Console.WriteLine("You've still got a LONG way to go...");

        // In programming you will want to check if something is true, do something,
        // otherwise do something else.  The else statement works nearly the same
        // as the if statement  When you have if and else statements you will only
        // get one result, either one or the other.  You can even do an "else if(<exp>)"
        // statement that works just like an if statement.  As we go into more tutorials
        // you will see more examples on how to use these statements.
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// So we went over using variables and if/else statements.  Basically, to create
// a variable you use the syntax:
//
// <type> <name>;
//
// Like:
//
// int myNumber;
//
// If you want to initialize the variable you can say:
//
// int myNumber = 0;
//
// Or:
//
// string strName = "GameTutorials";
//
// Now, using if/else statements is just as simple:
//
// if(<expression>)
// {
//
// }
// else if(<expression>)
// {
//
// }
// else
// {
//
// }
//
// For example:
//
// if(number == 20)
// {
//    Console.WriteLine("It's 20!");
// }
// else if(number == 40)
// {
//    Console.WriteLine("It's 40!");
// }
// else
// {
//    Console.WriteLine("It's neither 20 or 40!");
// }
//
// What about if you want to check string values?  For example, say you want
// to check if the user typed in a "yes" or "no"?  Check this code out below:
//
// Console.Write("Do you want to borrow my Porsche for your date? ");
// string strAnswer = Console.ReadLine();
//
// if(strAnswer == "yes")
//      Console.WriteLine("Yah right, go make your own millions!");
// else
//      Console.WriteLine("What?  You're just Jealous???");
// 
// When we want to check strings we just need to enclose them in double quotes.
// It works just like numbers would, but with double quotes around the values.
//
// Be sure you are comfortable with using if statements.  You will use them
// everywhere in your code.  Once you have tried to create your own program
// with some if statements to practice the concepts, jump to the next tutorial
// shows you how to do loops; specifically "for loops".
//
//
// © 2000-2006 GameTutorials