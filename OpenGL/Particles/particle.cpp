#include "particle.h"
#include "gl_obj.h"

// Default constructor -- Zero everything out
CParticle::CParticle()
{
	mColor = ARGB(255, 255, 255, 255);
	mSize = 0.0f;
	mLife = 0.0f;
	mAngle = 0.0f;
	
	// All other variables gets initialized with it's constructor
}

// Init the particle
bool CParticle::init(const CPos &p, const CVector &v, float lifeSpan, float s, float a, int c,
					 const char *texName)
{
	mPos = p; // Set the position
	mVel = v; // Set the velocity
	
	// Don't allow initialization of a "dead" particle 
	if(lifeSpan <= 0.0f)
		return false;
	
	mLife = lifeSpan; // Set the time, in seconds, the particle will remain alive
	
	// Size must be positive
	if(s <= 0.0f)
		return false;
		
	mSize = s; // Set the size
	mAngle = a; // Set the angle of texture UV rotation per second
	mColor = c; // Set the color
	
	if(texName) // If a texture name was specified
		return mTexture.load(texName); // Load the texture
	else
		return true;
}

void CParticle::process(float dt)
{
	// If particle is dead, reset it's position
	if(isAlive() == false)
	{
		mLife = RAND(0.0f, 2.0f); // Make it alive again
		mPos = CPos(0,0,0); // Reposition to the emission point
			return;
	}		

	// Apply velocity
	mPos.x += mVel.x * dt;
	mPos.y += mVel.y * dt;
	mPos.z += mVel.z * dt;
	
	// Apply gravity
	mPos.y += kParticleGravity * dt;
		
	mLife -= dt; // Decrease particle's life span
	
	// Apply rotation by "angle" per second if applicable
	if(mAngle != 0.0f)
		mTexture.setRotation(mTexture.getRotAngle() + (mAngle * dt));
}

void CParticle::render()
{
	// If particle isn't alive, then there is nothing to render
	if(isAlive() == false)
		return;
	
	// Can't render a particle that does not have a valid texture 
	if(mTexture.getId() == TEXTURE_NOT_LOADED)
		return;
	
	// This OpenGL functions makes the Z-buffer "read only".  That means OpenGL will
	// use current z-buffer values to determine if a particle should be rendered or not, BUT,
	// if it does render a particle it will NOT set any z-buffer values.  Why do we do this?
	// Well we want the particles to blend, and not cover each other up.  So now we can
	// render them in the same Z-depth and they will blend, but at the same time, they 
	// won't render over top of anything with a lesser Z-depth
	glDepthMask(false);
	
	BYTE red, green, blue;
	
	// Set the color based on the life of the particle
	if(mLife < 1.0f)
	{
		// If the particle has less than a second to live, we will
		// decrease the R, G, and B value of it's color by multipling
		// by "mLife", this will in turn fade the particle to black, which
		// is our applications background color 
		red = BYTE(GET_R(mColor) * mLife);
		green = BYTE(GET_G(mColor) * mLife);
		blue = BYTE(GET_B(mColor) * mLife);
	}
	else
	{	
		// Just get the RGB components of the color
		red = GET_R(mColor);
		green = GET_G(mColor);
		blue = GET_B(mColor);
	}
	
	// Set the color for rendering
	glColor3ub(red, green, blue); 

	// Set the texture for rendering
	mTexture.select();
	
	glPushMatrix();
	
		// Move to the world position of where to draw the particle
		glTranslatef(mPos.x, mPos.y, mPos.z);
		
		float halfSize = mSize * 0.5f;
			
		// Draw the particle
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-halfSize, halfSize, 0.0f); // Top left vertex
			
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-halfSize, -halfSize, 0.0f); // Bottom left vertex
			
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(halfSize, -halfSize, 0.0f); // Bottom right vertex
			
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(halfSize, halfSize, 0.0f); // Top right vertex
		glEnd();
		
	glPopMatrix();
	
	glDepthMask(true); // Put the Z-buffer back into it's normal "Z-read and Z-write" state
}
	
	
	
		
	
	
	
	
	
	
