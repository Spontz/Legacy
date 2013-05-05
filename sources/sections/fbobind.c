#include "../interface/demo.h"

// ******************************************************************

void preload_fbobind () {
}

// ******************************************************************

void load_fbobind () {
	// Check for the right number of parameters in the script
	if (mySection->paramNum < 3) {
		section_error("fbobind: 3 params required.");
		return;
	}
	
	// Check for the right kind of parameters
	if ((mySection->param[0] < 0.0f) || (mySection->param[0] > (float)FBO_BUFFERS)) {
		section_error("fbobind: Invalid fbo buffer number: %i", mySection->param[0]);
		return;
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_fbobind () {
}

// ******************************************************************

void render_fbobind () {
	// Enable the buffer in which we are going to paint
	fbo_bind(demoSystem.fboRenderingBuffer[(int)mySection->param[0]]);

	// Clear the screen and depth buffers depending of the parameters passed by the user
	if (fabs(mySection->param[1] - 1.0f) < FLT_EPSILON) glClear (GL_COLOR_BUFFER_BIT);
	if (fabs(mySection->param[2] - 1.0f) < FLT_EPSILON) glClear (GL_DEPTH_BUFFER_BIT);
}

// ******************************************************************

void end_fbobind () {
}