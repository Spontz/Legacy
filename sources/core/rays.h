#ifndef __RAYS_H
#define __RAYS_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#include "math3d.h"

// ******************************************************************

typedef struct {
	tVector source;					// Origin of the ray
	tVector destination;			// Destination of the ray
		
	void *nextRay;					// Pointer to the next ray
	void *previousRay;				// Pointer to the previous ray
	
	double size;					// 1.0 means a size equal to the texture size
	double life;					// Life remaining for the ray. If life < 0, the ray will be deleted
	double creationTime;			// Time (in seconds and relative to the rection) in which the ray was created
} tRay;

typedef struct {
	tVector *sources;				// 3D Position of ray sources
	tColor  *sourcesColor;			// Color assigned to every source
	int      sourcesCount;			// Total count of ray sources
	
	tVector *destinations;			// 3D Position of ray destinations
	tColor  *destinationsColor;		// Color assigned to every destinations
	int      destinationsCount;		// Total count of ray destinationss
	
	tRay *firstRay;					// Pointer to the first ray
	int raysCount;					// Number of rays existing at this time
	int disableDepthTest;			// 1: Disable depth test // 0: Enable depth test
	
	tExpression Positioning;		// A equation containing the calculations used for section positioning
	tExpression Evaluator;			// A equation containing the calculations used for ray evaluation
	tExpression rate;				// Equation describing the rate at which thte Rays must be created
	
	float remaining;				// rate = 6 rays/sec; frame drawing time = 0,3 sec; Then we should create 1,8 rays. We will create 1 and will save the 0,8 in this variable.
	
	int firstFrame;					// 1: This is the first painted frame // 0: This is not the first painted frame
	int renderingBuffer;			// Integer from 0 to 9 with the rendering buffer being used as Ray texture
} tRaySystem;

// ******************************************************************

tRaySystem* rays_init	     (tRaySystem *myRaySystem);
       void rays_generate    (tRaySystem *myRaySystem);
       void rays_render      (tRaySystem *myRaySystem);
       void rays_update      (tRaySystem *myRaySystem);
       void clearRays        (tRaySystem *myRaySystem);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
