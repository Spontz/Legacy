#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	int index;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float position[4];
} light_section;

static light_section *local;

// ******************************************************************

void preload_light () {
}

// ******************************************************************

void load_light () {

	int index;

	// script validation
	if ((mySection->paramNum  != 1) || (mySection->splineNum != 2)) {
		section_error("1 param and 2 splines needed");
		return;
	}

	local = malloc(sizeof(light_section));
	mySection->vars = (void *) local;

	index = (int) mySection->param[0];
	switch (index) {
		case 0: local->index = GL_LIGHT0; break;
		case 1: local->index = GL_LIGHT1; break;
		case 2: local->index = GL_LIGHT2; break;
		case 3: local->index = GL_LIGHT3; break;
		case 4: local->index = GL_LIGHT4; break;
		case 5: local->index = GL_LIGHT5; break;
		case 6: local->index = GL_LIGHT6; break;
		case 7: local->index = GL_LIGHT7; break;
		default: section_error("Invalid light index"); break;
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_light () {
}

// ******************************************************************

static void prepare_light (float step) {

	ChanVec colors, position;

	dkernel_getSpline(0, step, colors);
	dkernel_getSpline(1, step, position);

	local->ambient[0] = colors[0];
	local->ambient[1] = colors[1];
	local->ambient[2] = colors[2];
	local->ambient[3] = 1.0f;

	local->diffuse[0] = colors[3];
	local->diffuse[1] = colors[4];
	local->diffuse[2] = colors[5];
	local->diffuse[3] = 1.0f;

	local->specular[0] = colors[6];
	local->specular[1] = colors[7];
	local->specular[2] = colors[8];
	local->specular[3] = 1.0f;

	local->position[0] = position[0];
	local->position[1] = position[1];
	local->position[2] = position[2];
	local->position[3] = 0.0f;
}

// ************************************************************

void render_light () {
	local = (light_section *) mySection->vars;

	prepare_light(mySection->runTime);

	glEnable(local->index);
	glLightfv(local->index, GL_AMBIENT, local->ambient);
	glLightfv(local->index, GL_DIFFUSE, local->diffuse);
	glLightfv(local->index, GL_SPECULAR, local->specular);
	glLightfv(local->index, GL_POSITION, local->position);
	
	AxisDrawing();
}

// ******************************************************************

void end_light () {

	local = (light_section *) mySection->vars;
	glDisable(local->index);
}
