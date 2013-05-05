#ifndef __RECT3D_H
#define __RECT3D_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

typedef struct {
	int tex_id;
	double x0,y0,x1,y1;
	double tc[4][2];
	double pc[4][3];
} rect3d_t;

// ******************************************************************

void rect3d_init (rect3d_t *rect, int tex_id, int width, int height);
void rect3d_render (rect3d_t *rect);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
