/*
	efxMotionBlurAccum.c: MotionBlur effect with accumulation buffer
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {

	float decay;

} efxMotionBlurAccum_section;

static efxMotionBlurAccum_section *local;

// ******************************************************************

void preload_efxMotionBlurAccum () {
}

// ******************************************************************

void load_efxMotionBlurAccum () {

	// capabilities validation
	if (!glDriver.accum)
	{
		section_error("accum buffer is not active");
		return;
	}
	
	// script validation
	if (mySection->paramNum != 1) {
		section_error("1 param needed");
		return;
	}
	
	local = malloc(sizeof(efxMotionBlurAccum_section));
	mySection->vars = (void *) local;

	local->decay = mySection->param[0];
	mySection->loaded=1;
}

// ******************************************************************

void init_efxMotionBlurAccum () {

	glClear(GL_ACCUM_BUFFER_BIT);
}

// ******************************************************************

void render_efxMotionBlurAccum () {

	local = (efxMotionBlurAccum_section *) mySection->vars;

	glAccum(GL_MULT, local->decay);
	glAccum(GL_ACCUM, 1 - local->decay);
	glAccum(GL_RETURN, 1);
}

// ******************************************************************

void end_efxMotionBlurAccum () {
}
