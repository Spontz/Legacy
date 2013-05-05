#ifndef __3DSLOAD_H
#define __3DSLOAD_H

// ******************************************************************

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

typedef signed char  sbyte;
typedef signed short sword;
typedef signed long  sdword;

// ******************************************************************

typedef struct {
	float red;
	float green;
	float blue;
} H3dsColor;

// ******************************************************************

typedef struct {
	void *next;

	char *Matname;     // asciiz material name
	char *TexFilename; // asciiz filename of texture if any
	unsigned int hTex; // texture handle, 0 = no texture

	H3dsColor Ambient;
	H3dsColor Diffuse;
	H3dsColor Specular;

	float shiny, shinyStrength;
	float transp;
	char twoSided;
	word flags;

	float uScale, vScale;
	float uOffset, vOffset;
	float rotAngle;
} H3dsMaterial;

// ******************************************************************

typedef struct {
	float x;
	float y;
	float z;
} H3dsPoint;

// ******************************************************************

typedef struct {
	word vertex;
	float u;
	float v;
} H3dsVFData;

// ******************************************************************

typedef struct {
	word vertexes;
	H3dsVFData *v;

	word flags;
	float normal[3];
	H3dsMaterial *material;
	int tag;
} H3dsFace;

// ******************************************************************

typedef struct {
	union { float x; dword ix; };
	union { float y; dword iy; };
	union { float z; dword iz; };

	// number of sharing faces
	float sharings;

	// 3 vertex normals
	float normal[3];
	float tangent[3];
	float binormal[3];

	// aux for bump mapping
	float result[3];
} H3dsVert;

// ******************************************************************

typedef struct {
	union { float u; dword iu; };
	union { float v; dword iv; };

	int vertex;
	int face;
} H3dsMap;

// ******************************************************************

typedef struct {
	void *next;
	int maps;
	int perPoly;
	H3dsMap *map;
} H3dsLinkedMap;

// ******************************************************************

typedef struct {
	float min[3];
	float max[3];
} H3dsBoundingBox;

// ******************************************************************

typedef struct {
	float center[3];
	float radius;
} H3dsBoundingSphere;

// ******************************************************************

typedef struct {
	char *name;             // asciiz object name
	int faces;              // number of faces in facelist
	int verts;              // number of vertices in vertlist
	int maps;                // number of maplists in maplist
	H3dsFace *facelist;
	H3dsVert *vertlist;
	H3dsLinkedMap *maplist;
	H3dsBoundingBox box;
	H3dsBoundingSphere sphere;

	int transpfaces;
	H3dsFace *transpfacelist;

	//char matrix;            // 1 if transformation matrix is loaded
	//float traMatrix[3*4];   // 3*3 transformation matrix
	//float traPivot[3*4];    // pivot

	byte meshColor;

	int hasUnknownPolys;
	int hasUnknownTexmaps;
} H3dsMeshObj;

// ******************************************************************

typedef struct {
	char *name;
	H3dsMaterial *mat;
	void *next;
} H3dsTag;

// ******************************************************************

typedef struct {
	unsigned int index;
	char *name;
	void *next;
} H3dsClip;

// ******************************************************************

typedef struct {
	// original format of the file
	int sourceFile;

	int mats;
	H3dsMaterial *materials;

	int tags;
	H3dsTag *taglist;

	int clips;
	H3dsClip *cliplist;

	int meshobjs;
	H3dsMeshObj *meshobjlist;

	// bounding shapes
	H3dsBoundingBox box;
	H3dsBoundingSphere sphere;

	// counters
	int faces, opaqueFaces, transpFaces, vertexes;

	// flags
	int hasTransp;
	int hasUnknownPolys;
	int hasNonTriangles;
	int hasUnknownTexmaps;
} H3dsScene;

// ******************************************************************

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#define FILE_3DS 1
#define FILE_LWO 2

// ******************************************************************

// import help procedures
H3dsMaterial *GetMaterial();
H3dsMeshObj *GetMeshObj(H3dsScene *scene);

H3dsScene *readLWO(void *ptr);

// ******************************************************************

void calculateNormals(H3dsScene *scene);
H3dsScene *model_load(const char *name, int usecache);
int model_upload_tex(H3dsScene *scene, char *dir);
void model_free(H3dsScene *scene);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
