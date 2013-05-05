/*
	efxRadialBlur.c: radial blur effect
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {

	int iterations;   // number of quads drawn
	float aini, afin; // alpha retlsed variables
	float sini, sfin; // separation related variables
	float xini, xend; // initial X and Y coordinates for the center (from 0 to 1)
	float yini, yend; // final X and Y coordinates for the center (from 0 to 1)

} efxRadialBlur_section;

static efxRadialBlur_section *local;

// ******************************************************************

void preload_efxRadialBlur () {
}

// ******************************************************************

void load_efxRadialBlur () {

	// script validation
	if (mySection->paramNum != 10) {
		section_error("10 params needed");
		return;
	}
	
	local = malloc(sizeof(efxRadialBlur_section));
	mySection->vars = (void *) local;

	local->iterations = (int) mySection->param[0];

	local->aini = mySection->param[2];
	local->afin = mySection->param[3];
	local->sini = mySection->param[4];
	local->sfin = mySection->param[5];

	local->xini = mySection->param[6];
	local->yini = mySection->param[7];
	local->xend = mySection->param[8];
	local->yend = mySection->param[9];
	mySection->loaded=1;
}

// ******************************************************************

void init_efxRadialBlur () {
}

// ******************************************************************

void render_efxRadialBlur () {

	float t, tEnd, step;
	float alpha, separation;
	float coffsetX, coffsetY;
	float offset, offsetX, offsetY;
	int i;

	local = (efxRadialBlur_section *) mySection->vars;

	t = mySection->runTime;
	tEnd = mySection->duration;
	step = t / tEnd;

	alpha      = local->aini + (local->afin - local->aini) * step;
	separation = local->sini + (local->sfin - local->sini) * step;
	coffsetX   = local->xini * (1 - step) + local->xend * step;
	coffsetY   = local->yini * (1 - step) + local->yend * step;

	glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}

	camera_set2d();

	tex_bind(demoSystem.rtt);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	for (i=0; i<local->iterations; i++) {

		gldrv_copyColorBuffer();

		step = (float) i / local->iterations;
		offset = step * separation * (float) i;
		offsetX = offset * (coffsetX - 0.5f);
		offsetY = offset * (0.5f - coffsetY);

		glBegin(GL_QUADS);
			glTexCoord2f (offsetX,offsetY+(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f   (-offset,1+offset);

			glTexCoord2f (offsetX,offsetY);
			glVertex2f   (-offset,-offset);

			glTexCoord2f (offsetX+gldrv_get_viewport_aspect_ratio(),offsetY);
			glVertex2f   (1+offset,-offset);

			glTexCoord2f (offsetX+gldrv_get_viewport_aspect_ratio(),offsetY+(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f   (1+offset,1+offset);

		glEnd();
	}

	camera_restore();

	glClear(GL_DEPTH_BUFFER_BIT);

	if (mySection->hasBlend) glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxRadialBlur () {
}
