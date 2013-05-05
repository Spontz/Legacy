/*
	3dsload.c: 3DStudio/LWO object reader
*/

#include "../main.h"

// ******************************************************************

#define MAX_SCENES 64

typedef struct {
	char *name;
	H3dsScene *scene;
} sceneCache_t;

static int sceneCount = 0;
static sceneCache_t sceneCache[MAX_SCENES];

// ******************************************************************

typedef struct {
	word	id;
	dword	len;
	dword	readed;
} tChunk;

// ******************************************************************

static FILE *inFile = 0;

#define dread(dest,len) (fread(dest,len,1,inFile))
#define dsetpos(pos)    (fseek(inFile,pos,SEEK_CUR))
#define dgetpos()       (ftell(inFile))

// ******************************************************************

void readString(char *buff, tChunk *chunk) {

	int readchars = 0;
	unsigned char ch;
	do {
		dread(&ch,1);
		buff[readchars++] = ch;
	} while (ch != (unsigned char)0);
	buff[readchars++] = '0';
	chunk->readed += readchars;
}

// ******************************************************************

byte readByte(tChunk *chunk) {

	byte c;
	dread(&c,1);
	chunk->readed ++;
	return c;
}

// ******************************************************************

word readWord(tChunk *chunk) {

	word result;
	dread(&result,2);
	chunk->readed += 2;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	unsigned char t1, t2;
	t1 = result >> 8;
	t2 = result & 0xff;
	result = (t2 << 8) | t1;
#endif

	return result;
}

// ******************************************************************

dword readDWord(tChunk *chunk) {

	dword result;
	dread(&result,4);
	chunk->readed += 4;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	unsigned char t1, t2, t3, t4;
	t1 = result >> 24;
	t2 = (result >> 16) & 0xff;
	t3 = (result >> 8) & 0xff;
	t4 = result & 0xff;
	result = (t4 << 24) | (t3 << 16) | (t2 << 8) | t1;
#endif

	return result;
}

// ******************************************************************

float readFloat(tChunk *chunk) {

	float f;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	unsigned char t1, t2, t3, t4;
	char *p = (char *) &f;
#endif

	dread(&f,4);        
	chunk->readed += 4;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	t1 = (unsigned char) p[0];
	t2 = (unsigned char) p[1];
	t3 = (unsigned char) p[2];
	t4 = (unsigned char) p[3];

	p[0] = t4;
	p[1] = t3;
	p[2] = t2;
	p[3] = t1;
#endif

	return f;
}

// ******************************************************************

tChunk *newChunk() {
	tChunk *chunk = (tChunk *) malloc(sizeof(tChunk));
	if (chunk)
		{
		chunk->len = 0;
		chunk->readed = 0;
		}
	return chunk;
}

void readChunk(tChunk *chunk) {
	chunk->id = readWord(chunk);   // 2 bytes
	chunk->len = readDWord(chunk); // 4 bytes
}

// ******************************************************************

H3dsMaterial *GetMaterial() {

	H3dsMaterial *mat = (H3dsMaterial *) malloc(sizeof(H3dsMaterial));
	if (mat)
		{
		mat->next = NULL;
		mat->Matname = NULL;
		mat->TexFilename = NULL;
		mat->hTex = 0;

		mat->Ambient.red = 0.0f;
		mat->Ambient.green = 0.0f;
		mat->Ambient.blue = 0.0f;

		mat->Diffuse.red = 0.7f;
		mat->Diffuse.green = 0.7f;
		mat->Diffuse.blue = 0.7f;

		mat->Specular.red = 0.0f;
		mat->Specular.green = 0.0f;
		mat->Specular.blue = 0.0f;

		mat->shiny = 1;
		mat->transp = 0;
		mat->twoSided = 0;
		mat->flags = 0;
		mat->uOffset = mat->vOffset = 0;
		mat->uScale = mat->vScale = 1;
		mat->rotAngle = 0;
		}
	return mat;
}

// ******************************************************************

H3dsMeshObj *GetMeshObj(H3dsScene *scene) {

	void *mem, *tmp;
	H3dsMeshObj *obj;

	tmp = scene->meshobjlist;
	mem = malloc(sizeof(H3dsMeshObj)*(scene->meshobjs+1));
	memcpy(mem,tmp,sizeof(H3dsMeshObj)*scene->meshobjs);
	free(tmp);

	scene->meshobjlist = (H3dsMeshObj *) mem;
	obj = &scene->meshobjlist[scene->meshobjs++];
	memset(obj,0,sizeof(H3dsMeshObj));
	return obj;
}

// ******************************************************************

// Each 3DS data-chunk starts with a 6 byte header.
// The first item in the header is a 2 byte (word) id-number.
// After that follows a dword wich gives the size of
// the data-chunk including the header. The size can be used
// as an relative offset to the next chunk.

// tab 4
enum {

	CHUNK_MAIN      = 0x4D4D,

		CHUNK_VERSION   = 0x0002,

		CHUNK_OBJMESH   = 0x3D3D,
			CHUNK_BKGCOLOR  = 0x1200,
			CHUNK_AMBCOLOR  = 0x2100,
			CHUNK_OBJBLOCK  = 0x4000,
				CHUNK_TRIMESH   = 0x4100,
					CHUNK_VERTLIST  = 0x4110,
					CHUNK_FACELIST  = 0x4120,
					CHUNK_FACEMAT   = 0x4130,
					CHUNK_MAPLIST   = 0x4140,
					CHUNK_SMOOLIST  = 0x4150,
					CHUNK_TRMATRIX  = 0x4160,
					CHUNK_MESHCOLOR = 0x4165,
					CHUNK_TXTINFO   = 0x4170,
				CHUNK_LIGHT     = 0x4600,
					CHUNK_SPOTLIGHT = 0x4610,
				CHUNK_CAMERA    = 0x4700,
				CHUNK_HIERARCHY = 0x4F00,

		CHUNK_VIEWPORT  = 0x7001,

		CHUNK_MATERIAL  = 0xAFFF,
			CHUNK_MATNAME   = 0xA000,
			CHUNK_AMBIENT   = 0xA010,
			CHUNK_DIFFUSE   = 0xA020,
			CHUNK_SPECULAR  = 0xA030,
			CHUNK_TEXTURE   = 0xA200,
			CHUNK_BUMPMAP   = 0xA230,
			CHUNK_REFLECT   = 0xA220,

			CHUNK_MAPFILE   = 0xA300,
			CHUNK_MAPFLAGS  = 0xA351,
			CHUNK_MAPUSCALE = 0xA354,
			CHUNK_MAPVSCALE = 0xA356,
			CHUNK_MAPUOFF   = 0xA358,
			CHUNK_MAPVOFF   = 0xA35A,
			CHUNK_MAPROTANG = 0xA35C,

		CHUNK_KEYFRAMER = 0xB000,
			CHUNK_AMBIENTKEY    = 0xB001,
			CHUNK_TRACKINFO = 0xB002,
				CHUNK_TRACKOBJNAME  = 0xB010,
				CHUNK_TRACKPIVOT    = 0xB013,
				CHUNK_TRACKPOS      = 0xB020,
				CHUNK_TRACKROTATE   = 0xB021,
				CHUNK_TRACKSCALE    = 0xB022,
				CHUNK_OBJNUMBER     = 0xB030,
			CHUNK_TRACKCAMERA = 0xB003,
				CHUNK_TRACKFOV  = 0xB023,
				CHUNK_TRACKROLL = 0xB024,
			CHUNK_TRACKCAMTGT = 0xB004,
			CHUNK_TRACKLIGHT  = 0xB005,
			CHUNK_TRACKLIGTGT = 0xB006,
			CHUNK_TRACKSPOTL  = 0xB007,
			CHUNK_FRAMES    = 0xB008,

	CHUNK_PRJ       = 0xC23D,
	CHUNK_MLI       = 0x3DAA,

	TYPE_MAT_SHININESS    = 0xA040,
	TYPE_MAT_SHINSTRENGTH = 0xA041,
	TYPE_MAT_TRANSPARENCY = 0xA050,
	TYPE_MAT_XPFALL       = 0xA052,
	TYPE_MAT_REFBLUR      = 0xA053,
	TYPE_MAT_2_SIDED      = 0xA081,
	TYPE_MAT_SELF_ILPCT   = 0xA084,
	TYPE_MAT_SHADING      = 0xA100,

	CHUNK_RGBF      = 0x0010,
	CHUNK_RGBB      = 0x0011,
	CHUNK_RGBB2     = 0x0012,  // LIN
	CHUNK_RGBF2     = 0x0013,  // LIN

	TYPE_PERCENT_I  = 0x0030,
	TYPE_PERCENT_F  = 0x0031,
};

// ******************************************************************

H3dsColor readColor(tChunk *prevChunk) {

	H3dsColor rgb = {0,0,0};
	tChunk *chunk = newChunk();

	while (prevChunk->readed < prevChunk->len) {

		chunk->readed = 0;
		readChunk(chunk);
		switch (chunk->id) {

			case CHUNK_RGBF:
			case CHUNK_RGBF2:
				rgb.red   = readFloat(chunk);
				rgb.green = readFloat(chunk);
				rgb.blue  = readFloat(chunk);
				break;

			case CHUNK_RGBB:
			case CHUNK_RGBB2:
				rgb.red   = (float)readByte(chunk)/255.0f;
				rgb.green = (float)readByte(chunk)/255.0f;
				rgb.blue  = (float)readByte(chunk)/255.0f;
				break;

			default:
				dsetpos(chunk->len - chunk->readed);
				chunk->readed += chunk->len - chunk->readed;
				break;
		}
		prevChunk->readed += chunk->readed;
	}
	free(chunk);
	return rgb;
}

// ******************************************************************

float readPercentage(tChunk *prevChunk) {

	float p = 0;
	tChunk *chunk = newChunk();

	while (prevChunk->readed < prevChunk->len) {

		chunk->readed = 0;
		readChunk(chunk);
		switch (chunk->id) {

			case TYPE_PERCENT_I:
				p = (float)readWord(chunk)/100.0f;
				break;

			case TYPE_PERCENT_F:
				p = readFloat(chunk);
				break;

			default:
				dsetpos(chunk->len - chunk->readed);
				chunk->readed += chunk->len - chunk->readed;
				break;
		}
		prevChunk->readed += chunk->readed;
	}
	free(chunk);
	return p;
}

// ******************************************************************

static void ReadVertList(H3dsMeshObj *meshobj, tChunk *chunk) {

	int i;

	meshobj->verts = readWord(chunk);
	meshobj->vertlist = (H3dsVert *) malloc(sizeof(H3dsVert)*meshobj->verts);
	memset(meshobj->vertlist, 0, sizeof(H3dsVert)*meshobj->verts);
	for (i=0; i<meshobj->verts; i++) {
		meshobj->vertlist[i].x =  readFloat(chunk);
		meshobj->vertlist[i].z = -readFloat(chunk);
		meshobj->vertlist[i].y =  readFloat(chunk);
	}
}

// ******************************************************************

static void ReadFaceList(H3dsMeshObj *meshobj, tChunk *chunk) {

	int i;

	meshobj->faces = readWord(chunk);
	meshobj->facelist = (H3dsFace *) malloc(sizeof(H3dsFace)*meshobj->faces);
	memset(meshobj->facelist, 0, sizeof(H3dsFace)*meshobj->faces);
	for (i=0; i<meshobj->faces; i++) {
		meshobj->facelist[i].vertexes = 3;
		meshobj->facelist[i].v = (H3dsVFData *) malloc(sizeof(H3dsVFData)*3);
		meshobj->facelist[i].v[0].vertex = readWord(chunk);
		meshobj->facelist[i].v[1].vertex = readWord(chunk);
		meshobj->facelist[i].v[2].vertex = readWord(chunk);
		meshobj->facelist[i].flags = readWord(chunk);
	}
}

// ******************************************************************

static void ReadFaceMaterials(H3dsScene *scene, H3dsMeshObj *meshobj, tChunk *chunk) {

	char Matname[256];
	H3dsMaterial *pMaterial;
	int i, index;
	word n;

	readString(Matname, chunk);

	pMaterial = scene->materials;
	while (pMaterial != NULL) {
		if (pMaterial->Matname != NULL) {
			if (spz_strcmpi(pMaterial->Matname, Matname) == 0) break;
		}
		pMaterial = (H3dsMaterial*) pMaterial->next;
	}

	n = readWord(chunk);
	if (n > 0) {
		for (i=0; i<n; i++) {
			index = readWord(chunk);
			meshobj->facelist[index].material = pMaterial;
		}
	}
}

// ******************************************************************

static void ReadMapList(H3dsMeshObj *meshobj, tChunk *chunk) {

	H3dsLinkedMap *linkedmap;
	H3dsMap *map;
	int i;

	linkedmap = (H3dsLinkedMap *) malloc(sizeof(H3dsLinkedMap));
	linkedmap->maps = readWord(chunk);
	linkedmap->map = (H3dsMap *) malloc(sizeof(H3dsMap)*linkedmap->maps);
	linkedmap->next = meshobj->maplist;
	meshobj->maplist = linkedmap;
	meshobj->maps++;

	map = linkedmap->map;

	for (i=0; i<linkedmap->maps; i++) {
		map[i].u = readFloat(chunk);
		map[i].v = readFloat(chunk); // 1 - ???
		// direct relationship between map and vertex
		// map X -> vertex X
	}
}

// ******************************************************************
/*
static void ReadTraMatrix(H3dsMeshObj *meshobj, tChunk *chunk) {

	meshobj->matrix = 1;
	dread(&meshobj->traMatrix,36);
	dread(&meshobj->traPivot,12);
	chunk->readed += 48;
}
*/
// ******************************************************************

static void ReadObject(H3dsScene *scene, H3dsMeshObj *meshObj, tChunk *prevChunk) {

	tChunk *chunk = newChunk();

	while (prevChunk->readed < prevChunk->len) {

		chunk->readed = 0;
		readChunk(chunk);
		switch (chunk->id) {

			case CHUNK_TRIMESH: ReadObject(scene,meshObj,chunk); break;
			case CHUNK_VERTLIST: ReadVertList(meshObj,chunk); break;
			case CHUNK_FACELIST: ReadFaceList(meshObj,chunk); break;
			case CHUNK_FACEMAT: ReadFaceMaterials(scene,meshObj,chunk); break;
			case CHUNK_MAPLIST: ReadMapList(meshObj,chunk); break;
			//case CHUNK_TRMATRIX: ReadTraMatrix(meshObj,chunk); break;
			case CHUNK_MESHCOLOR: meshObj->meshColor = readByte(chunk); break;

			default:
				dsetpos(chunk->len - chunk->readed);
				chunk->readed += chunk->len - chunk->readed;
				break;
		}
		prevChunk->readed += chunk->readed;
	}

	free(chunk);
}

// ******************************************************************

static void ReadMaterial(H3dsMaterial *mat, tChunk *prevChunk) {

	char text[256];
	tChunk *chunk = newChunk();

	while (prevChunk->readed < prevChunk->len) {

		chunk->readed = 0;
		readChunk(chunk);
		switch (chunk->id) {

			case CHUNK_MATNAME:
				readString(text, chunk);
				mat->Matname = _strdup(text);
				break;

			case CHUNK_AMBIENT:  mat->Ambient = readColor(chunk); break;
			case CHUNK_DIFFUSE:  mat->Diffuse = readColor(chunk); break;
			case CHUNK_SPECULAR: mat->Specular = readColor(chunk); break;

			case TYPE_MAT_SHININESS: mat->shiny = readPercentage(chunk); break;
			case TYPE_MAT_SHINSTRENGTH: mat->shinyStrength = readPercentage(chunk); break;

			case TYPE_MAT_TRANSPARENCY: mat->transp = readPercentage(chunk); break;

			case TYPE_MAT_2_SIDED: mat->twoSided = 1; break;

			case CHUNK_TEXTURE: ReadMaterial(mat,chunk); break;

			case CHUNK_MAPFILE:
				readString(text, chunk);
				mat->TexFilename = strdup(text);
				mat->hTex = 1;
				break;

			case CHUNK_MAPFLAGS: mat->flags = readWord(chunk); break;

			case CHUNK_MAPUSCALE: mat->uScale = readFloat(chunk); break;
			case CHUNK_MAPUOFF: mat->uOffset = readFloat(chunk); break;

			case CHUNK_MAPVSCALE: mat->vScale = readFloat(chunk); break;
			case CHUNK_MAPVOFF: mat->vOffset = readFloat(chunk); break;

			case CHUNK_MAPROTANG: mat->rotAngle = readFloat(chunk); break;

			default:
				dsetpos(chunk->len - chunk->readed);
				chunk->readed += chunk->len - chunk->readed;
				break;
		}
		prevChunk->readed += chunk->readed;
	}

	free(chunk);
}

// ******************************************************************
/*
static void ReadKeyframer(tChunk *prevChunk) {

	tChunk *chunk = newChunk();

	while (prevChunk->readed < prevChunk->len) {

		chunk->readed = 0;
		readChunk(chunk);
		switch (chunk->id) {

			default:
				dsetpos(chunk->len - chunk->readed);
				chunk->readed += chunk->len - chunk->readed;
				break;
		}
		prevChunk->readed += chunk->readed;
	}

	free(chunk);
}
*/
// ******************************************************************

static void ChunkReader(H3dsScene *scene, tChunk *prevChunk) {

	char text[256];
	H3dsMeshObj *meshObj;
	H3dsMaterial *mat;
	tChunk *chunk = newChunk();

	while (prevChunk->readed < prevChunk->len) {

		chunk->readed = 0;
		readChunk(chunk);
		switch (chunk->id) {

			case CHUNK_OBJMESH:
				ChunkReader(scene, chunk);
				break;

			case CHUNK_MATERIAL:
				// create a new material
				mat = GetMaterial();
				// read material chunks
				ReadMaterial(mat,chunk);
				// link new material to scene material list
				mat->next = scene->materials;
				scene->materials = mat;
				scene->mats++;
				break;

			case CHUNK_OBJBLOCK:
				// create a new object
				meshObj = GetMeshObj(scene);
				// load object name
				readString(text,chunk);
				meshObj->name = strdup(text);
				// load object chunks
				ReadObject(scene,meshObj,chunk);
				break;
/*
			case CHUNK_KEYFRAMER:
				ReadKeyframer(chunk);
				break;
*/
			default:
				dsetpos(chunk->len - chunk->readed);
				chunk->readed += chunk->len - chunk->readed;
				break;
		}
		prevChunk->readed += chunk->readed;
	}

	free(chunk);
}

// ******************************************************************

void HFree3dsScene(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	H3dsFace *face;
	H3dsLinkedMap *linkedmap, *linkedmap2;
	H3dsMap *map;
	H3dsMaterial *mat1, *mat2;
	H3dsClip *clip1, *clip2;
	H3dsTag *tag1, *tag2;
	int m, f;

	if (scene) {

		// free meshes
		if (scene->meshobjlist) {

			for (m=0; m<scene->meshobjs; m++) {
				mesh = &scene->meshobjlist[m];

				// free vertex indexes of opaque faces
				for (f=0; f<mesh->faces; f++) {
					face = &mesh->facelist[f];
					if (face->v) free(face->v);
				}
				// free vertex indexes of transparent faces
				for (f=0; f<mesh->transpfaces; f++) {
					face = &mesh->transpfacelist[f];
					if (face->v) free(face->v);
				}

				// remove arrays
				if (mesh->facelist) free(mesh->facelist);
				if (mesh->transpfacelist) free(mesh->transpfacelist);
				if (mesh->vertlist) free(mesh->vertlist);
				if (mesh->name) free(mesh->name);

				linkedmap = mesh->maplist;
				while (linkedmap) {
					map = linkedmap->map;
					if (map) free(map);
					linkedmap2 = linkedmap;
					linkedmap = (H3dsLinkedMap *) linkedmap->next;
					free(linkedmap2);
				}
			}
			free(scene->meshobjlist);
		}

		// free materials
		if (scene->materials) {
			mat1 = scene->materials;
			while (mat1) {
				mat2 = (H3dsMaterial *) mat1->next;
				if (mat1->Matname) free(mat1->Matname);
				if (mat1->TexFilename) free(mat1->TexFilename);
				free(mat1);
				mat1 = mat2;
			}
		}

		// free tags
		if (scene->taglist) {
			tag1 = scene->taglist;
			while (tag1) {
				tag2 = (H3dsTag *) tag1->next;
				free(tag1);
				tag1 = tag2;
			}
		}

		// free clips
		if (scene->cliplist) {
			clip1 = scene->cliplist;
			while (clip1) {
				clip2 = (H3dsClip *) clip1->next;
				if (clip1->name) free(clip1->name);
				free(clip1);
				clip1 = clip2;
			}
		}

		free(scene);
	}
}

// ******************************************************************

static void postProcessUV(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	int nmesh, nface, nvert, id;

	for (nmesh=0; nmesh<scene->meshobjs; nmesh++) {

		mesh = &scene->meshobjlist[nmesh];
		for (nface=0; nface<mesh->faces; nface++) {

			for (nvert=0; nvert<mesh->facelist[nface].vertexes; nvert++) {

				if (!mesh->maplist) continue;

				id = mesh->facelist[nface].v[nvert].vertex;
				mesh->facelist[nface].v[nvert].u = mesh->maplist->map[id].u;
				mesh->facelist[nface].v[nvert].v = 1 - mesh->maplist->map[id].v;
			}
		}

	}
}

// ******************************************************************

H3dsScene *HRead3dsScene(void *ptr) {

	tChunk *chunk;
	H3dsScene *scene;
	inFile = (FILE *) ptr;

	chunk = newChunk();
	readChunk(chunk);
	if (chunk->id != CHUNK_MAIN) {
		free(chunk);
		return NULL;
	}

	scene = (H3dsScene *) malloc(sizeof(H3dsScene));
	if (!scene) return NULL;
	memset(scene, 0, sizeof(H3dsScene));

	ChunkReader(scene, chunk);
	free(chunk);

	postProcessUV(scene);
	scene->sourceFile = FILE_3DS;
	return scene;
}

// ******************************************************************

void tangent_basis(float v0[3], float v1[3], float v2[3], float t0[2], float t1[2], float t2[2], float *tangent, float *binormal) {

	float cp[3];
	float e0[3] = { v1[0]-v0[0], t1[0]-t0[0], t1[1]-t0[1] };
	float e1[3] = { v2[0]-v0[0], t2[0]-t0[0], t2[1]-t0[1] };

	cp[0] = e0[1]*e1[2] - e0[2]*e1[1];
	cp[1] = e0[2]*e1[0] - e0[0]*e1[2];
	cp[2] = e0[0]*e1[1] - e0[1]*e1[0];

	if (fabs(cp[0]) > 0.00001f) {
		tangent[0]  = -cp[1]/cp[0];
		binormal[0] = -cp[2]/cp[0];
	}

	e0[0] = v1[1]-v0[1];
	e1[0] = v2[1]-v0[1];

	cp[0] = e0[1]*e1[2] - e0[2]*e1[1];
	cp[1] = e0[2]*e1[0] - e0[0]*e1[2];
	cp[2] = e0[0]*e1[1] - e0[1]*e1[0];

	if (fabs(cp[0]) > 0.00001f) {
		tangent[1]  = -cp[1]/cp[0];
		binormal[1] = -cp[2]/cp[0];
	}

	e0[0] = v1[2]-v0[2];
	e1[0] = v2[2]-v0[2];

	cp[0] = e0[1]*e1[2] - e0[2]*e1[1];
	cp[1] = e0[2]*e1[0] - e0[0]*e1[2];
	cp[2] = e0[0]*e1[1] - e0[1]*e1[0];

	if (fabs(cp[0]) > 0.00001f) {
		tangent[2]  = -cp[1]/cp[0];
		binormal[2] = -cp[2]/cp[0];
	}
}

// ******************************************************************

void calculateNormals(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	H3dsVert *v0p, *v1p, *v2p;
	float mag, a[3], b[3], c[3];
	int m, j, k, i;

	// calculate face and vertex normals
	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		for (j=0; j<mesh->faces; j++) {

			if (mesh->facelist[j].vertexes < 3) continue;

			// calculate face normal using the first 3 vertexes
			v0p = &mesh->vertlist[mesh->facelist[j].v[0].vertex];
			v1p = &mesh->vertlist[mesh->facelist[j].v[1].vertex];
			v2p = &mesh->vertlist[mesh->facelist[j].v[2].vertex];

			a[0] = v0p->x - v1p->x;
			a[1] = v0p->y - v1p->y;
			a[2] = v0p->z - v1p->z;
			b[0] = v2p->x - v1p->x;
			b[1] = v2p->y - v1p->y;
			b[2] = v2p->z - v1p->z;
			c[0] = (a[1]*b[2]) - (a[2]*b[1]);
			c[1] = (a[2]*b[0]) - (a[0]*b[2]);
			c[2] = (a[0]*b[1]) - (a[1]*b[0]);

			// normalize
			mag = -1.f / (float) sqrt((c[0]*c[0]) + (c[1]*c[1]) + (c[2]*c[2]));
			mesh->facelist[j].normal[0] = c[0] * mag;
			mesh->facelist[j].normal[1] = c[1] * mag;
			mesh->facelist[j].normal[2] = c[2] * mag;

			// update normal for all the face vertexes
			for (i=0; i<mesh->facelist[j].vertexes; i++) {
				v0p = &mesh->vertlist[mesh->facelist[j].v[i].vertex];
				for (k=0; k<3; k++)
					v0p->normal[k] += mesh->facelist[j].normal[k];
				v0p->sharings++;
			}
		}

		// calculate vertex normals
		for (j=0; j<mesh->verts; j++) {

			v0p = &mesh->vertlist[j];
			if (v0p->sharings == 0) continue;

			// ponderate number of sharings
			v0p->sharings = 1.f / v0p->sharings;
			v0p->normal[0] *= v0p->sharings;
			v0p->normal[1] *= v0p->sharings;
			v0p->normal[2] *= v0p->sharings;

			// reset vertex tangents
			v0p->tangent[0] = 0;
			v0p->tangent[1] = 0;
			v0p->tangent[2] = 0;

			v0p->binormal[0] = 0;
			v0p->binormal[1] = 0;
			v0p->binormal[2] = 0;
		}
	}

	// calculate vertex tangents
	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		if (mesh->maps == 0) continue;

		for (j=0; j<mesh->faces; j++) {

			float v0[3], v1[3], v2[3], t0[2], t1[2], t2[2], tangent[3], binormal[3];

			if (mesh->facelist[j].vertexes != 3) continue;

			v0p = &mesh->vertlist[mesh->facelist[j].v[0].vertex];
			v1p = &mesh->vertlist[mesh->facelist[j].v[1].vertex];
			v2p = &mesh->vertlist[mesh->facelist[j].v[2].vertex];

			v0[0] = v0p->x;
			v0[1] = v0p->y;
			v0[2] = v0p->z;

			v1[0] = v1p->x;
			v1[1] = v1p->y;
			v1[2] = v1p->z;

			v2[0] = v2p->x;
			v2[1] = v2p->y;
			v2[2] = v2p->z;

			t0[0] = mesh->facelist[j].v[0].u;
			t0[1] = mesh->facelist[j].v[0].v;

			t1[0] = mesh->facelist[j].v[1].u;
			t1[1] = mesh->facelist[j].v[1].v;

			t2[0] = mesh->facelist[j].v[2].u;
			t2[1] = mesh->facelist[j].v[2].v;

			tangent_basis(v0, v1, v2, t0, t1, t2, tangent, binormal);

			for (k=0; k<3; k++) {
				v0p->tangent[k] = tangent[k];
				v1p->tangent[k] = tangent[k];
				v2p->tangent[k] = tangent[k];

				v0p->binormal[k] = binormal[k];
				v1p->binormal[k] = binormal[k];
				v2p->binormal[k] = binormal[k];
			}
		}

		// normalize the new vertex normals
		for (j=0; j<mesh->verts; j++) {

			float len;
			v0p = &mesh->vertlist[j];

			len = (float) sqrt((v0p->tangent[0]*v0p->tangent[0])+(v0p->tangent[1]*v0p->tangent[1])+(v0p->tangent[2]*v0p->tangent[2]));
			if (len != 0.0f) {
				v0p->tangent[0] /= len;
				v0p->tangent[1] /= len;
				v0p->tangent[2] /= len;
			}

			len = (float) sqrt((v0p->binormal[0]*v0p->binormal[0])+(v0p->binormal[1]*v0p->binormal[1])+(v0p->binormal[2]*v0p->binormal[2]));
			if (len != 0.0f) {
				v0p->binormal[0] /= len;
				v0p->binormal[1] /= len;
				v0p->binormal[2] /= len;
			}
		}
	}
}

// ******************************************************************

static int compare(const void *arg1, const void *arg2) {

	H3dsFace *f1 = (H3dsFace *) arg1;
	H3dsFace *f2 = (H3dsFace *) arg2;

	if (!f1->material || !f2->material) return 0;

	return spz_strcmpi(f1->material->Matname, f2->material->Matname);
}

void reorderFaces(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	H3dsFace *face, *oldfaces;
	int m, j;
	int transp, opaque;
	int hasTransp = 0;

	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		transp = 0;

		for (j=0; j<mesh->faces; j++) {
			face = &mesh->facelist[j];
			if (face->material) {
				if (face->material->transp != 0.0f) transp++;
			}
		}

		if (transp) {

			hasTransp++;

			oldfaces = mesh->facelist;
			mesh->facelist = (H3dsFace *) malloc(sizeof(H3dsFace)*(mesh->faces-transp));
			mesh->transpfacelist = (H3dsFace *) malloc(sizeof(H3dsFace)*transp);
			transp = 0;
			opaque = 0;
			for (j=0; j<mesh->faces; j++) {
				face = &oldfaces[j];
				if (face->material) {
					if (face->material->transp != 0.0f) {
						memcpy(&mesh->transpfacelist[transp], face, sizeof(H3dsFace));
						transp++;
					} else {
						memcpy(&mesh->facelist[opaque], face, sizeof(H3dsFace));
						opaque++;
					}
				} else {
					memcpy(&mesh->facelist[opaque], face, sizeof(H3dsFace));
					opaque++;
				}
			}
			mesh->faces = opaque;
			mesh->transpfaces = transp;
			free(oldfaces);
		}
/*
		if (mesh->faces > 0)
			qsort((void *) mesh->facelist, (size_t) mesh->faces, sizeof(H3dsFace), compare);

		if (mesh->transpfaces > 0)
			qsort((void *) mesh->transpfacelist, (size_t) mesh->transpfaces, sizeof(H3dsFace), compare);
*/
	}

	scene->hasTransp = hasTransp;
}

// ******************************************************************

static void resetBoundingBox(H3dsBoundingBox *box) {
	box->min[0] = box->min[1] = box->min[2] = +99999;
	box->max[0] = box->max[1] = box->max[2] = -99999;
}

static void computePoint(float x, float y, float z, H3dsBoundingBox *box) {
	if (x < box->min[0]) box->min[0] = x;
	if (x > box->max[0]) box->max[0] = x;
	if (y < box->min[1]) box->min[1] = y;
	if (y > box->max[1]) box->max[1] = y;
	if (z < box->min[2]) box->min[2] = z;
	if (z > box->max[2]) box->max[2] = z;
}

void calculateBoundingBoxes(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	H3dsVert *v;
	int m, j;

	resetBoundingBox(&scene->box);
	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		if (mesh->verts == 0) continue;

		resetBoundingBox(&mesh->box);
		for (j=0; j<mesh->verts; j++) {

			v = &mesh->vertlist[j];
			computePoint(v->x, v->y, v->z, &mesh->box);
		}

		computePoint(mesh->box.min[0], mesh->box.min[1], mesh->box.min[2], &scene->box);
		computePoint(mesh->box.max[0], mesh->box.max[1], mesh->box.max[2], &scene->box);
	}
}

// ******************************************************************

static void computeBox(H3dsBoundingSphere *sphere, H3dsBoundingBox *box) {

	float dx, dy, dz;

	// center of the sphere
	sphere->center[0] = (box->max[0] + box->min[0]) / 2.0f;
	sphere->center[1] = (box->max[1] + box->min[1]) / 2.0f;
	sphere->center[2] = (box->max[2] + box->min[2]) / 2.0f;

	// radius
	dx = box->max[0] - sphere->center[0];
	dy = box->max[1] - sphere->center[1];
	dz = box->max[2] - sphere->center[2];
	sphere->radius = (float) sqrt(dx*dx + dy*dy + dz*dz);
}

void calculateBoundingSpheres(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	int m;

	// sphere for whole scene
	computeBox(&scene->sphere, &scene->box);

	// spheres for each mesh
	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		if (mesh->verts == 0) continue;

		computeBox(&mesh->sphere, &mesh->box);
	}
}

// ******************************************************************

void calculateTightBoundingSpheres(H3dsScene *scene) {

	H3dsPoint xmin, xmax, ymin, ymax, zmin, zmax;
	H3dsPoint dia1, dia2;
	float xspan, yspan, zspan, dx, dy, dz;
	float maxspan, rad_sq, new_rad_sq, new_rad, dif;
	H3dsMeshObj *mesh;
	H3dsVert *v;
	H3dsBoundingSphere *sphere;
	int m, i;

	// sphere for whole scene
	computeBox(&scene->sphere, &scene->box);

	// spheres for each mesh
	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		if (mesh->verts == 0) continue;

		sphere = &mesh->sphere;
		xmin.x = ymin.y = zmin.z = +99999;
		xmax.x = ymax.y = zmax.z = -99999;

		for (i=0; i<mesh->verts; i++) {

			v = &mesh->vertlist[i];
			if (v->x < xmin.x) {
				xmin.x = v->x;
				xmin.y = v->y;
				xmin.z = v->z;
			}
			if (v->x > xmax.x) {
				xmax.x = v->x;
				xmax.y = v->y;
				xmax.z = v->z;
			}
			if (v->y < ymin.y) {
				ymin.x = v->x;
				ymin.y = v->y;
				ymin.z = v->z;
			}
			if (v->y > ymax.y) {
				ymax.x = v->x;
				ymax.y = v->y;
				ymax.z = v->z;
			}
			if (v->z < zmin.z) {
				zmin.x = v->x;
				zmin.y = v->y;
				zmin.z = v->z;
			}
			if (v->z > zmax.z) {
				zmax.x = v->x;
				zmax.y = v->y;
				zmax.z = v->z;
			}
		}

		dx = xmax.x - xmin.x;
		dy = xmax.y - xmin.y;
		dz = xmax.z - xmin.z;
		xspan = dx*dx + dy*dy + dz*dz;

		dx = ymax.x - ymin.x;
		dy = ymax.y - ymin.y;
		dz = ymax.z - ymin.z;
		yspan = dx*dx + dy*dy + dz*dz;

		dx = zmax.x - zmin.x;
		dy = zmax.y - zmin.y;
		dz = zmax.z - zmin.z;
		zspan = dx*dx + dy*dy + dz*dz;

		dia1 = xmin;
		dia2 = xmax;
		maxspan = xspan;
		if (yspan > maxspan) {
			dia1 = ymin;
			dia2 = ymax;
			maxspan = yspan;
		}
		if (zspan > maxspan) {
			dia1 = zmin;
			dia2 = zmax;
		}

		// initial center
		computeBox(&mesh->sphere, &mesh->box);

		// or this other center
		// sphere->center[0] = (dia1.x + dia2.x) / 2.0f;
		// sphere->center[1] = (dia1.y + dia2.y) / 2.0f;
		// sphere->center[2] = (dia1.z + dia2.z) / 2.0f;

		// initial radius
		dx = dia2.x - sphere->center[0];
		dy = dia2.y - sphere->center[1];
		dz = dia2.z - sphere->center[2];
		rad_sq = dx*dx + dy*dy + dz*dz;
		sphere->radius = (float) sqrt(rad_sq);

		for (i=0; i<mesh->verts; i++) {

			v = &mesh->vertlist[i];
			dx = v->x - sphere->center[0];
			dy = v->y - sphere->center[1];
			dz = v->z - sphere->center[2];

			new_rad_sq = dx*dx + dy*dy + dz*dz;
			if (new_rad_sq > rad_sq) {

				// new radius
				new_rad = (float) sqrt(new_rad_sq);
				mesh->sphere.radius = (sphere->radius + new_rad) / 2.0f;
				rad_sq = sphere->radius * sphere->radius;

				// new center
				dif = new_rad - sphere->radius;
				sphere->center[0] = (sphere->radius*sphere->center[0] + dif*v->x) / new_rad;
				sphere->center[1] = (sphere->radius*sphere->center[1] + dif*v->y) / new_rad;
				sphere->center[2] = (sphere->radius*sphere->center[2] + dif*v->z) / new_rad;
			}

		}
	}
}

// ******************************************************************

void calculateCounters(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	H3dsFace *face;
	int hasNonTriangles;
	int m, f;

	scene->faces = 0;
	scene->opaqueFaces = 0;
	scene->transpFaces = 0;
	scene->vertexes = 0;

	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		scene->faces += mesh->faces + mesh->transpfaces;
		scene->opaqueFaces += mesh->faces;
		scene->transpFaces += mesh->transpfaces;
		scene->vertexes += mesh->verts;

		hasNonTriangles = 0;
		for (f=0; f<mesh->faces; f++) {
			face = &mesh->facelist[f];
			if (face->vertexes != 3) {
				hasNonTriangles = 1;
				break;
			}
		}
		// scene->hasTransp has already value
		scene->hasUnknownPolys |= mesh->hasUnknownPolys;
		scene->hasNonTriangles |= hasNonTriangles;
		scene->hasUnknownTexmaps |= mesh->hasUnknownTexmaps;
	}
}

// ******************************************************************

H3dsScene *model_load(const char *name, int cache)
	{
	H3dsScene *scene;
	FILE *f;
	int i;

	// If cache is enabled, check if the model is cached (only in loading mode)
	if (demoSystem.state == DEMO_LOADING)
		{
		if (cache == 1)
			{
			for (i=0; i<sceneCount; i++)
				{
				if (spz_strcmpi(sceneCache[i].name, name) == 0)
					{
					return sceneCache[i].scene;
					}
				}
			}
		}
	
	f = fopen(name,"rb");
	if (!f)
		return NULL;

	// first try to load as a 3DS
	scene = HRead3dsScene(f);
	// if error try as a LWO
	if (!scene)
		{
		fseek(f, SEEK_SET, 0);
		scene = readLWO(f);
		}

	fclose(f);
	if (!scene)
		return NULL;

	calculateNormals(scene);
	reorderFaces(scene);
	//calculateBoundingBoxes(scene);
	//calculateTightBoundingSpheres(scene);
	//calculateCounters(scene);

	// If the cache is enabled, add scene to the cache
	// If cache is enabled, check if the model is cached (only in loading mode)
	if (demoSystem.state == DEMO_LOADING) {
		if (cache == 1) {
			sceneCache[sceneCount].name = strdup(name);
			sceneCache[sceneCount].scene = scene;
			if (++sceneCount >= MAX_SCENES) dkernel_error("Too many 3ds scenes. Only %i simultaneous scenes are supported.", MAX_SCENES);
		}
	}
	
	return scene;
}

// ******************************************************************
//-1 = LOAD ERROR
// 1 = LOAD OK
int model_upload_tex(H3dsScene *scene, char *dir)
	{
	char fullName[256];
	int error = 0;
	
	H3dsMaterial *mat = scene->materials;
	while (mat)
		{
		// Always load the model texture since it can have other
		// properties, different from already loaded textures
		if (mat->hTex > 0)
			{
			sprintf(fullName, "%s%s", dir, mat->TexFilename);
			
			dkernel_trace("model_upload_tex: Uploading texture: %s\n", fullName);
			
			// hack : No entiendo porque hay que forzar la recarga de las texturas y no usar mipmaps
			mat->hTex = tex_load(fullName, NO_CACHE | NO_MIPMAP, FALSE);
			if (mat->hTex==-1)
				error++;
			tex_properties(mat->hTex, 0);
			tex_upload(mat->hTex, NO_CACHE | NO_MIPMAP);
			}
		mat = (H3dsMaterial*) mat->next;
		}

	if (error!=0)
		return -1;
	else
		return 1;
	}

// ******************************************************************

void model_free(H3dsScene *scene)
	{
	HFree3dsScene(scene);
	}
