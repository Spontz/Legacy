/*
	efxMotionBlur.c: MotionBlur effect
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {

	int tex;
	float alpha1, alpha2;
	int first;

} efxMotionBlur_section;

static efxMotionBlur_section *local;

// ******************************************************************

void preload_efxMotionBlur () {
}

// ******************************************************************

void load_efxMotionBlur ()
	{
	// script validation
	if (mySection->paramNum != 2)
		{
		section_error("2 params needed");
		return;
		}
	
	local = malloc(sizeof(efxMotionBlur_section));
	mySection->vars = (void *) local;

	local->tex = tex_new(glDriver.vpWidth, glDriver.vpHeight, GL_RGB, 3);

	tex_properties(local->tex, MODULATE | NO_MIPMAP | CLAMP);
	tex_upload(local->tex, USE_CACHE);

	local->alpha1 = mySection->param[0];
	local->alpha2 = mySection->param[1];
	mySection->loaded=1;
	}

// ******************************************************************

void init_efxMotionBlur () {

	local = (efxMotionBlur_section *) mySection->vars;
	local->first = 1;
}

// ******************************************************************

void render_efxMotionBlur () {

	float alpha;

	local = (efxMotionBlur_section *) mySection->vars;

	glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}

	camera_set2d();

	alpha = local->alpha1 + mySection->runTime * (local->alpha2 - local->alpha1) / mySection->duration;
	glColor4f(1, 1, 1, alpha);

	// set texture
	tex_bind(local->tex);

	// If this is the first frame, don't draw the quad since the previous screen has not been recorded
	if (local->first) local->first = 0;
	else gldrv_texscreenquad();

	camera_restore();

	// Only capture the screen one time every 1/25 seconds
	if ((int)(100 * mySection->runTime) % 25 >= 1) gldrv_copyColorBuffer();

	if (mySection->hasBlend) glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxMotionBlur () {
}
