#include "../interface/demo.h"

// types

typedef struct
	{
	float radius;
	} efxBoxBlur_section;

// variables

static efxBoxBlur_section*	local;

// functions

void	preload_efxBoxBlur()
	{
	}

void	load_efxBoxBlur()
	{
	local = malloc(sizeof(efxBoxBlur_section));
	mySection->vars = (void *) local;

	// script validation
	if ((mySection->paramNum != 1))
		{
		dkernel_trace("1 param needed");
		local->radius =  1;
		}
	else
		local->radius = mySection->param[0];

	mySection->loaded=1;
	}

void	init_efxBoxBlur()
	{
	}

void	render_efxBoxBlur()
	{
	local = (efxBoxBlur_section *) mySection->vars;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera_set2d();

	render_boxblur_ext(demoSystem.rtt, local->radius);

	camera_restore();

	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	}

void end_efxBoxBlur()
	{
	}
