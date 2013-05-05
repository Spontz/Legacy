#include "../interface/demo.h"

// ******************************************************************

void preload_renderFbo() {
}

// ******************************************************************

void load_renderFbo() {
	// Check for the right number of parameters in the script
	if (mySection->paramNum > 4) {
		section_error("renderFbo: 4 params required.");
		return;
	}

	// Check for the right kind of parameters
	if ((mySection->param[0] < -1.0f) || (mySection->param[0] > (float)FBO_BUFFERS)) {
		section_error("renderFbo: Invalid texture fbo number: %i", mySection->param[0]);
		return;
	}
	mySection->loaded=1;
}
// ******************************************************************

void init_renderFbo () {
}

// ******************************************************************

void render_renderFbo () {
	// Clear the screen and depth buffers depending of the parameters passed by the user
	if (fabs(mySection->param[1] - 1.0f) < FLT_EPSILON) glClear (GL_COLOR_BUFFER_BIT);
	if (fabs(mySection->param[2] - 1.0f) < FLT_EPSILON) glClear (GL_DEPTH_BUFFER_BIT);
	
	// Exit if shaders not supported (param==-1 when we want to use renderfbo for shaders)
	if (!glDriver.ext.glslshaders && (int)mySection->param[0]==-1)
		return;
	
	// Enable the buffer in which we are going to paint
	if ((int)mySection->param[0]>=0)
		fbo_bind_tex(demoSystem.fboRenderingBuffer[(int)mySection->param[0]]);
	
	// Check if the user wants to use some kind of blending state or not
	if (mySection->hasBlend) {
		// The user wants to use blending, so first of all let's activate the blending register
		glEnable (GL_BLEND);
		
		// And then set the start and end blending factors
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}
	
	// Deactivate the depth buffer calculations to avoid interferences with other objects
	glDisable (GL_DEPTH_TEST);
	
	if (fabs(mySection->param[3] - 1 ) < FLT_EPSILON) { // Fullscreen rendering
		// Set the camera matrix to a 2D one
		camera_set2d ();
		
		// Paint a textured quad covering all the screen area
		// The used texture will be the one corresponding to the previously used buffer
		gldrv_texscreenquad ();
		
		// Restore the camera
		camera_restore ();
		
	} else { // Rendering into a free quad
	
		glBegin (GL_QUADS);
			glTexCoord2f (0,0);
			glVertex2f (0,0);

			glTexCoord2f (gldrv_get_viewport_aspect_ratio(),0);
			glVertex2f (gldrv_get_aspect_ratio(),0);

			glTexCoord2f (gldrv_get_viewport_aspect_ratio(),(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f (gldrv_get_aspect_ratio(),1);

			glTexCoord2f (0,(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f (0,1);
		glEnd ();
	
	}
	
	// Activate again the depth buffer calculation
	glEnable (GL_DEPTH_TEST);
	
	// If the section was using blending, deactivate the BLEND register
	if (mySection->hasBlend) glDisable (GL_BLEND);
}

// ******************************************************************

void end_renderFbo () {
}
