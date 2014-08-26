//**********************************************************************************//
//																					//
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -					//
//																					//
//		$Author:		Ben Humphrey	digiben@gametutorials.com					//
//																					//
//		$Program:		Pointers												    //
//																					//
//		$Description:	Understanding pointers the simplest way					    //
//																					//
//**********************************************************************************//


// You just entered one of the more confusing tutorials for students--Pointers.  If
// you have heard about pointers before, you probably heard people complain how hard
// they are to understand and you should just use Java so you don't have to deal with
// them.  DON'T LISTEN TO THEM.  Well, Java is great too, but don't jump over to Java
// because of scary pointers.  Basically, pointers all you to control variables and
// the memory address that the variable's data is stored at.  You would use pointers
// for dynamic lists, change variables from one function in another function, and many
// other important things.  In this tutorial we will show you a basic example of changing
// a variable's data by using a function.  You might not understand why at first why you
// would do this, but as you program more you will understand why.  I know, I hate it when
// people say that too, so if you think you can handle at least a couple examples I will
// explain a bit more.
//
// When you have thousands of lines of code, you want to make many functions to make your
// code clean and robust/reusable.  Sometimes you want to change a value in another function
// by a separate function that is called within that function, like in our example here.
// Also, if you create a "class" or a "structure" and it has a lot of data stored in it,
// it's faster to pass in an instance of a structure/class as a "pointer" to a function
// because it don't have to copy all the structure data into a new variable in the
// called function.  Instead, you just pass in the memory address of the variable so the
// called function can just access the variable by it's memory address and have control
// to change it.  That should be a start to understand why pointers are necessary.  When
// you get into linked lists you will further understand its power.  Let's see how we use
// them below:


#include <stdio.h>										// We just need to include this for printf().


void IncreaseNumber(int Number)							// Here is a function that just increases "Number" by 5.
{														// "Number" is a local variable created and assigned the value of the number passed into the function.
	Number += 5;										// Increase the Number by 5.
}														// This didn't touch the number passed in because we didn't use pointers.

														// If we want to change the actual number passed in, we need to give the function the memory address. with a '*'.

void IncreaseNumber2(int *Number)						// Here is a function that increases the actual number passed in
{														// Now "Number" is called a "pointer".
	*Number += 5;										// Because "Number" is a memory address, the value of "Number" equals something like:
}														// "0x00fffec2"   If we don't put the '*' in front of "Number" it will assign
														// "Number"'s memory address to "0x00000005"  ... which will SCREW our program up.
														// By putting the '*' in front of "Number" get the actual VALUE held AT the memory address,
														// which happens to be 0.  Let me show you:
														// "Number += 5"  == "0x00fffec2 += 5"  :  Which equals SCREWED! :)
														// "*Number += 5" == "0 += 5"           :  Which equals 5.
														// (Side Note:  0x00fffec2 is a hexadecimal number which is an offset into memory).
														// (Number doesn't actually equal that number, I just made it up, but that is what a memory address looks like).
														// A little hint on HEX, a = 10, b = 11, c = 12, d = 13, e = 14, f = 15.
														// Letters are used instead of numbers after 9 because 10 and up are 2 digits.  Each slot in HEX means something...
														// 0x10 does not mean 10.  0xa = 10.  Not really important right now, but neat to know when you see HEX :)


void main()												// Start of our program
{														
	int num = 0;										// Declare a integer and set it to 0;

	IncreaseNumber(num);								// Pass num into our first function.

	printf("Num now equals: %d\n\n", num);				// Print out the value of num.  You'll notice when the program run's it prints out 0.
														// Num doesn't change at all.

	IncreaseNumber2(&num);								// Now we pass in num to our second function that requires a memory address.
														// We put a '&' in front of "num" to give the function "num"'s memory address.
															
	printf("Num now equals: %d\n\n", num);				// Now we print out the value of "num".  It's 5!  See the difference? :)
}

// Pointers seem to be really hard for people to grasp, but can be really simple to understand 
// once you get the purpose of them.  You use pointers in this case to change a variable's
// value that is in another function.  There are many other reasons, but stick to this for now.
// Just think of having to give permission to change the value by putting a '&' in front, and
// think of actually changing the value by putting a '*' in front of the pointer variable.

// © 2000-2005 GameTutorials
