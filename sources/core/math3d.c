/*
	math3d.c: util mathematic functions
*/

#include "../main.h"


void matrix_mul (matrix_t a, matrix_t b, matrix_t out)
{
	out[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
	out[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
	out[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];
	out[0][3] = 0;

	out[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
	out[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
	out[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];
	out[1][3] = 0;

	out[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];
	out[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];
	out[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];
	out[2][3] = 0;
	
	out[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + b[3][0];
	out[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + b[3][1];
	out[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + b[3][2];
	out[3][3] = 1;
/*
	int i, j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			out[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] +
						a[i][2] * b[2][j] + a[i][3] * b[3][j];
*/
}


void matrix_cpy (matrix_t a, matrix_t b)
{
	memcpy (a, b, sizeof (matrix_t));
}


void matrix_id (matrix_t m)
{
	memset (m, 0, sizeof (matrix_t));
	m[0][0] = 1;
	m[1][1] = 1;
	m[2][2] = 1;
	m[3][3] = 1;
}


void matrix_scale (matrix_t m, float sx, float sy, float sz)
{
	m[0][0] *= sx;
	m[0][1] *= sx;
	m[0][2] *= sx;

	m[1][0] *= sy;
	m[1][1] *= sy;
	m[1][2] *= sy;

	m[2][0] *= sz;
	m[2][1] *= sz;
	m[2][2] *= sz;
}


void matrix_xrotation (matrix_t m, float angle)
{
	float s, c;
	matrix_t mat, tmp;

	s = (float) sin (angle*DTR);
	c = (float) cos (angle*DTR);

	matrix_id (mat);
	mat[1][1] = c;
	mat[1][2] = -s;
	mat[2][1] = s;
	mat[2][2] = c;
	memcpy (tmp, m, sizeof (matrix_t));
	matrix_mul (tmp, mat, m);
}


void matrix_yrotation (matrix_t m, float angle)
{
	float s, c;
	matrix_t mat, tmp;

	s = (float) sin (angle*DTR);
	c = (float) cos (angle*DTR);

	matrix_id (mat);
	mat[0][0] = c;
	mat[0][2] = s;
	mat[2][0] = -s;
	mat[2][2] = c;
	memcpy (tmp, m, sizeof (matrix_t));
	matrix_mul (tmp, mat, m);
}


void matrix_zrotation (matrix_t m, float angle)
{
	float s, c;
	matrix_t mat, tmp;

	s = (float) sin (angle*DTR);
	c = (float) cos (angle*DTR);

	matrix_id (mat);
	mat[0][0] = c;
	mat[0][1] = -s;
	mat[1][0] = s;
	mat[1][1] = c;
	memcpy (tmp, m, sizeof (matrix_t));
	matrix_mul (tmp, mat, m);
}


void matrix_rotation_matrix (matrix_t mat, float xa, float ya, float za)
{
	float sinx, cosx, siny, cosy, sinz, cosz;

	sinx = (float) sin (xa*DTR); cosx = (float) cos (xa*DTR);
	siny = (float) sin (ya*DTR); cosy = (float) cos (ya*DTR);
	sinz = (float) sin (za*DTR); cosz = (float) cos (za*DTR);

	mat[0][0] = cosy*cosz;
	mat[0][1] = cosy*sinz;
	mat[0][2] = -1*siny;
	mat[0][3] = 0;
	mat[1][0] = sinx*siny;
	mat[1][1] = mat[1][0]*sinz+cosx*cosz;
	mat[1][0] = mat[1][0]*cosz-cosx*sinz;
	mat[1][2] = sinx*cosy;
	mat[1][3] = 0;
	mat[2][0] = cosx*siny;
	mat[2][1] = mat[2][0]*sinz-sinx*cosz;
	mat[2][0] = mat[2][0]*cosz+sinx*sinz;
	mat[2][2] = cosx*cosy;
	mat[2][3] = 0; //-V525

	mat[3][0] = 0;  mat[3][1] = 0;  mat[3][2] = 0;  mat[3][3] = 1;
}


void matrix_rotation (matrix_t m, float xa, float ya, float za)
{
	matrix_t mat, tmp;

	matrix_rotation_matrix (mat, xa, ya, za);
	memcpy (tmp, m, sizeof (matrix_t));
	matrix_mul (tmp, mat, m);
}


void matrix_translate (matrix_t m, float tx, float ty, float tz)
{
	m[3][0] += tx;
	m[3][1] += ty;
	m[3][2] += tz;
}


void matrix_transpose (matrix_t mt, matrix_t m)
{
	int i, j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			mt[i][j] = m[j][i];
}


float matrix_cofactor (matrix_t m, int i, int j)
{
	int i0 = 0, i1 = 0, i2 = 0, j0 = 0, j1 = 0, j2 = 0;
	float det;

	switch (i)
	{
		case 0: i0 = 1; i1 = 2; i2 = 3; break; //-V525
		case 1: i0 = 0; i1 = 2; i2 = 3; break;
		case 2: i0 = 0; i1 = 1; i2 = 3; break;
		case 3: i0 = 0; i1 = 1; i2 = 2; break;
	}

	switch (j)
	{
		case 0: j0 = 1; j1 = 2; j2 = 3; break; //-V525
		case 1: j0 = 0; j1 = 2; j2 = 3; break;
		case 2: j0 = 0; j1 = 1; j2 = 3; break;
		case 3: j0 = 0; j1 = 1; j2 = 2; break;
	}

	det = m[i0][j0] * (m[i1][j1] * m[i2][j2] - m[i1][j2] * m[i2][j1]) -
			m[i0][j1] * (m[i1][j0] * m[i2][j2] - m[i1][j2] * m[i2][j0]) +
			m[i0][j2] * (m[i1][j0] * m[i2][j1] - m[i1][j1] * m[i2][j0]);

	return ((i + j) & 1) ? -det : det; // if the entry index is odd, the cofactor is inverted
}


float matrix_determinant (matrix_t m)
{
	return m[0][0] * matrix_cofactor (m, 0, 0) +
			m[0][1] * matrix_cofactor (m, 0, 1) +
			m[0][2] * matrix_cofactor (m, 0, 2) +
			m[0][3] * matrix_cofactor (m, 0, 3);
}


void matrix_inverse (matrix_t mi, matrix_t m)
{
	short i, j;
	float det;
	
	det = 1.0f / matrix_determinant (m);
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			mi[i][j] = matrix_cofactor (m, j, i) * det;
}


float vector_magnitude (vector_t *v)
{
	return (float) sqrt((v->x*v->x) + (v->y*v->y) + (v->z*v->z));
}


void vector_normalize (vector_t *v)
{
	float m = 1.f / vector_magnitude (v);

	v->x *= m;
	v->y *= m;
	v->z *= m;
}


float vector_dotproduct (vector_t *a, vector_t *b)
{
	return (a->x*b->x + a->y*b->y + a->z*b->z);
}


void vector_crossproduct (vector_t *a, vector_t *b, vector_t *c)
{
	c->x = (a->y*b->z) - (a->z*b->y);
	c->y = (a->z*b->x) - (a->x*b->z);
	c->z = (a->x*b->y) - (a->y*b->x);
}


void vector_add (vector_t *a, vector_t *b, vector_t *r)
{
	r->x = b->x + a->x;
	r->y = b->y + a->y;
	r->z = b->z + a->z;
}


void vector_sub (vector_t *a, vector_t *b, vector_t *r)
{
	r->x = b->x - a->x;
	r->y = b->y - a->y;
	r->z = b->z - a->z;
}


void vector_scale (vector_t *a, float scale, vector_t *r)
{
	r->x = a->x * scale;
	r->y = a->y * scale;
	r->z = a->z * scale;
}


void vector_div (vector_t *a, float scale, vector_t *r)
{
	float div = 1.f / scale;

	r->x = a->x * div;
	r->y = a->y * div;
	r->z = a->z * div;
}


float vector_angle (vector_t *a, vector_t *b)
{
	float dot, am, bm;

	dot = vector_dotproduct (a, b);
	am = vector_magnitude (a);
	bm = vector_magnitude (b);
	return (float) acos (dot / (am * bm));
}


void vector_transform (vector_t *v, matrix_t m)
{

#ifdef WIN32

	//	about 30% +speed asm version
	__asm {
		mov		eax, v
		mov		edx, m

		fld		[eax]				// x
		fmul	dword ptr[edx]		// x*m[0][0]
		fld		[eax]				// x | x*m[0][0]
		fmul	dword ptr[edx+4]	// x*m[0][1] x*m[0][0]
		fld		[eax]				// x | x*m[0][1] | x*m[0][0]
		fmul	dword ptr[edx+8]	// x*m[0][2] | x*m[0][1] | x*m[0][0]

		fld		[eax+4]				// y | ...
		fmul	dword ptr[edx+16]	// y*m[1][0] | ...
		fld		[eax+4]				// y | y*m[1][0] | ...
		fmul	dword ptr[edx+4+16]	// y*m[1][1] | y*m[1][0] | ...
		fld		[eax+4]				// y | y*m[1][1] | y*m[1][0] | ...
		fmul	dword ptr[edx+8+16]	// y*m[1][2] | y*m[1][1] | y*m[1][0] | ...

		fxch	st(2)				// y*m[1][0] | y*m[1][1] | y*m[1][2] | ...
		faddp	st(5), st(0)		// y*m[1][1] | y*m[1][2] | x*m[0][2] | x*m[0][1] | xacc
		faddp	st(3), st(0)		// y*m[1][2] | x*m[0][2] | yacc | xacc
		faddp	st(1), st(0)		// zacc | yacc | xacc

		fld		[eax+8]				// z | ...
		fmul	dword ptr[edx+32]	// z*m[2][0] | ...
		fld		[eax+8]				// z | z*m[2][0] | ...
		fmul	dword ptr[edx+4+32]	// z*m[2][1] | z*m[2][0] | ...
		fld		[eax+8]				// z | z*m[2][1] | z*m[2][0] | ...
		fmul	dword ptr[edx+8+32]	// z*m[2][2] | z*m[2][1] | z*m[2][0] | ...

		fxch	st(2)				// z*m[2][0] | z*m[2][1] | z*m[2][2] | ...
		faddp	st(5), st(0)		// z*m[2][1] | z*m[2][2] | zacc | yacc | xacc
		faddp	st(3), st(0)		// z*m[2][2] | zacc | yacc | xacc
		faddp	st(1), st(0)		// zacc | yacc | xacc

		fld		[edx+48]			// m[3][0] | zacc | yacc | xacc
		fld		[edx+4+48]			// m[3][1] | m[3][0] | zacc | yacc | xacc
		fld		[edx+8+48]			// m[3][2] | m[3][1] | m[3][0] | zacc | yacc | xacc

		fxch	st(2)				// z*m[2][0] | z*m[2][1] | z*m[2][2] | ...
		faddp	st(5), st(0)		// z*m[2][1] | z*m[2][2] | zacc | yacc | xacc
		faddp	st(3), st(0)		// z*m[2][2] | zacc | yacc | xacc
		faddp	st(1), st(0)		// zacc | yacc | xacc

		fstp	[eax+8]				// v->z
		fstp	[eax+4]				// v->y
		fstp	[eax]				// v->x
	}

#else

	float x, y, z;

	x = v->x;
	y = v->y;
	z = v->z;

	v->x = x * m[0][0] + y * m[1][0] + z * m[2][0] + m[3][0];
	v->y = x * m[0][1] + y * m[1][1] + z * m[2][1] + m[3][1];
	v->z = x * m[0][2] + y * m[1][2] + z * m[2][2] + m[3][2];

#endif
}

float smoothstep( float a, float b, float x )
{
   if( x<a ) return 0.0f;
   if( x>b ) return 1.0f;
   x = (x-a)/(b-a);
   return x*x*(3.0f-2.0f*x);
}
