#ifndef __CAMERA_H
#define __CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#include "math3d.h"

// ******************************************************************
typedef enum
	{
	camera_type__angles		= 0,
	camera_type__look_at	= 1
	} enum_camera_type;

// ******************************************************************

// camera definition
typedef struct
	{
	char*				name;				// could be used for something...
	enum_camera_type	type;				// angles or lookup camera?
	vector_t			eye, target;		// eye -> target
	float				roll;				// roll (eye -> target) rotation angle

	vector_t			pan;				// panning control

	vector_t			angle;				// camera angles (only in angles camera)
	matrix_t			pm, im;				// projection & inversed projection matrixes
	vector_t			front, up, side;	// unitary vectors for angle defined camera

	float				fov;				// camera lens variables
	float				aspectratio;		// hack : no deberia estar aqui, depende del viewport exclusivamente
	float				znear, zfar;		// near and far clip planes
	} camera_t;

// ******************************************************************

// camera functions
camera_t *camera_init (char type);

// eye -> target camera
void camera_locate (camera_t *cam, float x, float y, float z);
void camera_target (camera_t *cam, float x, float y, float z);
void camera_roll (camera_t *cam, float roll);
void camera_reset(camera_t *cam);

// pan control
void camera_pan (camera_t *cam, float x, float y, float z);

// angle camera
void camera_angle (camera_t *cam, float x, float y, float z);

// camera lens
void camera_fov_aspect (camera_t *cam, float fov, float aspect);

// znear and zfar
void camera_zplanes (camera_t *cam, float znear, float zfar);

// release camera memory
void camera_free (camera_t *cam);

// make camera matrix (not used on openGL !)
void camera_matrix (camera_t *cam);

// make openGL matrixes
void camera_glmatrix (camera_t *cam);
void camera_glprojection (camera_t *cam);


// set 2d camera, and restore original 3d camera
void camera_set2d();
void camera_restore();
void camera_2d_get_rect(float* pLeft, float* pRight, float* pBottom, float* pTop);
void camera_2d_fit_to_viewport(float RectangleAspectRatio, float* pLeft, float* pRight, float* pBottom, float* pTop);
void camera_2d_fit_to_content(float RectangleAspectRatio, float* pLeft, float* pRight, float* pBottom, float* pTop);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
