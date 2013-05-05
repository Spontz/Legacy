#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	rect3d_t	quad;
	int			texture;
} image_section;

static image_section *local;

// ******************************************************************

void preload_image () {
}

// ******************************************************************

void load_image () {

	// script validation
	if ((mySection->stringNum != 1)||(mySection->splineNum != 1)) {
		section_error("image: 1 string and 1 spline needed");
		return;
	}

	local = malloc(sizeof(image_section));
	mySection->vars = (void *) local;

	// load texture
	if (mySection->paramNum == 2 && fabs(mySection->param[0] - 1.0f) < FLT_EPSILON) {
		local->texture = demoSystem.texRenderingBuffer[(int)mySection->param[1]];
	} else {
		local->texture = tex_load (mySection->strings[0], USE_CACHE, 0);
		if (local->texture == -1)
			return;
		tex_upload (local->texture, USE_CACHE);
	}
	
	// rect3D initialization
	// rect3d_init(&local->quad, demoSystem.rtt, glDriver.rttWidth, glDriver.rttHeight);
	rect3d_init(&local->quad, local->texture, tex_array[local->texture]->width, tex_array[local->texture]->height);
	mySection->loaded=1;
}

// ******************************************************************

void init_image () {
}

// ******************************************************************

static void prepare_object (float step) {

	ChanVec resVec;

	dkernel_getSpline(0, step, resVec);

	glScalef (resVec[0], resVec[1], resVec[2]);
	glRotatef (resVec[3], 1, 0, 0);
	glRotatef (resVec[4], 0, 1, 0);
	glRotatef (resVec[5], 0, 0, 1);
	glTranslatef (resVec[6], resVec[7], resVec[8]);
}

// ************************************************************

void render_image () {
	local = (image_section *) mySection->vars;

	glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}
	
	if (mySection->hasAlpha) {
		float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(mySection->alphaFunc, alpha);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		prepare_object(mySection->runTime);
		tex_properties(local->texture, NO_MIPMAP);
		rect3d_render(&local->quad);
	glPopMatrix();

	if (mySection->hasAlpha) glDisable(GL_ALPHA_TEST);
	if (mySection->hasBlend) glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_image () {
}
