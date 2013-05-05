/*
	skybox.c: skybox render library
*/

#include "../main.h"

// ******************************************************************

void add_vertex (skybox_t *skybox, int i, float x, float y, float z) {

	skybox->points[i].x = x;
	skybox->points[i].y = y;
	skybox->points[i].z = z;
}

// ******************************************************************

void add_quad (skybox_t *skybox, int i, int vtx0, int vtx1, int vtx2, int vtx3, int t,
				float u0, float v0, float u1, float v1, float u2, float v2, float u3, float v3) {

	skybox->quads[i].point[0] = vtx0;
	skybox->quads[i].u[0] = u0;
	skybox->quads[i].v[0] = v0;
	skybox->quads[i].point[1] = vtx1;
	skybox->quads[i].u[1] = u1;
	skybox->quads[i].v[1] = v1;
	skybox->quads[i].point[2] = vtx2;
	skybox->quads[i].u[2] = u2;
	skybox->quads[i].v[2] = v2;
	skybox->quads[i].point[3] = vtx3;
	skybox->quads[i].u[3] = u3;
	skybox->quads[i].v[3] = v3;
	skybox->quads[i].skin = t;
}

// ******************************************************************

void skybox_geometry (skybox_t *skybox) {

	// #point + 3 float coordinates
	add_vertex (skybox, 0, +10, -10, +10);
	add_vertex (skybox, 1, +10, -10, -10);
	add_vertex (skybox, 2, -10, -10, -10);
	add_vertex (skybox, 3, -10, -10, +10);
	add_vertex (skybox, 4, +10, +10, +10);
	add_vertex (skybox, 5, +10, +10, -10);
	add_vertex (skybox, 6, -10, +10, -10);
	add_vertex (skybox, 7, -10, +10, +10);

	// #quad + 4 point indexes + skin + 4 texture coordinates
	add_quad (skybox, 0, 2, 1, 0, 3, SKYB_FRONT, 0.99f, 0.99f, 0.01f, 0.99f, 0.01f, 0.01f, 0.99f, 0.01f);
	add_quad (skybox, 1, 5, 6, 7, 4, SKYB_REAR,  0.99f, 0.99f, 0.01f, 0.99f, 0.01f, 0.01f, 0.99f, 0.01f);
	add_quad (skybox, 2, 6, 2, 3, 7, SKYB_LEFT,  0.99f, 0.99f, 0.01f, 0.99f, 0.01f, 0.01f, 0.99f, 0.01f);
	add_quad (skybox, 3, 1, 5, 4, 0, SKYB_RIGHT, 0.99f, 0.99f, 0.01f, 0.99f, 0.01f, 0.01f, 0.99f, 0.01f);
	add_quad (skybox, 4, 0, 4, 7, 3, SKYB_UP,    0.99f, 0.99f, 0.01f, 0.99f, 0.01f, 0.01f, 0.99f, 0.01f);
	add_quad (skybox, 5, 2, 6, 5, 1, SKYB_DOWN,  0.99f, 0.99f, 0.01f, 0.99f, 0.01f, 0.01f, 0.99f, 0.01f);
}

// ******************************************************************

/*
 *	Skybox construction
 *
 *	Formats:
 *	1 - pcx
 *  2 - tga
 *  3 - jpg
 *
 */

skybox_t *skybox_make (char *tex_name, char format, int multiple_tex) {

	char buf[64], buf2[64], ext[5];
	// char tex[13] = "ftbklfrtupdn"; // Quake criteria
	char tex [13] = "rtlfbkftupdn"; // Spontz criteria
	skybox_t *skybox;
	int i, t;

	if (!tex_name) return NULL;

	skybox = (skybox_t *) malloc(sizeof(skybox_t));
	if (!skybox) return NULL;
	skybox_geometry (skybox);
	skybox->multiple_tex = multiple_tex;

	// single repeated texture skybox
	if (!skybox->multiple_tex) {

		t = tex_load (tex_name, USE_CACHE, 0);
		if (t < 0) return NULL;

		tex_properties (t, NO_MIPMAP | CLAMP);
		for (i = 0; i < 6; i++) {
			skybox->quads[i].skin = t;
		}
	}

	// multiple texture skybox
	else {
		switch (format) {
			case 1: strcpy (ext, ".pcx"); break;
			case 2: strcpy (ext, ".tga"); break;
			case 3: strcpy (ext, ".jpg"); break;
		}

		for (i = 0; i < 6; i++) {
			sprintf (buf2, "%c%c%s", tex [i*2], tex [i*2+1], ext);
			strcpy (buf, tex_name);
			strcat (buf, buf2);

			t = tex_load (buf, USE_CACHE, 0);
			if (t < 0) return NULL;

			tex_properties (t, NO_MIPMAP | CLAMP);
			skybox->quads[i].skin = t;
		}
	}

	return skybox;
}

// ******************************************************************

void skybox_upload (skybox_t *skybox) {

	int i;

	if (!skybox->multiple_tex) {
		tex_properties (skybox->quads[0].skin, NO_MIPMAP | CLAMP_TO_EDGE);
		tex_upload (skybox->quads[0].skin, USE_CACHE);

	} else {
		for (i = 0; i < 6; i++)
			tex_properties (skybox->quads[i].skin, NO_MIPMAP | CLAMP_TO_EDGE);
			tex_upload (skybox->quads[i].skin, USE_CACHE);
	}
}

// ******************************************************************

void skybox_render (skybox_t *skybox, camera_t *cam, int texBuffer) {

	int i, j, point;
	skybox_quad_t *quad;

	glDepthMask(GL_FALSE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(cam->eye.x,cam->eye.y,cam->eye.z);
	glRotatef(270,1,0,0);

	if (!skybox->multiple_tex) {
		tex_bind (skybox->quads[0].skin);
	}

	for (i = 0; i < 6; i++) {
		quad = &skybox->quads[i];
		
		if (texBuffer >= 0) {
			// There is a valid passed buffer, so let's use it instead of the texture
			tex_bind(demoSystem.texRenderingBuffer[texBuffer]);
		} else if (skybox->multiple_tex) {
			// No valid buffer passed, o let's use the textures
			tex_bind(quad->skin);
		}
		
		glBegin(GL_QUADS);
		for (j = 0; j < 4; j++) {
			point = quad->point[j];
			
			if (texBuffer >= 0) {
				// We are using a screen buffer, so use corected texture coordinates
				glTexCoord2f(quad->u[j] / gldrv_get_aspect_ratio(), quad->v[j] / gldrv_get_aspect_ratio());
			} else {
				// We are using a static texture, so use coordinates from 0 to 1
				glTexCoord2f(quad->u[j], quad->v[j]);
			}
				
			glVertex3fv(&skybox->points[point].x);
		}
		glEnd();
	}

	glPopMatrix();
	glDepthMask(GL_TRUE);
}

// ******************************************************************

void skybox_free (skybox_t *skybox) {

	int i;

	if (!skybox->multiple_tex) {
		tex_free (skybox->quads[0].skin);

	} else {
		for (i = 0; i < 6; i++)
			tex_free (skybox->quads[i].skin);
	}
	free (skybox);
}
