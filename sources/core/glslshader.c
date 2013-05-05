/*
	glslshader.c: GLSL shader management library
*/

#include "../main.h"
#include "files.h"

// ******************************************************************

#define MAX_SHADERS 256

glslshader_t *glslshad_array[MAX_SHADERS];

// shaders counter
static int glslshad_count = 0;

// ******************************************************************

// actual bind and env mode
static int glslshad_current = -1;

// ******************************************************************

int glslshad_init () {

	if (glslshad_count >= MAX_SHADERS) dkernel_error ("glslshad_init: Too many GLSL shaders (limited to %d)", MAX_SHADERS);

	// create shader
	glslshad_array[glslshad_count] = (glslshader_t *) malloc(sizeof(glslshader_t));
	memset(glslshad_array[glslshad_count], 0, sizeof(glslshader_t));

	// glslshader is not uploaded
	glslshad_array[glslshad_count]->id_fragment = -1;
	glslshad_array[glslshad_count]->id_vertex = -1;
	glslshad_array[glslshad_count]->id_program = -1;

	return glslshad_count++;
}

// ******************************************************************

int glslshad_new () {
	
	int glslshad = glslshad_init ();
	return glslshad;
}

// ******************************************************************

void glslshad_upload (int index) {
	
	GLint v_compiled, f_compiled;
	GLint p_linked;
	GLchar* compiler_log;
	glslshader_t *glslshad = glslshad_array[index];
	
	// Create the shader objects
	glslshad->id_vertex = glCreateShader(GL_VERTEX_SHADER);
	glslshad->id_fragment = glCreateShader(GL_FRAGMENT_SHADER);
	// Specify the sources of the shaders
	glShaderSource(glslshad->id_vertex, 1, (const GLchar**)&glslshad->data_v, (const GLint*)&glslshad->size_v);
	glShaderSource(glslshad->id_fragment, 1, (const GLchar**)&glslshad->data_f, (const GLint*)&glslshad->size_f);
	// Compile the shaders
	glCompileShader(glslshad->id_vertex);
	glCompileShader(glslshad->id_fragment);
	
	// Check if shaders have been compiled OK
	glGetShaderiv(glslshad->id_vertex, GL_COMPILE_STATUS, &v_compiled);
	glGetShaderiv(glslshad->id_fragment, GL_COMPILE_STATUS, &f_compiled);

	if (!v_compiled || !f_compiled)
	{
		GLint v_blen = 0;
		GLint f_blen = 0;
		GLsizei v_slen = 0;
		GLsizei f_slen = 0;
		glGetShaderiv(glslshad->id_vertex, GL_INFO_LOG_LENGTH , &v_blen);
		glGetShaderiv(glslshad->id_fragment, GL_INFO_LOG_LENGTH , &f_blen);
		if (v_blen > 1)
		{
			compiler_log = (GLchar*)malloc(v_blen);
			glGetShaderInfoLog(glslshad->id_vertex, v_blen, &v_slen, compiler_log);
			//glGetInfoLogARB(glslshad->id_vertex, v_blen, &v_slen, compiler_log);
			dkernel_error("glslshad_upload: Error compiling Vertex Shader '%s': %s", glslshad->name_v, compiler_log);
			free (compiler_log);
		}
		if (f_blen > 1)
		{
			compiler_log = (GLchar*)malloc(f_blen);
			glGetShaderInfoLog(glslshad->id_fragment, f_blen, &f_slen, compiler_log);
			dkernel_error("glslshad_upload: Error compiling Fragment Shader '%s': %s", glslshad->name_f, compiler_log);
			free (compiler_log);
		}
		return;
	}
	
	// Link the shaders into one program

	// Now we link vertex and fragment shaders into one single SHADER PROGRAM
	glslshad->id_program = glCreateProgram();
	glAttachShader(glslshad->id_program, glslshad->id_vertex);
	glAttachShader(glslshad->id_program, glslshad->id_fragment);
	glLinkProgram(glslshad->id_program);
	
	// Check it all has been linked OK
	glGetProgramiv(glslshad->id_program, GL_LINK_STATUS, &p_linked);
	if (!p_linked)
	{
		GLint p_blen = 0;
		GLsizei p_slen = 0;
		glGetShaderiv(glslshad->id_program, GL_INFO_LOG_LENGTH , &p_blen);
		if (p_blen > 1)
		{
			compiler_log = (GLchar*)malloc(p_blen);
			glGetShaderInfoLog(glslshad->id_program, p_blen, &p_slen, compiler_log);
			dkernel_error("GLSL: Error Linking Shaders [v:'%s'] [f: '%s'] : %s", glslshad->name_v, glslshad->name_f, compiler_log);
			free (compiler_log);
		}
		return;
	}
	
	// The bind shader has changed
	glslshad_reset_bind();
}

// ******************************************************************

void glslshad_reset_bind () {

	glslshad_current = -1;
}

// ******************************************************************

void glslshad_bind (int index) {

	glslshader_t *glslshad = glslshad_array[index];
	
	if (glslshad->id_program != glslshad_current) {
		glUseProgram(glslshad->id_program);
		glslshad_current = glslshad->id_program;
	}
}

// ******************************************************************

int glslshad_getCurrent () {
	return glslshad_current;
}

// *************************************************************

GLint glslshad_getUniformLocation(int program_index, const char *varname)
	{
	GLint val = -1;
	glslshader_t *glslshad = glslshad_array[program_index];
	
	if (glslshad)
		{
		if (glslshad->id_program != -1)
			{
			val = glGetUniformLocation(glslshad->id_program, varname);	// Get the location of the var in the shader (name is not used anymore)
			if (val == -1)
				dkernel_warn("GLSL: Uniform variable '%s' not found in vs '%s' or in ps '%s'", varname, glslshad->name_v, glslshad->name_f);
			}
		}
	return val;
	}

// *************************************************************

int glslshad_load (char *fname_vert, char *fname_frag) {
	char* vertex;
	long size_v;
	char* fragment;
	long size_f;

	glslshader_t *glslshad;
	int glslshad_number, i;

	// shader is already loaded? check only if the demo is loading
	// otherwise the shader will be sent by the network and, probably,
	// it will be a different shader
	if (demoSystem.state == DEMO_LOADING) {
		for (i=0; i<glslshad_count; i++) {
			if (glslshad_array[i]) {
				if ((glslshad_array[i]->name_v) && (glslshad_array[i]->name_f)) {
					if ((spz_strcmpi(glslshad_array[i]->name_v, fname_vert) == 0) &&
						(spz_strcmpi(glslshad_array[i]->name_f, fname_frag) == 0) ) {
						return i;
					}
				}
			}
		}
	}
	
	// Load shaders
	vertex = loadFile (fname_vert);	// Load vertex shader
	size_v = sizeFile(fname_vert);
	fragment = loadFile (fname_frag);	// Load fragment shader
	size_f = sizeFile(fname_frag);
	
	// If shaders not succesfully loaded, exit!
	if (!vertex || !fragment)
		return -1;
	
	// Init shader
	glslshad_number = glslshad_init();
	glslshad = glslshad_array[glslshad_number];

	// Upload the text shaders to memory
	glslshad->name_v = strdup (fname_vert);
	glslshad->name_f = strdup (fname_frag);
	glslshad->data_v = vertex;
	glslshad->data_f = fragment;
	glslshad->size_v = size_v;
	glslshad->size_f = size_f;
	
	// TODO: Verificar si no hi han memory leaks al deixar "vertex" i "fragment" sense alliberar... crec que no s'han d'alliberar perque els he redireccionat a glslshad->data_v/f
	return glslshad_number;
}

// ******************************************************************

void glslshad_free (int Index)
	{
	glslshader_t *glslshad = glslshad_array[Index];

	if (glslshad)
		{
		if (glslshad->id_program != -1)
			{
			glDetachShader (glslshad->id_program, glslshad->id_vertex);
			glDetachShader (glslshad->id_program, glslshad->id_fragment);
			glDeleteShader (glslshad->id_vertex);
			glDeleteShader (glslshad->id_fragment);
			glDeleteProgram(glslshad->id_program);
			}
		free(glslshad->data_v);
		free(glslshad->name_v);
		free(glslshad->data_f);
		free(glslshad->name_f);
		free(glslshad);
		glslshad_array[Index] = NULL;
		}
	}
