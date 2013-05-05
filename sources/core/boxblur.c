#include "../main.h"

/*
void drawOffsetQuadMulti(float offsetX, float offsetY) {

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);
	glVertex2f(0.0f+offsetX,0.0f+offsetY);

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, gldrv_get_viewport_aspect_ratio(), 0);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, gldrv_get_viewport_aspect_ratio(), 0);
	glVertex2f(1.0f+offsetX,0.0f+offsetY);

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
	glVertex2f(1.0f+offsetX,1.0f+offsetY);

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, (1 / gldrv_get_viewport_aspect_ratio()));
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, (1 / gldrv_get_viewport_aspect_ratio()));
	glVertex2f(0.0f+offsetX,1.0f+offsetY);
}
*/

extern unsigned int get_closest_power_of_two(unsigned int value);

void draw_offset_quad_multi(float offsetX, float offsetY)
	{
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)get_closest_power_of_two(glDriver.vpWidth);
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)get_closest_power_of_two(glDriver.vpHeight);

	glBegin(GL_QUADS);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);
		glVertex2f(x0 + offsetX, y0 + offsetY);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 0);
		glVertex2f(x1 + offsetX, y0 + offsetY);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 1);
		glVertex2f(x1 + offsetX, y1 + offsetY);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 1);
		glVertex2f(x0 + offsetX, y1 + offsetY);
	glEnd();
	}

void render_boxblur_ext(int tex, float radius)
	{
	float color[4] = {0.5f,0.5f,0.5f,0.5f};
	float pixelX, pixelY;
	float nOffset;

	gldrv_enable_multitexture();

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color); //-V525
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_CONSTANT_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_CONSTANT_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);

	glColor4f(1,1,1,0.33f);

	nOffset = 1.0f;
	pixelX = nOffset / (float) glDriver.vpWidth;
	pixelY = nOffset / (float) glDriver.vpHeight;

	for (;;)
		{
		gldrv_copyColorBuffer();
		draw_offset_quad_multi(pixelX, 0);

		gldrv_copyColorBuffer();
		draw_offset_quad_multi(0, pixelY);

		if (nOffset < radius)
			{
			nOffset *= 2;
			if (nOffset > radius)
				nOffset = radius;
			pixelX = nOffset / (float) glDriver.vpWidth;
			pixelY = nOffset / (float) glDriver.vpHeight;
			}
		else
			{
			break;
			}
		}

	gldrv_disable_multitexture();
	}
