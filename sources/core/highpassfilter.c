/*
	highpassfilter.c: high pass filter related code
*/

#include "../main.h"

#include "drivers/gldriver.h"
#include "texture.h"

// ******************************************************************

void render_highpassfilter (int tex, float threshold_R, float threshold_G, float threshold_B, int accum) {

	int i;

	// disable texturing
	glDisable (GL_TEXTURE_2D);

	// invert color buffer
	glBlendFunc (GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glColor4f (1,1,1,1);
	gldrv_screenquad ();

	// add the threshold
	glBlendFunc (GL_ONE, GL_ONE);
	glColor4f (threshold_R, threshold_G ,threshold_B, 1);
	gldrv_screenquad ();

	// invert color buffer
	glBlendFunc (GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glColor4f (1,1,1,1);
	gldrv_screenquad ();

	// enable texturing
	glEnable (GL_TEXTURE_2D);

	// accumulate color buffer
	tex_bind (tex);
	gldrv_copyColorBuffer ();

	glBlendFunc (GL_ONE, GL_ONE);
	glColor4f (1,1,1,1);
	glBegin (GL_QUADS);
	for (i=0; i<accum; i++) {
		glTexCoord2f (0,0);
		glVertex2f (0,0);

		glTexCoord2f (gldrv_get_viewport_aspect_ratio(),0);
		glVertex2f (1,0);

		glTexCoord2f (gldrv_get_viewport_aspect_ratio(),(1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (1,1);

		glTexCoord2f (0,(1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (0,1);
	}
	glEnd ();
}

// ******************************************************************

extern void draw_offset_quad_multi(float offsetX, float offsetY);
extern unsigned int get_closest_power_of_two(unsigned int value);

void render_highpassfilter_ext (int tex, float threshold_R, float threshold_G, float threshold_B, int accum)
	{
	int i;
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)get_closest_power_of_two(glDriver.vpWidth);
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)get_closest_power_of_two(glDriver.vpHeight);

	// disable texturing
	glDisable (GL_TEXTURE_2D);

	// substract the threshold
	glBlendFunc (GL_ONE, GL_ONE);
	glBlendEquationEXT (GL_FUNC_REVERSE_SUBTRACT_EXT);
	glColor4f (threshold_R, threshold_G ,threshold_B, 0);
	gldrv_screenquad ();
	glBlendEquationEXT (GL_FUNC_ADD_EXT);

	// enable multitexturing
	gldrv_enable_multitexture();

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	gldrv_copyColorBuffer ();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glActiveTextureARB (GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD);

	glBlendFunc (GL_ONE, GL_ONE);
	glColor4f (1,1,1,1);
	glBegin (GL_QUADS);
	for (i=0; i<accum/2; i++)
		{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);
		glVertex2f(x0, y0);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 0);
		glVertex2f(x1, y0);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 1);
		glVertex2f(x1, y1);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 1);
		glVertex2f(x0, y1);

		/*
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);
		glVertex2f (0,0);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, gldrv_get_viewport_aspect_ratio(), 0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, gldrv_get_viewport_aspect_ratio(), 0);
		glVertex2f (1,0);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (1,1);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, (1 / gldrv_get_viewport_aspect_ratio()));
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, (1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (0,1);
		*/
		}
	glEnd ();

	gldrv_disable_multitexture();
}
