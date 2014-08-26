//***********************************************************************//
//									 //
//	- "Talk to me like I'm a 3 year old!" Programming Lessons -	 //
//                                                                       //
//	$Author:	Ben Humphrey	digiben@gametutorilas.com	 //
//									 //
//	$Program:	Triangle					 //
//									 //
//	$Description:	Init OpenGL and draw a triangle to the screen	 //
//									 //
//***********************************************************************//

import java.awt.BorderLayout;                   // Add this for the BorderLayout class 
import javax.swing.JApplet;                     // Add this for creating an Applet
import javax.swing.JPanel;                      // Add this to create a drawing panel

//OpenGl imports (using JOGL API)
import net.java.games.jogl.GLCanvas;            // Add this for an OpenGL scene
import net.java.games.jogl.GLCapabilities;      // This gives us our video capabilities
import net.java.games.jogl.GLDrawableFactory;   // This allows for cross-platform rendering


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This tutorial is the first tutorial on doing 3D with the OpenGL API.  We will
// be doing 3D in an Applet, which can allow us to have 3D applications on the web
// that users can view and manipulate.  Apparently, doing 3D in a Java "frame" is
// faster, but we will be doing it in an Applet for simplicity reasons.  In order
// to use OpenGL in java we chose the JOGL API, which is an open-source project.
// If you haven't already, read the Beginner FAQ on our CD or website.  The FAQ will
// explain how to install and link to the JOGL libraries so that you don't spend
// 50 hours trying to compile, and then eventually run the applet.  We are assuming
// that you are using NetBeans 5.0, or a similar program/version.
//
// Remember, since this is an Applet, to run this Applet in NetBeans you can right
// click on this file in the Projects tab and choose the "Run File..." menu option.
// You can also use the Shift+F6 keyboard shortcut.  You can find this file in the
// "default package" node of the "Source Packages" folder of the "Projects" tab tree.
//
// * What is OpenGL *
//
// OpenGL is an API (A set of functions) that allow us to quickly render (draw) 3D
// data to the screen using the power of our video card.  Instead of writing functions
// to draw 3D stuff to the screen ourselves and reinvent the wheel, most people use
// OpenGL or Microsoft's Direct3D libraries.
//
// * What this program does *
// 
// This program displays a triangle to the screen in a Java Applet using OpenGL's
// 3D rendering APIs.
//
// * How to setup OpenGL in an Applet *
//
// So, since this file has the Applet that we run the program with, we will start
// here.  Let's talk about what we need to do to create an OpenGL program in an 
// Applet.  First, we create our JApplet class.  We then need to create 3 important
// classes to handle our rendering and events of OpenGL.
//
// * The JPanel class *
//
// This class helps us with drawing our GL code in the applet.  It helps with
// double buffering and such so there is no flicker when drawing.  We create a
// panel, and then assign our 3D scene to that panel.
//
// * The GLCanvas class *
//
// This class is what holds our 3D scene.  We create a GLCanvas where 3D scene
// will be rendered, and then assign the scene to our Applet's JPanel.
//
// * The OpenGL class *
//
// This is the class that we created in OpenGL.java to handle the initialization
// and rendering in OpenGL.  Check out OpenGL.java for the OpenGL code on drawing
// a triangle to the screen.  We use this class when adding a "GLEventListener".
// This will init our OpenGL and handle our mouse and keyboard input.
//
// Let's go over the code to init our applet for OpenGL.
//


// This is our main class for the applet.  We extend the JApplet class.
public class GLApplet extends JApplet
{
    // These three private variables are to init OpenGL for our Applet
    private JPanel panel = null;        // The panel attached to the Applet
    private GLCanvas scene = null;      // The 3D scene object to add to the panel
    
    // This function is called when our Applet is created
    public void init()
    {
        // This sets the size of our Applet
        this.setSize(800, 600);

        // Now we need to get a GLCanvas where the 3D scene will be drawn.  To do
        // this we use the GLDrawableFactory object to get a canvas that is compatible
        // with the current machine.  This means this code can run on a Mac, Windows,
        // or Linux computer.  By passing in a GLCapabilities object into createGLCanvas
        // we are returned a compatible GLCanvas.  The getFactory() function returns an
        // object to create a canvas from the GLDrawableFactory class.
        scene = GLDrawableFactory.getFactory().createGLCanvas(new GLCapabilities());
        
        // Create an instance of our OpenGL class that was created in OpenGL.java,
        // then add it as a listener that will handle our input and drawing.
        scene.addGLEventListener(new OpenGL());
        
        // Now that we have a scene object, we will then create a panel that will be
        // attached to our Applet.  But before adding the panel to our Applet we must
        // first add our scene to the panel.
        
        // Create a new panel
        panel = new JPanel();
        
        // When creating a panel we have to set the layout.  We pass in a new border-
        // layout object for the panel, then add the scene object and set the layout
        // to CENTER.  This means the 3D viewport will fill the window in the center.
        panel.setLayout(new BorderLayout());
        panel.add(scene, BorderLayout.CENTER);
        
        // Finally, we add the panel to our Applet
        this.setContentPane(panel);
    }
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// To initialize the Applet isn't too bad is it?  Just like normal, we create a
// class that extends the JApplet.  Then, in our Applet init() function we create
// a JPanel, attach a GLCanvas to the panel, then finally we want to add the panel
// to our Applet.
//
// JPanel - A panel is used to attach to an Applet and species a layout for drawing.
//
// GLCanvas - This is our object that the OpenGL will draw to.  We add this to a panel.
//
// OpenGL - This object handles our input and our OpenGL code
//
// Now go check out the OpenGL.java file and see the actual OpenGL code.  This file
// is just to initialize the applet to get it ready for rendering with OpenGL.
// 
// * JOGL *
//
// To compile run this application you need the JOGL libraries.  You can download
// the libraries at: https://jogl.dev.java.net/
//
// There are new version of JOGL that are different, but if you want to compile this
// code you will have to download JOGL 1.1.  Check our FAQ on the CD or our website
// to learn how to download, install and link JOGL.
//
//
// © 2000-2006 GameTutorials
