#ifndef __GLDRIVER_H
#define __GLDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#define NO_SDL_GLEXT 1

#ifdef WIN32
	#pragma warning( disable : 4273 )
	
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	#include "gl/glew.h"
	#include "gl/wglew.h"
	//#include "gl/glext.h"
#endif

#include <SDL.h>
#include <SDL_endian.h>
#include <SDL_opengl.h>
#include "../../SDL_image/SDL_image.h"

#ifdef WIN32

#elif __APPLE__

	#include <OpenGL/glext.h>

#else

	#define GL_GLEXT_PROTOTYPES 1
	#include "../gl/glext.h"
	#define glBlendEquationEXT glBlendEquation

#endif

	#include "../dkernel.h"

// ******************************************************************

typedef struct {

	int max_tex_size;
	int max_tus;
	int max_color_attachments;
	int max_draw_buffers;

	// ARB extensions
	char multitexture;
	char multisample;
	char texture_compression;
	char texture_cube_map;
	char texture_env_add;
	char texture_env_combine;
	char texture_env_dot3;
	char glslshaders; // GLSL shaders support (vertex & pixel shader 2.0)

	// EXT extensions
	char anisotropic;
	char vertex_array;
	char blend_subtract;

	// SGIS extensions
	char generate_mipmap;

	// Windows only extensions
	char ext_extensions_string;
	char pbuffer;

} tGlExtensions;

// ******************************************************************

typedef struct
	{
	int width, height;
	int tex_iformat;
	int tex_format;
	int tex_type;
	char *format;
	} tGLFboFormat;
	
// ******************************************************************

typedef struct
	{
	float			AspectRatio;

	int				fullScreen;
	int				saveInfo;
	tGlExtensions	ext;

	// Current rendertarget width and height
	int				width, height;

	// Current viewport (this data depends on: width, height and AspectRatio)
	int				vpWidth, vpHeight, vpYOffset, vpXOffset;

	int				bpp;
	int				zbuffer;
	int				stencil;
	int				accum;
	int				multisampling;
	float			gamma;
	tGLFboFormat	fbo[FBO_BUFFERS];
	} tGlDriver;

float	gldrv_get_aspect_ratio();
float	gldrv_get_viewport_aspect_ratio();

extern tGlDriver glDriver;

// ******************************************************************

#define GL_EPSILON 0.005f

// ******************************************************************

void gldrv_create();
void gldrv_init();
void gldrv_swap();
void gldrv_close();

// ******************************************************************

void gldrv_screenquad();
void gldrv_texscreenquad();

// ******************************************************************

void gldrv_initRender(int clear);
void gldrv_endRender();

// ******************************************************************

void gldrv_copyColorBuffer();

// ******************************************************************

void gldrv_enable_multitexture();
void gldrv_disable_multitexture();

// ******************************************************************

void gldrv_screenshot();
void gldrv_capture();

// ******************************************************************

int gl_drv_check_for_gl_errors(char* pOut);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
