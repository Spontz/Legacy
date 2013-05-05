/*
	efxRadialBlur2.c: radial blur effect
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	tExpression tParameters;
} efxRadialBlur2_section;

static efxRadialBlur2_section *local;

// ******************************************************************

void preload_efxRadialBlur2 () {
}

// ******************************************************************

void load_efxRadialBlur2 () {
	char *equation;
	int equationSize;
	
	// script validation
	if (mySection->stringNum != 5) {
		section_error("5 strings needed");
		return;
	}
	
	local = malloc(sizeof(efxRadialBlur2_section));
	mySection->vars = (void *) local;

	equationSize  = strlen(mySection->strings[0]) +1;
	equationSize += strlen(mySection->strings[1]    );
	equationSize += strlen(mySection->strings[2]    );
	equationSize += strlen(mySection->strings[3]    );
	equationSize += strlen(mySection->strings[4]    );
		
	equation = calloc(equationSize, sizeof(char));

	strcpy(equation, (char *) mySection->strings[0]);
	strcat(equation, (char *) mySection->strings[1]);
	strcat(equation, (char *) mySection->strings[2]);
	strcat(equation, (char *) mySection->strings[3]);
	strcat(equation, (char *) mySection->strings[4]);
	
	local->tParameters.equation = equation;
	
	// Init the expression evaluation library
	initExpression(&local->tParameters);
	mySection->loaded=1;
}

// ******************************************************************

void init_efxRadialBlur2 () {
}

// ******************************************************************

void render_efxRadialBlur2 () {
	double alpha, separation;
	double originX, originY;
	double iterations;
	
	int i;
	
	float step;
	float offset;
	float offsetX, offsetY;

	local = (efxRadialBlur2_section *) mySection->vars;

	// Insert the default section values into the size equation
	insertSectionVariables(&local->tParameters);
	
	// Evaluate the equations
	local->tParameters.err = exprEval(local->tParameters.o, &local->tParameters.result);

	// Check for errors
	if(local->tParameters.err != EXPR_ERROR_NOERROR) section_error("efxRadialBlur2: render_efxRadialBlur2: Expression evaluation Error (%d): %s", local->tParameters.err, local->tParameters.equation);

	// Retrieve the values and assign them
	exprValListGet(local->tParameters.v, "iterations", &iterations);
	exprValListGet(local->tParameters.v,      "alpha", &alpha     );
	exprValListGet(local->tParameters.v, "separation", &separation);
	exprValListGet(local->tParameters.v,    "originX", &originX   );
	exprValListGet(local->tParameters.v,    "originY", &originY   );
	
	glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}

	camera_set2d();

	tex_bind(demoSystem.rtt);
	glColor4d(1.0, 1.0, 1.0, alpha);

	for (i=0; i<(int)iterations; i++) {

		gldrv_copyColorBuffer();

		step = (float) i / (float)iterations;
		offset = step * (float)separation;
		offsetX = offset * (float)originX;
		offsetY = offset * (float)originY;

		glBegin(GL_QUADS);
			glTexCoord2f (0,(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f   (-offset-offsetX,1+offset-offsetY);

			glTexCoord2f (0,0);
			glVertex2f   (-offset-offsetX,-offset-offsetY);

			glTexCoord2f (gldrv_get_viewport_aspect_ratio(),0);
			glVertex2f   (1+offset-offsetX,-offset-offsetY);

			glTexCoord2f (gldrv_get_viewport_aspect_ratio(),(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f   (1+offset-offsetX,1+offset-offsetY);

		glEnd();
	}

	camera_restore();

	glClear(GL_DEPTH_BUFFER_BIT);

	if (mySection->hasBlend) glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxRadialBlur2 () {
}
