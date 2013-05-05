 #include <assert.h>

#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	H3dsScene	*object;
	H3dsScene   *sources;
	layer_t		*layers;	
	int			enableDepthBufferClearing;
	
	tExpression evalPositioning;			// A equation containing the calculations used to position the object
	tExpression evalSources;				// A equation containing the calculations for iterative object renderings
} objectShader_section;

static objectShader_section *local;

// ******************************************************************

static void objects_render() {
	H3dsMeshObj *mesh;
	int nmesh, nvert;
	double r;
	
	local = (objectShader_section *) mySection->vars;

	/* if (demoSystem.drawTiming) {
		// Draw the source points
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		render_onlyvertex3ds(local->sources);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_TEXTURE_2D);
	} */

	glMatrixMode(GL_MODELVIEW);

	for (nmesh=0; nmesh<local->sources->meshobjs; nmesh++) {

		mesh = &local->sources->meshobjlist[nmesh];
		for (nvert=0; nvert<mesh->verts; nvert++) {
			r = sqrt(mesh->vertlist[nvert].x * mesh->vertlist[nvert].x + mesh->vertlist[nvert].y * mesh->vertlist[nvert].y + mesh->vertlist[nvert].z * mesh->vertlist[nvert].z);

			exprValListAdd(local->evalPositioning.v, "x" , (EXPRTYPE)mesh->vertlist[nvert].x);
			exprValListAdd(local->evalPositioning.v, "y" , (EXPRTYPE)mesh->vertlist[nvert].y);
			exprValListAdd(local->evalPositioning.v, "z" , (EXPRTYPE)mesh->vertlist[nvert].z);
			exprValListAdd(local->evalPositioning.v, "r" , (EXPRTYPE)r);
			exprValListAdd(local->evalPositioning.v, "n" , (EXPRTYPE)nvert+1);
			
			glPushMatrix();

				// Translate the object to his source position
				glTranslatef (mesh->vertlist[nvert].x, mesh->vertlist[nvert].y, mesh->vertlist[nvert].z);
				deprecated_place_object(local->evalPositioning);
				render_objectLayers (local->object, local->layers, local->enableDepthBufferClearing);
				
			glPopMatrix();
		}
	}
}

// ******************************************************************

void preload_objectShader () {
}

// ******************************************************************

void load_objectShader () {
	char *fullName;
	char *positionEquation;
	int theSize;
	
	// script validation
	if ((mySection->paramNum  != 1)||(mySection->stringNum <  10)) {
		section_error("1 param and 10 strings needed");
		return;
	}

	local = malloc(sizeof(objectShader_section));
	if (!local)
	{
		dkernel_error("insuficient memory");
		return;
	}
	else
	{
		mySection->vars = (void *) local;

		// Depth Buffer Clearing Flag
		local->enableDepthBufferClearing = (int)mySection->param[0];

		// load object model
		theSize = strlen(mySection->strings[0]) + strlen(mySection->strings[1]);
		fullName = calloc(theSize+1, sizeof(char));
		sprintf(fullName, "%s%s", mySection->strings[0], mySection->strings[1]);

		local->object = model_load(fullName, USE_CACHE);
		if (!local->object) {
			section_error("ObjectShader: Error loading object %s for drawing", fullName);
			return;
		}
		
		if (model_upload_tex(local->object, mySection->strings[0]) == -1)
			return;

		// load layers for drawing the object
		local->layers = layers_load(mySection->strings[2]);

		// load sources model
		if (strcmp(mySection->strings[3], "default") != 0) {
			local->sources = model_load(mySection->strings[3], USE_CACHE);
			if (!local->object) {
				section_error("ObjectShader: Error loading object %s for sources positioning", mySection->strings[3]);
				return;
			}
		} else {
			local->sources = NULL;
		}

		// load the object positioning equation
		theSize = strlen(mySection->strings[4]) + strlen(mySection->strings[5]) + strlen(mySection->strings[6]);
		positionEquation = calloc(theSize+1, sizeof(char));
		sprintf(positionEquation, "%s%s%s", mySection->strings[4], mySection->strings[5], mySection->strings[6]);
		local->evalPositioning.equation = positionEquation;

		// load the sources positioning equation
		theSize = strlen(mySection->strings[7]) + strlen(mySection->strings[8]) + strlen(mySection->strings[9]);
		positionEquation = calloc(theSize+1, sizeof(char));
		sprintf(positionEquation, "%s%s%s", mySection->strings[7], mySection->strings[8], mySection->strings[9]);
		local->evalSources.equation = positionEquation;

		// Init the expression evaluation library
		initExpression(&local->evalPositioning);
		initExpression(&local->evalSources);
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_objectShader () {
}

// ******************************************************************

void render_objectShader () {	
	local = (objectShader_section *) mySection->vars;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

		if (local->sources == NULL) {
			place_object (&local->evalPositioning);
			render_objectLayers (local->object, local->layers, local->enableDepthBufferClearing);			
		} else {
			deprecated_place_object (local->evalSources);
			objects_render();			
		}
		
	glPopMatrix();
}

// ******************************************************************

void end_objectShader () {
}
