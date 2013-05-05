/*
	camera.c: camera library
*/

#include "../main.h"

// ******************************************************************
// camera routines
// ******************************************************************

void camera_reset(camera_t *cam) {
	// default camera values
	camera_locate(cam, 0, 0, 0);
	camera_angle(cam, 0, 0, 0);
	camera_target(cam, 0,  0,  -100);
	camera_fov_aspect(cam, 90.0f, gldrv_get_aspect_ratio());
	camera_zplanes(cam, 0.1f, 1000.0f);
	camera_roll(cam, 0);
}

// ******************************************************************

camera_t *camera_init (char type)
{
	camera_t *cam;
	cam = (camera_t *) malloc(sizeof(camera_t));
	memset (cam,0,sizeof(camera_t));
	cam->type = type;
	
	camera_reset(cam);
	
	return cam;
}


void camera_locate (camera_t *cam, float x, float y, float z)
{
	cam->eye.x = x;
	cam->eye.y = y;
	cam->eye.z = z;
}


void camera_target (camera_t *cam, float x, float y, float z)
{
	cam->target.x = x;
	cam->target.y = y;
	cam->target.z = z;
}


void camera_roll (camera_t *cam, float roll)
{
	cam->roll = roll;
}


void camera_pan (camera_t *cam, float x, float y, float z)
{
	cam->pan.x = x;
	cam->pan.y = y;
	cam->pan.z = z;
}


void camera_angle (camera_t *cam, float x, float y, float z)
{
	cam->angle.x = x;
	cam->angle.y = y;
	cam->angle.z = z;
}


void camera_fov_aspect (camera_t *cam, float fov, float aspect)
{
	cam->fov = fov;
	cam->aspectratio = aspect;
}


void camera_zplanes (camera_t *cam, float znear, float zfar)
{
	cam->znear = znear;
	cam->zfar = zfar;
}


void camera_free (camera_t *cam)
	{
	free(cam->name);
	free(cam);
	}


// ******************************************************************
// camera construction routines
// ******************************************************************

void camera_glmatrix(camera_t* cam)
	{
	float matrix[16];

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch(cam->type)
		{
		case camera_type__angles:
			{
			glTranslatef (-cam->pan.x, -cam->pan.y, -cam->pan.z);
			
			if (fabs(cam->roll) > FLT_EPSILON)
				glRotatef (-cam->roll, 0, 0, 1);

			glRotatef(-cam->angle.x, 1, 0, 0);
			glRotatef(-cam->angle.z, 0, 0, 1);
			glRotatef(-cam->angle.y, 0, 1, 0);
			
			glTranslatef(-cam->eye.x, -cam->eye.y, -cam->eye.z);

			break;
			}

		case camera_type__look_at:
			{
			if (fabs(cam->roll) > FLT_EPSILON)
				glRotatef(-cam->roll, 0, 0, 1);

			// para facilitar el control por mouse
			glRotatef(-cam->angle.x, 1, 0, 0);
			glRotatef(-cam->angle.z, 0, 0, 1);
			glRotatef(-cam->angle.y, 0, 1, 0);

			gluLookAt
				(
				cam->eye.x+cam->pan.x,		cam->eye.y+cam->pan.y,		cam->eye.z+cam->pan.z,
				cam->target.x+cam->pan.x,	cam->target.y+cam->pan.y,	cam->target.z+cam->pan.z,
				0,							1,							0
				);

			break;
			}

		default:
			{
			dkernel_error("Invalid camera type");
			break;
			}
		}

	// capturar los vectores side, up, front
		{
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

		cam->side.x = matrix[0];
		cam->side.y = matrix[4];
		cam->side.z = matrix[8];

		cam->up.x = matrix[1];
		cam->up.y = matrix[5];
		cam->up.z = matrix[9];

		cam->front.x = matrix[2];
		cam->front.y = matrix[6];
		cam->front.z = matrix[10];
		}

	// set sve variables so we can later retrieve the data in spo scripts
	set_sve_variable_matrix_4x4f(sve_variable_matrix_view, (const matrix_t*)&matrix);
	}


void camera_glprojection (camera_t *cam)
	{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(cam->fov, cam->aspectratio, cam->znear, cam->zfar);
	}


// ******************************************************************

void camera_2d_get_rect(float* pLeft, float* pRight, float* pBottom, float* pTop)
	{
	if (gldrv_get_viewport_aspect_ratio() > 1)
		{
		*pLeft = (1 - gldrv_get_viewport_aspect_ratio()) / 2;
		*pRight = (1 - gldrv_get_viewport_aspect_ratio()) / 2 + gldrv_get_viewport_aspect_ratio();
		*pBottom = 0;
		*pTop = 1;
		}
	else
		{
		*pLeft = 0;
		*pRight = 1;
		*pBottom = (1 - 1/gldrv_get_viewport_aspect_ratio()) / 2;
		*pTop = (1 - 1/gldrv_get_viewport_aspect_ratio()) / 2 + 1/gldrv_get_viewport_aspect_ratio();
		}
	}

void camera_2d_fit_to_content(float RectangleAspectRatio, float* pLeft, float* pRight, float* pBottom, float* pTop)
	{
	float a, b, c, d, e;

	camera_2d_get_rect(pLeft, pRight, pBottom, pTop);

	if (RectangleAspectRatio > gldrv_get_viewport_aspect_ratio())
		{
		a = (*pRight - *pLeft);
		b = (*pTop - *pBottom);
		c = RectangleAspectRatio * b;
		e = (c - a) / 2;

		*pLeft -= e;
		*pRight += e;
		}
	else
		{
		a = (*pRight - *pLeft);
		b = (*pTop - *pBottom);
		d = a / RectangleAspectRatio;
		e = (d - b) / 2;

		*pBottom -= e;
		*pTop += e;
		}
	}

/*

Dado un aspect ratio  (width/height) calcula el rect del viewport en funcion del modo de
video actual del motor.

Parametros de entrada:

RectangleAspectRatio	: Aspect ratio de la ventana / modo de pantalla completa

Parametros de salida:

pLeft					: Coordenada x izquierda
pRight					: Coordenada x derecha
pBottom					: Coordenada y superior
pTop					: Coordenada y inferior

*/

void camera_2d_fit_to_viewport(float RectangleAspectRatio, float* pLeft, float* pRight, float* pBottom, float* pTop)
	{
	float a, b, c, d;

	camera_2d_get_rect(pLeft, pRight, pBottom, pTop);

	if (RectangleAspectRatio < gldrv_get_viewport_aspect_ratio())
		{
		// obtener coordenadas de un rectangulo mas alto que el viewport (eliminar zonas superiores e inferiores del rectangulo)
		b = (*pRight - *pLeft);
		c = RectangleAspectRatio;
		d = (b - c) / 2;

		*pRight = *pLeft + c + d;
		*pLeft += d;
		}
	else
		{
		// obtener coordenadas de un rectangulo mas ancho que el viewport (eliminar zonas laterales del rectangulo)
		a = (*pRight - *pLeft);
		b = (*pTop - *pBottom);
		c = a / RectangleAspectRatio;
		d = (b - c) / 2;

		*pTop = *pBottom + c + d;
		*pBottom += d;
		}
	}

void camera_set2d()
	{
	// establecer proyeccifon ortografica con un cuadrado de diagonal (0,0)-(1,1)
	// de area maxima en el centro del viewport

	float l, r, b, t;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	camera_2d_get_rect(&l, &r, &b, &t);
	glOrtho(l, r, b, t, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	}

// ******************************************************************

void camera_restore () {

	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	glMatrixMode (GL_MODELVIEW);
}