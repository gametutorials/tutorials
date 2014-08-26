// Done by TheTutor

/*  
	This tutorial is going to use the STL (Standard Template Library) "list" to do some simple
	things.  The STL provides quite a few useful well-tested templates to use in your programs.
	Typically there is no need to reinvent the wheel, so STL is a great choice for using a list
	in your program.

	To help you better understand the STL list class, this tutorial is going to make some 
	sample lists and call commonly used methods of the list and iterator objects (more on iterators further
	down...).  This tutorial doesn't use all the methods in the STL list class, but is intended to 
	be an great introduction to it %)	
*/

#include <list> // The header file for STL list
#include <iostream>
using namespace std; // The namespace STL list is contained within 

// Functions are are friends!
// A quick little function to print a number of successive blank lines
inline void BlankLines(unsigned int num)
{
	while(num--)
		cout << endl;
}

/*
	**NOTE** Quick template refresher.  When declaring an object a templated class, the 
			 type of the object is declared between the <>'s.  Example:

			 list<int> l; // Creates a list of type "int"
			 list<float> l; // Creates a list of type "float"
 */

int main()
{
	list<int> numList; // Create the list of integers

	// Next we create an iterator.  What is an iterator?  Well an iterator is
	// a generalization of a pointer, so you can think of it as a template class for 
	// pointers/references.  As such, iterators are used by STL to access the data contained 
	// within an STL class.  For our example, we have a list of type "int", but if we wanted to
	// change to type "float" we don't want to have to rewrite the code that access and uses the 
	// list.  So we use an iterator as the data type that allows us to get and manipulate the 
	// data in the list.	
	list<int>::iterator walker; // A iterator to walk the list

	// **NOTE** When declaring an iterator, the data type you specify in the <>'s needs
	//			match the data type used when declaring the list

	// Method 1:  empty()
	// Nothing fancy here, if your list doesn't have anything in it, empty() 
	// returns true, otherwise empty() returns false.
	if(numList.empty())
		cout << "List is empty, all is good" << endl; // We should see this message

	// Method 2:  push_front()
	// This method adds an element to the list at the front of the
	// list.  Therefore, whatever element is passed to push_front() becomes
	// the new head of the list.
	// After the code executes below, numList will look like this:
	// 9-->8-->7-->6-->5-->4-->3-->2-->1-->0
	for(int i = 0; i < 10; i++)
		numList.push_front(i);

	// Okay we added some integers to the list, now lets display them to 
	// the screen.  Lets break down the code to do that.
	// 
	// Method 3:  begin()
	// This returns an iterator that refers to the beginning of
	// the list.  In our specific case, this means begin() returns an iterator that
	// points to the integer '9'.  This is illustrated below:
	//		9-->8-->7-->6-->5-->4-->3-->2-->1-->0
	//		|
	//		|
	//  <iterator> returned from begin()
	//
	// Method 4:  end()
	// This returns an iterator that refers to one element past the end of the
	// array.  As you will recall, we said iterators can be thought of as a pointers/references.
	// Since in C++ a reference must be valid, in the same  way an iterator must be valid (ie can't
	// be NULL).  Therefore to mark the end of a list, an empty "end" node is created and appended
	// to the end.  This means, in memory our list looks like this:
	//		9-->8-->7-->6-->5-->4-->3-->2-->1-->0-->END
	//												 |
	//												 |-- A node in the list that signifies the
	//													 end of the list; much like a NULL
	//													 character signifies the end of a character
	//													 array.  end() returns the iterator
	//													 that points to this location.
	// Method 5: ++
	// Because C++ is all about the ++ goodness.  The iterator class overloads the ++ operator
	// so if you have an iterator that points to the first element in a list for example, calling
	// ++ on it, will make the iterator now point to the second element in the list.
	//
	// Method 6:  * 
	// We keep saying it, but an iterator is a template class for pointers/references
	// so when you want to access the data that a pointer points to in C++ you use
	// the * (star) operator.  So you access what an iterator points to using the same
	// same syntax as you would accessing what a pointer points to.
	//
	// Add all of these together and you get our tight little loop below.
	for(walker = numList.begin(); walker != numList.end(); walker++)
		cout << *walker << " ";	

	// Method 6:  clear()
	// Like a well placed stink bomb, clear() clears every living thing out of sight.
	// It is the straightforward way to empty out your list.
	numList.clear(); // Clear the list

	// **NOTE** What if my list contains a bunch of stuff that has dynamic memory?  Well then
	//			my friend, you are responsible for freeing that memory up!  clear() will not
	//			do that for you.
	
	BlankLines(2);

	// Method 7:  push_back()
	// This method adds an element to the list at the back of the
	// list.  Therefore, whatever element is passed to push_back() becomes
	// the last element on the list.
	// After the code executes below, numList will look like this:
	// 0-->1-->2-->3-->4-->5-->6-->7-->8-->9
	for(int i = 0; i < 10; i++)
		numList.push_back(i);

	// Method 8:  size()
	// This method is almost self explanatory by name.  size() returns the
	// count of elements in the list.  Therefore, for our example, size() should
	// return 10.
	cout << "Number of elements in list = " << numList.size(); 

	BlankLines(2);

	// Print out the list
	for(walker = numList.begin(); walker != numList.end(); walker++)
		cout << *walker << " ";

	BlankLines(2);

	// Method 9:  front()
	// Three guesses what this method does and the first two don't count :)
	// front() returns a reference (NOT an iterator) to the front of the list.
	// Given our data set, this should be 0
	cout << "Front of list = " << numList.front();

	BlankLines(1);
	
	// Method 10:  back()
	// As your genius has already deducted, back() returns a reference (NOT an iterator) to
	// the back of the list.  Given our data set, this should be 9.
	cout << "Back of list = " << numList.back();

	BlankLines(2);

	// Clear the list
	numList.clear(); 
		
	// Lastly, lets add some random numbers to the list
	for(int i = 0; i < 10; i++)
		numList.push_front(rand());

	// Method 11:  sort()
	// sort() is a great method for what else, sorting!  Optionally you can
	// pass it a parameter to tell it how to sort, but by default it 
	// sorts from least to greatest.
	numList.sort();

	// **NOTE** If you had a list of some classes, you would need the
	//			< operator (less than operator) to be defined because that is
	//			what sort() uses to sort the elements in th list.

	// The last time we print this list!
	for(walker = numList.begin(); walker != numList.end(); walker++)
		cout << *walker << " ";

	BlankLines(1);
		return 0; // And we're out :)
}

/* Grocery list

  There are more methods for the STL list class than used in this tutorial.  Check out MSDN or
  other C++ language references to check 'em all out.

  If you have any questions, feel free to post them at www.GameTutorials.com.
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/