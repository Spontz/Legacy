#ifndef __PARTICLE_H
#define __PARTICLE_H
#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#include "math3d.h"

// ******************************************************************

typedef struct {
	tVector source;					// Position of the place in which the particle was created
	tColor  sourceColor;			// Color assigned to the sources in which this particle was born
	
	tVector position;				// Current position of the particle
	tVector speed;					// Current speed of particle
	tVector acceleration;			// Current acceleration of the particle
	tVector rotation;				// Current rotation (facing the user means 0,0,0)
	tVector scale;					// Current scale (default scale is 1,1,1)

	double red, green, blue, alpha;	// Current rgba color of the particle
	
	double size;					// 1.0 means a size equal to the texture size
	double life;					// Life remaining for the particle. If life < 0, the particle will be deleted
	double creationTime;			// Time (in seconds and relative to the rection) in which the particle was created
	
	void *nextParticle;				// Pointer to the next particle
	void *previousParticle;			// Pointer to the previous particle
	
	double aspectRatio;				// Aspect ratio of the particle
} tParticle;

typedef struct {
	tVector *sources;				// 3D Position of particle sources
	tColor  *sourcesColor;			// Color assigned to this source
	int      sourcesCount;			// Total count of particle sources
	
	tParticle *firstParticle;		// Pointer to the first particle
	int particlesCount;				// Number of particles existing at this time
	int disableDepthTest;			// 1: Disable depth test // 0: Enable depth test
	
	tExpression Sources;			// A equation containing the calculations used for sources positioning
	tExpression Evaluator;			// A equation containing the calculations used for particle evaluation
	tExpression rate;				// Equation describing the rate at which thte particles must be created
	
	float remaining;				// rate = 6 part/sec; frame drawing time = 0,3 sec; Then we should create 1,8 parts. We will create 1 and will save the 0,8 in this variable.
	
	float particleUmax;
	float particleVmax;
	
	int firstFrame;					// 1: This is the first painted frame // 0: This is not the first painted frame
	int renderingBuffer;			// Integer from 0 to 9 with the rendering buffer bwing used as particle texture
	
	//int leaveTrails;				// 1: leave particle trails. 0: no trails.
	
	int mode;						// 0: normal. 1: gravity. 2: lorentz
	
	tVector gravityCenter;			// The particle attractor center
	double gravityCenterMass;			// Mass of the gravity attractor (particles have a mass of 1)
} tParticleSystem;

// ******************************************************************

tParticleSystem* particles_init     (tParticleSystem *myParticleSystem);
void particles_generate (tParticleSystem *myParticleSystem);
void particles_render   (tParticleSystem *myParticleSystem);
void particles_update   (tParticleSystem *myParticleSystem);
void clearParticles     (tParticleSystem *myParticleSystem);

// ******************************************************************

typedef struct
	{
	vector_t	m_TransformedPosition;
	tParticle*	m_pParticle;
	}
particle_reference;

extern void sort_particles(particle_reference* pFirst, unsigned int count);

#ifdef __cplusplus
}
#endif

#endif
