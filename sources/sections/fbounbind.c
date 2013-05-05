#include "../interface/demo.h"
#include "../core/drivers/events.h"

// ******************************************************************

// ******************************************************************

void preload_fbounbind () {
}

// ******************************************************************

void load_fbounbind () {
	// Check for the right number of parameters in the script
	if (mySection->paramNum < 2) {
		section_error("fbounbind: 2 params required.");
		return;
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_fbounbind () {
}

// ************************************************************

void render_fbounbind () {
	
	fbo_unbind();

	// Clear the screen and depth buffers depending of the parameters passed by the user
	if (fabs(mySection->param[0] - 1.0f) < FLT_EPSILON) glClear (GL_COLOR_BUFFER_BIT);
	if (fabs(mySection->param[1] - 1.0f) < FLT_EPSILON) glClear (GL_DEPTH_BUFFER_BIT);

}

// ******************************************************************

void end_fbounbind () {
}
