#include "../interface/demo.h"
#include "../core/axis.h"
// ******************************************************************

typedef struct {
	rect3d_t	quad1, quad2;
	int			texture;
	int			wrap_s, wrap_t;
	// float		u0, v0, u1, v1;
	float startPerOne, endPerOne;
} imagePart_section;

static imagePart_section *local;

// ******************************************************************

void preload_imagePart () {
}

// ******************************************************************

void load_imagePart () {

	// script validation
	if ((mySection->paramNum  != 2)||(mySection->stringNum != 1)||(mySection->splineNum != 1)) {
		section_error("2 params needed: 1 string and 1 spline");
		return;
	}

	local = malloc(sizeof(imagePart_section));
	mySection->vars = (void *) local;

	// load texture
	local->texture = tex_load (mySection->strings[0], USE_CACHE, 0);
	if (local->texture == -1)
		return;
	tex_properties(local->texture, CLAMP);
	tex_upload (local->texture, USE_CACHE);

	// Start and end for rendering
	local->startPerOne = mySection->param[0];
	local->endPerOne   = mySection->param[1];
	
	// First and second rect3D initialization
	rect3d_init(&local->quad1, local->texture, tex_array[local->texture]->owidth, tex_array[local->texture]->oheight);
	rect3d_init(&local->quad2, local->texture, tex_array[local->texture]->owidth, tex_array[local->texture]->oheight);

	// get texture filter parameters
	local->wrap_s = GL_CLAMP;
	local->wrap_t = GL_CLAMP;
	mySection->loaded=1;
}

// ******************************************************************

void init_imagePart () {
}

// ******************************************************************

static void prepare_image (float step) {
	ChanVec resVec;

	dkernel_getSpline(0, step, resVec);

	glScalef (resVec[0], resVec[1], resVec[2]);
	glRotatef (resVec[3], 1, 0, 0);
	glRotatef (resVec[4], 0, 1, 0);
	glRotatef (resVec[5], 0, 0, 1);
	glTranslatef (resVec[6], resVec[7], resVec[8]);
}

// ************************************************************

void render_imagePart () {
	float zeroToOne;
	
	local = (imagePart_section *) mySection->vars;

	zeroToOne = local->startPerOne + (local->endPerOne - local->startPerOne) * mySection->runTime / mySection->duration;

	glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}
	
	if (mySection->hasAlpha) {
		float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(mySection->alphaFunc, alpha);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		prepare_image(mySection->runTime);

		local->quad1.pc[1][0] = (float) tex_array[local->texture]->owidth * (zeroToOne - 0.5f);
		local->quad1.pc[2][0] = local->quad1.pc[1][0];
		
		local->quad2.pc[0][0] = (float) tex_array[local->texture]->owidth * (zeroToOne - 0.5f);
		local->quad2.pc[3][0] = local->quad2.pc[0][0];

		tex_bind (local->quad1.tex_id);
		
		// change texture parameters
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, local->wrap_s);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, local->wrap_t);
				
		glBegin (GL_QUADS);
			// glTexCoord2f (local->u0, local->v0);
			glTexCoord2f (0.0f, 0.0f);
			glVertex3dv (local->quad1.pc[0]);

			// glTexCoord2f (local->u1, local->v0);
			glTexCoord2f (zeroToOne, 0.0f);
			glVertex3dv (local->quad1.pc[1]);

			// glTexCoord2f (local->u1, local->v1);
			glTexCoord2f (zeroToOne, 1.0f);
			glVertex3dv (local->quad1.pc[2]);

			// glTexCoord2f (local->u0, local->v1);
			glTexCoord2f (0.0f, 1.0f);
			glVertex3dv(local->quad1.pc[3]);
		glEnd (); 

			// **************************************
			
		glBegin (GL_QUADS);
			// glTexCoord2f (local->u0, local->v0);
			glTexCoord2f (zeroToOne, 0.0f);
			glVertex3dv (local->quad2.pc[0]);

			// glTexCoord2f (local->u1, local->v0);
			glTexCoord2f (zeroToOne, 0.0f);
			glVertex3dv (local->quad2.pc[1]);

			// glTexCoord2f (local->u1, local->v1);
			glTexCoord2f (zeroToOne, 1.0f);
			glVertex3dv (local->quad2.pc[2]);

			// glTexCoord2f (local->u0, local->v1);
			glTexCoord2f (zeroToOne, 1.0f);
			glVertex3dv (local->quad2.pc[3]);
		glEnd (); 
	glPopMatrix();

	AxisDrawing();

	if (mySection->hasAlpha) glDisable(GL_ALPHA_TEST);
	if (mySection->hasBlend) glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_imagePart () {
}
