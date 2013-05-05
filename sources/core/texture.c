/*
	texture.c: texture management library
*/

#include "../main.h"

// ******************************************************************

#define MAX_TEXTURES 256

texture_t *tex_array[MAX_TEXTURES];

// texture counter
static int tex_count = 0;

// actual bind and env mode
static int tex_current = -1;
static int env_mode = -1;

// ******************************************************************

int powerOfTwo(int number);
int getCompressedFormat(texture_t *tex);

// ******************************************************************

int tex_init () {

	if (tex_count >= MAX_TEXTURES) dkernel_error ("Too many textures (limited to %d)", MAX_TEXTURES);

	// create texture
	tex_array[tex_count] = (texture_t *) malloc(sizeof(texture_t));
	memset(tex_array[tex_count], 0, sizeof(texture_t));

	// texture is not uploaded
	tex_array[tex_count]->id = -1;

	// reset texture properties
	tex_properties (tex_count, 0);

	return tex_count++;
}

// ******************************************************************

int tex_new (int width, int height, int format, int components) {

	int tex = tex_init ();

	// setup texture configuration
	// custom textures are not restricted to texture detail setting
	tex_array[tex]->data = malloc (width * height * components);
	tex_array[tex]->width = width;
	tex_array[tex]->height = height;
	tex_array[tex]->format = format;
	tex_array[tex]->iformat = components;
	tex_array[tex]->dynamic = TRUE;

	return tex;
}

// ******************************************************************

void tex_properties (int index, int flags) {

	texture_t *tex = tex_array[index];
	tex->properties = flags;

	if (flags & TEXTURE_1D) tex->target = GL_TEXTURE_1D;
	else tex->target = GL_TEXTURE_2D;

	if (flags & NO_MIPMAP) tex->mipmap = GL_LINEAR;
	else tex->mipmap = GL_LINEAR_MIPMAP_NEAREST;

	if (flags & CLAMP) tex->wrap = GL_CLAMP;
	else if (flags & CLAMP_TO_EDGE) tex->wrap = GL_CLAMP_TO_EDGE;
	else tex->wrap = GL_REPEAT;
	
	if (flags & MODULATE) {
		tex->texfunc = GL_MODULATE;
	} else {
		tex->texfunc = GL_REPLACE;
	}
}

// ******************************************************************

void tex_persistent (int index, int value) {
	tex_array[index]->persistent = value;
}

// ******************************************************************

void tex_clean (int index) {

	texture_t *tex = tex_array[index];
	memset (tex->data, 0, tex->width * tex->height * tex->iformat);
}

// ******************************************************************

void tex_upload (int index, int cache)
	{
	GLenum error;
	int curW, curH, format;
	texture_t *tex = tex_array[index];

	// texture already uploaded? check only when the engine is loading scripts from the hard disk
	if (demoSystem.state == DEMO_LOADING)
		if (cache == 1)
			if (tex->id != -1) return;

	// clear error flags
	error = glGetError ();
	if (error != GL_NO_ERROR)
		dkernel_error("tex_upload: Error before upload texture '%s': %s", tex->name, gluErrorString (error));

	// get texture size
	curW = tex->width;
	curH = tex->height;
	if (tex->mipmap != GL_LINEAR_MIPMAP_NEAREST)
		tex->memory = curW * curH * tex->iformat;
	else
		{
		tex->memory = 0;
		while ((curW != 1) || (curH != 1))
			{
			tex->memory += curW * curH * tex->iformat;
			if (curW > 1) curW /= 2;
			if (curH > 1) curH /= 2;
			}
		}

	// reserve a new texture object
	glGenTextures (1, (GLuint *)&tex->id);
	glBindTexture (tex->target, tex->id);

	// setup texture parameters
	glTexParameteri (tex->target, GL_TEXTURE_MIN_FILTER, tex->mipmap);
	glTexParameteri (tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (tex->target, GL_TEXTURE_WRAP_S, tex->wrap);
	glTexParameteri (tex->target, GL_TEXTURE_WRAP_T, tex->wrap);

	if (tex->properties & TEXTURE_1D)
		{
		// 1D textures
		glTexImage1D (GL_TEXTURE_1D, 0, tex->iformat, tex->width, 0, tex->format, GL_UNSIGNED_BYTE, tex->data);
		}
	else
		{
		// 2D textures
		if (powerOfTwo(tex->width) && powerOfTwo(tex->height))
			{
			if (tex->mipmap != GL_LINEAR_MIPMAP_NEAREST)
				{
				format = getCompressedFormat(tex);
				glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->data);
				}
			else if (glDriver.ext.generate_mipmap)
				{
				format = getCompressedFormat(tex);
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
				glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->data);
				}
			else
				{
				gluBuild2DMipmaps(GL_TEXTURE_2D, tex->iformat, tex->width, tex->height, tex->format, GL_UNSIGNED_BYTE, tex->data);
				}
			}
		else
			{
			gluBuild2DMipmaps(GL_TEXTURE_2D, tex->iformat, tex->width, tex->height, tex->format, GL_UNSIGNED_BYTE, tex->data);
			}
		}

	// error uploading the texture ?
	error = glGetError();

	if (error != GL_NO_ERROR)
		dkernel_error("tex_upload: Error uploading texture '%s': %s", tex->name, gluErrorString (error));

	if (!tex->persistent)
		{
		// free SDL allocated data
		if (tex->SDL_surface)
			{
			SDL_FreeSurface(tex->SDL_surface);
			tex->SDL_surface = NULL;
			tex->data = NULL;
			}
		else if (tex->data)
			{
			// free custom data
			free(tex->data);
			tex->data = NULL;
			}
		}

	// the binded texture has changed
	tex_reset_bind ();
}

// ******************************************************************

void tex_reset_bind()
	{
	tex_current = -1;
	env_mode = -1;
	}

// ******************************************************************

void tex_bind(int index)
	{
	char OGLError[1024];

	texture_t* pTexture = tex_array[index];

	if (pTexture->id != tex_current)
		{
		glBindTexture(pTexture->target, pTexture->id);
		// Advise to the fbo array that a normal texture has been used
		fbo_reset_bind_tex();
		tex_current = pTexture->id;
		}

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OGL Error in tex_bind glBindTexture:\n\n%s", OGLError);

	if (env_mode != pTexture->texfunc)
		{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, pTexture->texfunc);
		env_mode = pTexture->texfunc;
		}

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OGL Error in tex_bind glTexEnvi:\n\n%s", OGLError);
	}

// *************************************************************

void tex_envmode (int mode)
	{
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
	env_mode = mode;
	}

// *************************************************************

extern unsigned int get_closest_power_of_two(unsigned int value);

int tex_load(const char* fname, int cache, int ForcePowerOfTwo)
	{
	SDL_Surface*	TextureImage;
	texture_t*		tex;
	char*			new_tex;
	int				tex_number, i;
	int				new_width, new_height;

	// texture is already loaded? check only if the engine is in the loading state
	if (demoSystem.state == DEMO_LOADING)
		{
		if (cache == 1)
			{
			for (i=0; i<tex_count; i++)
				{
				if (tex_array[i] && tex_array[i]->name && spz_strcmpi(tex_array[i]->name, fname) == 0)
					return i;
				}
			}
		}
	
	// load texture resource
	if (!(TextureImage = IMG_Load(fname)))
		{
		dkernel_warn("Error loading texture \"%s\":\n%s", fname, IMG_GetError());
		return -1;
		}

	// paletized textures not allowed
	if (TextureImage->format->palette)
		{
		dkernel_warn("Unsupported paletted texture '%s'", fname);
		return -1;
		}

	tex_number = tex_init();
	tex = tex_array[tex_number];

	tex->name = strdup (fname);
	tex->data = TextureImage->pixels;
	tex->SDL_surface = (void *) TextureImage;

	tex->width = new_width = TextureImage->w;
	tex->height = new_height = TextureImage->h;
	tex->iformat = TextureImage->format->BytesPerPixel;

	// 24 bits texture format finder
	if (TextureImage->format->BitsPerPixel == 24)
		{
		#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			if (TextureImage->format->Rshift < TextureImage->format->Bshift)
				tex->format = GL_RGB;
			else
				tex->format = GL_BGR_EXT;
		#else
			if (TextureImage->format->Rshift < TextureImage->format->Bshift)
				tex->format = GL_BGR;
			else
				tex->format = GL_RGB;
		#endif
		}

	// 32 bits texture format finder
	else if (TextureImage->format->BitsPerPixel == 32)
		{
		#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			if (TextureImage->format->Rshift < TextureImage->format->Bshift)
				tex->format = GL_RGBA;
			else
				tex->format = GL_BGRA_EXT;
		#else
			if (TextureImage->format->Rshift < TextureImage->format->Bshift)
				tex->format = GL_BGRA;
			else
				tex->format = GL_RGBA;
		#endif
		}
	else
		{
		// unsupported texture format
		dkernel_warn("Unsupported texture format in \"%s\"", fname);
		return -1;
		}

	// scale texture according to a power of two
	if (ForcePowerOfTwo == TRUE)
		{
		new_width = get_closest_power_of_two(tex->width);
		new_height = get_closest_power_of_two(tex->height);

		if (new_width > 4096 || new_height > 4096)
			{
			dkernel_error("A texture size can't exceed 4096 pixels.");
			return -1;
			}

		tex->owidth = tex->width;
		tex->oheight = tex->height;

		if ((tex->width != new_width) || (tex->height != new_height))
			{
			// generate a new pixel data
			new_tex = (char *) malloc(tex->iformat * new_width * new_height);

			if (TextureImage->format->BitsPerPixel == 24)
				scale_texture_24(tex->data, tex->width, tex->height, new_width, new_height, new_tex);
			else
				scale_texture_32(tex->data, tex->width, tex->height, new_width, new_height, new_tex);

			// change texture properties
			tex->data = new_tex;
			tex->width = new_width;
			tex->height = new_height;

			// free SDL surface
			SDL_FreeSurface(tex->SDL_surface);
			tex->SDL_surface = NULL;
			}
		}

	return tex_number;
	}

// ******************************************************************

void tex_free (int index) {

	texture_t *tex = tex_array[index];

	if (tex) {
		if (tex->id != -1)
			glDeleteTextures(1, (GLuint *)&tex->id);
		free(tex->name);
		if (tex->SDL_surface)
			SDL_FreeSurface(tex->SDL_surface);
		free(tex->data);
		free(tex);
		tex_array[index] = NULL;
	}
}

// ******************************************************************

int tex_get_used_memory () {
	int i, memory = 0;

	for (i = 0; i < tex_count; i++) {
		if (tex_array[i] != NULL) {
			memory += tex_array[i]->memory;
		}
	}

	return memory;
}

// ******************************************************************

void load_palette (const char *fname, palette pal)
	{
	FILE *f = fopen(fname, "rb");
	if (!f)
		dkernel_error("Error opening '%s'", fname);
	fread(pal, 1, 768, f);
	fclose(f);
	}

// ******************************************************************

void scale_texture_24(const unsigned char* tex, int tx, int ty, int x, int y, unsigned char* b)
	{
	int i, j;
	unsigned int frac, fracstep, offs;
	const unsigned char* inrow;

	fracstep = tx * 65536 / x;
	for (i = 0; i < y; i++, b += x * 3)
		{
		inrow = tex + tx * (i * ty / y) * 3;
		frac = fracstep >> 1;

		for (j = 0; j < x; j++)
			{
			offs = (frac >> 16) * 3;
			b [j * 3 + 0] = inrow[offs + 0];
			b [j * 3 + 1] = inrow[offs + 1];
			b [j * 3 + 2] = inrow[offs + 2];
			frac += fracstep;
			}
		}
	}

// ******************************************************************

void scale_texture_32(const unsigned char* tex, int tx, int ty, int x, int y, unsigned char *b)
	{
	int i, j;
	unsigned int frac, fracstep, offs;
	const unsigned char* inrow;

	fracstep = tx * 65536 / x;
	for (i = 0; i < y; i++, b += x * 4)
		{
		inrow = tex + tx * (i * ty / y) * 4;
		frac = fracstep >> 1;

		for (j = 0; j < x; j++)
			{
			offs = (frac >> 16) * 4;
			b [j * 4 + 0] = inrow[offs + 0];
			b [j * 4 + 1] = inrow[offs + 1];
			b [j * 4 + 2] = inrow[offs + 2];
			b [j * 4 + 3] = inrow[offs + 3];
			frac += fracstep;
			}
		}
	}

// ******************************************************************

void scale_texture_8_to_24(const unsigned char* tex, int tx, int ty, int x, int y, unsigned char* b, palette pal)
	{
	int						i, j;
	unsigned int			frac, fracstep;
	const unsigned char*	inrow;
	unsigned char			c;

	fracstep = tx * 65536 / x;
	for (i = 0; i < y; i++, b += x * 3)
		{
		inrow = tex + tx * (i * ty / y);
		frac = fracstep >> 1;

		for (j = 0; j < x; j++)
			{
			c = inrow[frac >> 16];
			b [j * 3 + 0] = pal[c].r;
			b [j * 3 + 1] = pal[c].g;
			b [j * 3 + 2] = pal[c].b;
			frac += fracstep;
			}
		}
	}

// ******************************************************************

int powerOfTwo(int number)
	{
	int a, b;

	a = number / 2;
	b = number % 2;
	while ((a > 1) && (b == 0))
		{
		b = a % 2;
		a = a / 2;
		}
	return (b == 0);
	}

// ******************************************************************

int getCompressedFormat(texture_t *tex) {

	if (!glDriver.ext.texture_compression) return tex->iformat;
	if (tex->dynamic) return tex->iformat;

	// get new format for compressed texture
	switch (tex->iformat) {
		case 3: return GL_COMPRESSED_RGB_ARB;
		case 4: return GL_COMPRESSED_RGBA_ARB;
	}

	return tex->iformat;
}
