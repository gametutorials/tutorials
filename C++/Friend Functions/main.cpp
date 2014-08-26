// Done by TheTutor

// In the following tutorial, we are going to talk about "friend functions".
// Basically, a friend function is a function that is NOT part of the class, but
// has access to the classes' private/protected variables.

// So, what we're going to do is create a simple CVector class.  This class
// will have ONE friend function "dotProduct", which will calculate the dot product
// of two CVector's.

// And with that being said, lets get rockin'

#include "vector_class.h"
#include <iostream>
using namespace std;

int main()
{
	// Create our vectors
	CVector vec1(5.0f,4.0f,3.0f);
	CVector vec2(-2.0f,1.0f,-2.0f);

	// Calculate the dot product
	float dp = dotProduct(vec1,vec2);

	// Spit it out to the screen
	cout << "The dot product = " << dp << endl << endl;
	return 0;
}
 
/*
	If you didn't find this tutorial friendly enough, post your questions/comments at
	www.GameTutorials.com.  There's plenty of friends to be found there (How much more
	cheese can I squeeze from this friend thing??? %)
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
