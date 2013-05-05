#ifndef __LAYERS_H
#define __LAYERS_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#define RMODE_VERTEXES		1
#define RMODE_WIRE			2
#define RMODE_FILL			3
#define RMODE_FLAT			4
#define RMODE_SHADED		5
#define RMODE_NORMALS		6
#define RMODE_TEXTURE		7
#define RMODE_ENV			8
#define RMODE_CELL			9
#define RMODE_OUTLINE		10
#define RMODE_BUMPDOT3		11
#define RMODE_GLSL			12
#define RMODE_GLSL_BINORMAL	13

typedef struct {

	void *next;
	char *node;

	int rMode;

	int hasBlend;
	int sfactor, dfactor;

	int hasAlpha;
	int alphaFunc;
	float alpha1, alpha2;

	float r, g, b, alpha;
	float lineWidth;

	int cullface;
	int hasCullface;

	int texture;

} layer_t;

// ******************************************************************

layer_t *layers_load(char *name);
int layer_counter(layer_t *s);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
