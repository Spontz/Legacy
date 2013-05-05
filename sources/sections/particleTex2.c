#include "../interface/demo.h"

// ******************************************************************

// section local variables
typedef struct {
	tParticleSystem *myParticleSystem;		// Pointer to the particle system
	int texture;							// Texture that will serve to position the sources
	int threshold;							// Value between 0 and 255 used to discriminate the valid and invalid particles
	int useRTTsources;						// Use a RTT texture as particle sources
} particleTex2_section;

static particleTex2_section *local;

// ******************************************************************

void preload_particleTex2 () {
}

// ******************************************************************

void update_sources(tParticleSystem* myParticleSystem, int threshold) {
	char *tex;
	int i, j, parts, pixels;
	unsigned char *valid;
	unsigned int k,k0,k1,k2;
	int width, height;
	
	parts = 0;
	
	tex    = tex_array[(int)local->texture]->data;

	if (tex == 0)
		{
		dkernel_trace("particleTex2: update_sources: tex == 0");
		return;
		}

	width  = tex_array[(int)local->texture]->width;
	height = tex_array[(int)local->texture]->height;
	pixels = width * height;
	
	valid = calloc (pixels, sizeof(int));
	
	switch(tex_array[(int)local->texture]->format)
		{
		case GL_BGR_EXT:
			k = 3;
			k0 = 2;
			k1 = 1;
			k2 = 0;
			break;
		case GL_BGRA_EXT:
			k = 4;
			k0 = 2;
			k1 = 1;
			k2 = 0;
			break;
		case GL_RGBA:
			k = 4;
			k0 = 0;
			k1 = 1;
			k2 = 2;
			break;
		default:
			k = 3;
			k0 = 0;
			k1 = 1;
			k2 = 2;
			break;
		}

	threshold = threshold * k;

	#pragma omp parallel for
	for (i=0; i<pixels; ++i)
		{
		// mark valid sources 
		if ((tex[i*k] + tex[i*k+1] + tex[i*k+2]) > threshold)
			{
			valid[i] = 1;
			#pragma omp atomic
				++parts;
			}
		}

	myParticleSystem->sources      = (tVector *) malloc(parts * sizeof(tVector));
	myParticleSystem->sourcesColor = (tColor  *) malloc(parts * sizeof(tColor ));
	myParticleSystem->sourcesCount = parts;

	parts = 0;
	pixels = 0;
	
	for (i=0; i<height; i++) {
		for (j=0; j<width; j++) {

			if (valid[pixels]) {
				
				if (local->useRTTsources > 0) {
					myParticleSystem->sources[parts].x = (double)j;
					myParticleSystem->sources[parts].y = (double)i; 
				} else {
					myParticleSystem->sources[parts].x =    (double)j - (double)(0.5 * width);
					myParticleSystem->sources[parts].y = - ((double)i - (double)(0.5 * height));
				}
			
				myParticleSystem->sources[parts].z =    (double)0;

				myParticleSystem->sourcesColor[parts].r = (double)(tex[pixels * k+k0] * 0.0039215686);
				myParticleSystem->sourcesColor[parts].g = (double)(tex[pixels * k+k1] * 0.0039215686);
				myParticleSystem->sourcesColor[parts].b = (double)(tex[pixels * k+k2] * 0.0039215686);

				parts++;
			}

			pixels++;
		}
	}

	free (valid);
}

// ******************************************************************

void load_particleTex2 () {
	char *evalEquation;
	char *evalSources;
	int equationSize;

	// script validation
	if ((mySection->paramNum < 3) || (mySection->stringNum != 10)) {
		section_error("at least 3 params and 10 strings needed");
		return;
	}

	local = malloc(sizeof(particleTex2_section));
	memset(local, 0, sizeof(particleTex2_section));

	local->myParticleSystem = particles_init(local->myParticleSystem);

	mySection->vars = (void *) local;

	// texture rendering buffer to use as particle texture
	local->myParticleSystem->renderingBuffer = (int)mySection->param[0];

	if (mySection->paramNum > 1) {
		local->myParticleSystem->disableDepthTest = (int)mySection->param[1];
	} else {
		local->myParticleSystem->disableDepthTest = 1;
	}
	
	local->threshold = (int)mySection->param[2];
	
	// Load the texture that will serve to position the sources
	if (mySection->param[3] > 0)
		{
		local->texture = demoSystem.texRenderingBuffer[(int)mySection->param[4]];
		local->useRTTsources = 1;
		tex_array[local->texture]->data = malloc (tex_array[local->texture]->width * tex_array[local->texture]->height * tex_array[local->texture]->iformat * sizeof(char));
		}
	else
		{
		local->texture = tex_load((char *)mySection->strings[0], USE_CACHE, FALSE);
		if (local->texture==-1)
			return;
		local->useRTTsources = 0;
		// Calculate where to put the particles
		update_sources    (local->myParticleSystem, local->threshold);
		}

	// Equation defining the rate at which the particles are created (particles created per second)
	local->myParticleSystem->rate.equation = mySection->strings[1];
	
	// Particle evaluator
	equationSize  = strlen(mySection->strings[2]);
	equationSize += strlen(mySection->strings[3]);
	equationSize += strlen(mySection->strings[4]);
	equationSize += strlen(mySection->strings[5]);
	equationSize += strlen(mySection->strings[6]);
	evalEquation = calloc(equationSize+1, sizeof(char));
	strcpy(evalEquation, (char *) mySection->strings[2]);
	strcat(evalEquation, (char *) mySection->strings[3]);
	strcat(evalEquation, (char *) mySection->strings[4]);
	strcat(evalEquation, (char *) mySection->strings[5]);
	strcat(evalEquation, (char *) mySection->strings[6]);
	local->myParticleSystem->Evaluator.equation = evalEquation;
	
	// Sources evaluator
	equationSize  = strlen(mySection->strings[7]);
	equationSize += strlen(mySection->strings[8]);
	equationSize += strlen(mySection->strings[9]);
	evalSources = calloc(equationSize+1, sizeof(char));
	strcpy(evalSources, (char *) mySection->strings[7]); //-V525
	strcat(evalSources, (char *) mySection->strings[8]);
	strcat(evalSources, (char *) mySection->strings[9]);
	local->myParticleSystem->Sources.equation = evalSources;
	
	// Init the expression evaluation library
	initExpression(&local->myParticleSystem->Sources);
	initExpression(&local->myParticleSystem->Evaluator);
	initExpression(&local->myParticleSystem->rate);
	
	// Particles UV coordinates
	local->myParticleSystem->particleUmax = 1;
	local->myParticleSystem->particleVmax = 1;
	mySection->loaded=1;
	}

// ******************************************************************

void init_particleTex2 () {
	local = (particleTex2_section *) mySection->vars;
	local->myParticleSystem->firstFrame = 1;
	clearParticles(local->myParticleSystem);
}

// ******************************************************************

void render_particleTex2 () {
	local = (particleTex2_section *) mySection->vars;

	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}

	if (mySection->hasAlpha) {
		float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(mySection->alphaFunc, alpha);
	}
	
	// Update the particle sources if we are using a RTT texture
	if (local->useRTTsources == 1) {
		// Update the sources texture
		tex_bind(local->texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_array[local->texture]->data);
		update_sources (local->myParticleSystem, local->threshold);
	}
	
	if (local->useRTTsources > 0) {
		// Put the camera in 2D mode
		glMatrixMode (GL_PROJECTION);
		glPushMatrix ();
		glLoadIdentity ();
		glOrtho(glDriver.vpXOffset, glDriver.vpWidth, glDriver.vpYOffset, glDriver.vpHeight, -1, 1);
		glMatrixMode (GL_MODELVIEW);
		glPushMatrix ();
		glLoadIdentity ();
	} else {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();		
	}
	
		// Insert the default section values into the size equation
		insertSectionVariables(&local->myParticleSystem->Evaluator);
		insertSectionVariables(&local->myParticleSystem->rate);
		
		particles_generate(local->myParticleSystem);
	
		place_object (&local->myParticleSystem->Sources);
		
		particles_update  (local->myParticleSystem);
		particles_render  (local->myParticleSystem);
	
	if (local->useRTTsources > 0) {
		// Restore the camera to the previous state
		glMatrixMode (GL_MODELVIEW);
		glPopMatrix ();
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
	} else {
		glPopMatrix();
	}
	
	if (local->useRTTsources > 0) {
		free(local->myParticleSystem->sources);
		free(local->myParticleSystem->sourcesColor);
	}
	
	if (mySection->hasAlpha) glDisable(GL_ALPHA_TEST);
	if (mySection->hasBlend) glDisable(GL_BLEND);
}

// ******************************************************************

void end_particleTex2() {
	local = (particleTex2_section *) mySection->vars;
	clearParticles(local->myParticleSystem);
	// if (local->useRTTsources == 1) free(tex_array[local->texture]->data);
}

// ******************************************************************