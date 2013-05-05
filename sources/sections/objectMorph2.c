#include <assert.h>

#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	H3dsScene	*objectStart;
	H3dsScene	*objectCurrent;
	H3dsScene	*objectEnd;

	layer_t		*layers;
	
	int			enableDepthBufferClearing;
	
	tExpression Evaluator;			// A equation containing the calculations used to position and interpolate the object
} objectMorph2_section;

static objectMorph2_section *local;

// ******************************************************************

void preload_objectMorph2 () {
}

// ******************************************************************

void load_objectMorph2 ()
{
	char *equation;
	int equationSize;
	char fullName[256];
	
	// script validation
	if ((mySection->paramNum  != 1) || (mySection->stringNum != 8)) {
		section_error("objectMorph2: 1 param needed (%d passed), 8 strings needed (%d passed)",   mySection->paramNum, mySection->stringNum );
		return;
	}
	
	local = calloc(1, sizeof(objectMorph2_section));
	if (!local) {
		dkernel_error("insuficient memory");
		return;
	}
	else
	{
		mySection->vars = (void *) local;
		
		// Depth Buffer Clearing Flag
		local->enableDepthBufferClearing = (int)mySection->param[0];
		
		// load start object model from the start object model
		sprintf(fullName, "%s", mySection->strings[0]);
		local->objectStart = model_load(fullName, NO_CACHE);
		if (!local->objectStart) {
			section_error("ObjectMorph2: Error loading start object %s", fullName);
			return;
		}
		// load current object model
		local->objectCurrent = model_load(fullName, NO_CACHE);
		
		// load end object model
		sprintf(fullName, "%s", mySection->strings[1]);
		local->objectEnd = model_load(fullName, NO_CACHE);
		if (!local->objectEnd) {
			section_error("ObjectMorph2: Error loading end object %s", fullName);
			return;
		}
		// load layers
		local->layers = layers_load(mySection->strings[3]);
		
		dkernel_trace("objectMorph2: Loading textures from the base directory...\n");
		if (model_upload_tex(local->objectStart, mySection->strings[2]) == -1)
			return;
		if (model_upload_tex(local->objectCurrent, mySection->strings[2]) == -1)
			return;
		if (model_upload_tex(local->objectEnd, mySection->strings[2]) == -1)
			return;
		dkernel_trace("done.\n");
		
		// Equation defining the x, y, z
		equationSize  = strlen(mySection->strings[4]);
		equationSize += strlen(mySection->strings[5]);
		equationSize += strlen(mySection->strings[6]);
		equationSize += strlen(mySection->strings[7]);
		
		equation = calloc(equationSize+1, sizeof(char));
		if (!equation)
		{
			dkernel_error("insuficient memory");
			return;
		}
		else
		{
			// Put everything inside the same equation
			sprintf(equation, "%s%s%s%s", mySection->strings[4], mySection->strings[5], mySection->strings[6], mySection->strings[7]);
			
			local->Evaluator.equation = equation;
			
			// Init the expression evaluation library
			initExpression(&local->Evaluator);
		}
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_objectMorph2 () {

}

// ******************************************************************

static void prepare_object () {
	word v;
	double interpolation;

	H3dsMeshObj *meshStart, *meshCurrent, *meshEnd;
	int nmesh, nvert, nface;
	int totalVerts;
	
	// Extract the interpolation amount from the equation
	exprValListGet(local->Evaluator.v, "value", &interpolation);
	
	// For every mesh in the current scene
	for (nmesh=0; nmesh<local->objectStart->meshobjs; nmesh++) {

		meshStart   = &local->objectStart->meshobjlist  [nmesh];
		meshCurrent = &local->objectCurrent->meshobjlist[nmesh];
		meshEnd     = &local->objectEnd->meshobjlist    [nmesh];
		
		totalVerts = meshStart->verts;
		if (meshEnd->verts < meshStart->verts) totalVerts = meshEnd->verts;
		
		// For every vertex in the current mesh
		for (nvert=0; nvert<totalVerts; nvert++) {
		
			meshCurrent->vertlist[nvert].x = meshStart->vertlist[nvert].x + (meshEnd->vertlist[nvert].x - meshStart->vertlist[nvert].x) * (float)interpolation;
			meshCurrent->vertlist[nvert].y = meshStart->vertlist[nvert].y + (meshEnd->vertlist[nvert].y - meshStart->vertlist[nvert].y) * (float)interpolation;
			meshCurrent->vertlist[nvert].z = meshStart->vertlist[nvert].z + (meshEnd->vertlist[nvert].z - meshStart->vertlist[nvert].z) * (float)interpolation;
		}
		
		// Now interpolate the texture coordinates
		if (meshCurrent->maps == 0) continue;
		
		for (nface=0; nface<meshCurrent->faces; nface++) {
			for (nvert=0; nvert<meshCurrent->facelist[nface].vertexes; nvert++) {
				v = meshCurrent->facelist[nface].v[nvert].vertex;
				meshCurrent->facelist[nface].v[nvert].u = meshStart->facelist[nface].v[nvert].u + (meshEnd->facelist[nface].v[nvert].u - meshStart->facelist[nface].v[nvert].u) * (float)interpolation;
				meshCurrent->facelist[nface].v[nvert].v = meshStart->facelist[nface].v[nvert].v + (meshEnd->facelist[nface].v[nvert].v - meshStart->facelist[nface].v[nvert].v) * (float)interpolation;
			}
		}
	}
	
	calculateNormals(local->objectCurrent);
}

// ******************************************************************

void render_objectMorph2 () {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

		// Calculate the place in which the object will be laid down
		place_object (&local->Evaluator);
		
		// prepare object, modified by the given equations
		prepare_object();
		
		local = (objectMorph2_section *) mySection->vars;
		render_objectLayers (local->objectCurrent, local->layers, local->enableDepthBufferClearing);

	glPopMatrix();
}

// ******************************************************************

void end_objectMorph2 () {
}
