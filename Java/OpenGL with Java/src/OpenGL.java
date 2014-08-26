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

import java.awt.event.KeyEvent;                 // Used for key definitions 
import java.awt.event.KeyListener;              // Used for keyboard input

import net.java.games.jogl.GLEventListener;     // Used for OpenGL messages
import net.java.games.jogl.GLDrawable;          // Used for our rendering object
import net.java.games.jogl.GL;                  // Used for basic GL functions
import net.java.games.jogl.GLU;                 // Used for camera functions
import net.java.games.jogl.Animator;            // Used for multithreaded rendering


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This file is where we actually write our OpenGL rendering code.  GLApplet is
// the code for our Applet.  Please read GLApplet.java before coming to this file.
// We are using the JOGL libraries for OpenGL.
// 
// So, you are about to program in 3D?!?  If you have never programmed in 3D you
// are about to take your programming experience to a whole new level.  Once you
// go 3D, you never go back.
//
// This application displays a 3D rendered triangle to the screen.  You might be
// confused because the triangle looks like it's in 2D, but you can change the
// camera position and notice that it will change the view of the triangle.  You
// don't really get the 3D feel until you have see 3D data moving or with camera
// movement.
//
// * Important OpenGL functions *
//
// void init(GLDrawable glRenderer);
// void reshape(GLDrawable glRenderer, int x, int y, int width, int height);
// void display(GLDrawable glRenderer);
//
// Those are the important functions that you will want to focus on for OpenGL.
// The init() function is where we setup our keyboard listener and Animator Object.
// The reshape() function is where we setup the 3D matrices and perspectives, as
// well as resizing the 3D viewport.  Finally, the display() function is where
// we actually put our 3D rendering code.  In this tutorial we add our code to
// draw the triangle there.
//
// We also have a keyReleased() function, which handles our keyboard input.  The
// three remaining functions aren't used in this tutorial, but we need to include
// them because we have overridden the KeyListener and GLEventListener classes.
//
// Let's go through the code and explain each line.
//


// Our OpenGL class uses the GLEventListener and KeyListener objects
public class OpenGL implements GLEventListener, KeyListener
{
    // This is our Animator object that allows us to render using multithreading
    private Animator animator;	
    
    // This function is called when our OpenGL object is created
    public void init(GLDrawable glRenderer)
    {
        // We are given a glRenderer object for drawing.  We will add the 
        // "this"" pointer to the glRenderer object for keyboard input.
        glRenderer.addKeyListener(this);
		
        // For efficiency we create an Animator object, used for multithreaded 
        // rendering.  Next we start the animator object.
        animator = new Animator(glRenderer);
        animator.start();
    }

   
    // This function sets our viewport size and initializes the perspective and matrices.
    public void reshape(GLDrawable glRenderer, int x, int y, int width, int height)
    {
        // To use basic OpenGL functions we need to create an object for GL and GLU.
        // We use the "final" keyword so that we can keep the keywords in memory and
        // efficiency.  The glRenderer object returns these GL and GLU objects.
	final GL gl = glRenderer.getGL();
	final GLU glu = glRenderer.getGLU();
	
        // Make sure the height is at least 1
	if(height <= 0) height = 1;					
		
	gl.glViewport(0,0,width,height);					// Make our viewport the whole window.
										// We could make the view smaller inside
										// Our window if we wanted too.
										// The glViewport takes (x, y, width, height).
										// This basically means, what are our drawing boundaries
										// on the screen in client coordinates (you can have like 4
										// viewports rendering different things in one window, like 3DS Max.

	gl.glMatrixMode(gl.GL_PROJECTION);					// Select The Projection Matrix
	gl.glLoadIdentity();							// Reset The Projection Matrix

										// Calculate the aspect ratio of the window
										// The parameters are:
										// (view angle, aspect ratio of the width to the height, 
										//  the closest distance to the camera before it clips, 
                         // FOV		// Ratio                                //  and the farthest distance before it stops drawing).
	glu.gluPerspective(45.0f, (float)width / (float)height, 1, 150.0f);

	// * Note * - The farthest distance should be at least 1 if you don't want some
	// funny artifacts when dealing with lighting and distance polygons.  This is a special
	// thing that not many people know about.  If it's less than 1 it creates little flashes
	// on far away polygons when lighting is enabled.

	gl.glMatrixMode(gl.GL_MODELVIEW);					// Select The Modelview Matrix
	gl.glLoadIdentity();							// Reset The Modelview Matrix
    }

        
    // This is the actuall function that renders our triangle to the screen
    public void display(GLDrawable glRenderer)
    {
        // Create gl and glu objects as final
	final GL gl = glRenderer.getGL();
	final GLU glu = glRenderer.getGLU();
		
	gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	gl.glLoadIdentity();						// Reset The View
	
	// 	      Position      View	   Up Vector
	glu.gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

	// The position has an X Y and Z.  Right now, we are standing at (0, 0, 6).
	// The view also has an X Y and Z.  We are looking at the center of the axis (0, 0, 0).
	// The up vector is 3D too, so it has an X Y and Z.  We say that up is (0, 1, 0).
	// This can pretty much stay the same, unless you start rotating your position around the
	// x or z-axis.  For moving left, right and forward you should be okay to leave it at (0,1,0).
									
        // Below we say that we want to draw triangles:		
	gl.glBegin (gl.GL_TRIANGLES);					// This is our BEGIN to draw
				   gl.glVertex3f(0, 1, 0);		// Here is the top point of the triangle
		gl.glVertex3f(-1, 0, 0);	gl.glVertex3f(1, 0, 0);	// Here are the left and right points of the triangle
	gl.glEnd();							// This is the END of drawing

	// I arranged the functions like that in code so you could visualize better
	// where they will be on the screen.  Usually they would each be on their own line.
	// The code above draws a triangle to those points and fills it in.
	// You can have as many points inside the BEGIN and END, but it must be in three's.
	// Try GL_LINES or GL_QUADS.  Lines are done in 2's and Quads done in 4's.									//This is the END of drawing
    }
    
    // This function handles our keyboard input
    public void keyReleased(KeyEvent event)
    {
        // Here we check which code was pressed from the keyboard
    	switch(event.getKeyCode())
    	{
            // If the key was the escape key stop our animator thread and quit
            case KeyEvent.VK_ESCAPE:
                animator.stop();			
    		System.exit(0);				
    		break;
    	}
    }
    
    
    // We won't use these functions below, but we must define them to "implement[s]" the listener classes.
    public void displayChanged(GLDrawable glDrawable, boolean modeChanged, boolean deviceChanged){}
    public void keyTyped(KeyEvent ke){}
    public void keyPressed(KeyEvent ke){}
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// That's all there is to initializing OpenGL!  Now that you can display a 
// triangle to the screen, you can go onto many other cool things from here.
// A lot of this (most of it) might be confusing the first time.
// The only stuff you should REALLY understand is the display() function.
// This function is what draws everything.  The rest of the stuff is init junk
// that you will very rarely ever look at again.  You might add a line here
// and there, but mostly it stays the same.  If you can understand that you have 
// a camera, and it points in a direction, and that you plot points to make a 
// triangle, you are good for now :)
//
// Here are the basic steps to initializing OpenGL:
//
// 1) First, we create an Animator object to render with multithreading for speed.
//
// 2) Next, we need to setup our screen translations.  If we didn't use OpenGL 
//    this part would be complicated and tricky.  Luckily, OpenGL keeps us from
//    all the math needed to do the translations.  We just tell OpenGL how large
//    our viewport is and which perspective we want to look at in our world. 
//    
// 3) After that, we are all set and we just need to DRAW SOMETHING!
//
// We plotted 3 points in this tutorial.  3 points make up a triangle.
// We used the GL_TRIANGLES flag to tell OpenGL to render triangles.
// 
// Like so (cleaner):
//
//	gl.glBegin (gl.GL_TRIANGLES);	
//		gl.glVertex3f(0, 1, 0);     // Top point			
//		gl.glVertex3f(-1, 0, 0);    // Bottom right point
//		gl.glVertex3f(1, 0, 0);     // Bottom left point
//	gl.glEnd();
//
//  If we wanted 2 triangles, we could say:
//
//	gl.glBegin (gl.GL_TRIANGLES);	
//		gl.glVertex3f(0, 1, 0);     // Top point			
//		gl.glVertex3f(-1, 0, 0);    // Bottom right point
//		gl.glVertex3f(1, 0, 0);     // Bottom left point
//
//		gl.glVertex3f(0, 1, 1);     // Top point			
//		gl.glVertex3f(-1, 0, 1);    // Bottom right point
//		gl.glVertex3f(1, 0, 1);     // Bottom left point
//	gl.glEnd();
//
// Notice that we rendered the points in a different order than
// in the display() function.  In this tutorial we won't
// get too much into the order, but just know that there is a
// clockwise and counter clockwise way to draw triangles.  In our
// display() function we used counter clockwise, because we
// draw the left side before the right side of the triangle.
// Above we draw using clockwise.  Usually you will find that
// many people will use clockwise.  You can tell OpenGL not to
// render the back's of triangles (speeds up rendering).  This
// is called Z-Buffering.  You can set in OpenGL either CW
// (ClockWise) or CCW (Counter ClockWise).  Don't worry about
// that too much right now though.
//
//  Here is a horrible attempt to draw the 3D axis's
//
//				Y
//				|
//				|
//				|________ X
//				 \
//				  \
//				   Z
//
//  This is drawn in a rotated manner so you can see the Z axis.
//  Otherwise it would be coming out of the screen and you couldn't see it.
//  Positive Z comes out of the screen towards us, where -z goes into the screen.
//  Positive X is on the right, negative X is on the left.
//  Positive Y goes straight up, where negative Y goes straight down
//
//  So here is what our triangle looks like with the axis:
//                          Y
//                          |
//			   /|\
//			  /_|_\______ X
//                           \
//                            \
//                             Z
//
// As you can see, our triangle is half on the negative X side.
// and half on the positive X side.  It is also 0 on the Z axis,
// so really, it's seen as 2D more than 3D because we
// aren't using the third dimension, which is Z.
//
// If this all seems overwhelming, it will get easier and MUCH more fun! 
//
// As for messing around with this tutorial, try changing camera positions
// to see how it affects your view.  Also, try plotting in other points and
// other triangles/lines/Quads (rectangles) to see what you can create so far.
//
// One last note about this code.  It might be a good idea, if you don't
// understand anything about 3D or matrices (matrix math),  go on the internet
// and do a little investigating and learning.  It couldn't hurt.  We will teach you
// all the main math you will need to know, but the more versed you are in math the better.
// If you are not good at math, or at least you don't know anything over algebra/geometry,
// you are going to get a little frustrated.  You will want to be pretty comfortable
// with cosine and sine, as well as plane and vector math.  You will want to know what
// a vector is, and what purpose it serves. 
//
// However, don't get scared if you are not a math person, I wasn't really when I first started
// doing 3D programming, but you learn REALLY fast, and it's fun when you use math
// to do something cool.  Yah, you heard me, math can be fun!
//
// 3D programming is by FAR the coolest thing to program.
//
//
// © 2000-2006 GameTutorials
