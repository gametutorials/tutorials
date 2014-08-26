//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com        //
//									 //
//	$Program:	FirstJavaApp					 //
//									 //
//	$Description:	Displays "Hello World!" to the screen            //
//									 //
//***********************************************************************//

// What you are reading right now is what are called "comments".  Comments
// allow you to write descriptions and explanations of your code, but are
// ignored by the compiler.  The compiler is what actually takes all of our
// code and converts it into a library or program that you can run, like a 
// ".exe" file.  If you haven't already, please review the Beginner FAQ which
// explains basic programming vocabulary so you won't be lost when reading
// the tutorials.  There is a big section for using Java so it's important
// that you read it.  The FAQ will answer many questions about using Java.
// You can find it on the CD or at the below address:
//
// http://www.GameTutorials.com/beginnerfaq.htm
//
// To create comments in code you just use the "//" characters.  Notice that
// the text turns green by default, which lets you know that the compiler
// will ignore the text.  The "//" comments only create comments for one
// line of text, where as the using the "/*   */" characters comment multiple
// lines.  Look below for an example:

/*
   This is an example of commenting multiple lines without using the "//"
   characters.  Do you see how this text is green? 
*/

//
// Below we will show the source code for your first Java application.  You
// can read our comments next to the code to learn what each line of code
// does, and then apply those concepts to create your own programs.  After
// each tutorial we encourage you to create your own project from scratch
// and test the skills you learned to make sure you understand the concepts.
//
// Right before we jump into the code let's go over what is going on with
// this program.  When we run this program we will see the output in the
// "Output" window of NetBeans.  The output will say "Hello World!".  This will
// demonstrate how to print text using java, the easiest first program.
// Below is the actual code for our first Java application:


public class Main 
{    
    public static void main(String[] args) 
    {
        System.out.println("Hello World!");
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// That's all the code required to run your first Java application.  To run this
// program you will want to use the menu:  "Run->Run Main Project"
// You can also just use the quicker shortcut:  F6
// To just build the project and check for possible errors, use: F11
//
// Be sure to watch the step-by-step video tutorial that shows how to create
// a Java application.  Basically, you want to go:  "File->New Project...",
// then choose "Java Application" in the "General" folder.  Next, just name
// the project and click OK.  Now you have a project with a new file to start 
// coding in.  In order to just show the smallest amount of code possible to
// print text to the screen I got rid of a few lines of code from the default
// "Java Application" project that was created by NetBeans.
//
// We didn't stick comments in the code above so that you could see the code
// all by itself.  Keep in mind that all the code in Java is case sensitive, which
// means you must use upper- and lower-case characters where appropriate.  For
// example, "public class Main" and "Public Class main" is totally different code.  
// Now let's go through each line and explain what it does.

/*


// This line below defines our class.  Since the older languages like C and Pascal,
// Object-Oriented Programming (OOP) has become the new technique in programming.
// Instead of having tons of functions floating around wherever they want, we
// now include them in what we call a class (also called an "object").  In short,
// this gives us many benefits, namely better code organization and readability.
// As you go through the rest of the tutorials you will see some more advanced
// benefits to classes, but for now just think of them as ways to organize your code.
// Notice that we use the keywords "public class" in front, with our class name afterwards.
// The class name has the be the same name as the first part of the java file, "Main.java".
// This says that we are creating a class called "Main".  The "public" keyword says
// that the class is public, which means that any other class can use it.
public class Main

// This next line is a curly bracket and tells us that we are starting a new "scope"
// for our class.  The starting of a scope is a '{' character, with the end being a '}'.
// Think of this as being a beginning and an end statement.  Whenever you create a new
// scope you want to indent the next code with the tab key.  This gives us clean code,
// but will still compile and run fine if you don't tab in.  You could have the whole
// program on one line and it would still compile and run fine, but would make people
// want to vomit on your keyboard when they saw it :)
{
 
    // This next line actually starts our main() function for our program.  That means
    // that when our program starts this is the first function called.  The "static"
    // keyword is mandatory and says that the main() function will stay in memory until
    // the program closes.  The "void" keyword tells us that this function won't return
    // any data, it will just run and quit.  Some functions, like the square root function
    // on your calculator, will return a number right?  Well, our main() doesn't need to.
    // The "main" must be spelled exactly like that, with a lower-case 'm'.  The "()"
    // characters let us know that this is a function.  The "public" keyword makes the
    // main() function public so any other function or class can call it.  In between
    // the parenthesis we declare a variable of type "String", which hold any special
    // arguments that are passed into the program.  The "[]" characters mean that the
    // variable name "args" is an array of type "String" (or a list of Strings).  We
    // will go over variables, strings and arrays in later tutorials.  Don't worry
    // about this line, because later we will be explain more of these concepts.
    public static void main(String[] args)
 
    // Now we want to start a new scope for the main() function so we use a curly bracket.
    {
        // This is the line that prints out the text to the screen.  Notice that we need
        // to use the "System.out" class (or object) to access the println() function.
        // You can store functions in classes, and you just need to type a '.' character
        // after an object to start using its functions.  In NetBeans you will notice
        // that once you type in the period character a bunch of functions and class
        // variables will pop up.  This is called "IntelliSense", which helps you to easily
        // select functions or variables stored in the class.  You can choose to ignore
        // this and keep typing whatever you want.
        //
        // So, we call println(), which takes a string of characters (also called a
        // char string) to print out to the screen.  You must use the '"' characters to
        // surround the string that you want to "pass into" the println() function.
        // For example, this would error and not compile: System.out.println(Hello World!);
        // Notice that we pass the string in between the open and closed parenthesis.  To
        // end the line of code we put a semicolen after the last parenthesis.  Since you
        // can have many lines of code on one line, the ';' tells the compiler to process
        // the previous code before going on to the next code; stating that the previous
        // code and the next code are not connected.
        System.out.println("Hello World!");
 
    // This line ends the scope of the main() function.  Notice how it lines up with
    // the start of the scope.  This is how you line up scopes so it's easy to read.
    }
 
// This ends the scope for the entire "Main" class.  See how it lines up with
// the first open curly backet.  This is what good, clean code looks like.
}

*/

// That's all there is to writing a basic java application.  There were probably
// a lot of things that you might not still understand fully, but you should be at least
// able to create a program that prints text out to the screen.  The greater understanding
// will come later as you see more examples and explanations of the concepts used in the
// next tutorials.  
//
// What about if you want to print out many lines of text?  Good question, just call the
// println() function as many times as you need like this:

/*
 
   System.out.println("Hello World!");
   System.out.println("Hello World Again!");

*/

// Of course, you would put that code in between your main() function's scope (curly brackets).
// After practicing your new skills please move on to the next tutorial that discusses 
// "Keyboard Input".
//
//
// © 2000-2006 GameTutorials
