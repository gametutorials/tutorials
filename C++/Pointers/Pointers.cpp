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


#include <iostream>										// We do this so we can use "cout"
using namespace std;

// Create prototypes for our 2 functions that "attempt" to increase the variable "num"'s value.
void IncreaseNumber(int Number);						// These should probably go in a .h file, 
void IncreaseNumber2(int *pNumber);						// but I don't want to create a new file for 2 declarations.

void IncreaseNumber(int Number)							// Here is a function that just increases "Number" by 5.
{														// "Number" is a local variable created and assigned the value of the number passed into the function.
	Number += 5;										// Increase the Number by 5.
}														// This didn't touch the number passed in.

														// If we want to change the actual number passed in, we need to give the function the memory address. with a "*"

void IncreaseNumber2(int *pNumber)						// Here is a function that increases the actual number passed in
{														// Now "Number" is called a "pointer".  We put a small 'p' in front of the variable to tell us it's a memory address not a value.
	*pNumber += 5;										// Because "pNumber" is a memory address, the value of equals something like:
}														// "0x00fffec2"   If we don't put the "*" in front of "pNumber" it will assign
														// "pNumber"'s memory address to "0x00000005"  ... Which will SCREW our program up.
														// By putting the "*" in front of "pNumber" we get the actual VALUE held AT the memory address,
														// which happens to be 0 (since we set the variable "num" to 0 in the beginning of the program).  
														// When we put a '*' in front of a pointer this is called "dereferencing a pointer", which means we 
														// are converting the pointer to a normal value (like 5 - not a memory address).	Let me show you:
														// "Number += 5"  == "0x00fffec2 += 5"  :  Which equals SCREWED! :)
														// "*Number += 5" == "0 += 5"           :  Which equals 5.
														// (Side Note:  0x00fffec2 is a hexadecimal number which is an offset into memory).
														// (pNumber doesn't actually equal that number, I just made it up, but that is what a memory address looks like).
														// A little hint on HEX, a = 10, b = 11, c = 12, d = 13, e = 14, f = 15.
														// Letters are used instead of numbers after 9 because 10 and up are 2 digits.  Each slot in HEX means something...
														// 0x10 does not mean 10.  0xa = 10.  Not really important right now, but neat to know when you see HEX :)


int main()												// Start of our program
{														
	int num = 0;										// Declare an integer and set it to 0;

	IncreaseNumber(num);								// Pass num into our first function.

	cout << "Num now equals: " << num << endl << endl;	// Print out the value of num.  You'll notice when the program runs, it prints out 0.
														// Num doesn't change at all.

	IncreaseNumber2(&num);								// Now we pass in num to our second function that requires a memory address.
														// We put a "&" in front of "num" to give the function "num"'s memory address.
														// If we pass in a variable to a parameter that is a pointer, it must have the &
														// or else it won't compile and will give you a compile time error.  Think of using
														// the '&' character as giving the function permission to change or work with the variable.
															
	cout << "Num now equals: " << num << endl << endl;	// Now we print out the value of "num".  It's 5!  See the difference? :)
	return 0;											// Return with zero problems	
}


////////////////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// You might be confused on when we would use this, but you use pointers 
// all the time in programming with C/C++.  The more you program, the more 
// functions you will create.  These functions are going to need to change 
// the data that is passed into them, and that is where functions become 
// very useful.  You don't want to have all of your data global (declared 
// outside of any function so every function can access/change it).  It's 
// not a good idea to have much, if any global data in your large applications.  
// It can lead to problems.  There are many other applications of pointers,
// but just get comfortable how they work, and as we continue through the
// tutorials you will start to see when and when not to use them.
//
//
// © 2000-2005 GameTutorials  
