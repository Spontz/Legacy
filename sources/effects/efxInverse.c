/*
	efxInverse.c: inverse screen color effect
*/

#include "../interface/demo.h"

// ******************************************************************

void preload_efxInverse () {
}

// ******************************************************************

void load_efxInverse () {
}

// ******************************************************************

void init_efxInverse () {
}

// ******************************************************************

void render_efxInverse () {

	// set render state
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glColor4f (1,1,1,1);

	// render quad
	camera_set2d ();
	gldrv_screenquad ();
	camera_restore ();

	// unset render state
	glDisable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxInverse () {
}
