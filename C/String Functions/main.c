// Done by TheTutor

/*
	The C language gives us lots of SWEET functions for string manipulation.  This tutorial
	is going to cover some of 'em.  A recap of all the string related function we use
	is located at the bottom of this file.  But first a quick trip down memory lane as to
	what in the heck a string is...

	A string is quite simply an array of characters that ends with a special character
	called the "NULL terminator".  So take the following code:

	char str[] = "Hi";

	This creates a string with the variable name "str".  If we were to look at each index
	in this char array, we'd see this:

	str[0] = 'H'
	str[1] = 'i'
	str[2] = NULL

	We say the length of this string is 3 because it contains 2 letters (Hi) and the 
	NULL terminator (which is equivalent to 0 by the way)

	And this concludes the journey down memory lane... Now on with the show.

*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define STR_MAX 256 // Max number of characters for a string (if you type more than
				   // this, you got way too much free time %).

// This tutorial is going to do a couple of things:
//
//	1)  We're going to have the user type in two strings.
//	2)  We're going to make one long string out of the two strings the user entered
//		and print it to the screen.
//	3)  We are going to print out whether the two strings are equal (case sensitive) or not. 
//	4)	Lastly, we are going to print out either true or false, depending on if the
//		first sting	entered is in it's entirety contained within the second string.
int main()
{
	// Like all good C programs, we'll start out with a few variables

	// These are the two character buffers (strings) that we'll use for user input
	char str1[STR_MAX] = {0};
	char str2[STR_MAX] = {0};

	char *buffy = NULL; // This will be a string we dynamically allocate a little later on

	int len1 = 0; // This will hold the length of str1
	int len2 = 0; // This will hold the length of str2

	// First we'll ask for a string and read in what is typed
	printf("Type in a string: ");
	scanf("%s",str1);

	rewind(stdin); // Get ready to read again from the keyboard buffer 

	// Now we'll ask for another string
	printf("Type in another string: ");
	scanf("%s",str2);

	// Okay we have our two strings, lets get the length of each of these

	// The strlen() function does what you'd expect, it gives you the number of characters
	// in the string.  However, it is very important to note that strlen() DOES NOT include
	// the NULL terminator.  So, for example, the length of "Hello" is five.
	len1 = strlen(str1); // Get the length of str1 -- (NULL is not included)
	len2 = strlen(str2); // Get the length of str2 -- (NULL is not included)

	// Dynamically allocate a string the size of str1 plus str2
	buffy = (char*)malloc(sizeof(char) * (len1 + len2 + 1)); // Notice the +1, this is 
															// for a NULL terminator

	// Error Check
	if(!buffy)
		return 0; // Couldn't allocate memory... This would be bad :(
	
	// It is time for some string function sweetness.
	// The first order of business is to concatenate both str1 and str2.
	
	// We'll start by sticking str1 into buffy
	sprintf(buffy,"%s",str1);
	
	// If you understand printf() then sprintf() is pretty simple to understand.  Instead
	// of printing to the screen, sprintf() "prints" to the buffer passed in as the first
	// argument of the function (this is "buffy" in our case).

	// For the sake of clarity, let's break down the sprintf() function:

	/* sprintf(buffy,"%s",str1);
	             |
				 |__ This parameter is a char* that is to be filled in.  You can think
				     of it as the "storage string" because it will store the result 
					 of the call to sprintf().

	   sprintf(buffy,"%s",str1);
					   |
					   |__ This is the "formatting parameter" for how "buffy" (in our case)
						   should be filled.  The formatting parameters are exactly the 
						   same as they are for printf().

	   sprintf(buffy,"%s",str1);
					        |
							|__ Also just like printf(), this is an "optional argument"
							    that only needs to be there IF specified in the "formatting
							    parameter".  Since our formatting parameter specifies one
							    string (%s = char string) we have to pass an additional 
							    parameter (which has to be a char string), thus we pass "str1".
	*/

	// Okay we've sprintf() str1 to buffy (which means buffy is now the same as str1),
	// let's concatenate str2 onto buffy.

	strcat(buffy,str2); // This tacks on "str2" onto the end of buffy
					  
	/*
		The strcat() function can be thought of like this:

		strcat(dest, addMe);

		Basically the "dest" buffer gets added (on the back end) whatever is
		in "addMe".  So here's a quick code demonstration to show how this
		all works:
		
			char aaa[64] = "The";
			char bbb[64] = "Tutor";

			strcat(aaa,bbb); // We concatenate the strings

		The string "aaa" would now look like this:

		aaa = "TheTutor"		// (this INCLUDES the NULL terminator)
	*/

	// We'll print the concatenated string to the screen
	printf("%s\n",buffy);

	// Now let's check to see if "str1" and "str2" are equal.  By equal we mean
	// EVERY character is EXACTLY the same.  So for instance:
	// "hello" equals "hello", BUT "thetutor" does not equal "TheTutor"
	/*
		The strcmp() function works like this:

		If "str1" is less than "str2", the return value is less than zero
		If "str1" is identical to "str2", the return value is zero
		If "str1" is greater than "str2", the return value is greater than zero
	*/
	if(strcmp(str1,str2) == 0) // If "str1" is identical to "str2"
		printf("%s is equal to %s\n",str1,str2);

	else // "str1" and "str2" are different
		printf("%s is not equal to %s\n",str1,str2);
					  
	
	// Finally, let's print out whether "str1" is contained within "str2".
	// So for example if we had this:
	//
	//	char aaa[64] = "The";
	//  char bbb[64] = "TheTutor";
	//
	//  We'd want to print out TRUE, because "The" IS contained within "TheTutor".
	/*
		We'll break down the strstr() function:

		strstr(str2,str1)
		        |
				|__ This is the string we are SEARCHING IN.  We want to search this string
					for any occurrences of "str1" (in our case).

		strstr(str2,str1)
					  |
					  |__ This is our "search string" (what we are looking for)

		The return value of strstr() is a char* that points to the beginning
		of the FIRST occurrence of str1 in str2 (if one exists).  Otherwise the 
		return value is NULL.
	*/
	if(strstr(str2,str1)) // If "str1" is contained within "str2"
		printf("%s is contained within %s is TRUE!\n",str1,str2);

	else // "str1" IS NOT contained within "str2"
		printf("%s is contained within %s is FALSE!\n",str1,str2);

	free(buffy); // Time to let her go
	return 0;    // And we are out

}

// Quit stringing me along...

/*
	As promised a recap of the string functions we used:

	size_t strlen(const char*); 
	
	// Takes a char* (a string) returns the length of the string NOT INCLUDING 
	// the NULL character.
	
	// **NOTE** size_t, the return parameter, is an unsigned int.

***************************************************************************************
**************************************************************************************
*************************************************************************************

	int sprintf(char *buffer, const char *format [, argument] ...);

	// Isn't the declaration for this function ugly?  Anyway this works 
	// just like printf() with the exception that "buffer" gets filled with
	// what is printed instead of the screen.  So examples usually help the best...
	//
	//	char buff[64] = {0};
	//	
	//	sprintf(buff,"%s = %d","My age",3);
	// 
	//	After this function call, buff would look just like this:
	//
	//	buff[0] = 'M'			buff[6] = ' '
	//	buff[1] = 'y'			buff[7] = '='
	//	buff[2] = ' '			buff[8] = ' '
	//	buff[3] = 'a'			buff[9] = '3'
	//	buff[4] = 'g'			buff[10] = NULL		// It looks like:  "My age = 3"
	//	buff[5] = 'e'			
	//
	//	Now in this example we passed "My age" (the string) and 3 (the int) directly,
	//	but they could of been variables just as well.  If you are still not
	//	comfortable with sprintf() I HIGHLY suggest you play with printf() for like
	//	a day, then come back and look at sprintf() again.  Chances are it will
	//	make 100x more sense %)
	//
	//	Oh yeah, sprintf() returns the number of bytes (char's) that is stored
	//	in "buffer", but this does NOT count the NULL character.

***************************************************************************************
**************************************************************************************
*************************************************************************************

	char* strcat(char *str_dest, const char *str_source);

	// strcat() appends the "str_source" to the back of "str_dest".  It looks
	// something like this:
	//	
	//	char aaa[64] = "The";
	//	char bbb[64] = "Tutor";
	//
	//	aaa[0] = 'T'
	//	aaa[1] = 'h'
	//	aaa[2] = 'e'
	//	aaa[3] = NULL	--	Gets bbb[0] = 'T'
	//	aaa[4] =  ?		--	Gets bbb[1] = 'u'
	//	aaa[5] =  ?		--  Gets bbb[2] = 't'
	//	aaa[6] =  ?		--  Gets bbb[3] = 'o'
	//  aaa[7] =  ?		--	Gets bbb[4] = 'r'
	//	aaa[8] = NULL	--  strcat() terminates the string with a NULL character
	//
	//	This results with aaa = "TheTutor"

	// The return value of strcat() is the same as "str_dest".  Don't ask me why I 
	// didn't write it %)

***************************************************************************************
**************************************************************************************
*************************************************************************************

	int strcmp(const char *str1, const char *str2);

	// This compares "str1" with "str2".  If they are equal (which means identical
	// and is case sensitive) the return value is ZERO, otherwise it returns some
	// other number.

***************************************************************************************
**************************************************************************************
*************************************************************************************

  char* strstr(const char *searchMe, const char *str2SearchFor);

	// strstr() finds a substring (if one exists) in "searchMe".  So what happens
	// is this function searches "searchMe" for the smaller string "str2SearchFor".
	// If it finds an occurrence of "str2SearchFor" in "searchMe" it returns a
	// pointer to the first letter of that occurrence in "searchMe",
	// otherwise it returns NULL.

***************************************************************************************
**************************************************************************************
*************************************************************************************

|== Challenge ==|

  Look up these string functions:

	strchr()
	stricmp()
	strcpy()

Questions, comments?  Post 'em at www.gametutorials.com

*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/