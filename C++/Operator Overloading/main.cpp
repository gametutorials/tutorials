// Done by TheTutor

/*	Operator Overloading

		This really isn't too difficult of a concept, it's just that the code looks a little 
		funky so it tends to intimidate people.  All operator overloading is trying
		to accomplish is that in our main() (or other .cpp files) we want to be able treat
		user defined structs/classes as we would treat atomic data types (int, double, float, etc).

		So if I have float num1 = 3.4;
					 float num2 = 7.6666;

					float answer = 0.0;

					// You wouldn't even blink at the line of code
					answer = num1 + num2;

		What operating overloading does for us, is overload C++ opeators (such as the plus sign) for
		our own defined classes.  So we can do something like this:

			CVector vec1(2,2,2);
			CVector vec2(5,5,-5);

			CVector answer;

			// Now you shouldn't blink at this either!
			answer = vec1 + vec2;

	Let the operating begin...
*/				

#include "Vector.h"
#include <iostream>

using namespace std;

// We're just going to make a couple CVector's and use 'em to illustrate operator overloading :)
int main()
{
	CVector vec1(5,5);
	CVector vec2(3,3);

	CVector answer;

	// Display the 3 VECTORs initial values
	cout << "vec1 = " << vec1 << endl;
	cout << "vec2 = " << vec2 << endl;
	cout << "answer = " << answer << endl;

	cout << endl << endl; // Blank lines

	// Display the 3 VECTORs values after adding vec1 to vec2 storing the result in answer
	answer = vec1 + vec2;

	cout << "vec1 = " << vec1 << endl;
	cout << "vec2 = " << vec2 << endl;
	cout << "answer = " << answer << endl;

	cout << endl << endl; // Blank lines

	// Display the 3 VECTORs values after subtracting vec1 to vec2 storing the result in answer
	answer = vec1 - vec2;

	cout << "vec1 = " << vec1 << endl;
	cout << "vec2 = " << vec2 << endl;
	cout << "answer = " << answer << endl;

	// Lastly, set vec1 equal to vec2 and display the two
	vec1 = vec2;

	cout << endl << "Setting vec1 = vec2" << endl;
	cout << "vec1 = " << vec1 << endl;
	cout << "vec2 = " << vec2 << endl;
	
	return 0;
}

// Feeling overloaded...

// Well don't sweat it!  Type your questions into the forums at www.GameTutorials.com and rest easy! 

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
