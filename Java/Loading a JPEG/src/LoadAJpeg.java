//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorials.com	 //
//									 //
//	$Program:	LoadAJpeg    					 //
//									 //
//	$Description:	Displays a Jpeg image centered on the screen     //
//									 //
//***********************************************************************//

// In this lesson we show how to simply display an image to the screen.  We
// also make it centered to the window.  In the previous tutorials we left
// the applet window the default size, but now we will go over how to change
// the size of the window.  We load a Jpeg of the GT cube, but this code will
// also load other file formats like GIF and PNG.  Apparently, BMP files will
// load in Java 5.0.
//
// Let's go over each line and explain what is going on:
//

// In order to create an applet we need to import the JApplet class.
import javax.swing.JApplet;

// We include all the awt package's classes since we use many of them
import java.awt.*;


// Create our main class and tell Java we want an Applet class
public class LoadAJpeg extends JApplet 
{    
    // In the JApplet class the "init()" function is called at the beginning when
    // the applet is created.  We want to set the size of our window to 640 by 480
    // so we use the setSize() function in the class' init() function.  The init()
    // function is a public function that does not return anything, thus it is void.
    public void init()
    {
        // The setSize function changes the applet's size.  The parameters are the
        // new width and height of the window respectively.  We don't want to use
        // setSize() in the paint() function because the window will be resized and
        // you won't be able to see what was drawn because it will get painted over.
        setSize(640, 480);
    }
    
    // Here we fill in the JApplet's paint() function to draw an image to the screen.
    public void paint(Graphics painter)
    {
        // To display an image in Java is easy.  We use the getImage() function,
        // which takes a URL (web page location) and the image name in a String.
        // The function then returns an "Image" object, which stores the loaded 
        // image data.  Instead of giving a URL, we just use the function
        // getDocumentBase(), which returns the URL of the directory that the applet
        // is running in.  We placed the GTCube.jpg in the Build directory of this
        // project; however, there is no build directory until you "build" the project.
        // That means you have to create the applet project, build it, then place any
        // images you want to use in the Build directory with the .html page that runs
        // the applet.
        Image img = getImage(getDocumentBase(), "GTCube.jpg");
        
        // The Image class is great because it can give us a lot of information about
        // the image that was loaded, like its width, height and pixel data.  To get
        // the width and height of the image we use getWidth() and getHeight().  Each
        // of these functions require that we pass in an applet object that is using
        // the image.  We can just pass in the "this" object that refers to our current
        // applet.  Each class has a "this" object that points to the place in memory
        // where the class data is stored.
        int width = img.getWidth(this);
        int height = img.getHeight(this);
        
        // Now comes the fun part, we actually get to draw the image.  Using the
        // drawImage() function, we pass in the Image object we want to draw, the
        // x and y positions to draw the image, then the this object for our class.
        // In order to center the image on the screen, we use the width and height of
        // the window and the image.  By dividing the applet's width and height by
        // 2 we get the center of the applet, then we can minus the width and height
        // of the image to position the image directly in the middle of the applet.
        // Keep in mind, when working with x and y positions in Java the shapes and
        // images are referenced from the top left corner, not the centers.
        painter.drawImage(img, 640/2 - width/2, 480/2 - height/2, this);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// Above we loaded an image and displayed it centered in the applet.  Below are
// the new functions that we used:
//
// setSize(width, height);  // Sets the applet size and should be done in init()
// getImage(URL, FileName); // This returns an Image object for a loaded image
// getDocumentBase();       // Returns the URL of the applet running
// getWidth(this);          // Returns the width of the image-takes an applet object
// getHeight(this);         // Returns the height of the image-takes an applet object
// drawImage(image, x, y, this); // Draws an image at the x and y in the current applet
//
// Simple huh?  Java is great for doing graphics in a small amount of code, and easy
// to follow.  So, to draw an image we first use getImage(), then we use our Image
// object that we receive from the function to pass into drawImage().  Remember that
// when we are drawing, the x and y position of the drawing position refers to the
// top left corner of the image.
//
// You might notice that images don't immediately appear when the applet loads.  This
// is because it can take a second or so to load the image into memory before it
// displays it.  In fact, the image isn't even loaded until the applet tries to query
// or draw the image.  getImage() returns immediately after it is called, even though
// the image hasn't even loaded.  This is because Java doesn't want you to wait while
// images are being loaded if you are on a slow Internet connection.  That means the
// rest of the code will get executed while the images are being loaded.
//
// One thing to keep in mind when changing the size of the applet is that NetBeans
// creates a .html for the applet to be viewed, but it is around 350 by 200.  Since
// our applet is 640 by 480 you will have to go into the LoadAJpeg.html file and set 
// the size of the applet accordingly if you want to view it in a web page correctly.  
//
// To view this applet you can go to the "Build" directory of this project and open up 
// the LoadAJpeg.html file if you want to see it used in a web page.
//
//
// © 2000-2006 GameTutorials
