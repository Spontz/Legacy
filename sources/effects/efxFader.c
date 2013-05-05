/*
	efxFader.c: simple fade effect
*/

#include "../interface/demo.h"

// ******************************************************************

void preload_efxFader () {
}

// ******************************************************************

void load_efxFader () {

	// script validation
	if (mySection->paramNum != 8) {
		section_error("8 params needed");
		return;
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_efxFader () {
}

// ******************************************************************

void render_efxFader () {

	float t, tfin, unitVar;

	// init and end colors and alpha
	float Ri, Gi, Bi, Ai;
	float Re, Ge, Be, Ae;

	// script variables initialization
	Ri = mySection->param[0];
	Gi = mySection->param[1];
	Bi = mySection->param[2];
	Ai = mySection->param[3];

	Re = mySection->param[4];
	Ge = mySection->param[5];
	Be = mySection->param[6];
	Ae = mySection->param[7];

	// time variables
	t = mySection->runTime;
	tfin = mySection->duration;
	unitVar = t / tfin;

	// set render state
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// render quad
	camera_set2d ();

		glColor4f(Ri + (Re - Ri) * unitVar,
				  Gi + (Ge - Gi) * unitVar,
				  Bi + (Be - Bi) * unitVar,
				  Ai + (Ae - Ai) * unitVar);

		gldrv_screenquad();

	camera_restore ();

	// unset render state
	glDisable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxFader () {
}
