/*
	particle.c: simple particle render
*/

#include "../main.h"

// ******************************************************************
tParticleSystem* particles_init(tParticleSystem* myParticleSystem) {	
	myParticleSystem = malloc(sizeof(tParticleSystem));
	memset(myParticleSystem, 0, sizeof(tParticleSystem));
	
	myParticleSystem->firstParticle = NULL;
	myParticleSystem->particlesCount = 0;
	myParticleSystem->firstFrame = 1;
	
	return myParticleSystem;
}


void vector_transform2(vector_t *v, matrix_t m)
	{
	float x, y, z;

	x = v->x;
	y = v->y;
	z = v->z;

	v->x = x * m[0][0] + y * m[0][1] + z * m[0][2] + m[0][3];
	v->y = x * m[1][0] + y * m[1][1] + z * m[1][2] + m[1][3];
	v->z = x * m[2][0] + y * m[2][1] + z * m[2][2] + m[2][3];
	}

void particles_render (tParticleSystem *myParticleSystem) {
	float matrix[16];
	vector_t v1;
	vector_t v2;
	vector_t v3;
	vector_t v4;
	vector_t side;
	vector_t up;
	tVector *p;
	vector_t side0;
	vector_t up0;
	tParticle *myParticle;
	int i;
	particle_reference* pSortedParticles;
	int SortedParticlesCount;
	
	if (myParticleSystem->particlesCount <= 0)
		return;
	
	// Set the texture to the chosen RTT buffer
	tex_properties(demoSystem.texRenderingBuffer[myParticleSystem->renderingBuffer], MODULATE);
	tex_bind(demoSystem.texRenderingBuffer[myParticleSystem->renderingBuffer]);
	
	if (myParticleSystem->disableDepthTest == 1)
		glDisable(GL_DEPTH_TEST);
	
	myParticle = myParticleSystem->firstParticle;
	
	glGetFloatv (GL_MODELVIEW_MATRIX, &matrix[0]);
	side0.x = matrix[0];
	side0.y = matrix[4];
	side0.z = matrix[8];
	
	up0.x = matrix[1];
	up0.y = matrix[5];
	up0.z = matrix[9];
	
	//	glDisable(GL_TEXTURE_2D);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// vector_normalize(&side0);
	// vector_normalize(&up0);
	
	SortedParticlesCount = 0;
	pSortedParticles = malloc(sizeof(tParticle)*myParticleSystem->particlesCount);
	
	for (i = 0; i < myParticleSystem->particlesCount; ++i, myParticle = myParticle->nextParticle)
	{
		if (myParticle->life <= 0.0f) // skip dead particles
			continue;
		
		pSortedParticles[SortedParticlesCount].m_TransformedPosition.x = (float)myParticle->position.x;
		pSortedParticles[SortedParticlesCount].m_TransformedPosition.y = (float)myParticle->position.y;
		pSortedParticles[SortedParticlesCount].m_TransformedPosition.z = (float)myParticle->position.z;
		
		vector_transform(&pSortedParticles[SortedParticlesCount].m_TransformedPosition, *(matrix_t*)(void*)&matrix);
		
		pSortedParticles[SortedParticlesCount++].m_pParticle = myParticle;
	}
	
	sort_particles(pSortedParticles, SortedParticlesCount);
	
	for (i = 0; i < SortedParticlesCount; ++i)
	{
		myParticle = pSortedParticles[i].m_pParticle;
		
		if (myParticle->aspectRatio <= 0.0) {
			vector_scale(&side0, (float)myParticle->size * gldrv_get_aspect_ratio(), &side);
			
		} else {
			vector_scale(&side0, (float)(myParticle->size * myParticle->aspectRatio), &side);
			
			myParticleSystem->particleVmax = 1;
			myParticleSystem->particleUmax = 1;
		}
		
		vector_scale(&up0,   (float)myParticle->size, &up);
		
		glColor4d(myParticle->red, myParticle->green, myParticle->blue, myParticle->alpha);
		
		p = &myParticle->position;

/*		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		{
			glRotated(myParticle->rotation.x, 1, 0, 0);
			glRotated(myParticle->rotation.y, 0, 1, 0);
			glRotated(myParticle->rotation.z, 0, 0, 1);
			//glRotated((float)p->y*100, 0, 0, 1);
			
			glBegin(GL_QUADS);
			{
				v1.x = (float)p->x - side.x;
				v1.y = (float)p->y - side.y;
				v1.z = (float)p->z - side.z;
				v4 = v1;
				
				v1.x = v1.x + up.x;
				v1.y = v1.y + up.y;
				v1.z = v1.z + up.z;
				
				glTexCoord2f(0, myParticleSystem->particleVmax);
				glVertex3fv(&v1.x);
				
				v2.x = (float)p->x + side.x;
				v2.y = (float)p->y + side.y;
				v2.z = (float)p->z + side.z;
				v3 = v2;
				
				v2.x = v2.x + up.x;
				v2.y = v2.y + up.y;
				v2.z = v2.z + up.z;
				
				glTexCoord2f(myParticleSystem->particleUmax, myParticleSystem->particleVmax);
				glVertex3fv(&v2.x);	
				
				v3.x = v3.x - up.x;
				v3.y = v3.y - up.y;
				v3.z = v3.z - up.z;
				
				glTexCoord2f(myParticleSystem->particleUmax, 0);
				glVertex3fv(&v3.x);
				
				v4.x = v4.x - up.x;
				v4.y = v4.y - up.y;
				v4.z = v4.z - up.z;
				
				glTexCoord2f(0, 0);
				glVertex3fv(&v4.x);
			}
			glEnd();
		}
		glPopMatrix();
*/
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		{
			glTranslatef((GLfloat)p->x, (GLfloat)p->y, (GLfloat)p->z);
			glRotated(myParticle->rotation.x, 1, 0, 0);
			glRotated(myParticle->rotation.y, 0, 1, 0);
			glRotated(myParticle->rotation.z, 0, 0, 1);
			
			glBegin(GL_QUADS);
			{
				v1.x = -side.x;
				v1.y = -side.y;
				v1.z = -side.z;
				v4 = v1;
				
				v1.x += up.x;
				v1.y += up.y;
				v1.z += up.z;
				
				glTexCoord2f(0, myParticleSystem->particleVmax);
				glVertex3fv(&v1.x);
				
				v2.x = side.x;
				v2.y = side.y;
				v2.z = side.z;
				v3 = v2;
				
				v2.x += up.x;
				v2.y += up.y;
				v2.z += up.z;
				
				glTexCoord2f(myParticleSystem->particleUmax, myParticleSystem->particleVmax);
				glVertex3fv(&v2.x);	
				
				v3.x -= up.x;
				v3.y -= up.y;
				v3.z -= up.z;
				
				glTexCoord2f(myParticleSystem->particleUmax, 0);
				glVertex3fv(&v3.x);
				
				v4.x -= up.x;
				v4.y -= up.y;
				v4.z -= up.z;
				
				glTexCoord2f(0, 0);
				glVertex3fv(&v4.x);
			}
			glEnd();
		}
		glPopMatrix();

		
	}
	
	tex_properties(demoSystem.texRenderingBuffer[myParticleSystem->renderingBuffer], 0);
	
	if (myParticleSystem->disableDepthTest == 1)
		glEnable(GL_DEPTH_TEST);
	
	free(pSortedParticles);
}

// ******************************************************************

void particles_update (tParticleSystem *myParticleSystem) {
	// Update the properties of every particle based in the evaluation of the expressions defined by the user
	int i;
	double age, r;
	tParticle *myParticle;
	
	if (myParticleSystem->particlesCount <= 0) return;
	
	myParticle = myParticleSystem->firstParticle;
	
	exprValListAdd(myParticleSystem->Evaluator.v, "count", (EXPRTYPE)myParticleSystem->particlesCount);
			
	for (i = 0; i < myParticleSystem->particlesCount; i++) {
		age = (double)mySection->runTime - (double)myParticle->creationTime;

		if (  (myParticle->life >= 0) && (age >= 0)  )
			{
			/*
			TODO: Clean this comment
			isaac: Commented because it can't neve be < 0 because of "if (  (myParticle->life >= 0) && (age >= 0)  )"

			if (age < 0)
				dkernel_error("particleEngine: age < 0");
			*/
			
			r = sqrt((myParticle->position.x - myParticle->source.x)*(myParticle->position.x - myParticle->source.x) + (myParticle->position.y - myParticle->source.y)*(myParticle->position.y - myParticle->source.y) + (myParticle->position.z - myParticle->source.z)*(myParticle->position.z - myParticle->source.z));			
				
			// Insert the custom values into the equation
			exprValListAdd(myParticleSystem->Evaluator.v, "px", (EXPRTYPE)myParticle->position.x);
			exprValListAdd(myParticleSystem->Evaluator.v, "py", (EXPRTYPE)myParticle->position.y);
			exprValListAdd(myParticleSystem->Evaluator.v, "pz", (EXPRTYPE)myParticle->position.z);
			
			exprValListAdd(myParticleSystem->Evaluator.v, "px0", (EXPRTYPE)myParticle->source.x);
			exprValListAdd(myParticleSystem->Evaluator.v, "py0", (EXPRTYPE)myParticle->source.y);
			exprValListAdd(myParticleSystem->Evaluator.v, "pz0", (EXPRTYPE)myParticle->source.z);

			exprValListAdd(myParticleSystem->Evaluator.v,  "r0", (EXPRTYPE)myParticle->sourceColor.r);
			exprValListAdd(myParticleSystem->Evaluator.v,  "g0", (EXPRTYPE)myParticle->sourceColor.g);
			exprValListAdd(myParticleSystem->Evaluator.v,  "b0", (EXPRTYPE)myParticle->sourceColor.b);
								
			exprValListAdd(myParticleSystem->Evaluator.v,   "n", (EXPRTYPE)i);
			exprValListAdd(myParticleSystem->Evaluator.v,   "r", (EXPRTYPE)r);
			exprValListAdd(myParticleSystem->Evaluator.v, "age", (EXPRTYPE)age);
			
			// Evaluate the equations
			myParticleSystem->Evaluator.err = exprEval(myParticleSystem->Evaluator.o, &myParticleSystem->Evaluator.result);

			// Check for errors
			if(myParticleSystem->Evaluator.err != EXPR_ERROR_NOERROR) section_error("particleEngine:particles_update: Expression evaluation Error (%d): %s", myParticleSystem->Evaluator.err, myParticleSystem->Evaluator.equation);

			// Retrieve the values and assign them
			exprValListGet(myParticleSystem->Evaluator.v,   "red"       , &myParticle->red        );
			exprValListGet(myParticleSystem->Evaluator.v, "green"       , &myParticle->green      );
			exprValListGet(myParticleSystem->Evaluator.v,  "blue"       , &myParticle->blue       );
			exprValListGet(myParticleSystem->Evaluator.v, "alpha"       , &myParticle->alpha      );
			
			exprValListGet(myParticleSystem->Evaluator.v,     "x"       , &myParticle->position.x );
			exprValListGet(myParticleSystem->Evaluator.v,     "y"       , &myParticle->position.y );
			exprValListGet(myParticleSystem->Evaluator.v,     "z"       , &myParticle->position.z );
			
			exprValListGet(myParticleSystem->Evaluator.v,    "rx"       , &myParticle->rotation.x );
			exprValListGet(myParticleSystem->Evaluator.v,    "ry"       , &myParticle->rotation.y );
			exprValListGet(myParticleSystem->Evaluator.v,    "rz"       , &myParticle->rotation.z );

			exprValListGet(myParticleSystem->Evaluator.v,    "sx"       , &myParticle->scale.x );
			exprValListGet(myParticleSystem->Evaluator.v,    "sy"       , &myParticle->scale.y );
			exprValListGet(myParticleSystem->Evaluator.v,    "sz"       , &myParticle->scale.z );
			
			exprValListGet(myParticleSystem->Evaluator.v,  "size"       , &myParticle->size       );
			exprValListGet(myParticleSystem->Evaluator.v,  "life"       , &myParticle->life       );
			exprValListGet(myParticleSystem->Evaluator.v,  "ratio"      , &myParticle->aspectRatio);
		}
		
		myParticle = myParticle->nextParticle;
	}
}

// ******************************************************************

void particles_generate(tParticleSystem *myParticleSystem) {
	// Create the particles at the defined rate
	int i, chosen;
	tParticle *newParticle;
	tParticle *previousParticle;
	double particlesToCreate;
	double rate;
	double initialParticles;
	
	// Get the current creation rate
	myParticleSystem->rate.err = exprEval(myParticleSystem->rate.o, &myParticleSystem->rate.result);
	
	// Check for errors
	if(myParticleSystem->rate.err != EXPR_ERROR_NOERROR) {
		section_error("particleEngine: particles_generate: Expression evaluation Error (%d): %s", myParticleSystem->rate.err, myParticleSystem->rate.equation);
		rate = 0;
	} else {
		// Retrieve the creation rate
		exprValListGet(myParticleSystem->rate.v, "rate", &rate);
		exprValListGet(myParticleSystem->rate.v, "initialParticles", &initialParticles);
		
		// Retrieve the gravity center
		exprValListGet(myParticleSystem->rate.v, "cx", &myParticleSystem->gravityCenter.x);
		exprValListGet(myParticleSystem->rate.v, "cy", &myParticleSystem->gravityCenter.y);
		exprValListGet(myParticleSystem->rate.v, "cz", &myParticleSystem->gravityCenter.z);

		exprValListGet(myParticleSystem->rate.v, "mass", &myParticleSystem->gravityCenterMass);
	}
	
	if (myParticleSystem->firstFrame == 1) {
		particlesToCreate = initialParticles;
		myParticleSystem->firstFrame = 0;
	} else {	
		particlesToCreate = rate * (double)demoSystem.frameTime;
	}	
		
	particlesToCreate += (double)myParticleSystem->remaining;
	myParticleSystem->remaining = (float)(particlesToCreate - (double)((int)particlesToCreate)); // We save the decimal part of the variable, that will be taken into acount in the next iteration
		
	// Return, if there are no particles to create
	if (particlesToCreate <= 0) return;
		
	if (myParticleSystem->firstParticle == NULL) {
		previousParticle = NULL;
	} else {
		previousParticle = myParticleSystem->firstParticle->previousParticle;
	}
	
	for (i=0; i<(int)particlesToCreate; i++) {
		if (myParticleSystem->sourcesCount > 0) {
			chosen = rand() % myParticleSystem->sourcesCount;
			
			newParticle = malloc(sizeof(tParticle));
			
			/* Clear memory */
			memset(newParticle, 0, sizeof(tParticle));
			
			// Current position of the particle
			newParticle->position.x = myParticleSystem->sources[chosen].x;
			newParticle->position.y = myParticleSystem->sources[chosen].y;
			newParticle->position.z = myParticleSystem->sources[chosen].z;

			// Current position of the source
			newParticle->source.x = myParticleSystem->sources[chosen].x;
			newParticle->source.y = myParticleSystem->sources[chosen].y;
			newParticle->source.z = myParticleSystem->sources[chosen].z;
			
			// Particle source color
			newParticle->sourceColor.r = myParticleSystem->sourcesColor[chosen].r;
			newParticle->sourceColor.g = myParticleSystem->sourcesColor[chosen].g;
			newParticle->sourceColor.b = myParticleSystem->sourcesColor[chosen].b;
					
			// Store the time in which the particle is being created
			newParticle->creationTime = (double)mySection->runTime;
			
			// Other default values
			newParticle->life = 1;
			newParticle->size = 1;
			
			if (myParticleSystem->firstParticle == NULL) {
				myParticleSystem->firstParticle = newParticle;
			} else {
				previousParticle->nextParticle = newParticle;
				newParticle->previousParticle = previousParticle;
			}
			
			// Save the last particle in the first particle's last particle pointer (like in a loop of particles)
			myParticleSystem->firstParticle->previousParticle = newParticle;
			newParticle->nextParticle = myParticleSystem->firstParticle;
				
			previousParticle = newParticle;
			myParticleSystem->particlesCount++;
		}
	}
}

// ******************************************************************

void clearParticles (tParticleSystem *myParticleSystem) {
	// Delete all particles
	int i;
	tParticle *myParticle;
	tParticle *tempParticle;
	int deletedParticles;

	deletedParticles = 0;
	myParticle = myParticleSystem->firstParticle;
				
	for (i=0; i<myParticleSystem->particlesCount; i++) {
			// Adjust the next and previous points in order to leave the particle to delete alone
			tempParticle = myParticle->previousParticle;
			tempParticle->nextParticle = myParticle->nextParticle;
			tempParticle = myParticle->nextParticle;
			tempParticle->previousParticle = myParticle->previousParticle;
			
			// And free it from memory
			free(myParticle);
			myParticle = tempParticle;
			
			deletedParticles++;
	}

	// Refresh the particle count	
	// dkernel_trace("particleMatrix2: %i particles cleared from %i", deletedParticles, local->particlesCount);
	// hack, todo: what is this?:
	// myParticleSystem->particlesCount -= deletedParticles; 
	myParticleSystem->particlesCount = 0;
	myParticleSystem->firstParticle = NULL;
}