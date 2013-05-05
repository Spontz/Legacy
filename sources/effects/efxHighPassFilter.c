/*
	efxHighPassFilter.c: high pass filter
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {

	tExpression Evaluator;

} efxHighPassFilter_section;

static efxHighPassFilter_section *local;

// ******************************************************************

void preload_efxHighPassFilter () {
}

// ******************************************************************

void load_efxHighPassFilter () {
	char *equation;
	int equationSize;
	
	// script validation
	if (mySection->stringNum != 2) {
		section_error("2 strings needed");
		return;
	}
	
	local = calloc(1, sizeof(efxHighPassFilter_section));
	mySection->vars = (void *) local;

	equationSize  = strlen(mySection->strings[0]) + strlen(mySection->strings[1]) + 1;
	
	equation = calloc(equationSize, sizeof(char));

	strcpy(equation, (char *) mySection->strings[0]);
	strcat(equation, (char *) mySection->strings[1]);
	
	local->Evaluator.equation = equation;
	
	// Init the expression evaluation library
	initExpression(&local->Evaluator);
	mySection->loaded=1;
}

// ******************************************************************

void init_efxHighPassFilter () {
}

// ******************************************************************

void render_efxHighPassFilter () {
	double red, green, blue;
	double accum;

	local = (efxHighPassFilter_section *) mySection->vars;

	// set render state
	glDisable (GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	camera_set2d ();

	// Insert the default section values into the size equation
	insertSectionVariables(&local->Evaluator);

	// Evaluate the equations
	local->Evaluator.err = exprEval(local->Evaluator.o, &local->Evaluator.result);

	// Check for errors
	if(local->Evaluator.err != EXPR_ERROR_NOERROR) section_error("particleMatrix2: particles_update: Expression evaluation Error (%d): %s", local->Evaluator.err, local->Evaluator.equation);

	// Retrieve the values and assign them
	exprValListGet(local->Evaluator.v,   "red", &red  );
	exprValListGet(local->Evaluator.v, "green", &green);
	exprValListGet(local->Evaluator.v,  "blue", &blue );
	exprValListGet(local->Evaluator.v, "accum", &accum);
						
	render_highpassfilter_ext (demoSystem.rtt, (float)red, (float)green, (float)blue, (int)accum);

	// unset render state
	camera_restore ();
	glDisable (GL_BLEND);
	glEnable (GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxHighPassFilter () {
}
