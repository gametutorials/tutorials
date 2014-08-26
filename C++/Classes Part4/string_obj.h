#ifndef STRING_OBJ_H
#define STRING_OBJ_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
using namespace std;

// The maximum length a string can be
const int kMaxStrLen = 256;

// Create the prototype for the class
class CStrObj
{
	// Public specifier because we want to be able to call the following methods
	public:
	
		// Constructors
		CStrObj();
		CStrObj(const char *str);
		
		// Copy constructor
		CStrObj(const CStrObj &strObj);
		
		/*
			Assignment operator
			
			Just like the constructor and copy constructor, by default, every class
			gets an assignment operator.  The default assignment operator copies every 
			member variable contained within the class.  So lets look at a small program
			and see how that plays out:
			
				class CPoint
				{
					public:
						float x, y;
				};
				
				int main()
				{
					CPoint pt1, pt2;
					
					pt1.x = 15.0f;
					pt1.y = 25.0f;
					
					pt2 = pt1;  // Assignment operator gets called
								// In pseduo code it would like like this:
								//	{
								//		pt2.x = pt1.x;
								//		pt2.y = pt1.y;
								//	}
					return 0;
				} 
				
			In general, the default assignment operator will be sufficient for your needs.  However,
			if you have any dynamic data in your class, you MUST, overload the assignment operator.
			
			Now lets break down the signature for the overloaded assignment method:
			
			CStrObj& operator =(const CStrObj &strObj)
				|		 |				|
				|		 |				|
				|		 |				--- "const CStrObj &strObj" The input argument to the method
				|		 |					is a const reference to the class type
				|		 |
				|		 --- "operator =" is the name of the method.  When you are overloading an
				|			 operator, you use the syntax "operator" and then the symbol of the
				|			 operator you want to overload.  So for example if wanted to overload
				|			 the addition operator, we would put "operator +" 					
				|
				---	"CStrObj&" The return parameter is a reference to the class type.  We return a
					reference because we want to assign the value of one class to another and have
					it be as fast as possible.
					
			While it may not look like it, overloaded operators are implemented just like any method
			in a class.  So if looking at this code:
			
				CStrObj s1("Hello World");
				CStrObj s2;
				
				s2 = s1; // The method "operator =" is called
				
			If we wanted to, we could explictly call the method.  However this is not typically
			done.  Explicity calling the method looks like this:
			
				CStrObj s1("Hello World");
				CStrObj s2;
				
				s2.operator =(s1); // The method "operator =" is called			
		*/
		CStrObj& operator =(const CStrObj &strObj); 
	
		void set(const char *str); // Sets CStrObj to "str"
		const char* get(); // Gets the value of the string
		
		void print(); // Prints the string
	
	// Private specifier because we do not want direct access to the following
	// member variable	
	private:
	
		char mStr[kMaxStrLen]; // The character array holding the literal string
};

#endif