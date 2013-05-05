/*
	efxAccum.c: accumulate the color buffer
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {

	int accum;
	float alpha1, alpha2;

} efxAccum_section;

static efxAccum_section *local;

// ******************************************************************

void preload_efxAccum () {
}

// ******************************************************************

void load_efxAccum () {

	// script validation
	if (mySection->paramNum != 3) {
		section_error("3 params needed");
		return;
	}
	
	local = malloc(sizeof(efxAccum_section));
	mySection->vars = (void *) local;

	local->accum = (int) mySection->param[0];
	local->alpha1 = mySection->param[1];
	local->alpha2 = mySection->param[2];
	mySection->loaded=1;
}

// ******************************************************************

void init_efxAccum ()
	{
	// Clean the current RTT texture
	tex_free(demoSystem.rtt);
	
	// init shared rtt textures
	demoSystem.rtt = tex_new(glDriver.vpWidth, glDriver.vpHeight, GL_RGB, 3);
	tex_properties (demoSystem.rtt, NO_MIPMAP | CLAMP_TO_EDGE);
	tex_properties (demoSystem.rtt, NO_MIPMAP);
	tex_upload (demoSystem.rtt, NO_CACHE);
	}

// ******************************************************************

void render_efxAccum () {

	float alpha;
	int i;

	local = (efxAccum_section *) mySection->vars;

	glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}

	camera_set2d();

	alpha = local->alpha1 + mySection->runTime * (local->alpha2 - local->alpha1) / mySection->duration;
	glColor4f(1, 1, 1, alpha);

	// capture current color buffer
	tex_bind(demoSystem.rtt);
	gldrv_copyColorBuffer();

	// accumulate it several times
	for (i=0; i<local->accum; i++) {
		gldrv_texscreenquad();
	}

	camera_restore();

	if (mySection->hasBlend) glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxAccum () {
}
