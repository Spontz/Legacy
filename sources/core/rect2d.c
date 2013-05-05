/*
	rect2d.c: rectangles in 2d
*/

#include "../main.h"

// ******************************************************************

void rect2d_init (rect2d_t *rect, int tex_id, int width, int height) {

	rect->tex_id = tex_id;

	rect->x0 = ((float) glDriver.width - (float) height) / 2;
	rect->x1 =  (float) glDriver.width - (((float) glDriver.width  - (float) width) / 2);
	rect->y0 = ((float) glDriver.height - (float) width) / 2;
	rect->y1 =  (float) glDriver.height - (((float) glDriver.height - (float) height) / 2);

	rect->pc[0][0] = rect->x0; rect->pc[0][1] = rect->y0;
	rect->pc[1][0] = rect->x1; rect->pc[1][1] = rect->y0;
	rect->pc[2][0] = rect->x1; rect->pc[2][1] = rect->y1;
	rect->pc[3][0] = rect->x0; rect->pc[3][1] = rect->y1;

	rect->tc[0][0] = 0; rect->tc[0][1] = 0;
	rect->tc[1][0] = 1; rect->tc[1][1] = 0;
	rect->tc[2][0] = 1; rect->tc[2][1] = 0;
	rect->tc[3][0] = 0; rect->tc[3][1] = 0;
}

// ******************************************************************

void rect2d_render (rect2d_t *rect) {

	int i;

	tex_bind (rect->tex_id);

	glBegin (GL_QUADS);
	for (i=0; i<4; i++) {
		glTexCoord2fv (rect->tc[i]);
		glVertex2fv (rect->pc[i]);
	}
	fprintf(stderr,"\n");

	glEnd ();
}
