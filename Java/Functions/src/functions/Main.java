//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com	 //
//									 //
//	$Program:	Functions    					 //
//									 //
//	$Description:	Creates a custom function and executes it        //
//                                                                       //
//***********************************************************************//

// What we do in this tutorial is create a menu for the user to select options 
// from. You know how on a calculator there are things like square root, cosine, 
// sine, etc...?  Those are called functions.  Those functions allow us to do 
// things with just calling one function and not having to actually do the math 
// for the square root, cosine, etc.  Functions in programming are the same.  
// We create a function that does something, and then we can call that function 
// by just 1 line of code, not actually copying and pasting the code each time 
// we want to perform that function.  This makes code smaller, more readable and 
// less frustrating :)  In this tutorial we create a menu function that draws 
// the menu options.  Some functions take parameters and some return values.  
// For instance, square root takes a number to square, then it returns the result 
// of the square root right? : (double value = sqrt(49);) The "value" is the 
// return value, and the 49 is the parameter that is passed into the function.
// This is an example of using a function, however, in this tutorial we won't 
// use a input parameter.
//
// Below we have the code that will display a menu and ask the user for input.
// Depending on the menu item chosen, a message will be displayed, or if the
// user chooses menu item 6, then the program quit.  Check it out:


// Import the Scanner class so we can use nextInt() to read in user input
import java.util.Scanner;

// Create our class for the Main program
public class Main
{
    // If we want to create a new function we can do so inside of our class' scope.
    // Below we create a function called "DrawMenu()".  Notice that it's the
    // same way we have been creating our main() function.  We need to specify
    // that the function is "static" and our return type will be "void" because
    // we won't be returning data, just displaying the menu to the screen.
    public static void DrawMenu()								
    {											
        // Once inside our new function's scope, we can start coding what we
        // want to happen when this function is called.  Below we just do a
        // bunch of WriteLine's to display the menu all organized and clean.
        // The "\t" characters in a string stand for "tab".  They basically
        // create 5 space in the string instead of having to type 5 spaces
        // yourself.
        System.out.println("\t\t ****************Game Menu****************");
        System.out.println("\t\t *                                       *");	
        System.out.println("\t\t *   1) New Game                         *");
        System.out.println("\t\t *   2) Load Game                        *");	
        System.out.println("\t\t *   3) Save Game                        *");	
        System.out.println("\t\t *   4) Inventory                        *");
        System.out.println("\t\t *   5) Options                          *");	
        System.out.println("\t\t *   6) Quit                             *");
        System.out.println("\t\t *                                       *");
        System.out.println("\t\t *****************************************");
    }   // Now that we have finished coding our function we need to close its scope.

    // Start our main() function for our program
    public static void main(String[] args)
    {
        // Here we declare a type of "boolean". A boolean is usually a flag that holds 
        // a value: true or false (true is anything but 0, false is 0).
        boolean bStillPlaying = true;			
        int choice = 0;							
        
        // In this case, let's make a boolean and set it to true, meaning we
        // are "bStillPlaying" the game.  When Quit is chosen we will set our boolean to false, 
        // ending the game loop.  Once again, we put the 'b' in front of "StillPlaying" to show 
        // that it's a boolean.  This provides good readable code.

        // Let's create a while loop to continue until the user Presses '6' for quit.
        // The loop says to do everything in its scope until bStillPlaying == false.  Remember, 
        // a statement doesn't happen if 0 is the result from the expression.
        while (bStillPlaying)					
        {
            // All we need to do is call our function, like so.  This will draw the menu.
            DrawMenu();							
            
            // Since there are no parameters to pass in, we just closed the parenthesis and
            // added a semicolon.  That is how you "call" a function.			
												
            // Now comes the part where we need to be able to choose from the menu.
            // Just like in previous tutorials, display and prompt and convert our data to an int.
            System.out.println("Choose from the menu: ");                       // Prompt the user

            // Below we create a Scanner object and read in an integer from the user
            Scanner scan = new Scanner(System.in);
            choice = scan.nextInt();

            // Below we create a switch statement.  I chose to use this instead of if/else statements.											
            // For a quick review on switch statements, this is just a cleaner way to check values of a variable.
            switch (choice)
            {
                case 1: System.out.println("You chose a New Game!");		// if (choice == 1) print a message
                    break;					                // "break" breaks out of the switch statement 
                case 2: System.out.println("You chose to Load a Game!");	// if (choice == 2)
                    break;							// Print the choice and break
                case 3: System.out.println("You chose to Save a Game!");	// if (choice == 3)
                    break;							// Print the choice and break
                case 4: System.out.println("You chose your Inventory!");	// if (choice == 4)
                    break;							// Print the choice and break
                case 5: System.out.println("You chose Options!");               // if (choice == 5)
                    break;							// Print the choice and break
                case 6:
                    // bStillPlaying is set to false.  This means, when the code goes back up to the top, 
                    // while(bStillPlaying) will NOT be true and fail.
                    bStillPlaying = false;                                      // if (choice == 6)
                    System.out.println("Game over already?");                   // Print out the game over message

                    // A weird thing about switch statements is that you don't need 
                    // brackets {} for more than one line, You just need a "break".
                    break;									            
            }
            // It's a good idea to label what the closing bracket is coming 
            // from if you can't see the whole scope, like below:
        }   // while (bStillPlaying)							        
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// Doesn't this already feel like a game? :)  You are so close to making your
// own application that does cool things.  Basically, what happened in this
// program is that we just created a custom function in our class that displayed
// a simple menu, then do a while loop to probe for the user's input.  Depending
// on what the user chose, we displayed an appropriate message.
//
// When creating your own custom functions it's the same way you create your
// main() function:
//
// static void MyCustomFunction()
// {
//      // Do stuff in here
// }
//
// You will want to put the function in your program's class or else the program
// won't compile.  Unlike C/C++, you can't just put a function out in space.
//
// Finally, in order to call your function you just need to do this:
//
// MyCustomFunction();
//
// That's all there is to it, unless the function takes input/output parameters,
// but we will go over that in a later tutorial.  So go and create your own
// custom functions and make sure you understand the syntax before moving on.
// 
// Remember, that this tutorial does not handle error checking, so if the user puts
// in a 'q' instead of a 6 the program will not handle it correctly.  We will go
// over more on error checking later on.
//
//
// © 2000-2006 GameTutorials