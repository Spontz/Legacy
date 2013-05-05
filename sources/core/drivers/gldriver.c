/*
	gldriver.c: SDL OpenGL init and setup
*/

#include <math.h>
#include "gldriver.h"

#include "../../main.h"
#include "../dkernel.h"
#include "../texture.h"
#include "../glslshader.h"
#include "../tga.h"

// ******************************************************************

tGlDriver glDriver;

// ******************************************************************

#ifdef WIN32
/*
// WGL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD2FVARBPROC	glMultiTexCoord2fvARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD3FVARBPROC	glMultiTexCoord3fvARB = NULL;

// GL_ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = NULL;

// GL_EXT_compiled_vertex_array
PFNGLLOCKARRAYSEXTPROC		glLockArraysEXT = NULL;
PFNGLUNLOCKARRAYSEXTPROC	glUnlockArraysEXT = NULL;

// GL_EXT_blend_subtract
PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT = NULL;

// WGL_EXT_extensions_string
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL;

// WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC		wglCreatePbufferARB = NULL;
PFNWGLGETPBUFFERDCARBPROC		wglGetPbufferDCARB = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC	wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC		wglDestroyPbufferARB = NULL;
PFNWGLQUERYPBUFFERARBPROC		wglQueryPbufferARB = NULL;

// WGL_ARB_make_current_read
PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB = NULL;

// WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC	wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC	wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB = NULL;

// WGL_ARB_render_texture
PFNWGLBINDTEXIMAGEARBPROC		wglBindTexImageARB = NULL;
PFNWGLRELEASETEXIMAGEARBPROC	wglReleaseTexImageARB = NULL;
PFNWGLSETPBUFFERATTRIBARBPROC	wglSetPbufferAttribARB = NULL;

// GLSL SHADER
// PFNWGLUSEPROGRAMOBJECTARBPROC		wglUseProgramObjectARB = NULL;
*/
#endif

	
/* *************************************************************
 create gl driver and store default settings
**************************************************************** */

void gldrv_create()
	{
	int i;

	glDriver.fullScreen = 0;
	glDriver.AspectRatio = 16.0f / 10.0f;
	glDriver.saveInfo   = 0;

	glDriver.width   = 640;
	glDriver.height  = 480;
	glDriver.bpp     = 32;
	glDriver.zbuffer = 16;
	glDriver.stencil = 0;
	glDriver.accum   = 0;

	glDriver.multisampling = 0;

	glDriver.gamma = 1.0f;

	for (i=0; i<FBO_BUFFERS; i++)
		glDriver.fbo[i].width=glDriver.fbo[i].height=0;
	}

/* *************************************************************
 check if an extension is available
**************************************************************** */

int gldrv_checkEXT(const char *ext) {

	const char *s = NULL;

	s = (char*) glGetString(GL_EXTENSIONS);
	if (strstr(s, ext)) return TRUE;

#ifdef WIN32
	if (glDriver.ext.ext_extensions_string) {
		s = wglGetExtensionsStringARB(wglGetCurrentDC());
		if (strstr(s, ext)) return TRUE;
	}
#endif

	return FALSE;
}


/* *************************************************************
 get extensions procedures
**************************************************************** */

void gldrv_initEXT()
	{
	// reset the extension
	memset(&glDriver.ext, 0, sizeof(tGlExtensions));

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint *)&glDriver.ext.max_tex_size);
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, (GLint *)&glDriver.ext.max_tus);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, (GLint *)&glDriver.ext.max_color_attachments);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint *)&glDriver.ext.max_draw_buffers);
	
	// multitexture
	if (gldrv_checkEXT("GL_ARB_multitexture")) {
		glDriver.ext.multitexture = TRUE;

	/*
	#ifdef WIN32
			glActiveTextureARB       = (PFNGLACTIVETEXTUREARBPROC)       wglGetProcAddress("glActiveTextureARB");
			glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
			glMultiTexCoord2fARB     = (PFNGLMULTITEXCOORD2FARBPROC)     wglGetProcAddress("glMultiTexCoord2fARB");
			glMultiTexCoord2fvARB    = (PFNGLMULTITEXCOORD2FVARBPROC)    wglGetProcAddress("glMultiTexCoord2fvARB");
			glMultiTexCoord3fARB     = (PFNGLMULTITEXCOORD3FARBPROC)     wglGetProcAddress("glMultiTexCoord3fARB");
			glMultiTexCoord3fvARB    = (PFNGLMULTITEXCOORD3FVARBPROC)    wglGetProcAddress("glMultiTexCoord3fvARB");
			glMultiTexCoord4fARB     = (PFNGLMULTITEXCOORD4FARBPROC)     wglGetProcAddress("glMultiTexCoord4fARB");
			glMultiTexCoord4fvARB    = (PFNGLMULTITEXCOORD4FVARBPROC)    wglGetProcAddress("glMultiTexCoord4fvARB");
			//all = glActiveTextureARB && glClientActiveTextureARB;
			//all &= glMultiTexCoord2fARB && glMultiTexCoord2fvARB && glMultiTexCoord3fARB && glMultiTexCoord3fvARB;
			if (!(glActiveTextureARB && glClientActiveTextureARB && glMultiTexCoord2fARB && glMultiTexCoord2fvARB && glMultiTexCoord3fARB && glMultiTexCoord3fvARB)) glDriver.ext.multitexture = FALSE;
	#endif
	*/
	}

	// glsl shaders support
	if (gldrv_checkEXT("GL_ARB_vertex_program") && gldrv_checkEXT("GL_ARB_vertex_shader") &&
		gldrv_checkEXT("GL_ARB_fragment_program") && gldrv_checkEXT("GL_ARB_fragment_shader")) {
		glDriver.ext.glslshaders = TRUE;
	}
	else
		glDriver.ext.glslshaders = FALSE;
	
	// multisample
	if (gldrv_checkEXT("GL_ARB_multisample")) {
		glDriver.ext.multisample = TRUE;
	}

#ifdef WIN32
	// texture_compression
	if (gldrv_checkEXT("GL_ARB_texture_compression")) {
		glDriver.ext.texture_compression = TRUE;

		glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) wglGetProcAddress("glCompressedTexImage2DARB");
		if (!glCompressedTexImage2DARB) glDriver.ext.texture_compression = FALSE;
	}
#endif

	// texture cube map
	if (gldrv_checkEXT("GL_ARB_texture_cube_map")) {
		glDriver.ext.texture_cube_map = TRUE;
	}

	// texture env add
	if (gldrv_checkEXT("GL_ARB_texture_env_add")) {
		glDriver.ext.texture_env_add = TRUE;
	}

	// texture env combine
	if (gldrv_checkEXT("GL_ARB_texture_env_combine")) {
		glDriver.ext.texture_env_combine = TRUE;
	}

	// texture env dot3
	if (gldrv_checkEXT("GL_ARB_texture_env_dot3")) {
		glDriver.ext.texture_env_dot3 = TRUE;
	}

	// anisotropic
	if (gldrv_checkEXT("GL_EXT_texture_filter_anisotropic")) {
		glDriver.ext.anisotropic = TRUE;
	}

	// vertex_array
	if (gldrv_checkEXT("GL_EXT_compiled_vertex_array")) {
		glDriver.ext.vertex_array = TRUE;

#ifdef WIN32
		glLockArraysEXT		= (PFNGLLOCKARRAYSEXTPROC)		wglGetProcAddress("glLockArraysEXT");
		glUnlockArraysEXT	= (PFNGLUNLOCKARRAYSEXTPROC)	wglGetProcAddress("glUnlockArraysEXT");
		//all = glLockArraysEXT && glUnlockArraysEXT;
		if (!(glLockArraysEXT && glUnlockArraysEXT)) glDriver.ext.vertex_array = FALSE;
#endif
	}

	// blend subtract
	if (gldrv_checkEXT("GL_EXT_blend_subtract")) {

		glDriver.ext.blend_subtract = TRUE;

	#ifdef WIN32
			glBlendEquationEXT	= (PFNGLBLENDEQUATIONEXTPROC) wglGetProcAddress("glBlendEquationEXT");
			if (!glBlendEquationEXT) glDriver.ext.blend_subtract = FALSE;
	#endif
	}

	// generate_mipmap
	if (gldrv_checkEXT("GL_SGIS_generate_mipmap")) {
		glDriver.ext.generate_mipmap = TRUE;
	}

	#ifdef WIN32
		// HACK, check with WGLEW
		// ext_extensions_string
		if (gldrv_checkEXT("WGL_EXT_extensions_string"))
			glDriver.ext.ext_extensions_string = TRUE;

		// pbuffer
		glDriver.ext.pbuffer = TRUE;
	#endif

	if (demoSystem.debug) {
		dkernel_trace("\n%s", log_separator);
		dkernel_trace("Maximum texture size: %d\n",glDriver.ext.max_tex_size);
		dkernel_trace("Number of TUs: %d\n",glDriver.ext.max_tus);
		dkernel_trace("Maximum Color Attachments (buffers): %d\n",glDriver.ext.max_color_attachments);
		dkernel_trace("Max draw buffers: %d\n",glDriver.ext.max_draw_buffers);
		dkernel_trace("\n%s", log_separator);
		dkernel_trace("GL_ARB_multitexture: %s\n",glDriver.ext.multitexture ? "yes" : "no");
		dkernel_trace("GL_ARB_vertex/program: %s\n",glDriver.ext.glslshaders ? "yes" : "no");
		dkernel_trace("GL_ARB_multisample: %s\n",glDriver.ext.multisample ? "yes" : "no");
		dkernel_trace("GL_ARB_texture_compression: %s\n",glDriver.ext.texture_compression ? "yes" : "no");
		dkernel_trace("GL_ARB_texture_cube_map: %s\n",glDriver.ext.texture_cube_map ? "yes" : "no");
		dkernel_trace("GL_ARB_texture_env_add: %s\n",glDriver.ext.texture_env_add ? "yes" : "no");
		dkernel_trace("GL_ARB_texture_env_combine: %s\n",glDriver.ext.texture_env_combine ? "yes" : "no");
		dkernel_trace("GL_ARB_texture_env_dot3: %s\n",glDriver.ext.texture_env_dot3 ? "yes" : "no");
		dkernel_trace("GL_EXT_texture_filter_anisotropic: %s\n",glDriver.ext.anisotropic ? "yes" : "no");
		dkernel_trace("GL_EXT_compiled_vertex_array: %s\n",glDriver.ext.vertex_array ? "yes" : "no");
		dkernel_trace("GL_EXT_blend_subtract: %s\n",glDriver.ext.blend_subtract ? "yes" : "no");
		dkernel_trace("GL_SGIS_generate_mipmap: %s\n",glDriver.ext.generate_mipmap ? "yes" : "no");
#ifdef WIN32
		dkernel_trace("WGL_EXT_extensions_string: %s\n",glDriver.ext.ext_extensions_string ? "yes" : "no");
		dkernel_trace("WGL_ARB_pbuffer... : %s\n",glDriver.ext.pbuffer ? "yes" : "no");
#endif
	}

	if (!glDriver.ext.multitexture || !glDriver.ext.texture_cube_map ||
		!glDriver.ext.texture_env_add || !glDriver.ext.texture_env_combine ||
		!glDriver.ext.blend_subtract || !glDriver.ext.generate_mipmap) {
		dkernel_error("Required extension not available. OpenGL 1.5 required");
	}
	
	if (!glDriver.ext.glslshaders) {
		SpzMessageBox("GLSL shader support not available", "Pixel shader 2.0 support not available.\nThe demo will run with disabled shaders, so aome scenes may appear badly.");
		
		dkernel_warn("Vertex/Pixel shader support not available, running demo with disabled shaders.");
	}
}


/* *************************************************************
 write driver strings to file
**************************************************************** */

void writeSpacedString(char *s) {

	char seps[] = " ,\t\n";
	char *token;

	if (s == NULL) return;

	token = strtok(s, seps);
	while (token != NULL) {
		dkernel_trace("   %s\n", token);
		token = strtok(NULL, seps);
	}
}

void gldrv_saveInfo() {

	const unsigned char *s = NULL;

	dkernel_trace("\n%sOpenGL driver information\n", log_separator);
	dkernel_trace("vendor: %s\n", glGetString(GL_VENDOR));
	dkernel_trace("renderer: %s\n", glGetString(GL_RENDERER));
	dkernel_trace("version: %s\n", glGetString(GL_VERSION));

	fprintf(stdout,"standard extensions:\n");
	s = glGetString(GL_EXTENSIONS);
	writeSpacedString((char *) s);

#ifdef WIN32
	dkernel_trace("WGL extensions:\n");
	if (glDriver.ext.ext_extensions_string) {
		s = wglGetExtensionsStringARB(wglGetCurrentDC());
		writeSpacedString((char *) s);
	}
#endif
}


/* *************************************************************
 init default client state
**************************************************************** */

int gl_drv_check_for_gl_errors(char* pOut)
	{
	GLenum err = glGetError();
	strcpy(pOut, (const char*)gluErrorString(err));
	if (err == GL_NO_ERROR)
		return 0;
	return 1;
	}

void gldrv_initState()
	{
	char OGLError[1024];
	const float WindowAspect = ((float)glDriver.width) / ((float)(glDriver.height));
	int x, y, width, height;

	// Check for ogl errors
	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("An OpenGL error has been produced before gldrv_initState:\n\n%s", OGLError);

	// Set default ogl states
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 0, 0, 0);
	glDisable(GL_BLEND);						// blending disabled
	glDisable(GL_ALPHA_TEST);					// alpha test disabled
	glDisable(GL_LIGHTING);						// lighting disabled
	glDisable(GL_LIGHT0);						// default light disabled too
	glDisable(GL_CULL_FACE);					// cull face disabled
	glEnable(GL_TEXTURE_2D);					// textures enabled
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// draw cwise and ccwise in fill mode
	glShadeModel(GL_FLAT);						// polygon smoothing disabled
	glEnable(GL_DEPTH_TEST);					// depth test enabled
	glDepthFunc(GL_LEQUAL);						// depth test comparison function set to LEQUAL

	// Enable multisampling
	if (glDriver.multisampling && glDriver.ext.multisample)
		glEnable(GL_MULTISAMPLE_ARB);

	// Check for ogl errors
	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OpenGL Error while setting the default state in gldrv_initState:\n\n%s", OGLError);

	// Rebuild viewport data.
		{
		// 16/9 = 1.777, 4/3 = 1.333
		if (glDriver.AspectRatio > WindowAspect)
			{
			// we need to cut tops and downs
			x = 0;
			width = glDriver.width;
			height = (int)(  ((float)glDriver.height) / glDriver.AspectRatio * WindowAspect );
			y = (glDriver.height - height) / 2;
			}
		else
			{
			// we need to cut sides
			y = 0;
			height = glDriver.height;
			width = (int)(  ((float)glDriver.width) * glDriver.AspectRatio / WindowAspect );
			x = (glDriver.width - width) / 2;
			}

		glDriver.vpXOffset = x;
		glDriver.vpYOffset = y;
		glDriver.vpWidth = width;
		glDriver.vpHeight = height;
		}
	}

unsigned int get_closest_power_of_two(unsigned int value)
	{
	unsigned int p = 1;

	while (p < value)
		p = p<<1;

	return p;
	}

void gldrv_initViewport()
	{
	int i;
	char OGLError[1024];

	// Reset viewport
	glDriver.vpWidth = glDriver.width;
	glDriver.vpHeight = glDriver.height;
	glDriver.vpYOffset = 0;
	glDriver.vpXOffset = 0;

	// init shared rtt textures
	demoSystem.rtt = tex_new(get_closest_power_of_two(glDriver.vpWidth), get_closest_power_of_two(glDriver.vpHeight), GL_RGB, 3);
	tex_properties(demoSystem.rtt, NO_MIPMAP | CLAMP_TO_EDGE);
	tex_properties(demoSystem.rtt, NO_MIPMAP);
	tex_upload(demoSystem.rtt, NO_CACHE);

	demoSystem.backup = tex_new(get_closest_power_of_two(glDriver.vpWidth), get_closest_power_of_two(glDriver.vpHeight), GL_RGB, 3);

	tex_properties(demoSystem.backup, NO_MIPMAP);
	tex_upload(demoSystem.backup, NO_CACHE);

	for (i=0; i<RENDERING_BUFFERS; i++)
		{
		demoSystem.texRenderingBuffer[i] = tex_new(get_closest_power_of_two(glDriver.vpWidth), get_closest_power_of_two(glDriver.vpHeight), GL_RGB, 3);

		while( gl_drv_check_for_gl_errors(OGLError))
			section_error("OGL Error creating texRenderingBuffer:\n\n%s", OGLError);

		tex_properties(demoSystem.texRenderingBuffer[i], MODULATE);
		tex_upload(demoSystem.texRenderingBuffer[i], NO_CACHE);
		}

	// init shared fbo's
	for (i=0;i<FBO_BUFFERS;i++)
		{
		if ((glDriver.fbo[i].width!=0) && (glDriver.fbo[i].height!=0))
			{
			glDriver.fbo[i].tex_iformat = getTextureInternalFormatByName(glDriver.fbo[i].format);
			glDriver.fbo[i].tex_format = getTextureFormatByName(glDriver.fbo[i].format);
			glDriver.fbo[i].tex_type = getTextureTypeByName(glDriver.fbo[i].format);
			// Check if the format is valid
			if (glDriver.fbo[i].tex_format > 0)
				{
				demoSystem.fboRenderingBuffer[i] = fbo_new (glDriver.fbo[i].width, glDriver.fbo[i].height,  glDriver.fbo[i].tex_iformat, glDriver.fbo[i].tex_format, glDriver.fbo[i].tex_type);
				fbo_properties (demoSystem.fboRenderingBuffer[i], MODULATE | NO_MIPMAP ); // Delete "NO_MIPMAP" to disable filtering 
				fbo_upload (demoSystem.fboRenderingBuffer[i], NO_CACHE);
				dkernel_trace("Fbo %i uploaded: width: %i, height: %i, format: %s (%i), iformat: %i, type: %i", i, glDriver.fbo[i].width, glDriver.fbo[i].height, glDriver.fbo[i].format, glDriver.fbo[i].tex_format, glDriver.fbo[i].tex_iformat, glDriver.fbo[i].tex_type);
				}
			else
				{
				dkernel_error("Error in FBO definition: FBO number %i has a non recongised format: '%s', please check 'graphics.spo' file.", i, glDriver.fbo[i].format);
				}
			}
		}
	}

/* *************************************************************
 create window and opengl context
**************************************************************** */

void gldrv_init()
	{
	// information about the current video settings
	const SDL_VideoInfo* info = NULL;
	#ifdef WIN32
		unsigned int err;
	#endif

	dkernel_trace("Initializing SVE OpenGL driver...");

	dkernel_trace("Initializing SDL...");
	// first, initialize SDL's video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0 )
		dkernel_error("Video initialization failed: %s", SDL_GetError());

	dkernel_trace("Getting video info...");
	// get some video information
	info = SDL_GetVideoInfo();
	if (!info)
		dkernel_error("Get video info failed: %s", SDL_GetError());

	dkernel_trace("Setting cursor settings...");
	if (demoSystem.debug)
		SDL_ShowCursor(TRUE);
	else
		SDL_ShowCursor(FALSE);

	dkernel_trace("Setting OGL states...");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, glDriver.zbuffer);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, glDriver.stencil);

	if (glDriver.accum)
		{
		dkernel_trace("Setting OGL acumulation buffer states...");
		SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
		}

	if (glDriver.multisampling == 1)
		{
		dkernel_trace("Setting 2x multisampling...");
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
		}
	else if (glDriver.multisampling == 2)
		{
		dkernel_trace("Setting 4x multisampling...");
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
		}
	else
		{
		glDriver.multisampling = 0;
		}

	// set window properties
	SDL_WM_SetCaption(demoSystem.demoName, NULL);

	if (!demoSystem.debug)
		{
		// Enable double buffering in order to avoid screen tearing
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
		}
	
	if (glDriver.fullScreen)
		{
		// go to fullscreen
		dkernel_trace("Going fulscreen...");
		if (SDL_SetVideoMode(glDriver.width, glDriver.height, glDriver.bpp, SDL_HWSURFACE | SDL_OPENGL | SDL_FULLSCREEN) == 0)
			dkernel_error("Set video mode failed: %s", SDL_GetError());
		}
	else
		{
		// create window
		// overwrite script bpp by desktop bpp
		glDriver.bpp = info->vfmt->BitsPerPixel;

		dkernel_trace("Going windowed...");
		if (SDL_SetVideoMode(glDriver.width, glDriver.height, glDriver.bpp, SDL_HWSURFACE | SDL_OPENGL) == 0) {
			dkernel_error("Set video mode failed: %s", SDL_GetError());
		}
	}
	
	// set gamma correction
	dkernel_trace("Setting gamma...");
	SDL_SetGamma(glDriver.gamma, glDriver.gamma, glDriver.gamma);

	// clear buffers
	if (glDriver.stencil > 0)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

	// init extensions
	dkernel_trace("Initializing extensions (HACK)...");
	gldrv_initEXT();

	// init win32 glew
	#ifdef _WIN32
		dkernel_trace("Initializing glew...");
		err = glewInit();
		if (GLEW_OK != err)
			dkernel_error("glewInit failed:\n\n%s", glewGetErrorString(err));
	#endif

	// save opengl strings
	dkernel_trace("Saving ogl info...");
	if (glDriver.saveInfo)
		gldrv_saveInfo();

	// set the default gl state
	dkernel_trace("Setting default ogl state...");
	gldrv_initState();

	// init viewport, rtt shared texture and fbo's
	dkernel_trace("Initializing viewport, rtts and fbos...");
	gldrv_initViewport();

	dkernel_trace("SVE OpenGL Driver initialized.");
	}


/* *************************************************************
 delete window and opengl context
**************************************************************** */

void gldrv_close()
	{
	// clean up SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}


/* *************************************************************
 draw screen quads
**************************************************************** */

void gldrv_screenquad()
	{
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)get_closest_power_of_two(glDriver.vpWidth);
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)get_closest_power_of_two(glDriver.vpHeight);

	glBegin (GL_QUADS);
		glVertex2f (x0-GL_EPSILON, y0-GL_EPSILON);
		glVertex2f (x1+GL_EPSILON, y0-GL_EPSILON);
		glVertex2f (x1+GL_EPSILON, y1+GL_EPSILON);
		glVertex2f (x0-GL_EPSILON, y1+GL_EPSILON);
	glEnd ();
	}

void gldrv_texscreenquad()
	{
		
		float x0, y0, x1, y1;
		
		camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);
		
		x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)get_closest_power_of_two(glDriver.vpWidth);
		y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)get_closest_power_of_two(glDriver.vpHeight);
		
		glBegin (GL_QUADS);
		glTexCoord2f (0,0);glVertex2f (x0-GL_EPSILON, y0-GL_EPSILON);
		glTexCoord2f (1,0);glVertex2f (x1+GL_EPSILON, y0-GL_EPSILON);
		glTexCoord2f (1,1);glVertex2f (x1+GL_EPSILON, y1+GL_EPSILON);
		glTexCoord2f (0,1);glVertex2f (x0-GL_EPSILON, y1+GL_EPSILON);
		glEnd ();		
	}


/* *************************************************************
 this needs to be called before render a frame
**************************************************************** */

void gldrv_initRender(int clear)
	{
	// reset the default gl state
	gldrv_initState();
	// set the viewport to the correct size
	SPZ_VIEWPORT(glDriver.vpXOffset, glDriver.vpYOffset, glDriver.vpWidth, glDriver.vpHeight);
	// clear some buffers if needed
	if (clear)
		{
		glClearColor(0, 0, 0, 0);
		if (glDriver.stencil > 0)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		else
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}


/* *************************************************************
 this needs to be called after render a frame
**************************************************************** */

void gldrv_endRender()
	{
	// error rendering the frame ?
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		dkernel_warn("Error in OpenGL: %s", gluErrorString (error));

	// set the viewport to cover all the output display to show stats in top left
	SPZ_VIEWPORT(0, 0, glDriver.width, glDriver.height);

	return;
	}


/* *************************************************************
 swap front and back buffers
**************************************************************** */

void gldrv_swap()
	{
	glFlush();
	SDL_GL_SwapBuffers();
	}

/* *************************************************************
 copies the color buffer into the current texture
**************************************************************** */

void gldrv_copyColorBuffer()
	{
	char OGLError[1024];

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, glDriver.vpXOffset, glDriver.vpYOffset, glDriver.vpWidth, glDriver.vpHeight);

	while (gl_drv_check_for_gl_errors(OGLError))
		section_error("OGL Error in gldrv_copyColorBuffer:\n\n%s", OGLError);
	}

/* *************************************************************
 multitexture begin / end procedures
**************************************************************** */

void gldrv_enable_multitexture() {

	tex_reset_bind();

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glClientActiveTextureARB(GL_TEXTURE2_ARB);
	glEnable(GL_TEXTURE_2D);
	
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glClientActiveTextureARB(GL_TEXTURE3_ARB);
	glEnable(GL_TEXTURE_2D);
}

void gldrv_disable_multitexture() {
	
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glClientActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glClientActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	
	glEnable(GL_TEXTURE_2D);
	tex_reset_bind();
}


/* *************************************************************
 screenshot and capture code
**************************************************************** */

void gldrv_screenshot() {

	char *buffer, picname[80];
	int i;
	FILE *f;

	strcpy (picname,"screen00.tga");

	for (i=0; i<=99; i++) {
		picname[6] = i/10 + '0';
		picname[7] = i%10 + '0';
		f = fopen (picname, "rb");
		if (!f) break; // file doesn't exist
		fclose (f);
	}

	if (i == 100) {
		// 100 screenshots were made
		return;
 	}

	buffer = malloc (glDriver.width * glDriver.height * 4);
	glReadPixels (0, 0, glDriver.width, glDriver.height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	tga_save (picname, buffer, glDriver.width, glDriver.height, demoSystem.compressTga);
	free (buffer);
}


void gldrv_capture()
	{
	char *buffer, picname[80];

	sprintf (picname,"video/%08u.tga", demoSystem.frameCount);

	buffer = malloc (glDriver.width * glDriver.height * 3);
	glReadPixels (0, 0, glDriver.width, glDriver.height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	tga_save (picname, buffer, glDriver.width, glDriver.height, demoSystem.compressTga);
	free (buffer);
	}

float gldrv_get_aspect_ratio()
	{
	return (float)glDriver.width / (float)glDriver.height;
	}

float gldrv_get_viewport_aspect_ratio()
	{
	return (float)glDriver.vpWidth / (float)glDriver.vpHeight;
	}
