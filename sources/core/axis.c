/*
	axis.c: draw axis in the screen for the actual coordinate system
*/

#include "../main.h"

// ******************************************************************

// This function draws a reference coordinate system relative to the actual
// modelview matrix. Useful to know where we are in the scene.
// The axis have a length of 1 and are drawn in colors -> x,y,z = r,g,b

// Only drawn if the demo is in debug mode and the on screen trace is visible

void AxisDrawing() {
	if ((demoSystem.debug)) {
		glDisable(GL_TEXTURE_2D);

		glLineWidth(3.0f);
		glBegin(GL_LINES);
			glColor4f  (1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3f (0.0f, 0.0f, 0.0f);
			glVertex3f (1.0f, 0.0f, 0.0f);

			glColor4f  (0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3f (0.0f, 0.0f, 0.0f);
			glVertex3f (0.0f, 1.0f, 0.0f);
			
			glColor4f  (0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3f (0.0f, 0.0f, 0.0f);
			glVertex3f (0.0f, 0.0f, 1.0f);
		glEnd();
		
		glLineWidth(1.0f);

		 glEnable(GL_TEXTURE_2D);
		
		// Default Color
		glColor4f(1, 1, 1, 1);
	}
}
