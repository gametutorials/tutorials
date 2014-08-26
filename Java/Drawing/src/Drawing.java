//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com	 //
//									 //
//	$Program:	Drawing    					 //
//									 //
//	$Description:	Draws lines and shapes in a Java applet          //
//									 //
//***********************************************************************//

// In this lesson we will cover a bunch of different topics that deal with
// drawing to the screen.  This includes drawing text, drawing shapes, drawing
// lines, and setting the background and current color.  The nice thing about
// Java is that there is no need to initialize any data to begin drawing; you can
// start immediately drawing to the screen.  Just like in the first applet
// tutorial, we import our JApplet class and create an applet class.  However,
// in this lesson we create a paint() function that takes a "Graphics" object as
// a parameter.  In order to use that object we need to include the java.awt
// package.  Since there are a lot of classes in the awt package, we will just
// include them all by using the java.awt.* syntax.  The * character tells Java
// we want ALL the classes imported.  Is that selfish on our part? :)
//
// In this applet we display some ovals, a line and some text.  Check it out by
// running the Drawing.java file.
//
// Let's go over each line and explain what is going on:
//

// In order to create an applet we need to import the JApplet class.
import javax.swing.JApplet;

// We also need to include the awt package because it's necessary to work with 
// colors and drawing.  Notice that we use the .* syntax to tell Java we want
// to import all of the awt package classes.
import java.awt.*;

// Create our main class and tell Java we want an Applet class
public class Drawing extends JApplet 
{    
    // Now we want to create a function that will draw to our applet.  We call it
    // paint(). The paint() function is a function that is already defined in the
    // JApplet class, so when the applet is displayed the JApplet class calls the
    // paint() function.  We need to spell the function correctly or else nothing will
    // get drawn to the screen.  That means no uppercase P's :)
    //
    // The paint() function has a void return type, which means that paint() doesn't
    // return any values from it.  It is also defined as public.  Now, the function
    // also passes in a "Graphics" object that we will need to use when drawing to
    // the screen.  We call the variable (or instance) of the Graphics object "painter".
    public void paint(Graphics painter)
    {
        // First, let's go over a basic drawing function that changes the background
        // color of the window, setBackground().  The window is initially a light grey
        // color, but just to show how to change the background we set it to white.
        // This function is not part of the painter class, but the JApplet class.
        // Because our window is "extending" the JApplet class, we can use this function.
        // To select a color, we use the "Color" object and it's associated color
        // variables.  We pass in the color we want by saying "Color.<color>".  In
        // our case, we want the background color white so we use the .white variable.
        setBackground(Color.white);
       
        // Now we will get into using the Graphics object that was passed into the
        // paint() function.  Before we draw anything, we will first select the color
        // that we want to draw the next shape with.  How about we choose red?  This
        // is done by using the setColor() function in the Graphics object.  Since
        // painter is our variable, we access the function through it.  Like the
        // previous function we used to change the background color, we pass in a
        // color variable to our setColor().  If there are no arguments, red it is!
        painter.setColor(Color.red);
       
        // Now that the color is selected for which are shapes will be drawn, let's
        // draw an oval to the screen.  The fillOval() will do this for us.  It
        // takes as parameters an x and y position, then a width and a height.
        // The x and y position is where the top left corner of the shape will be.
        // This oval will be drawn red near the top of the screen with a width and
        // height of 75.  Keep in mind that the (0, 0) coordinates of the window are
        // at the top left of the window.  Refer to the "Quick Notes" section for more
        // explanation on this.
        painter.fillOval(125, 25, 75, 75);
       
        // Now let's create a white oval that will overlap the last red oval to give
        // a neat abstract effect.  This is done with setColor() and fillOval() again.
        painter.setColor(Color.white);
        painter.fillOval(137, 60, 50, 50);
       
        // Next we will create a smaller red oval
        painter.setColor(Color.red);
        painter.fillOval(147, 75, 30, 30);
       
        // Right below the ovals we will draw a vertical black line.  To draw a line
        // we use the drawLine() function.  The first two parameters are the x and
        // y position of the starting point for the line, and the second ones are
        // the x and y for the ending point of the line respectively.
        painter.setColor(Color.black);
        painter.drawLine(162, 110, 162, 250);
       
        // Finally, let's demonstrate how to draw text to the screen.  This is done
        // by using the drawString() function.  The first parameter is the text that
        // will be displayed.  Since it's a string, we need to use double quotes
        // around the text.  The last 2 parameters are for the x and y position that
        // we want to draw the text from.
        painter.drawString("Abstract", 140, 10);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// Do you realize what cool applications you can now create because you know how
// to draw to the screen?  I hope you are excited to start messing around with these
// functions.  Drawing objects in 3D is just around the corner; can you feel it? :)
// Well, before you start trying to create your own 3D RPG for the XBox, let's review
// what we went over in 2D :)
//
// 1) In order to be able to use classes necessary to start drawing and working with
//    colors we need to include all the awt package classes:
//
// import java.awt.*;
//
// 2) To use the JApplet function that is called when drawing to the screen we need
//    use the following function:
//
// public void paint(Graphics painter)
//
//    This function must be called "paint" and take a "Graphics" object.
//
// 3) To set the background color we use:
//
// painter.setBackground(Color.white);
//
// 4) To set the current color to draw shapes and lines with:
//
// painter.setColor(Color.red);
//
// 5) To draw an oval:
//
// painter.fillOval(0, 0, 50, 50);
//
//    The parameters are the x and y position, with the width and height respectively.
//
// 6) To draw a line:
//
// painter.drawLine(0, 0, 50, 50);
//
//    The parameters are the x and y position for the first point, and the x and y
//    for the last point of the line.
//
// 7) To draw text to the screen:
//
// painter.drawString("Draw me baby!", 100, 100);
//
//    The parameters are a String to print, and the x and y position the text will be drawn.
//
//
// * Screen Coordinates *
//
// As you might have noticed, the x and y position don't work like a normal x and y position
// that you are used to in your math classes.  The (0, 0) position is actually from the top
// left corner of the window.  The bottom right corner of the window is the width and height
// of the window.  So, as x increase it moves to the right of the window; however, as y
// increases it moves down the window.  This might be confusing at first and you might want
// to use graphing paper to work out positions in your head.  After a while, you will be
// used to this method and it will become natural to you.
// 
// To view this applet you can go to the "Build" directory of this project and open up the
// Drawing.html file if you want to see it used in a web page.
//
//
// © 2000-2006 GameTutorials
