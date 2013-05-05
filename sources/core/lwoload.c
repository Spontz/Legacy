/*
	lwoload.c: LWO object reader
*/

#include "../main.h"

// ******************************************************************

static FILE *inFile = 0;

#define dread(dest,len) (fread(dest,len,1,inFile))
#define dsetpos(pos)    (fseek(inFile,pos,SEEK_CUR))
#define dgetpos()       (ftell(inFile))

// ******************************************************************

void readS(char *buff) {

	int readchars = 0;
	unsigned char ch;
	do {
		dread(&ch,1);
		buff[readchars++] = ch;
	} while (ch != (unsigned char)0);
	buff[readchars] = '0';

	if ((readchars & 1) == 1) dread(&ch,1);
}

// ******************************************************************

char readB() {

	unsigned char f[1];
	if (dread(f,1) != 1) return -1;
	return f[0];
}

// ******************************************************************

short readW() {

	unsigned char f[2];
	if (dread(f,2) != 1) return -1;
	return (f[0] << 8) | f[1];
}

// ******************************************************************

long readDW() {

	unsigned char f[4];
	if (dread(f,4) != 1) return -1;
	return (f[0] << 24) | (f[1] << 16) | (f[2] << 8) | f[3];
}

// ******************************************************************

int readVX() {

	int i;
	unsigned char c1, c2, c3;

	if (dread(&c1,1) != 1) return -1;

	// 2 bytes long
	if (c1 != 0xFF) {
		if (dread(&c2,1) != 1) return -1;
		i = (c1 << 8) | c2;
	}
	// 4 bytes long (value on 3 last bytes)
	else {
		if (dread(&c1,1) != 1) return -1;
		if (dread(&c2,1) != 1) return -1;
		if (dread(&c3,1) != 1) return -1;
		i = (c1 << 16) | (c2 << 8) | c3;
	}

	return i;
}

// ******************************************************************

float readF() {

	float f;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	unsigned char t1, t2, t3, t4;
	char *p = (char *) &f;
#endif

	if (dread(&f,4) != 1) return -1;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
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

#define LWO2_OK				0
#define LWO2_ERROR			1

#define LWO_ID(a,b,c,d)		((a<<24) | (b<<16) | (c<<8) | d)

// ******************************************************************

#define ID_FORM  LWO_ID('F','O','R','M')
#define ID_LWO2  LWO_ID('L','W','O','2')
#define ID_LWOB  LWO_ID('L','W','O','B')

/* top-level chunks */
#define ID_LAYR  LWO_ID('L','A','Y','R')
#define ID_TAGS  LWO_ID('T','A','G','S')
#define ID_PNTS  LWO_ID('P','N','T','S')
#define ID_BBOX  LWO_ID('B','B','O','X')
#define ID_VMAP  LWO_ID('V','M','A','P')
#define ID_VMAD  LWO_ID('V','M','A','D')
#define ID_POLS  LWO_ID('P','O','L','S')
#define ID_PTAG  LWO_ID('P','T','A','G')
#define ID_ENVL  LWO_ID('E','N','V','L')
#define ID_CLIP  LWO_ID('C','L','I','P')
#define ID_SURF  LWO_ID('S','U','R','F')
#define ID_DESC  LWO_ID('D','E','S','C')
#define ID_TEXT  LWO_ID('T','E','X','T')
#define ID_ICON  LWO_ID('I','C','O','N')

/* polygon types */
#define ID_FACE  LWO_ID('F','A','C','E')
#define ID_CURV  LWO_ID('C','U','R','V')
#define ID_PTCH  LWO_ID('P','T','C','H')
#define ID_MBAL  LWO_ID('M','B','A','L')
#define ID_BONE  LWO_ID('B','O','N','E')

/* polygon tags */
#define ID_SURF  LWO_ID('S','U','R','F')
#define ID_PART  LWO_ID('P','A','R','T')
#define ID_SMGP  LWO_ID('S','M','G','P')
#define ID_TXUV  LWO_ID('T','X','U','V')

/* envelopes */
#define ID_PRE   LWO_ID('P','R','E',' ')
#define ID_POST  LWO_ID('P','O','S','T')
#define ID_KEY   LWO_ID('K','E','Y',' ')
#define ID_SPAN  LWO_ID('S','P','A','N')
#define ID_TCB   LWO_ID('T','C','B',' ')
#define ID_HERM  LWO_ID('H','E','R','M')
#define ID_BEZI  LWO_ID('B','E','Z','I')
#define ID_BEZ2  LWO_ID('B','E','Z','2')
#define ID_LINE  LWO_ID('L','I','N','E')
#define ID_STEP  LWO_ID('S','T','E','P')

/* clips */
#define ID_STIL  LWO_ID('S','T','I','L')
#define ID_ISEQ  LWO_ID('I','S','E','Q')
#define ID_ANIM  LWO_ID('A','N','I','M')
#define ID_XREF  LWO_ID('X','R','E','F')
#define ID_STCC  LWO_ID('S','T','C','C')
#define ID_TIME  LWO_ID('T','I','M','E')
#define ID_CONT  LWO_ID('C','O','N','T')
#define ID_BRIT  LWO_ID('B','R','I','T')
#define ID_SATR  LWO_ID('S','A','T','R')
#define ID_HUE   LWO_ID('H','U','E',' ')
#define ID_GAMM  LWO_ID('G','A','M','M')
#define ID_NEGA  LWO_ID('N','E','G','A')
#define ID_IFLT  LWO_ID('I','F','L','T')
#define ID_PFLT  LWO_ID('P','F','L','T')

/* surfaces */
#define ID_COLR  LWO_ID('C','O','L','R')
#define ID_LUMI  LWO_ID('L','U','M','I')
#define ID_DIFF  LWO_ID('D','I','F','F')
#define ID_SPEC  LWO_ID('S','P','E','C')
#define ID_GLOS  LWO_ID('G','L','O','S')
#define ID_REFL  LWO_ID('R','E','F','L')
#define ID_RFOP  LWO_ID('R','F','O','P')
#define ID_RIMG  LWO_ID('R','I','M','G')
#define ID_RSAN  LWO_ID('R','S','A','N')
#define ID_TRAN  LWO_ID('T','R','A','N')
#define ID_TROP  LWO_ID('T','R','O','P')
#define ID_TIMG  LWO_ID('T','I','M','G')
#define ID_RIND  LWO_ID('R','I','N','D')
#define ID_TRNL  LWO_ID('T','R','N','L')
#define ID_BUMP  LWO_ID('B','U','M','P')
#define ID_SMAN  LWO_ID('S','M','A','N')
#define ID_SIDE  LWO_ID('S','I','D','E')
#define ID_CLRH  LWO_ID('C','L','R','H')
#define ID_CLRF  LWO_ID('C','L','R','F')
#define ID_ADTR  LWO_ID('A','D','T','R')
#define ID_SHRP  LWO_ID('S','H','R','P')
#define ID_LINE  LWO_ID('L','I','N','E')
#define ID_LSIZ  LWO_ID('L','S','I','Z')
#define ID_ALPH  LWO_ID('A','L','P','H')
#define ID_AVAL  LWO_ID('A','V','A','L')
#define ID_GVAL  LWO_ID('G','V','A','L')
#define ID_BLOK  LWO_ID('B','L','O','K')

/* texture layer */
#define ID_TYPE  LWO_ID('T','Y','P','E')
#define ID_CHAN  LWO_ID('C','H','A','N')
#define ID_NAME  LWO_ID('N','A','M','E')
#define ID_ENAB  LWO_ID('E','N','A','B')
#define ID_OPAC  LWO_ID('O','P','A','C')
#define ID_FLAG  LWO_ID('F','L','A','G')
#define ID_PROJ  LWO_ID('P','R','O','J')
#define ID_STCK  LWO_ID('S','T','C','K')
#define ID_TAMP  LWO_ID('T','A','M','P')

/* texture coordinates */
#define ID_TMAP  LWO_ID('T','M','A','P')
#define ID_AXIS  LWO_ID('A','X','I','S')
#define ID_CNTR  LWO_ID('C','N','T','R')
#define ID_SIZE  LWO_ID('S','I','Z','E')
#define ID_ROTA  LWO_ID('R','O','T','A')
#define ID_OREF  LWO_ID('O','R','E','F')
#define ID_FALL  LWO_ID('F','A','L','L')
#define ID_CSYS  LWO_ID('C','S','Y','S')

/* image map */
#define ID_IMAP  LWO_ID('I','M','A','P')
#define ID_IMAG  LWO_ID('I','M','A','G')
#define ID_WRAP  LWO_ID('W','R','A','P')
#define ID_WRPW  LWO_ID('W','R','P','W')
#define ID_WRPH  LWO_ID('W','R','P','H')
#define ID_VMAP  LWO_ID('V','M','A','P')
#define ID_AAST  LWO_ID('A','A','S','T')
#define ID_PIXB  LWO_ID('P','I','X','B')

/* procedural */
#define ID_PROC  LWO_ID('P','R','O','C')
#define ID_COLR  LWO_ID('C','O','L','R')
#define ID_VALU  LWO_ID('V','A','L','U')
#define ID_FUNC  LWO_ID('F','U','N','C')
#define ID_FTPS  LWO_ID('F','T','P','S')
#define ID_ITPS  LWO_ID('I','T','P','S')
#define ID_ETPS  LWO_ID('E','T','P','S')

/* gradient */
#define ID_GRAD  LWO_ID('G','R','A','D')
#define ID_GRST  LWO_ID('G','R','S','T')
#define ID_GREN  LWO_ID('G','R','E','N')
#define ID_PNAM  LWO_ID('P','N','A','M')
#define ID_INAM  LWO_ID('I','N','A','M')
#define ID_GRPT  LWO_ID('G','R','P','T')
#define ID_FKEY  LWO_ID('F','K','E','Y')
#define ID_IKEY  LWO_ID('I','K','E','Y')

/* shader */
#define ID_SHDR  LWO_ID('S','H','D','R')
#define ID_DATA  LWO_ID('D','A','T','A')

// ******************************************************************

H3dsMeshObj *processLAYR(long dwSize, H3dsScene *scene) {

	H3dsMeshObj *meshobj = GetMeshObj(scene);
	return meshobj;
}

// ******************************************************************

void processTAGS(long dwSize, H3dsScene *scene) {

	char name[256];
	H3dsTag *tag;
	long opos, index, readed;

	opos = ftell(inFile);
	readed = 0;
	index = 1;
	while (readed < dwSize) {

		// read tag from file
		readS(name);

		// store tag
		tag = (H3dsTag *) malloc(sizeof(H3dsTag));
		tag->name = strdup(name);
		tag->mat = NULL;
		tag->next = (H3dsTag *) scene->taglist;
		scene->taglist = tag;
		scene->tags++;

		readed = ftell(inFile) - opos;
	}

	fseek(inFile, opos, SEEK_SET);
}

// ******************************************************************

void processPNTS(long dwSize, H3dsMeshObj *meshobj) {

	int i;

	meshobj->verts = dwSize/12;
	meshobj->vertlist = (H3dsVert *) malloc(sizeof(H3dsVert)*meshobj->verts);
	memset(meshobj->vertlist, 0, sizeof(H3dsVert)*meshobj->verts);
	for (i=0; i<meshobj->verts; i++) {
		meshobj->vertlist[i].x = readF();
		meshobj->vertlist[i].y = readF();
		meshobj->vertlist[i].z = readF();
	}
}

// ******************************************************************

void processVMAP_VMAD(long dwSize, H3dsMeshObj *meshobj, int perPoly) {

	char name[256];
	H3dsLinkedMap *linkedmap;
	H3dsMap *map;
	long type, opos, cpos, dwLoop;
	int i, wCount;
	short dim;
	unsigned char c;

	// get vmap properties
	opos = ftell(inFile);
	type = readDW();
	dim = readW();
	if ((type != ID_TXUV) || (dim != 2)){
		meshobj->hasUnknownTexmaps = 1;
		return;
	}

	readS(name);
	cpos = ftell(inFile);

	// count vmaps
	wCount = 0;
	dwLoop = cpos - opos;
	while (dwLoop < dwSize) {

		dread(&c,1);
		// 2 bytes long
		if (c != 0xFF) {
			dread(&c,1);
			dwLoop += 2;
		}
		// 4 bytes long (value on 3 last bytes)
		else {
			dread(&c,1);
			dread(&c,1);
			dread(&c,1);
			dwLoop += 4;
		}

		if (perPoly) {
			dread(&c,1);
			// 2 bytes long
			if (c != 0xFF) {
				dread(&c,1);
				dwLoop += 2;
			}
			// 4 bytes long (value on 3 last bytes)
			else {
				dread(&c,1);
				dread(&c,1);
				dread(&c,1);
				dwLoop += 4;
			}
		}

		readF();
		readF();
		dwLoop += 4*2;
		wCount++;
	}

	// read vmaps
	linkedmap = (H3dsLinkedMap *) malloc(sizeof(H3dsLinkedMap));
	linkedmap->maps = wCount;
	linkedmap->perPoly = perPoly;
	linkedmap->map = (H3dsMap *) malloc(sizeof(H3dsMap)*linkedmap->maps);
	linkedmap->next = meshobj->maplist;
	meshobj->maplist = linkedmap;
	meshobj->maps++;

	map = linkedmap->map;

	fseek(inFile, cpos, SEEK_SET);
	for (i=0; i<linkedmap->maps; i++) {

		map[i].vertex = readVX();
		if (perPoly) map[i].face = readVX();

		map[i].u = readF();
		map[i].v = readF();
   }
}

// ******************************************************************

void processPOLS(long dwSize, H3dsMeshObj *meshobj) {

	long numFaces;
	long dwType, dwLoop, i, cpos;
	char q = 0;
	unsigned char c;

	dwType = readDW();
	switch (dwType) {

	case ID_FACE: {
		int wCount, wFace;

		cpos = ftell(inFile);

		// count faces
		numFaces = 0;
		dwLoop = 4; // 4 bytes (type)
		while (dwLoop < dwSize) {
			wCount = readW();
			wCount &= 1023;
			dwLoop += 2;
			numFaces++;

			for (i=0; i<wCount; i++) {
				dread(&c,1);
				// 2 bytes long
				if (c != 0xFF) {
					dread(&c,1);
					dwLoop += 2;
				}
				// 4 bytes long (value on 3 last bytes)
				else {
					dread(&c,1);
					dread(&c,1);
					dread(&c,1);
					dwLoop += 4;
				}
			}
		}

		fseek(inFile, cpos, SEEK_SET);

		// allocate faces
		meshobj->faces = numFaces;
		meshobj->facelist = (H3dsFace *) malloc(sizeof(H3dsFace)*meshobj->faces);
		memset(meshobj->facelist, 0, sizeof(H3dsFace)*meshobj->faces);

		// read faces
		wFace = 0;
		while (wFace < numFaces) {
			wCount = readW();
			wCount &= 1023;

			meshobj->facelist[wFace].vertexes = wCount;
			meshobj->facelist[wFace].v = (H3dsVFData *) malloc(sizeof(H3dsVFData)*wCount);
			meshobj->facelist[wFace].tag = -1;

			for (i=0; i<wCount; i++) {
				meshobj->facelist[wFace].v[i].vertex = readVX();
				meshobj->facelist[wFace].v[i].u = 0;
				meshobj->facelist[wFace].v[i].v = 0;
			}

			wFace++;
		}
		break;
	}

	default:
		meshobj->hasUnknownPolys = 1;
		if (fseek(inFile, dwSize-4, SEEK_CUR) != 0) q = 1; // type has been readed
		break;
	}
}

// ******************************************************************

void processPTAG(long dwSize, H3dsMeshObj *meshobj) {

	long dwLoop, opos, id;
	int pol, tag;

	if (meshobj->faces == 0) return;

	opos = ftell(inFile);
	id = readDW();
	if (id != ID_SURF) return;

	dwLoop = 4;
	while (dwLoop < dwSize) {

		pol = readVX();
		tag = readVX();
		if (pol < meshobj->faces) meshobj->facelist[pol].tag = tag;

		dwLoop = ftell(inFile) - opos;
	}
}

// ******************************************************************
// Deletes the path of a given string
void strdelpath (char *mypath)
{	
	int lastdash = 0;
	int mysize = strlen(mypath);
	int i=0;
	for (i=0; i<mysize; i++)
	{
		if (mypath[i] == '\\' || mypath[i] == '/')
		{
			lastdash=i; //	printf("Lastdash found at: %d!!\n",i);
		}
	}
	memmove(mypath, mypath+lastdash, mysize-lastdash+1);
}

// ******************************************************************

void processCLIP(long dwSize, H3dsScene *scene) {

	char text[256];
	H3dsClip *clip;
	long dwIndex, dwType;
	short wLength;

	dwIndex = readDW();
	dwType = readDW();
	wLength = readW();

	switch (dwType) {

	case ID_STIL:
		readS(text);
		strdelpath (text);
		clip = (H3dsClip *) malloc(sizeof(H3dsClip));
		clip->index = dwIndex;
		clip->name = strdup(text);
		clip->next = (H3dsClip *) scene->cliplist;
		scene->cliplist = clip;
		scene->clips++;
		break;
	}
}

// ******************************************************************

void processSURF(long dwSize, H3dsScene *scene) {

	char text[256];
	H3dsMaterial *mat = GetMaterial();
	long opos, cpos, opos2, cpos2, dwLoopBlok, dwLoop;
	float percent;
	unsigned char c;

	opos = ftell(inFile);

	readS(text);
	mat->Matname = strdup(text);
	c = 0;
	while (c == 0) {
		c = readB();
	}
	fseek(inFile, -1, SEEK_CUR);

	dwLoop = ftell(inFile) - opos;
	while (dwLoop < dwSize) {

		long subid, id = readDW();
		int subsize, size = readW();

		cpos = ftell(inFile);

		switch (id) {

		case ID_COLR:
			mat->Ambient.red = readF();
			mat->Ambient.green = readF();
			mat->Ambient.blue = readF();
			readVX(); // envelope
			break;

		case ID_DIFF:
			percent = readF();
			mat->Diffuse.red = mat->Ambient.red * percent;
			mat->Diffuse.green = mat->Ambient.green * percent;
			mat->Diffuse.blue = mat->Ambient.blue * percent;
			readVX(); // envelope
			break;

		case ID_GLOS:
			percent = readF();
			mat->Specular.red = mat->Ambient.red * percent;
			mat->Specular.green = mat->Ambient.green * percent;
			mat->Specular.blue = mat->Ambient.blue * percent;
			readVX(); // envelope
			break;

		case ID_BLOK:
			dwLoopBlok = 4;
			opos2 = ftell(inFile);

			while (dwLoopBlok < size) {

				subid = readDW();
				subsize = readW();

				cpos2 = ftell(inFile);

				switch (subid) {

				case ID_IMAG:
					mat->hTex = readVX();
					break;
				}

				fseek(inFile, cpos2 + subsize, SEEK_SET);
				dwLoopBlok = ftell(inFile) - opos2;
			}
			break;
		}

		fseek(inFile, cpos + size, SEEK_SET);
		dwLoop = ftell(inFile) - opos;
	}

	// link new material to scene material list
	mat->next = scene->materials;
	scene->materials = mat;
	scene->mats++;
}

// ******************************************************************

void updateSurfaces(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	H3dsFace *face;
	H3dsMaterial *mat;
	H3dsTag *tag;
	H3dsClip *clip;
	int m, j, i;

	// link tags to materials
	tag = scene->taglist;
	while (tag) {
		mat = scene->materials;
		while (mat) {
			if (spz_strcmpi(mat->Matname, tag->name) == 0) break;
			mat = (H3dsMaterial *) mat->next;
		}
		if (mat) tag->mat = mat;
		tag = (H3dsTag *) tag->next;
	}

	// link faces to materials
	for (m=0; m<scene->meshobjs; m++) {

		mesh = &scene->meshobjlist[m];
		for (j=0; j<mesh->faces; j++) {

			face = &mesh->facelist[j];
			if (face->tag != -1) {
				tag = scene->taglist;
				i = scene->tags-1;
				while (tag && (i != face->tag)) {
					tag = (H3dsTag *) tag->next;
					i--;
				}
				if (tag) face->material = tag->mat;
			}
		}
	}

	// link materials to textures
	mat = scene->materials;
	while (mat) {
		if (mat->hTex > 0) {
			clip = scene->cliplist;
			while (clip) {
				if (clip->index == mat->hTex) break;
				clip = (H3dsClip *) clip->next;
			}
			if (clip) {
				if (clip->index == mat->hTex) {
					mat->TexFilename = strdup(clip->name);
				}
			}
			if (!mat->TexFilename) mat->hTex = 0;
		}
		mat = (H3dsMaterial *) mat->next;
	}
}

// ******************************************************************

static void postProcessUV(H3dsScene *scene) {

	H3dsMeshObj *mesh;
	H3dsLinkedMap *linkedmap;
	H3dsMap *map;
	int nmesh, nface, nvert, nmap, id;
	char hasUV;

	for (nmesh=0; nmesh<scene->meshobjs; nmesh++) {

		mesh = &scene->meshobjlist[nmesh];
		for (nface=0; nface<mesh->faces; nface++) {

			for (nvert=0; nvert<mesh->facelist[nface].vertexes; nvert++) {
				id = mesh->facelist[nface].v[nvert].vertex;
				hasUV = 0;

				linkedmap = mesh->maplist;
				while (linkedmap && !hasUV) {

					// vmads only
					if (linkedmap->perPoly) {
						map = linkedmap->map;
						for (nmap=0; nmap<linkedmap->maps; nmap++) {

							if ((map[nmap].face == nface) && (map[nmap].vertex == id)) {
								mesh->facelist[nface].v[nvert].u = 1 - map[nmap].u;
								mesh->facelist[nface].v[nvert].v = 1 - map[nmap].v;
								hasUV = 1;
								break;
							}
						}
					}
					linkedmap = (H3dsLinkedMap *) linkedmap->next;
				}

				linkedmap = mesh->maplist;
				while (linkedmap && !hasUV) {

					// vmaps only
					if (!linkedmap->perPoly) {
						map = linkedmap->map;
						for (nmap=0; nmap<linkedmap->maps; nmap++) {

							if (map[nmap].vertex == id) {
								mesh->facelist[nface].v[nvert].u = 1 - map[nmap].u;
								mesh->facelist[nface].v[nvert].v = 1 - map[nmap].v;
								hasUV = 1;
								break;
							}
						}
					}
					linkedmap = (H3dsLinkedMap *) linkedmap->next;
				}
			}
		}
	}
}

// ******************************************************************

H3dsScene *readLWO(void *ptr) {

	H3dsScene *scene;
	long id, dwFileSize, type, cpos;
	long dwChunkId = LWO2_OK, dwSize;
	H3dsMeshObj *meshobj = NULL;
	char q = 0;

	inFile = (FILE *) ptr;

	id = readDW();
	dwFileSize = readDW();
	type = readDW();
	if (id != ID_FORM) return NULL;
	if (type != ID_LWO2) return NULL;

	scene = (H3dsScene *) malloc(sizeof(H3dsScene));
	if (!scene) return 0;
	memset(scene, 0, sizeof(H3dsScene));

	while ((dwChunkId != LWO2_ERROR) && !q) {

		dwChunkId = readDW();
		dwSize = readDW();

		// capture current position
		cpos = ftell(inFile);

		// process current chunk
		switch (dwChunkId) {

		case ID_LAYR:
			meshobj = processLAYR(dwSize, scene);
			break;

		case ID_TAGS:
			processTAGS(dwSize, scene);
			break;

		case ID_PNTS:
			processPNTS(dwSize, meshobj);
			break;

		case ID_BBOX:
			break;

		case ID_VMAP:
			processVMAP_VMAD(dwSize, meshobj, 0);
			break;

		case ID_VMAD:
			processVMAP_VMAD(dwSize, meshobj, 1);
			break;

		case ID_POLS:
			processPOLS(dwSize, meshobj);
			break;

		case ID_PTAG:
			processPTAG(dwSize, meshobj);
			break;

		case ID_ENVL:
			break;

		case ID_CLIP:
			processCLIP(dwSize, scene);
			break;

		case ID_SURF:
			processSURF(dwSize, scene);
			break;
		}

		// go to the next chunk
		fseek(inFile, cpos + dwSize, SEEK_SET);
		if (ftell(inFile) >= dwFileSize + 8) q = 1;
	}

	updateSurfaces(scene);
	postProcessUV(scene);

	scene->sourceFile = FILE_LWO;
	return scene;
}
