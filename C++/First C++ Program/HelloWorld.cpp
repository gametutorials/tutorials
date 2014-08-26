//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		HelloWorld										 //
//																		 //
//		$Description:	Print "HelloWorld" to the screen				 //
//																		 //
//***********************************************************************//


// *Note*    Using "//" allows us to type anything we want and the compiler ignores it - called "Comments"

// This is the first tutorial that teaches you how to begin programming in
// C++.  C++ is a language you use to program, and in our humble opinion,
// the most powerful and robust language ever created for programming.  This
// is the language you would use to make the huge games and applications you
// see in the stores.  Please read our beginner FAQ before viewing the tutorials.
// Also, if you have our video tutorials, please watch those first so you know
// how to use Visual Studio.

// What you are reading now is comments.  When you put a "//" in front of the
// line of code the compiler ignores it so you can type pretty much whatever
// you want.  How our tutorials work is you read the comments usually at the top,
// then go through the code and read the comments above and alongside the code,
// then usually you have something else to sum up what you just learned at the
// bottom of the file.  If there is more than one .cpp file (code file), then
// you always want to start with Main.cpp.  Now, let's go through and learn
// how to create our first program.  Read the green comments below to see what
// everything does.  All we do in this application is just print "Hello World!"
// to the screen (a black and white console window resembling DOS).


#include <iostream>				// This line of code is necessary to include all the C++ standard functions\class definitions.
using namespace std;			// This confusing line isn't important right now.  
								// Just know that "std" is an abbreviation for standard and that
								// if we are using <iostream> verses <iostream.h> we need to use "namespaces".
								// In a nutshell, namespaces are for cases when you have thousands or millions of lines of
								// code and you might want to use the same name and possibly functionality for 2 different functions.
								// After a while, you might run out of names to call your functions, and this solves that.
								// It is something that you don't need to worry about so much when just beginning.
								// You can create your own namespaces, but we are just going to use the standard one, "std".

int main()						// This creates a function called "main".  Every program must have a "main()".  Ignore "int" for now.
{								// The "{" signifies the start of the function.
								// The next line displays "HelloWorld" to the screen, then skips down a line
	cout << "HelloWorld!" << endl;
								// cout is a standard "class" that stands for "console output".
								// We are inserting a string of characters, "HelloWorld", and an "endl" command into the class "cout".
								// The "endl" part stands for "end of line", which drops the cursor down to the next line in the console.
								// cout uses "<<" as syntax between each insertion.  You could say:
								//		cout << "Hello" << "World!" << endl;
								// and it would print out the same thing.  You can go on forever.  You could
								// also do this:  cout << "HelloWorld!" << endl << endl << endl;
								// This would print "HelloWorld!", then skip down 3 new lines in the console window upon execution of the program.
								// Notice the semicolon at the end of "endl;".  This tells the compiler we are done processing that statement.
								// It allows us to be able to type crazy things like this:
								//	cout <<
								//			"Hello" << "World!"
								//								<< endl;
								// Though it's ugly and unnecessary, we can put those on separate lines because it takes the whole
								// statement starting at the "cout" and goes to the ";".  This comes in handy some times.  Some compilers are line based with no ";".
	return 0;					// This returns from the function the number 0, meaning that there was no problems or errors.  See below for explanation.
}								// The "}" signifies the end of the function, and since it's the main() function, the end of the program.

// To run this with Visual Studio, open the .vcproj file
// and hit Control-F5. It will ask if you want to compile it, choose yes.
// The code must be compiled into an executable before it can be run.
// You should see: 

// HelloWorld!
// press any key to continue			<-- Windows puts this at the end to show the program is finished

// So, all this program does is just display text in a window.  A good start.
// Experiment with printing other words and sentences out, use more "<<" to get the idea.

// Let me explain more about the #include <iostream>:
// "#include" is a "preprocessor" command that happens FIRST before anything else.
// What is does, is basically cut and paste the file called "iostream" into the code
// before the compiler compiles the code into a program.  
// You can open the file "iostream" in the include\ folder in your Visual Studio directory.
// Take a look at it if you like.  It just holds a bunch of definitions to tell the compiler.
// When we call "cout" it knows what cout is by the file <iostream>.  The "<" ">" syntax
// means that the file is located in the include\ directory, where double quotes ("iostream")
// would mean that you have the file in your root directory (where your HelloWorld.cpp file is located).
// We will later create our own files.  These are called header files.  

// Let's talk briefly about what the "int main()" and "return 0" is about.  You don't need to understand
// it all right now since this is your first program, but maybe just try to get a basic understanding.
// the "int" in front of the "main()" function is called the "return type".  It means that there is
// going to be a "return" at the end of the function, and the "return" is going to be an integer, or 
// non-floating point number (1, 203, etc...).  Just like when you use a scientific calculator there are 
// functions that do things, and return values.  For instance, when you use the square root function it 
// takes a number to square root, like 49, and then returns the result: 7 right?  If we put "return 7" in 
// main(), then the "main()" function would return a 7.  There is some system function that started this 
// program, and it called our function "main()", which is the beginning of the program.  Once main() is 
// finished, the system (operating system) function that called main() waits for a "return" value.  We 
// don't care about what value gets returned, but maybe if we wrote a program we might want to know what 
// the return value was.  You don't need to worry about this at all, but just know that a function calls 
// main() and expects a return value.  

// If any of this is confusing, it should be.  It's strange :)
// As we go along, mostly everything will be explained and should be gradually understood.
// BUT!  You can't fully understand anything until you do it yourself.
// I suggest taking each tutorial and messing around with the code:
// Deleting things, adding things... seeing what it does, that way you
// will understand what each part of the code does.  A lot of the times
// You will get errors from deleting things, just put it back and try something else.

// *LAST NOTE*

// Everything below is necessary to run a program that uses console output:

// #include <iostream>
// using namespace std;

// int main()
// {
//	  return 0;
// }

// That is the "SHELL" you could say of a program, you must have all of that
// or else the program won't run.  If you ran a "SHELL" it will open a window
// and say:  "press any key to continue..."

// Before you start programming, it's a good idea to make sure your program runs first.
// Then start adding code like "cout" etc..

// A big part of programming you won't understand in the beginning, but you will later.
// Some things are too complex to explain in the beginning and you need to just take them
// at face value and say, "Ok, I don't know what this does right now, but I know I need it.."

// © 2000-2005 GameTutorials
