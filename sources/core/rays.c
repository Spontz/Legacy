/*
	ray.c: simple rays render
*/

#include "../main.h"

// ******************************************************************

tRaySystem* rays_init(tRaySystem* myRaySystem) {	
	myRaySystem = malloc(sizeof(tRaySystem));
	memset(myRaySystem, 0, sizeof(tRaySystem));
	
	myRaySystem->firstRay = NULL;
	myRaySystem->raysCount = 0;
	myRaySystem->firstFrame = 1;
	
	return myRaySystem;
}

void rays_render (tRaySystem *myRaySystem) {
	tRay *myRay;
	int i;

	if (myRaySystem->raysCount <= 0) return;

	// Set the texture to the chosen RTT buffer
	tex_properties(demoSystem.texRenderingBuffer[myRaySystem->renderingBuffer], MODULATE);
	tex_bind (demoSystem.texRenderingBuffer[myRaySystem->renderingBuffer]);
	
	if (myRaySystem->disableDepthTest == 1) glDisable(GL_DEPTH_TEST);

	myRay = myRaySystem->firstRay;
						
	//	glDisable(GL_TEXTURE_2D);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (i = 0; i < myRaySystem->raysCount; i++) {		
		// if the ray is alive
		if (myRay->life > 0.0f) {
	
			glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
			
			glLineWidth ((float)myRay->size);	
				
			glBegin (GL_LINES);
				glVertex3d (myRay->source.x     , myRay->source.y     , myRay->source.z     );
				glVertex3d (myRay->destination.x, myRay->destination.y, myRay->destination.z);
			glEnd ();
		}
		
		myRay = myRay->nextRay;
	}

	tex_properties(demoSystem.texRenderingBuffer[myRaySystem->renderingBuffer], 0);
	
	if (myRaySystem->disableDepthTest == 1) glEnable(GL_DEPTH_TEST);

}

// ******************************************************************

void rays_update (tRaySystem *myRaySystem) {
	// Update the properties of every ray based in the evaluation of the expressions defined by the user
	int i;
	double age;
	tRay *myRay;
	
	if (myRaySystem->raysCount <= 0) return;
	
	myRay = myRaySystem->firstRay;
	
	exprValListAdd(myRaySystem->Evaluator.v, "count", (EXPRTYPE)myRaySystem->raysCount);
			
	for (i = 0; i < myRaySystem->raysCount; i++) {
		age = (double)mySection->runTime - (double)myRay->creationTime;

		if ((myRay->life >= 0) && (age >= 0))
			{
			/*
			isaac: Can't never be 0 because of the previous if

			if (age < 0)
				dkernel_error("ray2: age < 0");
			*/
			
			// Insert the custom values into the equation
			exprValListAdd(myRaySystem->Evaluator.v, "sx", (EXPRTYPE)myRay->source.x);
			exprValListAdd(myRaySystem->Evaluator.v, "sy", (EXPRTYPE)myRay->source.y);
			exprValListAdd(myRaySystem->Evaluator.v, "sz", (EXPRTYPE)myRay->source.z);
			
			exprValListAdd(myRaySystem->Evaluator.v, "dx", (EXPRTYPE)myRay->destination.x);
			exprValListAdd(myRaySystem->Evaluator.v, "dy", (EXPRTYPE)myRay->destination.y);
			exprValListAdd(myRaySystem->Evaluator.v, "dz", (EXPRTYPE)myRay->destination.z);
								
			exprValListAdd(myRaySystem->Evaluator.v,   "n", (EXPRTYPE)i);
			exprValListAdd(myRaySystem->Evaluator.v, "age", (EXPRTYPE)age);
			
			// Evaluate the equations
			myRaySystem->Evaluator.err = exprEval(myRaySystem->Evaluator.o, &myRaySystem->Evaluator.result);

			// Check for errors
			if(myRaySystem->Evaluator.err != EXPR_ERROR_NOERROR) section_error("ray:rays_update: Expression evaluation Error (%d): %s", myRaySystem->Evaluator.err, myRaySystem->Evaluator.equation);

			// Retrieve the values and assign them
			/*exprValListGet(myRaySystem->Evaluator.v,   "red", &myRay->red       );
			exprValListGet(myRaySystem->Evaluator.v, "green", &myRay->green     );
			exprValListGet(myRaySystem->Evaluator.v,  "blue", &myRay->blue      );
			exprValListGet(myRaySystem->Evaluator.v, "alpha", &myRay->alpha     );
			
			exprValListGet(myRaySystem->Evaluator.v,     "x", &myRay->position.x);
			exprValListGet(myRaySystem->Evaluator.v,     "y", &myRay->position.y);
			exprValListGet(myRaySystem->Evaluator.v,     "z", &myRay->position.z);
			
			exprValListGet(myRaySystem->Evaluator.v,    "rx", &myRay->rotation.x);
			exprValListGet(myRaySystem->Evaluator.v,    "ry", &myRay->rotation.y);
			exprValListGet(myRaySystem->Evaluator.v,    "rz", &myRay->rotation.z);*/
			
			exprValListGet(myRaySystem->Evaluator.v,  "size", &myRay->size      );
		
			exprValListGet(myRaySystem->Evaluator.v,  "life", &myRay->life      );
		}
		
		myRay = myRay->nextRay;
	}
}

// ******************************************************************

void rays_generate(tRaySystem *myRaySystem) {
	// Create the rays at the defined rate
	int i, chosen;
	tRay *newRay;
	tRay *previousRay;
	double raysToCreate;
	double rate;
	double initialRays;
	
	// Get the current creation rate
	myRaySystem->rate.err = exprEval(myRaySystem->rate.o, &myRaySystem->rate.result);
	
	// Check for errors
	if(myRaySystem->rate.err != EXPR_ERROR_NOERROR) {
		section_error("ray:rays_generate: Expression evaluation Error (%d): %s", myRaySystem->rate.err, myRaySystem->rate.equation);
		rate = 0;
	} else {
		// Retrieve the creation rate
		exprValListGet(myRaySystem->rate.v, "rate", &rate);
		exprValListGet(myRaySystem->rate.v, "initialRays", &initialRays);
	}
	
	if (myRaySystem->firstFrame == 1) {
		raysToCreate = initialRays;
		myRaySystem->firstFrame = 0;
	} else {	
		raysToCreate = rate * (double)demoSystem.frameTime;
	}	
		
	raysToCreate += (double)myRaySystem->remaining;
	myRaySystem->remaining = (float)(raysToCreate - (double)((int)raysToCreate)); // We save the decimal part of the variable, that will be taken into acount in the next iteration
		
	// Return, if there are no rays to create
	if (raysToCreate <= 0) return;
		
	if (myRaySystem->firstRay == NULL) {
		previousRay = NULL;
	} else {
		previousRay = myRaySystem->firstRay->previousRay;
	}
	
	for (i=0; i<(int)raysToCreate; i++) {
		newRay = malloc(sizeof(tRay));
		
		/* Clear memory */
		memset(newRay, 0, sizeof(tRay));

		chosen = rand() % myRaySystem->sourcesCount;

		// Current position of the source
		newRay->source.x = myRaySystem->sources[chosen].x;
		newRay->source.y = myRaySystem->sources[chosen].y;
		newRay->source.z = myRaySystem->sources[chosen].z;
				
		chosen = rand() % myRaySystem->destinationsCount;
		
		// Current position of the Ray
		newRay->destination.x = myRaySystem->sources[chosen].x;
		newRay->destination.y = myRaySystem->sources[chosen].y;
		newRay->destination.z = myRaySystem->sources[chosen].z;
		
		// Store the time in which the Ray is being created
		newRay->creationTime = (double)mySection->runTime;
		
		// Other default values
		newRay->life = 1;
		newRay->size = 1;
		
		if (myRaySystem->firstRay == NULL) {
			myRaySystem->firstRay = newRay;
		} else {
			previousRay->nextRay = newRay;
			newRay->previousRay = previousRay;
		}
		
		// Save the last Ray in the first Ray's last Ray pointer (like in a loop of Rays)
		myRaySystem->firstRay->previousRay = newRay;
		newRay->nextRay = myRaySystem->firstRay;
			
		previousRay = newRay;
		myRaySystem->raysCount++;
	}
}

// ******************************************************************

void clearRays (tRaySystem *myRaySystem) {
	// Delete all rays
	int i;
	tRay *myRay;
	tRay *tempRay;
	int deletedRays;

	deletedRays = 0;
	myRay = myRaySystem->firstRay;
				
	for (i=0; i<myRaySystem->raysCount; i++) {
			// Adjust the next and previous points in order to leave the ray to delete alone
			tempRay = myRay->previousRay;
			tempRay->nextRay = myRay->nextRay;
			tempRay = myRay->nextRay;
			tempRay->previousRay = myRay->previousRay;
			
			// And free it from memory
			free(myRay);
			myRay = tempRay;
			
			deletedRays++;
	}

	// Refresh the ray count	
	// hack, todo: what is this?
	//myRaySystem->raysCount -= deletedRays;
	myRaySystem->raysCount = 0;
	myRaySystem->firstRay = NULL;
}