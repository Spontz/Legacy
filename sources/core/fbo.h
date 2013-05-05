#ifndef __FBO_H
#define __FBO_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

// texture definition
typedef struct {

	GLuint id_fbo;					// Our handle to the FBO
	GLuint id_depthBuffer;			// Our handle to the depth render buffer
	GLuint id_tex;					// Our handle to a texture

	int width, height;
	int iformat, format, ttype;

	int target;
	int properties;
	int mipmap;
	int wrap;
	int texfunc;

} fbo_t;

// ******************************************************************

// externalization of texture array
//extern fbo_t *fbo_array[];

// externalization of texture detail variable
extern int fbo_detail;

// ******************************************************************

int fbo_init ();
int fbo_new (int width, int height, int iformat, int format, int ttype);
void fbo_properties (int index, int flags);
void fbo_persistent (int index, int value);

void fbo_upload (int index, int usecache);
void fbo_reset_bind ();
void fbo_bind (int index);
void fbo_bind_tex (int index);
void fbo_reset_bind_tex ();
void fbo_unbind ();

// ******************************************************************

void fbo_free (int index);
int fbo_get_used_memory ();

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
