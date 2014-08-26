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

using System.Windows.Forms;                         // Used for "Application" object

// In this tutorial we are showing how to move from creating simple console
// applications to windowed applications.  You will notice that instead of
// using just one file, we have a "Form" .cs file called Window.cs.  This
// file allows us to use Visual Studio's Toolbox window and editor to add
// controls and manipulate the window.  This is similar to using VB.  Below
// are the things we need to do differently to have a windowed application:
// 
// 1) When creating a project you need to choose C#->Windows->Windows Application.
//    This creates a window application instead of a console application.
// 
// 2) When Visual Studio creates a Windows Application it adds a bunch of of
//    extra files that you don't need to create a window.  If you want, you
//    can delete all the files in the Properties folder, as well as the
//    designer.cs file that is associated with the Form1.cs that is created.
//    We renamed the form1.cs to Window.cs as it seems more appropriate.
//    So in the end, you can just have the Program.cs (renamed to Main.cs) and
//    the Form1.cs (renamed to Window.cs).  Of course, as you build your
//    application and add more things to it, you will want more files and also
//    possibly a "designer.cs" file for cleaner code; however, the two .cs files
//    we have here is all you need to compile and run the application.  Also
//    Visual Studio adds all the appropriate "references" in our reference folder
//    so that we are linked to the needed namespaces like "System", etc...
//    Only references that are needed are absolutey needed are present in
//    this project.
// 
// 3) Instead of just having a "Class" for our program, we create a "namespace".
//    Basically, a namespace just encompasses many classes together so that you
//    can reference the classes and other data within the namespace even though
//    they are defined in other .cs files.  Also, namespaces allow for classes,
//    functions and other data types to have the same name as other data types
//    in different namespaces.  For instance, you can have two classes called
//    "GTClass", but each one is defined in a different namespace.  This comes
//    in handy when you are working with millions of lines of code and creating
//    new names becomes a problem :)
//
// 4) Just like console applications, we create a static void Main(); however,
//    we need to call the "Application.Run()" function to actually display the
//    window.  We need to pass in a "new" instance of the BasicWindow class.
//    Above you will notice that we include the System.Windows.Forms namespace.
//    This is so we can use the Application object for loading a window, or "form".
//
// 5) Now we need to create a window.  Keep in mind that in C#, instead of calling 
//    windows "windows", "forms" are the the new term used.  To create a form we 
//    need to create a class that "inherits" from the "Form" class, meaning that 
//    we tell the compiler that our class is for making a window and will be able 
//    to access functions that windows can use.  Look in Window.cs for more 
//    information about forms. 
//
// That is basically the big changes from console applications to windows applications.
// If you want to look at the code of a "form" .cs file, like Window.cs, you can
// right click on the file in the Solution Explorer and choose "View Code".  Otherwise,
// if you double click the file it will go directly to the designer view for the form.
//
// Let's go over every line and make sure you know what's going on.
//

// Here we create our GameTutorials namespace that will encompass our entire program.
// The BasicWindow object also is in the GameTutorials namespace.  If it wasn't the
// compiler would error and say that it didn't understand what "BasicWindow" is.
namespace GameTutorials
{                                                   // Namespaces have a scope like classes and functions
    // By default, the class that holds the Main() function is called "Program".
    // We left this class called "Program".
    static class Program
    {                                               // The start of the scope for our Program class
        // Just like normal, create the Main() function with its scope
        static void Main()
        {
            // Now that we are at the start of our program, we want to load the "form"
            // that represents our window.  We do that by using the Application.Run()
            // function and pass in a new instance of our BasicWindow class, which is
            // defined in Window.cs.  The syntax to pass in our window is the following:
            // "new BasicWindow()".  That creates memory for the window and passes in the
            // memory address for our window into the Run() function.  The window will run
            // on the current thread that our program is running on.  The Run() function
            // will continue to run until the form for our window is closed.  To force an
            // exit on a form, just call Application.Exit().  When calling the "new" keyword
            // it also invokes the constructor of the class being created.  So basically,
            // the BasicWindow's constructor (init function) will be called when it is created.
            Application.Run(new BasicWindow());
        }
    }                                               // The end of the Program class scope
}                                                   // The end of the namespace scope for this file

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
