#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdio.h>
#include "vector.h"
#include "gl_texture.h"

// Creates an ARGB color
#define ARGB(A, R, G, B) ( (int)((A & 0xFF) << 24 | \
								 (R & 0xFF) << 16 | \
								 (G & 0xFF) << 8 | \
								 (B & 0xFF)) )
								 
// Gets the A, R, G, and B from an ARGB color								 
#define GET_A(c) ((c >> 24) & 0xFF)
#define GET_R(c) ((c >> 16) & 0xFF)
#define GET_G(c) ((c >> 8) & 0xFF)
#define GET_B(c) (c & 0xFF)

// Returns a random percent between 0 - 1
#define RAND_PERCENT() ((rand() & 0x7FFF) / ((float)0x7FFF))

// Returns a random number between (and including) "min" and "max"
// Assumes "min" < "max"
#define RAND(min, max) (min + (max - min) * RAND_PERCENT())

// The gravity that effects every particle
// This number was reached by the "what looks good" method %) 								 
const float kParticleGravity = -9.81f / 10.0f;

// The particle
class CParticle
{
	public:
		
		CParticle(); // Default constructor
		
		// This initializes a particle
		bool init(const CPos &pos, const CVector &vel, float lifeSpan, float size,
				  float angle = 0.0f, int color = 0xFFFFFFFF, const char *texName = NULL);
		
		void process(float dt); // Moves the particle each frame
		void render(); // Renders the particle to screen
		
		// If the a particle's life is greater than zero, it's considered active
		// otherwise it's deactivate
		bool isAlive() { return (mLife > 0.0f); }
	
	private:
	
		CPos mPos; // World position
		CVector mVel; // Velocity
		
		int mColor; // ARGB color of particle
		float mSize; // Width/Height dimensions of the particle
		float mLife; // Life span of particle in seconds
		float mAngle; // Angle, in degrees, to rotate the texture's UVs every second
		
		CGLTexture mTexture; // The texture			 
};

#endif
