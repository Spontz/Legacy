#ifndef __TEXTURE_H
#define __TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

// flags for texture properties
#define NO_MIPMAP 1
#define CLAMP_TO_EDGE 2
#define CLAMP 4
#define MODULATE 8
#define TEXTURE_1D 16

// ******************************************************************

// texture definition
typedef struct
	{
	char*			name;
	unsigned char*	data;
	void*			SDL_surface;

	int				id, memory;
	int				width, height;
	int				owidth, oheight;
	int				iformat, format;

	int				target;
	int				properties;
	int				mipmap;
	int				wrap;
	int				texfunc;
	int				persistent;
	int				dynamic;
	} texture_t;

// 8bit paletted color
typedef struct
	{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	} rgb_color;

// ******************************************************************

typedef rgb_color palette[256];

// ******************************************************************

extern texture_t *tex_array[];

// ******************************************************************

int		tex_init();
int		tex_new (int width, int height, int format, int components);
void	tex_properties(int index, int flags);
void	tex_persistent(int index, int value);

void	tex_clean(int index);
void	tex_upload(int index, int usecache);
void	tex_reset_bind();
void	tex_bind(int index);
void	tex_envmode(int mode);

// * tex_load *******************************************************

// cache == USE_CACHE : do not reload the same bitmap
// cache == NOT_CACHE : reload
// warning : USE_CACHE won't give you a power of two texture if you already loaded it without ForcePowerOfTwo == 0 or viceversa

int		tex_load(const char* pFilename, int cache, int ForcePowerOfTwo);

// ******************************************************************

void	tex_free(int index);
int		tex_get_used_memory();

void	load_palette(const char* pFilename, palette pal);

void	scale_texture_24(const unsigned char* pSrc, int SrcX, int SrcY, int DstX, int DstY, unsigned char* pOut);
void	scale_texture_32(const unsigned char* pSrc, int SrcX, int SrcY, int DstX, int DstY, unsigned char* pOut);
void	scale_texture_8_to_24(const unsigned char* pSrc, int SrcX, int SrcY, int DstX, int DstY, unsigned char* pOut, palette pal);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
