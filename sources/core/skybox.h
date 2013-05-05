#ifndef __SKYBOX_H
#define __SKYBOX_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#include "math3d.h"
#include "camera.h"

// ******************************************************************

// quad type
typedef struct
{
	int point[4];
	float u[4], v[4];
	int skin;
} skybox_quad_t;

// skybox type
typedef struct
{
	vector_t points[8];
	skybox_quad_t quads[6];
	int multiple_tex;
} skybox_t;

// ******************************************************************

// skybox textures
#define SKYB_FRONT	0
#define SKYB_REAR	1
#define SKYB_LEFT	2
#define SKYB_RIGHT	3
#define SKYB_UP		4
#define SKYB_DOWN	5

// ******************************************************************

skybox_t *skybox_make (char *tex_name, char bits, int multiple_tex);
void skybox_upload (skybox_t *skybox);
void skybox_render (skybox_t *skybox, camera_t *cam, int texBuffer);
void skybox_free (skybox_t *skybox);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
