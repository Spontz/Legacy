/*
	rect3d.c: rectangles in 3d
*/

#include "../main.h"

// ******************************************************************

void rect3d_init (rect3d_t *rect, int tex_id, int width, int height) {

	rect->x0 = (double)(-width  / 2.0);
	rect->x1 = (double)( width  / 2.0);
	rect->y0 = (double)( height / 2.0);
	rect->y1 = (double)(-height / 2.0);

	rect->pc[0][0] = rect->x0; rect->pc[0][1] = rect->y0; rect->pc[0][2] = 0.0;
	rect->pc[1][0] = rect->x1; rect->pc[1][1] = rect->y0; rect->pc[1][2] = 0.0;
	rect->pc[2][0] = rect->x1; rect->pc[2][1] = rect->y1; rect->pc[2][2] = 0.0;
	rect->pc[3][0] = rect->x0; rect->pc[3][1] = rect->y1; rect->pc[3][2] = 0.0;

	rect->tc[0][0] = 0.01; rect->tc[0][1] = 0.01;
	rect->tc[1][0] = 0.99; rect->tc[1][1] = 0.01;
	rect->tc[2][0] = 0.99; rect->tc[2][1] = 0.99;
	rect->tc[3][0] = 0.01; rect->tc[3][1] = 0.99;

	rect->tex_id = tex_id;
}

// ******************************************************************

void rect3d_render (rect3d_t *rect) {	

	tex_bind (rect->tex_id);

	glBegin (GL_QUADS);
		glTexCoord2dv (rect->tc[0]);
		glVertex3dv   (rect->pc[0]);

		glTexCoord2dv (rect->tc[1]);
		glVertex3dv   (rect->pc[1]);

		glTexCoord2dv (rect->tc[2]);
		glVertex3dv   (rect->pc[2]);

		glTexCoord2dv (rect->tc[3]);
		glVertex3dv   (rect->pc[3]);
	glEnd ();
}
