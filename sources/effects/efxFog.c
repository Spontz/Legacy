/*
	efxFog.c: simple fog effect
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	int mode;
	float density;
	float start, end;
	float r, g, b, a;
} efxFog_section;

static efxFog_section *local;

// ******************************************************************

void preload_efxFog () {
}

// ******************************************************************

void load_efxFog ()
	{
	float mode;

	// script validation
	if ((mySection->paramNum < 6) || (mySection->paramNum > 7))
		{
		section_error("invalid number of params");
		return;
		}

	local = malloc(sizeof(efxFog_section));
	mySection->vars = (void *) local;

	local->r = mySection->param[0];
	local->g = mySection->param[1];
	local->b = mySection->param[2];
	local->a = mySection->param[3];

	mode = mySection->param[4];

	if (fabs(mode) < FLT_EPSILON)
		{
		// linear fog
		local->mode = GL_LINEAR;
		local->start = mySection->param[5];
		local->end = mySection->param[6];
		}
	else
		{
		// exponential fogs
		if (fabs(mode-1) < FLT_EPSILON)
			local->mode = GL_EXP;
		else if (fabs(mode-2) < FLT_EPSILON)
			local->mode = GL_EXP2;
		else
			section_error("invalid fog mode");
		local->density = mySection->param[5];
		}
	mySection->loaded=1;
	}

// ******************************************************************

void init_efxFog () {

	local = (efxFog_section *) mySection->vars;

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, local->mode);

	glFogfv(GL_FOG_COLOR, &local->r);

	if (local->mode == GL_LINEAR) {
		glFogf(GL_FOG_START, local->start);
		glFogf(GL_FOG_END, local->end);
	}
	else {
		glFogf(GL_FOG_DENSITY, local->density);
	}
}

// ******************************************************************

void render_efxFog () {
}

// ******************************************************************

void end_efxFog () {

	glDisable(GL_FOG);
}
