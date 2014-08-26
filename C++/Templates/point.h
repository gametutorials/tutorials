#ifndef POINT_H
#define POINT_H

// Here is our templated class.  As you can see it's REALLY SIMPLE (the operator+ is the 
// only thing that looks a little menacing :)
// Lets go over this syntax once again.

// Okay, you see the keyword "template".  This says we're doing a templated class (or function).
// Directly after "template" you see <typename type>.  This says, "The name of our generic
// data type (that can become ANY data type) is "type".  So when you want variables to 
// be of this generic data type we will declare them as type "type".
template <typename type>
class CPoint
{
	public:

		CPoint(); // Default constructor

		// Copy constructor
		// Notice how we define the variables being passed in
		// as type "type".  This really is no different than if 
		// we were defining them as type "int" for example.
		CPoint(type xVal, type yVal, type zVal);

		// Same thing here, everywhere we want a "generic, can be anything variable"
		// we use the keyword "type" (which is what we defined at the top of the class).
		void set(type xVal, type yVal, type zVal);

		// Now here's slightly different syntax.  You'll notice that for
		// the RETURN variable we HAVE to put the class name and our 
		// templated variable name in angled brackets, but for the input parameter(s)
		// to the function, we ONLY use CPoint (just like normal).
		// **NOTE** For those who are not completely fluent with the overloading of operators,
		//			all this does is allow us to write code like this:
		//			ccc = aaa + bbb;
		//			where aaa, bbb, and ccc are all of type CPoint<type>.
		CPoint<type> operator +(const CPoint&);
	
		// This is our data
		type x;
		type y;
		type z;
};


// And now the implementation part...

// First we'll do the good old default constructor.
// For EVERY method you implement you will HAVE to start with the 
// EXACT same syntax you declared your class with.
template <class type> // See, it's the same as above the class declaration
CPoint<type>::CPoint()
{
	// Now if this WAS NOT a templated class, the syntax would look like this:
	//
	// CPoint::CPoint()
	//
	// So you see how you HAVE to add the <type> BEFORE the scope resolution operator?
	// **NOTE** the scope resolution operator equals :: (double colon's)

	// Let's default to zero
	x = 0;
	y = 0;
	z = 0;
}

// Okay, now it's the overloaded constructor.
// Again we HAVE to start out with the "template syntax".
template <class type>
CPoint<type>::CPoint(type xVal, type yVal, type zVal)
{
	// As you can syntax looks similar to the default constructor

	x = xVal;
	y = yVal;
	z = zVal;
}

// Okay, next we'll implement the set function.  Guess what syntax goes first?
template <class type>
void CPoint<type>::set(type xVal, type yVal, type zVal)
{
	// Notice here that first (like usual) is the return type of the function
	// After that the "pattern" remains the same:
	// void CPoint<type>::set(type xVal, type yVal, type zVal)
	//	|	 |     |      |
	//	|	 |     |	  |__ Name of function
	//	|	 |	   |
	//	|	 |	   |__ Name of templated data type we declared
	//	|	 |
	//	|	 |__ Name of class
	//  |
	//  |__ Return type

	x = xVal;
	y = yVal;
	z = zVal;
}

// Last but not least, we'll implement the overloaded + operator
template <class type>
CPoint<type> CPoint<type>::operator +(const CPoint &point)
{

	// Notice there's slightly different syntax:
	// 
	// CPoint<type> CPoint<type>::operator +(const CPoint&)
	//	|			|     |       |				  |
	//	|			|	  |		  |				  |__ Input parameter DOES NOT include
	//	|			|     |		  |					  templatized variable type
	//	|			|	  |		  |
	//	|			|	  |		  |__ Name of the function (yes "operator +" can be
	//	|			|	  |			  thought of as the name of a function)
	//	|			|	  |			 
	//	|			|	  |__ Name of templated data type we declared
	//	|			|
	//	|			|
	//	|			|__ Name of class
	//  |
	//	|
	//  |__ Return type.  This MUST include the templatized variable type
	//
	
	// Now here's the next (and last) piece of syntax knowledge.
	// When you declare a temporary variable of your templated class
	// inside of a function in your templated class (like we are doing here)
	// you DO NOT use the angled brackets/templated variable name (the <type> in our case)
	CPoint answer;

	// Calculate our answer
	answer.x = x + point.x;
	answer.y = y + point.y;
	answer.z = z + point.z;
	return answer;
}

#endif
