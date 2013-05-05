/*
	efxBlackWhite.c: black & white screen effect
*/

#include "../interface/demo.h"

// ******************************************************************

static int tex;

// ******************************************************************

void preload_efxBlackWhite ()
	{
	tex = tex_new(glDriver.vpWidth, glDriver.vpHeight, GL_LUMINANCE, 1);
	tex_properties(tex, NO_MIPMAP | CLAMP);
	tex_upload(tex, USE_CACHE);
	}

// ******************************************************************

void load_efxBlackWhite () {

	// script validation
	if (mySection->paramNum != 2) {
		section_error("2 params needed");
		return;
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_efxBlackWhite () {
}

// ******************************************************************

void render_efxBlackWhite () {

	// init and final alpha
	float Ai, Ae;
	float unitVar;

	// script variables initialization
	Ai = mySection->param[0];
	Ae = mySection->param[1];

	// time variables
	unitVar = mySection->runTime / mySection->duration;

	// set render state
	glDisable (GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// render quad
	camera_set2d ();

		tex_bind (tex);
		gldrv_copyColorBuffer ();

		glColor4f (1, 1, 1, Ai + (Ae - Ai) * unitVar);
		gldrv_texscreenquad ();

	camera_restore ();

	// unset render state
	glDisable (GL_BLEND);
	glEnable (GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxBlackWhite () {
}
