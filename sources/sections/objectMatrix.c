#include "../interface/demo.h"
#include "../core/drivers/events.h"
#include "../core/particle.h"
#include "../main.h"

// ******************************************************************

// particle definition


typedef struct {
	layer_t		*layers;	
	H3dsScene	*object;					// The object used to positionate the particle sources
	H3dsScene	*obj_part;					// The object used as a particle
	tParticleSystem *myParticleSystem;		// Pointer to the particle system
} objectMatrix_section;

static objectMatrix_section *local;

// ******************************************************************

void preload_objectMatrix () {
}

// ******************************************************************

void load_objectMatrix() {
	char *evalEquation;
	char *evalSources;
	int equationSize;
	H3dsMeshObj *mesh;
	int i;

	// script validation
	if (mySection->stringNum != 14) {
		section_error("14 strings needed");
		return;
	}

	local = malloc(sizeof(objectMatrix_section));
	memset(local, 0, sizeof(objectMatrix_section));

	local->myParticleSystem = particles_init(local->myParticleSystem);

	mySection->vars = (void *) local;

	// load the object that will server as sources
	local->object = model_load(mySection->strings[0], USE_CACHE);
	if (!local->object) {
		section_error("objectMatrix: Error loading object sources: %s", mySection->strings[0]);
		return;
	}
	// load the object that will server as particle (and the shader)
	local->obj_part = model_load(mySection->strings[2], USE_CACHE);
	if (!local->object) {
		section_error("objectMatrix: Error loading particle object: %s", mySection->strings[1]);
		return;
	}
	if (model_upload_tex(local->obj_part, mySection->strings[1]) == -1)
		return;
	
	// load layers for drawing the object
	local->layers = layers_load(mySection->strings[3]);
	
	if (mySection->param[0] > 0) {
		local->myParticleSystem->disableDepthTest = 1;
	} else {
		local->myParticleSystem->disableDepthTest = 0;
	}
	
	// Equation defining the rate at which the particles are created (particles created per second)
	local->myParticleSystem->rate.equation = mySection->strings[4];
	
	// We only take into account the first mesh as particle sources
	mesh = &local->object->meshobjlist[0];
	
	// Get and store the particle source positions
	local->myParticleSystem->sourcesCount = mesh->verts;
	local->myParticleSystem->sources      = (tVector *) malloc(sizeof(tVector) * mesh->verts);
	local->myParticleSystem->sourcesColor = (tColor  *) malloc(sizeof(tColor ) * mesh->verts);

	for (i=0; i<local->myParticleSystem->sourcesCount; i++) {
		local->myParticleSystem->sources[i].x = mesh->vertlist[i].x;
		local->myParticleSystem->sources[i].y = mesh->vertlist[i].y;
		local->myParticleSystem->sources[i].z = mesh->vertlist[i].z;
		
		local->myParticleSystem->sourcesColor[i].r = 1.0;
		local->myParticleSystem->sourcesColor[i].g = 1.0;
		local->myParticleSystem->sourcesColor[i].b = 1.0;
	}
	
	// Particle evaluator
	equationSize = strlen(mySection->strings[5]);
	equationSize += strlen(mySection->strings[6]);
	equationSize += strlen(mySection->strings[7]);
	equationSize += strlen(mySection->strings[8]);
	equationSize += strlen(mySection->strings[9]);
	equationSize += strlen(mySection->strings[10]);
	evalEquation = calloc(equationSize+1, sizeof(char));
	strcat(evalEquation, (char *) mySection->strings[5]);
	strcat(evalEquation, (char *) mySection->strings[6]);
	strcat(evalEquation, (char *) mySection->strings[7]);
	strcat(evalEquation, (char *) mySection->strings[8]);
	strcat(evalEquation, (char *) mySection->strings[9]);
	strcat(evalEquation, (char *) mySection->strings[10]);
	local->myParticleSystem->Evaluator.equation = evalEquation;
	
	// Sources evaluator
	equationSize  = strlen(mySection->strings[11]);
	equationSize += strlen(mySection->strings[12]);
	equationSize += strlen(mySection->strings[13]);
	evalSources = calloc(equationSize+1, sizeof(char));
	strcpy(evalSources, (char *) mySection->strings[11]); //-V525
	strcat(evalSources, (char *) mySection->strings[12]);
	strcat(evalSources, (char *) mySection->strings[13]);
	local->myParticleSystem->Sources.equation = evalSources;
	
	// Init the expression evaluation library
	initExpression(&local->myParticleSystem->Sources);
	initExpression(&local->myParticleSystem->Evaluator);
	initExpression(&local->myParticleSystem->rate);
	mySection->loaded=1;
}

// ******************************************************************

void init_objectMatrix () {
	local = (objectMatrix_section *) mySection->vars;
	local->myParticleSystem->firstFrame = 1;
	clearParticles(local->myParticleSystem);
}

// ************************************************************

void render_objectMatrix () {	
	int i;
	tParticle *myParticle;
	
	local = (objectMatrix_section *) mySection->vars;
	
	// Insert the default section values into the size equation
	insertSectionVariables(&local->myParticleSystem->Evaluator);
	insertSectionVariables(&local->myParticleSystem->rate);
		
	particles_generate(local->myParticleSystem);
	particles_update  (local->myParticleSystem);

	if (local->myParticleSystem->particlesCount <= 0) return;
	
	if (local->myParticleSystem->disableDepthTest > 0) glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
		// place the sources
		place_object (&local->myParticleSystem->Sources);
		
		// Render the objects instead of the particle system's particles
		myParticle = local->myParticleSystem->firstParticle;
	
		for (i = 0; i < local->myParticleSystem->particlesCount; i++) {
			if (myParticle->life <= 0.0f) // skip dead particles
				continue;
		
			// glColor4d(myParticle->red, myParticle->green, myParticle->blue, myParticle->alpha);
				
			glPushMatrix();

				glTranslated (myParticle->position.x, myParticle->position.y, myParticle->position.z);
				glRotated    (myParticle->rotation.x, 1, 0, 0);
				glRotated    (myParticle->rotation.y, 0, 1, 0);
				glRotated    (myParticle->rotation.z, 0, 0, 1);
				glScaled     (myParticle->size*myParticle->scale.x, myParticle->size*myParticle->scale.y, myParticle->size*myParticle->scale.z);	
			
				render_objectLayers (local->obj_part, local->layers, local->myParticleSystem->disableDepthTest);
			
			glPopMatrix();
			
		myParticle = myParticle->nextParticle;
	}
		
	if (local->myParticleSystem->disableDepthTest == 1)
		glEnable(GL_DEPTH_TEST);
			
	glPopMatrix();
}

// ******************************************************************

void end_objectMatrix() {
	local = (objectMatrix_section *) mySection->vars;
	clearParticles(local->myParticleSystem);
}
