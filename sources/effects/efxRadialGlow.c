/*
	efxRadialRadialGlow.c: radial glow effect
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {

	float threshold_R;
	float threshold_G;
	float threshold_B;
	int accum;

	int iterations; // number of quads drawed
	int recursive;  // recursive radial blur?

	float aini, afin; // alpha retaled variables
	float sini, sfin; // separation related variables
	float xini, xend; // initial X and Y coordinates
	float yini, yend; // final X and Y coordinates

} efxRadialGlow_section;

static efxRadialGlow_section *local;

// ******************************************************************

void preload_efxRadialGlow () {
}

// ******************************************************************

void load_efxRadialGlow () {

	// script validation
	if (mySection->paramNum != 14) {
		section_error("14 params needed");
		return;
	}
	
	local = malloc(sizeof(efxRadialGlow_section));
	mySection->vars = (void *) local;

	local->threshold_R = mySection->param[0];
	local->threshold_G = mySection->param[1];
	local->threshold_B = mySection->param[2];
	local->accum = (int) mySection->param[3];

	local->iterations = (int) mySection->param[4];
	local->recursive = fabs(mySection->param[5] - 1) < FLT_EPSILON;

	local->aini = mySection->param[6];
	local->afin = mySection->param[7];
	local->sini = mySection->param[8];
	local->sfin = mySection->param[9];

	local->xini = mySection->param[10];
	local->yini = mySection->param[11];
	local->xend = mySection->param[12];
	local->yend = mySection->param[13];
	mySection->loaded=1;
}

// ******************************************************************

void init_efxRadialGlow () {
}

// ******************************************************************

void render_efxRadialGlow () {

	float t, tEnd, step;
	float alpha, separation;
	float coffsetX, coffsetY;
	float offset, offsetX, offsetY;
	int i;

	local = (efxRadialGlow_section *) mySection->vars;

	// capture current color buffer
	tex_bind(demoSystem.backup);
	gldrv_copyColorBuffer ();

	// set render state
	glDisable (GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	camera_set2d ();

	// highpass filter
	render_highpassfilter_ext (demoSystem.rtt,
		local->threshold_R, local->threshold_G ,local->threshold_B,
		local->accum);

	// radial blur
	t = mySection->runTime;
	tEnd = mySection->duration;
	step = t / tEnd;

	alpha      = local->aini + (local->afin - local->aini) * step;
	separation = local->sini + (local->sfin - local->sini) * step;
	coffsetX   = local->xini * (1 - step) + local->xend * step;
	coffsetY   = local->yini * (1 - step) + local->yend * step;

	if (mySection->hasBlend) {
		// User-configured blending factor
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	} else {
		// Default blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	if (local->recursive) {

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

	} else {

		gldrv_copyColorBuffer();
		glBegin(GL_QUADS);

		for (i=0; i<local->iterations; i++) {

			step = (float) i / local->iterations;
			offset = step * separation * (float) i;
			offsetX = offset * (coffsetX - 0.5f);
			offsetY = offset * (0.5f - coffsetY);

			glTexCoord2f (offsetX,offsetY+(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f   (-offset,1+offset);

			glTexCoord2f (offsetX,offsetY);
			glVertex2f   (-offset,-offset);

			glTexCoord2f (offsetX+gldrv_get_viewport_aspect_ratio(),offsetY);
			glVertex2f   (1+offset,-offset);

			glTexCoord2f (offsetX+gldrv_get_viewport_aspect_ratio(),offsetY+(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f   (1+offset,1+offset);
		}

		glEnd();
	}

	// restore original color buffer
	tex_bind (demoSystem.backup);
	glBlendFunc (GL_ONE, GL_ONE);
	glColor4f (1,1,1,1);
	gldrv_texscreenquad ();

	// clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// unset render state
	camera_restore ();
	glDisable (GL_BLEND);
	glEnable (GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxRadialGlow () {
}
