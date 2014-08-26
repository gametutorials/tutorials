//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Window  										 //
//																		 //
//		$Description:	Creates a simple window in C#               	 //
//																		 //
//***********************************************************************//


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

using System.Windows.Forms;                         // This is needed for the "Form" object
using System.Drawing;                               // This is needed for the "Size" object

// In this file we create our window object, called a "form".  We will want
// to use the "GameTutorials namespace" so that our main program can access
// the BasicWindow object that we create here. 
//
// We talked about "inheriting" from the "Form" class in Main.cs.  This
// allows us to say that our BasicWindow class is a form (or window) that
// we want to be able to manipulate.  This gives the BasicWindow class access
// to functions and variables that forms use, like "ClientSize" and "Text".
// The ClientSize variable holds our form's width and height.  The Text
// variable stores the form's title or name that is shown.
// 
// Let's go through the code below and explain it.
//

// Specify that we are using the GameTutorials namespace, like in Main.cs.
namespace GameTutorials
{
    // This says that we are creating a class of type "Form", or in
    // technical terms, we are inheriting from the Form class.
    public class BasicWindow : Form
    {
        // This is our init function for the class, or the "constructor".
        public BasicWindow()
        {
            // Before the form is displayed to the screen we can set some
            // properties like the form's dimensions and the title.

            // This line sets the form's width and height by creating a
            // "Size" object that the variable ClientSize will store.
            // We use the "this" command which refers to "THIS class that
            // we are working with, BasicWindow".  So we set the form's size
            // to 640 by 480.  The "new Size(640,480) allocates memory to
            // hold those dimensions that will be stored in ClientSize.
            this.ClientSize = new Size(640, 480);

            // Now we set "this" form's title by setting the "Text" variable.
            this.Text = "GameTutorials - First Windows Application";
        }
    }
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
