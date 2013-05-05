#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	H3dsScene	*objectOriginal;
	H3dsScene	*objectRender;
	
	layer_t		*layers;	
	int			enableDepthBufferClearing;
	
	tExpression Evaluator;					// A big equation containing the calculations used to modify the object
	tExpression evalPositioning;			// A equation containing the calculations used to position the object
} objectShader2_section;

static objectShader2_section *local;

// ******************************************************************

void preload_objectShader2 () {
}

// ******************************************************************

void load_objectShader2 () {
	char *equation;
	int equationSize;
	char fullName[256];

	// script validation
	if ((mySection->paramNum  != 1) || (mySection->stringNum != 9)) {
		section_error("ObjectShader: 1 param & 9 strings needed");
		return;
	}
	
	local = malloc(sizeof(objectShader2_section));
	mySection->vars = (void *) local;

	// Depth Buffer Clearing Flag
	local->enableDepthBufferClearing = (int)mySection->param[0];
	
	// load object model
	sprintf(fullName, "%s%s", mySection->strings[0], mySection->strings[1]);
	
	local->objectOriginal = model_load(fullName, USE_CACHE);
	if (!local->objectOriginal) {
		section_error("ObjectShader2: Error loading %s%s", fullName);
		return;
	}

	local->objectRender = model_load(fullName, NO_CACHE);
	if (!local->objectRender) {
		section_error("ObjectShader2: Error loading %s%s", fullName);
		return;
	}
	
	if (model_upload_tex(local->objectRender, mySection->strings[0]) == -1)
		return;
	
	// Equation defining the x, y, z, u and v variations
	equationSize  = strlen(mySection->strings[3]);
	equationSize += strlen(mySection->strings[4]);
	equationSize += strlen(mySection->strings[5]);

	equation = calloc(equationSize+1, sizeof(char));

	// Put everything inside the same equation
	sprintf(equation, "%s%s%s", mySection->strings[3], mySection->strings[4], mySection->strings[5]);
	
	local->Evaluator.equation = equation;
	
	// load the initial positioning equation
	equationSize = strlen(mySection->strings[6]) + strlen(mySection->strings[7]) + strlen(mySection->strings[8]);
	equation = calloc(equationSize+1, sizeof(char));
	sprintf(equation, "%s%s%s", mySection->strings[6], mySection->strings[7], mySection->strings[8]);
	local->evalPositioning.equation = equation;	
	
	// Init the expression evaluation library
	initExpression(&local->Evaluator);
	initExpression(&local->evalPositioning);

	// load layers
	local->layers = layers_load(mySection->strings[2]);
	mySection->loaded=1;
}

// ******************************************************************

void init_objectShader2 () {
	int nmesh, nvert;
				
	for (nmesh=0; nmesh<local->objectOriginal->meshobjs; nmesh++) {
		for (nvert=0; nvert<local->objectOriginal->meshobjlist[nmesh].verts; nvert++) {
		
			// Reset the rendering object
			local->objectRender->meshobjlist[nmesh].vertlist[nvert].x = local->objectOriginal->meshobjlist[nmesh].vertlist[nvert].x;
			local->objectRender->meshobjlist[nmesh].vertlist[nvert].x = local->objectOriginal->meshobjlist[nmesh].vertlist[nvert].y;
			local->objectRender->meshobjlist[nmesh].vertlist[nvert].x = local->objectOriginal->meshobjlist[nmesh].vertlist[nvert].z;
		}
	}
}

// ******************************************************************

static void prepare_object () {
	double x, y, z, r, u, v;
	int nmesh, nvert, nface;
	H3dsMeshObj *meshOriginal, *meshRender;

	// Insert the default section values into the size equation
	insertSectionVariables(&local->Evaluator);
				
	for (nmesh=0; nmesh<local->objectOriginal->meshobjs; nmesh++) {
	
		meshOriginal = &local->objectOriginal->meshobjlist[nmesh];
		meshRender   = &local->objectRender  ->meshobjlist[nmesh];
	
		for (nvert=0; nvert<meshOriginal->verts; nvert++) {
		
			r = sqrt(meshRender->vertlist[nvert].x * meshRender->vertlist[nvert].x + meshRender->vertlist[nvert].y * meshRender->vertlist[nvert].y + meshRender->vertlist[nvert].z * meshRender->vertlist[nvert].z);
		
			// Insert the custom values into the equation
			exprValListAdd(local->Evaluator.v, "x0", (EXPRTYPE)meshOriginal->vertlist[nvert].x);
			exprValListAdd(local->Evaluator.v, "y0", (EXPRTYPE)meshOriginal->vertlist[nvert].y);
			exprValListAdd(local->Evaluator.v, "z0", (EXPRTYPE)meshOriginal->vertlist[nvert].z);
			
			// Insert the custom values into the equation
			exprValListAdd(local->Evaluator.v, "x" , (EXPRTYPE)meshRender->vertlist[nvert].x);
			exprValListAdd(local->Evaluator.v, "y" , (EXPRTYPE)meshRender->vertlist[nvert].y);
			exprValListAdd(local->Evaluator.v, "z" , (EXPRTYPE)meshRender->vertlist[nvert].z);
			exprValListAdd(local->Evaluator.v, "r" , (EXPRTYPE)r);
			exprValListAdd(local->Evaluator.v, "n" , (EXPRTYPE)nvert);
			exprValListAdd(local->Evaluator.v, "n0", (EXPRTYPE)meshOriginal->verts);
						
			// Evaluate the equations
			local->Evaluator.err = exprEval(local->Evaluator.o, &local->Evaluator.result);

			// Check for errors
			if(local->Evaluator.err != EXPR_ERROR_NOERROR) section_error("objectShader2: prepare_object: Expression evaluation Error (%d): %s", local->Evaluator.err, local->Evaluator.equation);
	
			exprValListGet(local->Evaluator.v, "x", &x);
			exprValListGet(local->Evaluator.v, "y", &y);
			exprValListGet(local->Evaluator.v, "z", &z);
						
			meshRender->vertlist[nvert].x = (float)x;
			meshRender->vertlist[nvert].y = (float)y;
			meshRender->vertlist[nvert].z = (float)z;
		}

		for (nface=0; nface<local->objectOriginal->meshobjlist[nmesh].faces; nface++) {
			for (nvert=0; nvert<local->objectOriginal->meshobjlist[nmesh].facelist[nface].vertexes; nvert++) {

				// Insert the custom values into the equation
				exprValListAdd(local->Evaluator.v, "x" , (EXPRTYPE)meshRender->vertlist[meshRender->facelist[nface].v[nvert].vertex].x);
				exprValListAdd(local->Evaluator.v, "y" , (EXPRTYPE)meshRender->vertlist[meshRender->facelist[nface].v[nvert].vertex].y);
				exprValListAdd(local->Evaluator.v, "z" , (EXPRTYPE)meshRender->vertlist[meshRender->facelist[nface].v[nvert].vertex].z);
				
				exprValListAdd(local->Evaluator.v, "n" , (EXPRTYPE)nvert);
				exprValListAdd(local->Evaluator.v, "n0", (EXPRTYPE)meshOriginal->faces);
			
				exprValListAdd(local->Evaluator.v, "u0", (EXPRTYPE)meshOriginal->facelist[nface].v[nvert].u);
				exprValListAdd(local->Evaluator.v, "v0", (EXPRTYPE)meshOriginal->facelist[nface].v[nvert].v);

				exprValListAdd(local->Evaluator.v,  "u", (EXPRTYPE)meshRender->facelist[nface].v[nvert].u);
				exprValListAdd(local->Evaluator.v,  "v", (EXPRTYPE)meshRender->facelist[nface].v[nvert].v);
										
				// Evaluate the equations
				local->Evaluator.err = exprEval(local->Evaluator.o, &local->Evaluator.result);

				// Check for errors
				if(local->Evaluator.err != EXPR_ERROR_NOERROR) section_error("objectShader2: prepare_object: Expression evaluation Error (%d): %s", local->Evaluator.err, local->Evaluator.equation);
						
				exprValListGet(local->Evaluator.v, "u", &u);
				exprValListGet(local->Evaluator.v, "v", &v);
			
				meshRender->facelist[nface].v[nvert].u = (float)u;
				meshRender->facelist[nface].v[nvert].v = (float)v;
			}
		}
	}
	
	calculateNormals(local->objectRender);
}

// ******************************************************************

void render_objectShader2 () {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		
		// Calculate the place in which the object will be laid down
		place_object (&local->evalPositioning);

		// prepare object, modified by the given equations
		prepare_object();
		
		local = (objectShader2_section *) mySection->vars;
		render_objectLayers (local->objectRender, local->layers, local->enableDepthBufferClearing);

	glPopMatrix();
}

// ******************************************************************

void end_objectShader2 () {
}
