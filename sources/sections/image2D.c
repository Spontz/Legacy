#include "../interface/demo.h"
#include "../core/drivers/events.h"

// ******************************************************************

typedef struct {
	rect2d_t	quad;
	int			texture;
	
	float xScale, yScale;
	float zRotation;
	float xTranslate, yTranslate;
} image_section;

static image_section *local;

// ******************************************************************

void preload_image2D () {
}

// ******************************************************************

void load_image2D () {

	// script validation
	// The first string represents the used texture
	// The first spline represents the positioning of the texture along time
	if ((mySection->stringNum != 1) || (mySection->splineNum > 1)) {
		section_error("1 string needed and Found %i splines, but only needed 1", mySection->splineNum);
		return;
	}

	local = malloc(sizeof(image_section));
	mySection->vars = (void *) local;
	
	// Variable initing
	local->xScale = 1.0f;
	local->yScale = 1.0f;
	local->zRotation = 0.0f;
	local->xTranslate = 0.0f;
	local->yTranslate = 0.0f;

	// load texture
	local->texture = tex_load (mySection->strings[0], USE_CACHE, 0);
	if (local->texture == -1)
		return;
	tex_properties(local->texture, NO_MIPMAP);
	tex_upload(local->texture, USE_CACHE);

	// rect2D initialization
	local->quad.x0 = -0.5f * (float) tex_array[local->texture]->owidth / (float) tex_array[local->texture]->oheight;
	local->quad.y0 = -0.5f;
	local->quad.x1 = 0.5f * (float) tex_array[local->texture]->owidth / (float) tex_array[local->texture]->oheight;
	local->quad.y1 =  0.5f;
			
	// Polygon coordinates
	local->quad.pc[0][0] = local->quad.x0; local->quad.pc[0][1] = local->quad.y0;
	local->quad.pc[1][0] = local->quad.x1; local->quad.pc[1][1] = local->quad.y0;
	local->quad.pc[2][0] = local->quad.x1; local->quad.pc[2][1] = local->quad.y1;
	local->quad.pc[3][0] = local->quad.x0; local->quad.pc[3][1] = local->quad.y1;

	// Texture coordinates
	local->quad.tc[0][0] = 0; local->quad.tc[0][1] = 1;
	local->quad.tc[1][0] = 1; local->quad.tc[1][1] = 1;
	local->quad.tc[2][0] = 1; local->quad.tc[2][1] = 0;
	local->quad.tc[3][0] = 0; local->quad.tc[3][1] = 0;
	mySection->loaded=1;
}

// ******************************************************************

void init_image2D () {
}

// ******************************************************************

static void prepare_object (float step) {


	ChanVec resVec;
	local = (image_section *) mySection->vars;

	if (!mySection->splineNum) {
		// Keyboard handling
		if (demoSystem.keys[KEY_FORWARD    ]) local->yTranslate = local->yTranslate + (5 / (local->yScale * glDriver.vpHeight));
		if (demoSystem.keys[KEY_BACKWARD   ]) local->yTranslate = local->yTranslate - (5 / (local->yScale * glDriver.vpHeight));
		if (demoSystem.keys[KEY_STRAFELEFT ]) local->xTranslate = local->xTranslate - (5 / (local->xScale * glDriver.vpWidth));
		if (demoSystem.keys[KEY_STRAFERIGHT]) local->xTranslate = local->xTranslate + (5 / (local->xScale * glDriver.vpWidth));
		if (demoSystem.keys[KEY_ROLLUP     ]) local->zRotation  = local->zRotation  + 1;
		if (demoSystem.keys[KEY_ROLLDOWN   ]) local->zRotation  = local->zRotation  - 1;
		
		if (demoSystem.keys[KEY_FOVUP      ]) {
			local->xScale  = local->xScale  + 0.01f;
			local->yScale  = local->yScale  + 0.01f;
		}
		
		if (demoSystem.keys[KEY_FOVDOWN    ]) {
			local->xScale  = local->xScale  - 0.01f;
			local->yScale  = local->yScale  - 0.01f;
		}
		
		if (demoSystem.keys[KEY_CAMRESET   ]) {
			// Reset the image position
			local->xScale = 1.0f;
			local->yScale = 1.0f;
			local->zRotation = 0.0f;
			local->xTranslate = 0.0f;
			local->yTranslate = 0.0f;
		}
		
		if (demoSystem.keys[KEY_CAPTURE]) {
				// Write the image properties to the standard output
				fprintf(stdout,"%f %f %f %f %f\n",
				local->xScale, local->yScale, local->zRotation, local->xTranslate, local->yTranslate);
			demoSystem.keys[KEY_CAPTURE] = FALSE;
		}
		
		// The section does not have a path so let's use the keys
		glTranslatef (local->xTranslate, local->yTranslate, 0);	
		glRotatef    (local->zRotation, 0, 0, 1);	
		glScalef     (local->xScale, local->yScale, 0);
	} else {		
		// The section has a spline, so let's follow it!
		dkernel_getSpline(0, step, resVec);
		glTranslatef (resVec[3], resVec[4], 0);
		glRotatef    (resVec[2], 0, 0, 1);	
		glScalef     (resVec[0], resVec[1], 0);
	}
}

// ************************************************************

void render_image2D () {
	int i;
	local = (image_section *) mySection->vars;
		
		glDisable(GL_DEPTH_TEST);
		
		// Set blending, if specified in the script
		if (mySection->hasBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(mySection->sfactor, mySection->dfactor);
		}
		
		// Set alpha animation, if specified in the script
		if (mySection->hasAlpha) {
			float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(mySection->alphaFunc, alpha);
		}
	
		// Put the camera in 2D mode
		glMatrixMode (GL_PROJECTION);
		glPushMatrix ();
		glLoadIdentity ();
				
		glOrtho (-1.0f * gldrv_get_aspect_ratio(), 1.0f * gldrv_get_aspect_ratio(), -1.0f, 1.0f, -1,1);
		glMatrixMode (GL_MODELVIEW);
		glPushMatrix ();
		glLoadIdentity ();
	
			// Modify the object prior to drawing
			prepare_object(mySection->runTime);

			// Choose the proper texture
			tex_bind (local->texture);
			
			// And draw the object
			glBegin (GL_QUADS);
				for (i=0; i<4; i++)	{
					glTexCoord2fv (local->quad.tc[i]);
					glVertex2fv (local->quad.pc[i]);
				}
			glEnd ();

		// Restore the camera to the previous state
		glMatrixMode (GL_MODELVIEW);
		glPopMatrix ();
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();

		// Restore Blending and alpha values (if needed)
		glMatrixMode (GL_MODELVIEW);
		if (mySection->hasAlpha) glDisable(GL_ALPHA_TEST);
		if (mySection->hasBlend) glDisable(GL_BLEND);
		
		glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_image2D () {
}
