#ifndef __RECT2D_H
#define __RECT2D_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

typedef struct {
	int tex_id;
	float x0,y0,x1,y1;
	float pc[4][2];
	float tc[4][2];
} rect2d_t;

// ******************************************************************

void rect2d_init (rect2d_t *rect, int tex_id, int QUADwidth, int QUADheight);
void rect2d_render (rect2d_t *rect);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
