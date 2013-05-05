#ifndef __GLSLSHADER_H
#define __GLSLSHADER_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

// glsl shader definition
typedef struct {
	GLchar *name_v, *data_v;	// Vertex program name and data
	GLint size_v;			// Size of the Vertex program
	GLchar *name_f, *data_f;	// Fragment program name and data
	GLint size_f;			// Size of the Fragment program

	GLuint id_program;	// Main Program shader ID
	GLuint id_vertex;	// Internal: Vertex shader ID
	GLuint id_fragment;	// Internal: Fragment shader ID

} glslshader_t;

// ******************************************************************

// externalization of texture array
extern glslshader_t *glslshad_array[];

// ******************************************************************

int glslshad_init ();
int glslshad_new ();

void glslshad_upload (int index);
void glslshad_reset_bind ();
void glslshad_bind (int index);

// ******************************************************************

GLint glslshad_getUniformLocation (int program_index, const char *varname);
int glslshad_load (char *fname_vert, char *fname_frag);
void glslshad_free (int index);
int glslshad_getCurrent ();
	
// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
