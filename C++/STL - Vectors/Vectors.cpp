//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Vectors										     //
//																		 //
//		$Description:	This Demonstrates how to use STL's vector class	 //
//																		 //
//***********************************************************************//

#include <iostream>									// Include our standard files
#include <string>									// Include this to use the string class
#include <vector>									// Include this to use the vector class
using namespace std;								// Set the namespace to standard

// When programming, you come across problems all the time that could be
// standard and you wonder why C/C++ didn't come up with something for it.
// Lists are one of those things that we use almost all the time when making
// games.  You would use a list for an inventory, monsters in the room,
// characters in your party, the amount of ammo you hold, polygons in your world...
// There are tons of applications for it.  For those of you who have written
// a link list class over and over again, for every data type you use, I
// have something that will change your world; it's called vectors.
// I don't mean 3D vectors, I mean the STL's (Standard Template Library)
// "vector" class.  This allows you dynamically allocate data with out ever
// touching new, malloc() or realloc().  The vector class is an array
// based link list.  If you want just a link list you can use STL's "list".
// There is also stack and queue.  You will never need to implement your
// own list class again.  These are most likely faster and more efficient
// than anything we could write.  Below I attempt to show you the benefits
// to using the vector class.  

// Here we create our own structure for items.  This will be used to
// demonstrate how we could easily create and manage an inventory.
struct tItem
{
	string strName;							// This will hold our item's name
	int weight;								// This will hold our item's weight
};


int main()									// Beginning of the program
{
	// Below we create our first list.  It is a list of integers.
	// To do so, we put the vector class name in front, then we use
	// the less than sign '<' with the list type we want in the middle,
	// and then close it with the greater than sign '>'.  If you have worked
	// with templated classes you will notice this syntax.  After we define
	// the type of list we want, we assign it to a variable name, vIntegers in this case.

	vector<int>		vIntegers;				// Create a list of integers using the vector class
	
	// Now that we have our list, we don't have anything in it.
	// We want to add an integer to it.  To do so you call the function:
	// push_back().  This function adds a new item to the list on the top.
	// We push back a 5, so now we have a list with the size of 1, with the
	// first index in the list the value 5.  It works just like a stack.
	// Pushing something on puts it on top, and popping something off takes
	// the top one off.  The difference is that we can access any index like an array.

	vIntegers.push_back(5);					// Add an integer to the list with the value 5

	// Here we print out the size (how many items) are in the list.
	// So far we only added one, so this should print out 1 to the screen.
	// To get the size of our list we call the function size().  This makes
	// everything so much easier when dealing with lists.  You don't need to
	// walk the list and count yourself, as well as it being dynamic.

	cout << "The current size of the integer list is: " << vIntegers.size() << endl;

	// Now we want to add more items to our list.  We just call the same function
	// we did up above, push_back(), and pass in the values that we want stored.

	vIntegers.push_back(10);				// Add 10 to our list (List size is now 2)
	vIntegers.push_back(15);				// Add 15 to our list (List size is now 3)
	vIntegers.push_back(20);				// Add 20 to our list (List size is now 4)

	// We should now have a list size of 4 with the values [5 10 15 20] in an array.
	// Instead of passing in constant values, I will demonstrate on how to pass in
	// variables.

	int number = 25;						// Create a variable and set it to 25

	// It works just like any other function, just pass in the variable and it
	// takes it's value and creates another slot to hold that value.

	vIntegers.push_back(number);			// Add 25 to our list (List size is now 5)

	// Here we print out the current size of our list which should be 5
	cout << "The current size of the integer list is: " << vIntegers.size() << "\n\n";

	unsigned int i;									// Create a counter variable

	// Below we print out all of the values in our list.  You can index
	// any value in the list like an array:  vIntegers[3] - (The 4th item in the list)

	// Go through the entire list
	for(i = 0; i < vIntegers.size(); i++)
	{
		// Here we print out the current value in our list of index 'i'
		cout << "vIntegers[" << i << "] is: " << vIntegers[i] << endl;
	}

	// Let's say we want to get rid of some items in our list.
	// To do that we call pop_back().  This destroys the item
	// on top of the list (The last one added on) like a stack does.
	// If we didn't just want the top one removed, we would then
	// just swap the one we wanted removed with the last one in the list,
	// then call pop_back().  Neat huh :)  Or, we could use the erase() function.

	vIntegers.pop_back();					// Remove the item with value 25 from the top
	vIntegers.pop_back();					// Remove the item with value 20 from the top

	// Once again, to demonstrate the awesome power of vectors, 
	// we print the size of the list :)  It should be 3 since we just removed 2.
	cout << "\nThe current size of the integer list is: " << vIntegers.size() << "\n\n";

	// To prove we removed the last 2 items, we go through them all again
	for(i = 0; i < vIntegers.size(); i++)
	{
		// Print out the current item and it's value in order from 0
		cout << "vIntegers[" << i << "] is: " << vIntegers[i] << endl;
	}

	// If we want to clear the list, we don't need to pop them all off.
	// We can just call the handy "clear()" function.  Guess what it does?

	vIntegers.clear();						// Reset the list of integers

	// If we print out the size of the list now we should see there is 0 items
	cout << "\nThe current size of the integer list is: " << vIntegers.size() << "\n\n";

	// Working with integers is easy, but let's try something a bit more complicated.
	// Below we create a vector list of tItems.  This is the structure we 
	// created above.  It's the same template syntax as working with
	// integers, except we substitute "int" with "tItem" as seen below.
	// There is one thing different though in this case.  We want to start
	// off with 3 items.  We just pass 3 into the class's constructor.
	// The items will be initialized to 0 so there is no need to memset().

	vector<tItem>	vInventory(3);			// Create an inventory list

	// Now if we print out the size of our list it would say 3
	cout << "\nThe current size of the vInventory is: " << vInventory.size() << "\n\n";

	// Go through all of the items in our inventory and initialize them
	for(i = 0; i < vInventory.size(); i++)
	{
		// We want to set our item name and weight.  Notice how it works
		// exactly like an array of tItems.  We just specify the index
		// we want to change and then access the members of that structure.

		// Set the item name to bread
		vInventory[i].strName = "Bread";

		// Set the weight to the current index
		vInventory[i].weight = i + 1;
	}

	// Go through the whole inventory and print out the information
	for(i = 0; i < vInventory.size(); i++)
	{
		// Here we print out the name of the current item
		cout << "vInventory[" << i << "]'s name is: " << vInventory[i].strName << endl;

		// Here we print out the weight of the current item
		cout << "vInventory[" << i << "]'s weight is: " << vInventory[i].weight << "\n\n";
	}

	// Here is one last example of using vectors.  If we have a normal array
	// of a data type, we can copy that entire array into a vector easily.
	// No, not through a for loop, but through the vector's constructor.

	// Create an array of 5 floats.  We put the 'f' after each one to
	// cast it as a float, otherwise the compiler will warn us about it.
	float myFloats[5] = {1.1f, 2.1f, 3.1f, 4.1f, 5.1f};

	// When you copy an array into a new vector you just pass in the
	// array name for the first parameter, then pass in a pointer to
	// the last item in the array.  To do that you just say "myFloats + 5".
	// That is how it knows the size of the vector to create.

	// Copy the array into the new vector
	vector<float> vFloats(myFloats, myFloats + 5);

	// To prove this worked, we go through the whole list and print the values
	for(i = 0; i < vFloats.size(); i++)
	{
		// Here we print out the value of each item in the list
		cout << "vFloats[" << i << "] is: " << vFloats[i] << endl;
	}

	return 0;								// Return success
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows us how to use the STL vector class.  If you don't know what
// STL is, it's the Standard Template Library.  A long time ago people got together
// and created a bunch of useful classes that help us do things that we are always
// doing.  This way there will be a standard and easier to port code.  Also, who wants
// to write or copy the same code over for every project?  With the invention of templates
// in C++, this also allows us to use ONE list for every data type.  No need to make
// a set of list functions for each type.  It's the most wonderful thing since sliced bread.
// 
// There is another function that you might find of use.  It's the resize() function.
// You can say:
//
//					vInventory.resize(20);
//
// This will resize your inventory to hold 20 objects.  That way you don't need to
// push_back() 20 times.
// 
// I use vectors constantly and I hope you see how important they are in programming
// with ease.  Once again, take a look at "stack", "queue" and "list" for more container
// classes.  It depends on what you want to do.  I usually just stick with vectors.
//
// * Remember STL containers free themselves, you don't have to free anything!  However,
//	 if you store something in the vector that has dynamic memory, you are responsible for
//	 freeing it. *
//
// Enjoy!
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
// ©2000-2006 GameTutorials
//
//
