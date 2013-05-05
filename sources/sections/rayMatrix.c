#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	H3dsScene	*source;			// The object used to positionate the ray sources
	H3dsScene	*destination;		// The object used to positionate the ray destinations
	tRaySystem  *myRaySystem;		// Pointer to the ray system
} rayMatrix_section;

static rayMatrix_section *local;

// ******************************************************************

void preload_rayMatrix() {
}

// ******************************************************************

void load_rayMatrix() {
	char *evalEquation;
	char *evalSources;
	int equationSize;
	H3dsMeshObj *mesh;
	int i;

	// script validation
	if ((mySection->paramNum   <  1) || (mySection->stringNum != 11)) {
		section_error("at least 1 param and 11 strings needed");
		return;
	}

	local = malloc(sizeof(rayMatrix_section));
	memset(local, 0, sizeof(rayMatrix_section));

	local->myRaySystem = (tRaySystem*)rays_init(local->myRaySystem);
	
	mySection->vars = (void *) local;

	// texture rendering buffer to use as ray texture
	local->myRaySystem->renderingBuffer = (int)mySection->param[0];

	// load object models for sources and destinations
	local->source = model_load(mySection->strings[0], USE_CACHE);
	if (!local->source) {
		section_error("rayMatrix: Error loading %s", mySection->strings[0]);
		return;
	}
	
	local->destination = model_load(mySection->strings[1], USE_CACHE);
	if (!local->destination) {
		section_error("rayMatrix: Error loading %s", mySection->strings[1]);
		return;
	}
	
	// Depth test configuration
	if (mySection->paramNum > 1) {
		local->myRaySystem->disableDepthTest = (int)mySection->param[1];
	} else {
		local->myRaySystem->disableDepthTest = 1;
	}
	
	// Equation defining the rate at which the rays are created (rays created per second)
	local->myRaySystem->rate.equation = mySection->strings[2];
	
	// We only take into account the first mesh of the sources object as ray sources
	mesh = &local->source->meshobjlist[0];
	
	// Get and store the Ray source positions
	local->myRaySystem->sourcesCount = mesh->verts;
	local->myRaySystem->sources      = (tVector *) malloc(sizeof(tVector) * mesh->verts);
	local->myRaySystem->sourcesColor = (tColor  *) malloc(sizeof(tColor ) * mesh->verts);

	for (i=0; i<local->myRaySystem->sourcesCount; i++) {
		local->myRaySystem->sources[i].x = mesh->vertlist[i].x;
		local->myRaySystem->sources[i].y = mesh->vertlist[i].y;
		local->myRaySystem->sources[i].z = mesh->vertlist[i].z;
		
		local->myRaySystem->sourcesColor[i].b = 1.0;
		local->myRaySystem->sourcesColor[i].g = 1.0;
		local->myRaySystem->sourcesColor[i].r = 1.0;
	}

	// We only take into account the first mesh of the destination object as ray destinations
	mesh = &local->destination->meshobjlist[0];
	
	// Get and store the ray destination positions
	local->myRaySystem->destinationsCount = mesh->verts;
	local->myRaySystem->destinations = (tVector *) malloc(sizeof(tVector) * mesh->verts);
	local->myRaySystem->destinationsColor = (tColor  *) malloc(sizeof(tColor ) * mesh->verts);

	for (i=0; i<local->myRaySystem->destinationsCount; i++) {
		local->myRaySystem->destinations[i].x = mesh->vertlist[i].x;
		local->myRaySystem->destinations[i].y = mesh->vertlist[i].y;
		local->myRaySystem->destinations[i].z = mesh->vertlist[i].z;
		
		local->myRaySystem->destinationsColor[i].b = 1.0;
		local->myRaySystem->destinationsColor[i].g = 1.0;
		local->myRaySystem->destinationsColor[i].r = 1.0;
	}	
	
	// Ray evaluator
	equationSize  = strlen(mySection->strings[3]);
	equationSize += strlen(mySection->strings[4]);
	equationSize += strlen(mySection->strings[5]);
	equationSize += strlen(mySection->strings[6]);
	equationSize += strlen(mySection->strings[7]);
	evalEquation = calloc(equationSize+1, sizeof(char));
	strcpy(evalEquation, (char *) mySection->strings[3]);
	strcat(evalEquation, (char *) mySection->strings[4]);
	strcat(evalEquation, (char *) mySection->strings[5]);
	strcat(evalEquation, (char *) mySection->strings[6]);
	strcat(evalEquation, (char *) mySection->strings[7]);
	local->myRaySystem->Evaluator.equation = evalEquation;
	
	// Sources and destinations positioning
	equationSize  = strlen(mySection->strings[8]);
	equationSize += strlen(mySection->strings[9]);
	equationSize += strlen(mySection->strings[10]);
	evalSources = calloc(equationSize+1, sizeof(char));
	strcpy(evalSources, (char *) mySection->strings[8]); //-V525
	strcat(evalSources, (char *) mySection->strings[9]);
	strcat(evalSources, (char *) mySection->strings[10]);
	local->myRaySystem->Positioning.equation = evalSources;
	
	// Init the expression evaluation library
	initExpression(&local->myRaySystem->Positioning);
	initExpression(&local->myRaySystem->Evaluator);
	initExpression(&local->myRaySystem->rate);
	mySection->loaded=1;
}

// ******************************************************************

void init_rayMatrix () {
	local = (rayMatrix_section *) mySection->vars;
	local->myRaySystem->firstFrame = 1;
	clearRays(local->myRaySystem);
}

// ************************************************************

void render_rayMatrix () {	
	local = (rayMatrix_section *) mySection->vars;

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
		insertSectionVariables(&local->myRaySystem->Evaluator);
		insertSectionVariables(&local->myRaySystem->rate);
		
		rays_generate(local->myRaySystem);
		
		// place the sources
		deprecated_place_object (local->myRaySystem->Positioning);
		
		glDisable(GL_TEXTURE_2D);
		glPointSize(10.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		render_vertex3ds(local->source);
		render_vertex3ds(local->destination);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_TEXTURE_2D);
		
		rays_update  (local->myRaySystem);
		rays_render  (local->myRaySystem);
		
	glPopMatrix();
		
	if (mySection->hasAlpha) glDisable(GL_ALPHA_TEST);
	if (mySection->hasBlend) glDisable(GL_BLEND);
}

// ******************************************************************

void end_rayMatrix() {
	local = (rayMatrix_section *) mySection->vars;
	clearRays(local->myRaySystem);
}
