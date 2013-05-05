#include "../interface/demo.h"

// ******************************************************************

// particle definition
typedef struct {
	H3dsScene	*object;					// The object used to positionate the particle sources
	tParticleSystem *myParticleSystem;		// Pointer to the particle system
} particleMatrix2_section;

static particleMatrix2_section *local;

// ******************************************************************

void preload_particleMatrix2 () {
}

// ******************************************************************

void load_particleMatrix2() {
	char *evalEquation;
	char *evalSources;
	int equationSize;
	H3dsMeshObj *mesh;
	int i;
	int i2;
	int p;

	// script validation
	if ((mySection->paramNum   <  1) || (mySection->stringNum != 10)) {
		section_error("at least 1 param and 10 strings needed");
		return;
	}

	local = malloc(sizeof(particleMatrix2_section));
	memset(local, 0, sizeof(particleMatrix2_section));

	local->myParticleSystem = particles_init(local->myParticleSystem);
	
	mySection->vars = (void *) local;

	// texture rendering buffer to use as particle texture
	local->myParticleSystem->renderingBuffer = (int)mySection->param[0];

	// load object model
	local->object = model_load(mySection->strings[0], USE_CACHE);
	if (!local->object) {
		section_error("particleMatrix2: Error loading %s", mySection->strings[0]);
		return;
	}
	
	if (mySection->param[1] > 0) {
		local->myParticleSystem->disableDepthTest = 1;
	} else {
		local->myParticleSystem->disableDepthTest = 0;
	}
	
	// Equation defining the rate at which the particles are created (particles created per second)
	local->myParticleSystem->rate.equation = mySection->strings[1];
	
	// We only take into account the first mesh as particle sources
	local->myParticleSystem->sourcesCount = 0;
	
	for (i2 = 0; i2 < local->object->meshobjs; ++i2)
	{
		mesh = &local->object->meshobjlist[i2];
		
		// Get and store the particle source positions
		local->myParticleSystem->sourcesCount += mesh->verts;		
	}
		
	local->myParticleSystem->sources      = (tVector *) malloc(sizeof(tVector) * local->myParticleSystem->sourcesCount);
	local->myParticleSystem->sourcesColor = (tColor  *) malloc(sizeof(tColor ) * local->myParticleSystem->sourcesCount);
	
	p=0;
	
	for (i2 = 0; i2 < local->object->meshobjs; ++i2)
		{
		
		mesh = &local->object->meshobjlist[i2];
			
		for (i=0; i<mesh->verts; ++i,++p)
			{
			local->myParticleSystem->sources[p].x = mesh->vertlist[i].x;
			local->myParticleSystem->sources[p].y = mesh->vertlist[i].y;
			local->myParticleSystem->sources[p].z = mesh->vertlist[i].z;
			
			local->myParticleSystem->sourcesColor[p].b = 1.0;
			local->myParticleSystem->sourcesColor[p].g = 1.0;
			local->myParticleSystem->sourcesColor[p].r = 1.0;
			}
		}
	
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

void init_particleMatrix2 () {
	local = (particleMatrix2_section *) mySection->vars;
	local->myParticleSystem->firstFrame = 1;
	clearParticles(local->myParticleSystem);
}

// ************************************************************

void render_particleMatrix2 () {	
	local = (particleMatrix2_section *) mySection->vars;
	
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}

	if (mySection->hasAlpha) {
		float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(mySection->alphaFunc, alpha);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
		// Insert the default section values into the size equation
		insertSectionVariables(&local->myParticleSystem->Evaluator);
		insertSectionVariables(&local->myParticleSystem->rate);
		
		particles_generate(local->myParticleSystem);
		
		// place the sources
		place_object (&local->myParticleSystem->Sources);
			
		particles_update  (local->myParticleSystem);
		particles_render  (local->myParticleSystem);
		
	glPopMatrix();
		
	if (mySection->hasAlpha) glDisable(GL_ALPHA_TEST);
	if (mySection->hasBlend) glDisable(GL_BLEND);
}

// ******************************************************************

void end_particleMatrix2() {
	local = (particleMatrix2_section *) mySection->vars;
	clearParticles(local->myParticleSystem);
}
