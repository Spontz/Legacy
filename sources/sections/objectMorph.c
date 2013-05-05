#include <assert.h>

#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	H3dsScene	*objectStart;
	H3dsScene	*objectCurrent;
	H3dsScene	*objectEnd;

	layer_t		*layers;
	
	int			enableDepthBufferClearing;
	int			interpolationMode;
} objectMorph_section;

static objectMorph_section *local;

// ******************************************************************

void preload_objectMorph () {
}

// ******************************************************************

void load_objectMorph () {
	char fullName[256];

	// script validation
	if (mySection->stringNum <  2) {
		section_error("at least 2 strings needed");
		return;
	}
	
	local = malloc(sizeof(objectMorph_section));
	assert(local);
	
	if (local) {
		/* Clear memory */
		memset(local, 0, sizeof(objectMorph_section));
		
		mySection->vars = (void *) local;

		// load current object model from the start object model
		sprintf(fullName, "%s", mySection->strings[0]);
		local->objectCurrent = model_load(fullName, NO_CACHE);
		
		// load start object model
		local->objectStart = model_load(fullName, NO_CACHE);
		if (!local->objectStart) {
			section_error("objectMorph: Error loading start object %s", fullName);
			return;
		}
		// load end object model
		sprintf(fullName, "%s", mySection->strings[1]);
		local->objectEnd = model_load(fullName, NO_CACHE);
		if (!local->objectEnd) {
			section_error("objectMorph: Error loading end object %s", fullName);
			return;
		}
		// load layers
		local->layers = layers_load(mySection->strings[2]);
		
		if (mySection->stringNum == 4) {
			dkernel_trace("objectMorph: Base directory found in script. Loading textures...\n");
			if (model_upload_tex(local->objectCurrent, mySection->strings[3]) == -1)
				return;
			dkernel_trace("done.\n");
		} else {
			dkernel_trace("objectMorph: No base directory found (String 4). Bypassing textures.\n");
		}
		
		// Interpolation mode
		local->interpolationMode = (int)mySection->param[1];
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_objectMorph () {
}

// ******************************************************************

static void prepare_object (float step, int path) {

	ChanVec resVec;

	dkernel_getSpline(path, step, resVec);

	glScalef (resVec[0], resVec[1], resVec[2]);
	glRotatef (resVec[3], 1, 0, 0);
	glRotatef (resVec[4], 0, 1, 0);
	glRotatef (resVec[5], 0, 0, 1);
	glTranslatef (resVec[6], resVec[7], resVec[8]);
}

// ******************************************************************

void render_objectMorph () {
	int i;
	float t = 1;
	word v;
//	H3dsMeshObj *mesh;

	H3dsMeshObj *meshStart, *meshCurrent, *meshEnd;
	int nmesh, nvert, nface;

	local = (objectMorph_section *) mySection->vars;
	
	if (local->interpolationMode == 1) {
		// Linear interpolation
		t = mySection->runTime / mySection->duration;
		
	} else if (local->interpolationMode == 2) {
		// Smooth interpolation: smooth start and smooth end
		t = mySection->runTime / mySection->duration;
		t = -2*t*t*t + 3*t*t;
		
	}
	
	// For every mesh in the current scene
	for (nmesh=0; nmesh<local->objectCurrent->meshobjs; nmesh++) {

		meshStart   = &local->objectStart->meshobjlist  [nmesh];
		meshCurrent = &local->objectCurrent->meshobjlist[nmesh];
		meshEnd     = &local->objectEnd->meshobjlist    [nmesh];
		
		// For every vertex in the current mesh
		for (nvert=0; nvert<meshCurrent->verts; nvert++) {
			meshCurrent->vertlist[nvert].x = meshStart->vertlist[nvert].x + (meshEnd->vertlist[nvert].x - meshStart->vertlist[nvert].x) * t;
			meshCurrent->vertlist[nvert].y = meshStart->vertlist[nvert].y + (meshEnd->vertlist[nvert].y - meshStart->vertlist[nvert].y) * t;
			meshCurrent->vertlist[nvert].z = meshStart->vertlist[nvert].z + (meshEnd->vertlist[nvert].z - meshStart->vertlist[nvert].z) * t;
		}
 		
		// Now interpolate the texture coordinates
		if (meshCurrent->maps == 0) continue;
		
		for (nface=0; nface<meshCurrent->faces; nface++) {
			for (nvert=0; nvert<meshCurrent->facelist[nface].vertexes; nvert++) {
				v = meshCurrent->facelist[nface].v[nvert].vertex;
				meshCurrent->facelist[nface].v[nvert].u = meshStart->facelist[nface].v[nvert].u + (meshEnd->facelist[nface].v[nvert].u - meshStart->facelist[nface].v[nvert].u) * t;
				meshCurrent->facelist[nface].v[nvert].v = meshStart->facelist[nface].v[nvert].v + (meshEnd->facelist[nface].v[nvert].v - meshStart->facelist[nface].v[nvert].v) * t;
			}
		}
	}
		
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// prepare object with the given spline
	for (i=0; i<mySection->splineNum; i++)
		prepare_object (mySection->runTime, i);
	
	calculateNormals(local->objectCurrent);
			
	render_objectLayers (local->objectCurrent, local->layers, local->enableDepthBufferClearing);

	glPopMatrix();
}

// ******************************************************************

void end_objectMorph () {
}
