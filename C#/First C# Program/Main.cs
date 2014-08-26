//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		FirstCSharpApp									 //
//																		 //
//		$Description:	Displays "Hello World!" to the screen            //
//																		 //
//***********************************************************************//

// What you are reading right now is what are called "comments".  Comments
// allow you to write descriptions and explanations of your code, but are
// ignored by the compiler.  The compiler is what actually takes all of our
// code and converts it into a library or program that you can run, like a 
// ".exe" file.  If you haven't already, please review the Beginner FAQ which
// explains basic programming vocabulary so you won't be lost when reading
// the tutorials.  You can find it on the CD or at the below address:
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
// Below we will show the source code for your first C# application.  You
// can read our comments next to the code to learn what each line of code
// does, and then apply those concepts to create your own programs.  After
// each tutorial we encourage you to create your own project from scratch
// and test the skills you learned to make sure you understand the concepts.
//
// Right before we jump into the code let's go over what is going on with
// this program.  When we run this program we will see a console window
// come up (a DOS-style window) that will say "Hello World!".  This will
// demonstrate how to print text to the screen, the easiest first program.
// Below is the actual code for our first C# application:

using System;

class FirstCSharpApp
{
    static void Main()
    {
        Console.WriteLine("Hello World!");
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// That's all the code required to run your first C# application.  To run this
// program you will want to use the menu:  "Debug->State Without Debugging"
// You can also just use the quicker keyboard shortcut:  Ctrl+F5
//
// Be sure to watch the step-by-step video tutorial that shows how to create
// a C# application.  Basically, you want to go:  "File->New->Project",
// then choose a C# "Empty Project".  This way you don't get uneeded files added
// to your project.  Then in the "Solutions Explorer" window just right click on
// the new project and choose "Add->New Item...".  Finally, choose "C# Code File"
// and type in your new .cs file name.  The .cs file format is for CSharp code
// files.  Now you have a project with a new file to start coding in.
//
// We didn't stick comments in the code above so that you could see the code
// all by itself.  Keep in mind that all the code in C# is case sensitive, which
// means you must use upper- and lower-case characters where appropriate.  For
// example, "using System" and "Using system" is totally different code.  Now 
// let's go through each line and explain what it does.

/*

// This line below allows us to the the "Console" object, which has the WriteLine()
// function.  This function is what prints text to the screen.  If the word
// "function" is confusing for you, just remember when you use the square root
// function on your calculator (sqrt()).  It's the same thing.  A function is
// just a way to perform an already programmed process quickly.  If you have
// programmed in C/C++ you can think of "using System" as "#include <system.h>";
// however, what is actually going on is that you are specifying a namespace.
// Don't stress about what a namespace is right now, but just know that if you
// didn't include "using System" at the top you would have to put "System." in
// front of every object that is in the "System" namespace, like this:
// "System.WriteLine("Hello World!");
// Notice that the word "using" is highlighted blue by default.  That tells us that
// that word is a "keyword" understood by the compiler for C#.
using System;

// This line below defines our class.  Since the older languages like C and Pascal,
// Object-Oriented Programming (OOP) has become the new technique in programming.
// Instead of having tons of functions floating around wherever they want, we
// now include them in what we call a class (also called an "object").  In short,
// this gives us many benefits, namely better code organization and readability.
// As you go through the rest of the tutorials you will see some more advanced
// benefits to classes, but for now just think of them as ways to organize your code.
// Notice that we use the keyword "class" in front, with our class name afterwards.
// This says that we are creating a class called "FirstCSharpApp".  You can name the
// class pretty much whatever you want, except using some characters like {,},',",(,)
// *,^,%,$,#,@,!,~,`,|,&, etc.  Pretty much most punctuation characters, except the '_'
// character.  This goes for any function or data type names.
class FirstCSharpApp

// This next line is a curly bracket and tells us that we are starting a new "scope"
// for our class.  The starting of a scope is a '{' character, with the end being a '}'.
// Think of this as being a beginning and an end statement.  Whenever you create a new
// scope you want to indent the next code with the tab key.  This gives us clean code,
// but will still compile and run fine if you don't tab in.  You could have the whole
// program on one line and it would still compile and run fine, but would make people
// want to vomit on your keyboard when they saw it :)
{
 
    // This next line actually starts our Main() function for our program.  That means
    // that when our program starts this is the first function called.  The "static"
    // keyword is mandatory and says that the Main() function will stay in memory until
    // the program closes.  The "void" keyword tells us that this function won't return
    // any data, it will just run and quit.  Some functions, like square root functions
    // on your calculator, will return a number right?  Well, our Main() doesn't need to.
    // The "Main" must be spelled exactly like that, with an upper-case 'M'.  The "()"
    // characters let us know that this is a function.
    static void Main()
 
    // Now we want to start a new scope for the Main() function so we use a curly bracket.
    {
        // This is the line that prints out the text to the screen.  Notice that we need
        // to use the "Console" class (or object) to access the WriteLine() function.
        // You can store functions in classes, and you just need to type a '.' character
        // after an object to start using its functions.  In Visual Studio you will notice
        // that once you type in the period character a bunch of functions and class
        // variables will pop up.  This is called "IntelliSense", which helps you to easily
        // select functions or variables stored in the class.  You can choose to ignore
        // this and keep typing whatever you want.
        //
        // So, we call WriteLine(), which takes a string of characters (also called a
        // char string) to print out to the screen.  You must use the '"' characters to
        // surround the string that you want to "pass into" the WriteLine() function.
        // For example, this would error and not compile: Console.WriteLine(Hello World!);
        // Notice that we pass the string in between the open and closed parenthesis.  To
        // end the line of code we put a semicolen after the last parenthesis.  Since you
        // can have many lines of code on one line, the ';' tells the compiler to process
        // the previous code before going on to the next code; stating that the previous
        // code and the next code are not connected.
        Console.WriteLine("Hello World!");
 
    // This line ends the scope of the Main() function.  Notice how it lines up with
    // the start of the scope.  This is how you line up scopes so it's easy to read.
    }
 
// This ends the scope for the entire FirstCSharpApp class.  See how it lines up with
// the first open curly backet.  This is what good, clean code looks like.
}

*/

// That's all there is to writing a basic C# console application.  There were probably
// a lot of things that you might not still understand fully, but you should be at least
// able to create a program that prints text out to the screen.  The greater understanding
// will come later as you see more examples and explanations of the concepts used in the
// next tutorials.  
//
// What about if you want to print out many lines of text?  Good question, just call the
// WriteLine() function as many times as you need like this:

/*
 
   Console.WriteLine("Hello World!");
   Console.WriteLine("Hello World Again!");

*/

// Of course, you would put that code in between your Main() function's scope (curly brackets).
// After practicing your new skills please move on to the next tutorial that discusses 
// "Keyboard Input".
//
//
// © 2000-2006 GameTutorials