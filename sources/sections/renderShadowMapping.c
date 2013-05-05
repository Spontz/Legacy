#include "../interface/demo.h"

typedef struct {
	tExpression evalPositioning;			// A equation containing the calculations used to position the rendering QUAD
	int		shadowMapSize;
	float	camx, camy, camz;
	float	lightx, lighty, lightz;
	GLuint	depthTextureId;
	GLuint	fboId;
} renderShadowMapping_section;

static renderShadowMapping_section *local;

// ******************************************************************

void preload_renderShadowMapping() {
}

// ******************************************************************

void load_renderShadowMapping() {
	// script validation
	if ((mySection->paramNum  != 1) || (mySection->stringNum != 9)) {
		section_error("RenderShadowMapping: 1 param & 9 strings needed");
		return;
	}
	
	local = (renderShadowMapping_section*) malloc(sizeof(renderShadowMapping_section));
	mySection->vars = (void *) local;
/*
	////////////
	char *positionEquation;
	int theSize;
	
	local = malloc(sizeof(renderShadowMapping_section));

	if (!local) {
		dkernel_error("insuficient memory");
		return;
		
	} else {
		mySection->vars = (void *) local;
		
		// Check for the right number of parameters in the script
		if (mySection->paramNum > 4) {
			section_error("renderBuffer: 4 params required.");
			return;
		}
		// Check for the right kind of parameters
		if ((mySection->param[0] < -1.0f) || (mySection->param[0] > (float)RENDERING_BUFFERS)) {
			section_error("renderBuffer: Invalid texture buffer number: %i", mySection->param[0]);
			return;
		}
		if (mySection->param[3] == 0) {
			// QUAD rendering, so load the positioning equations
			theSize = strlen(mySection->strings[0]) + strlen(mySection->strings[1]) + strlen(mySection->strings[2]);
			positionEquation = calloc(theSize+1, sizeof(char));
			sprintf(positionEquation, "%s%s%s", mySection->strings[0], mySection->strings[1], mySection->strings[2]);
		
			local->evalPositioning.equation = positionEquation;
			
			// Init the expression evaluation library
			initExpression(&local->evalPositioning);
		}
	}
 */
	mySection->loaded=1;
}

// ******************************************************************

void init_renderShadowMapping () {
}

// ******************************************************************

void render_renderShadowMapping () {
/*	local = (renderShadowMapping_section *) mySection->vars;

	// Clear the screen and depth buffers depending of the parameters passed by the user
	if (mySection->param[1] == 1.0f) glClear (GL_COLOR_BUFFER_BIT);
	if (mySection->param[2] == 1.0f) glClear (GL_DEPTH_BUFFER_BIT);
	
	// Exit if shaders not supported (param==-1 when we want to use renderbuffer for shaders)
	if (!glDriver.ext.glslshaders && (int)mySection->param[0]==-1)
		return;
	
	// Enable the buffer in which we are going to paint
	if ((int)mySection->param[0]>=0)
		tex_bind(demoSystem.texRenderingBuffer[(int)mySection->param[0]]);
	
	// Check if the user wants to use some kind of blending state or not
	if (mySection->hasBlend) {
		// The user wants to use blending, so first of all let's activate the blending register
		glEnable (GL_BLEND);
		
		// And then set the start and end blending factors
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}
	
	// Deactivate the depth buffer calculations to avoid interferences with other objects
	glDisable (GL_DEPTH_TEST);
	
	if (mySection->param[3] == 1) { // Fullscreen rendering
		// Set the camera matrix to a 2D one
		camera_set2d ();
		
		// Paint a textured quad covering all the screen area
		// The used texture will be the one corresponding to the previously used buffer
		gldrv_texscreenquad ();
		
		// Restore the camera
		camera_restore ();
		
	} else { // Rendering into a free quad
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		place_object (&local->evalPositioning);
		
		glBegin (GL_QUADS);
			glTexCoord2f (0,0);
			glVertex2f (-gldrv_get_aspect_ratio(),-1);

			glTexCoord2f (gldrv_get_viewport_aspect_ratio(),0);
			glVertex2f (gldrv_get_aspect_ratio(),-1);

			glTexCoord2f (gldrv_get_viewport_aspect_ratio(),(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f (gldrv_get_aspect_ratio(),1);

			glTexCoord2f (0,(1 / gldrv_get_viewport_aspect_ratio()));
			glVertex2f (-gldrv_get_aspect_ratio(),1);
		glEnd ();
	
		glPopMatrix();
	}
	
	// Activate again the depth buffer calculation
	glEnable (GL_DEPTH_TEST);
	
	// If the section was using blending, deactivate the BLEND register
	if (mySection->hasBlend) glDisable (GL_BLEND);
 */
}

// ******************************************************************

void end_renderShadowMapping () {
}
