//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com	 //
//									 //
//	$Program:	Window    					 //
//									 //
//	$Description:	Displays a window in Java                        //
//									 //
//***********************************************************************//

// This is the first tutorial for displaying a window in Java.  In Java,
// the window is called an "Applet".  When running the applet in NetBeans,
// the applet already have a menu attached to it that has some useful operations 
// for working with the window.  There isn't a lot of code to create an applet, 
// in fact it is just a couple lines.  It takes longer to actually create the 
// project than it does to write the code.
//
// * What is an applet? *
//
// An applet is a standalone application for Java that can be run in a 
// Java or ActiveX enabled web browser.  It's like a mini software application
// for the web.
//
// * How do you create an applet? *
//
// To create an applet in NetBeans you will want choose File->New Project...
// Then, once the dialog box comes up, make sure you are in the "General"
// folder of the "Categories" section, then choose "Java Class Library" from
// the "Projects" section.  Click the "Next" button, then in the following
// screen Choose the name of your applet.  You can browse to the directory
// you want to save the project in, or just leave it to the default path.
// Now click the "Finish" button to create the project.  This will open a blank
// project, so we need to add a file for our applet.
//
// In the "Projects" tab of NetBeans (in the top left usually) right click on the
// "Source Packages" folder and select the New->JApplet... menu option.  This will
// bring up a dialog box to enter in the name of the class.  Type any name in the
// "Class Name" field, and then click the "Finish" button.  Now you have a new applet!
//
// * How do you run an applet? *
//
// Assuming you are using NetBeans, expand the "Source Packages" folder in the
// "Projects" tab.  It should show a child node in the tree of "<default package>".
// Expand that node also and you should see your .java file that you just created
// when you created a new JApplet file.  To run the applet, just right click on the
// .java file and select "Run File".  You can also hit Shift+F6 for a keyboard shortcut.
// You should see your Java applet popup and that's all there is!
//
// NetBeans creates a default applet class for you, but we changed the code to how
// we like it down below.  Notice that we "import" the JApplet class instead of how
// NetBeans' default code is coded.
//
// Let's go over each line and explain what is going on:
//

// In order to create an applet we need to import the JApplet class.  This class is
// in the javax.swing package.  Now we can tell our window class below that we want
// to create a JApplet class so it loads a window when we run it.
import javax.swing.JApplet;

// Now we can create the window class.  There is no code necessary to display the
// window, we just tell Java that our class is a JApplet class and it does the work
// for us.  To tell Java the type of class we want, we use the "extends" keyword.
// This says that our "Window" is "inheriting" its attributes (variables, functions, etc.)
// from the JApplet class.  So, whatever the JApplet class has, our new "Window" class
// will also have them.  You can think of it as just borrowing them for a bit :)
// We make our class public, as well as we have to name the class "Window" because the
// .java file we created for our applet is called "Window.java".  Rules are Rules :)
public class Window extends JApplet 
{
    // We don't need any code in here to display the window.
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// That was simple huh?  Probably one of the simplest bits of code to create a
// window in any language.  We didn't put any code in the class' scope; we will
// save that for the next tutorial.  
//
// Let's review what we need in order to create a window (i.e. applet) in Java.
// 
// 1) Create a "Java Class Library" in Netbeans
// 2) Add a JApplet file to the class by right clicking on "Source Packages" in
//    the Projects window and selecting the New->JApplet... menu option.
// 3) NetBeans creates a default applet class with the name you chose when you
//    created the JApplet file.  To run the applet right click on the .java file
//    that is in your project tree and select "Run File".
//
// Now, in regards to the code, all we need to do is import the javax.swing.JApplet
// into our code, then say that our class "extends" the JApplet class.  That's it.
//
// Not very difficult, but this is very important to get a base going in order to
// start creating more useful java applications that can be used in your web pages.
//
//
// © 2000-2006 GameTutorials