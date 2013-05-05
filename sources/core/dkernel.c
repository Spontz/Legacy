/*
	dkernel + script.c: demo kernel and scripting code
*/

#ifdef WIN32
	#include <io.h>
#endif

#include "../main.h"
#include "timing.h"
#include "text.h"
#include "files.h"
#include "debug.h"
#include "drivers/events.h"

#include "../interface/sections.h"

// ******************************************************************

char *log_separator = "============================================================\n";

// ******************************************************************

// demo system data
tDemoSystem demoSystem;

// pointer to actual section data
tDemoSection *mySection;

// ******************************************************************
// scripting
// ******************************************************************

#define VTYPE_INT		0
#define VTYPE_FLOAT		1
#define VTYPE_STRING	2

typedef struct {
	char *cName;
	char vType;
	void *vAddr;
} tScriptCommand;

static tScriptCommand scriptCommand[] = {

	{"demo_name",		VTYPE_STRING,		&demoSystem.demoName		},
	{"config_dialog",	VTYPE_INT,			&demoSystem.configDialog	},
	{"debug",			VTYPE_INT,			&demoSystem.debug			},
	{"record",			VTYPE_INT,			&demoSystem.record			},
	{"record_fps",		VTYPE_FLOAT,		&demoSystem.recordFps		},
	{"compress_tga",	VTYPE_INT,			&demoSystem.compressTga		},
	{"loop",			VTYPE_INT,			&demoSystem.loop			},
	{"sound",			VTYPE_INT,			&demoSystem.sound			},
	{"bench",			VTYPE_INT,			&demoSystem.bench			},
	{"demo_start",		VTYPE_FLOAT,		&demoSystem.startTime		},
	{"demo_end",		VTYPE_FLOAT,		&demoSystem.endTime			},
	{"slave",			VTYPE_INT,			&demoSystem.slaveMode		},

	{"gl_fullscreen",	VTYPE_INT,			&glDriver.fullScreen		},
	{"gl_info",			VTYPE_INT,			&glDriver.saveInfo			},
	{"gl_width",		VTYPE_INT,			&glDriver.width				},
	{"gl_height",		VTYPE_INT,			&glDriver.height			},
	{"gl_bpp",			VTYPE_INT,			&glDriver.bpp				},
	{"gl_zbuffer",		VTYPE_INT,			&glDriver.zbuffer			},
	{"gl_stencil",		VTYPE_INT,			&glDriver.stencil			},
	{"gl_accum",		VTYPE_INT,			&glDriver.accum				},
	{"gl_multisampling",VTYPE_INT,			&glDriver.multisampling		},

	{"gl_gamma",		VTYPE_FLOAT,		&glDriver.gamma				},

	{"fbo_0_width",		VTYPE_INT,			&glDriver.fbo[0].width		},
	{"fbo_0_height",	VTYPE_INT,			&glDriver.fbo[0].height		},
	{"fbo_0_format",	VTYPE_STRING,		&glDriver.fbo[0].format		},

	{"fbo_1_width",		VTYPE_INT,			&glDriver.fbo[1].width		},
	{"fbo_1_height",	VTYPE_INT,			&glDriver.fbo[1].height		},
	{"fbo_1_format",	VTYPE_STRING,		&glDriver.fbo[1].format		},

	{"fbo_2_width",		VTYPE_INT,			&glDriver.fbo[2].width		},
	{"fbo_2_height",	VTYPE_INT,			&glDriver.fbo[2].height		},
	{"fbo_2_format",	VTYPE_STRING,		&glDriver.fbo[2].format		},

	{"fbo_3_width",		VTYPE_INT,			&glDriver.fbo[3].width		},
	{"fbo_3_height",	VTYPE_INT,			&glDriver.fbo[3].height		},
	{"fbo_3_format",	VTYPE_STRING,		&glDriver.fbo[3].format		},

	{"fbo_4_width",		VTYPE_INT,			&glDriver.fbo[4].width		},
	{"fbo_4_height",	VTYPE_INT,			&glDriver.fbo[4].height		},
	{"fbo_4_format",	VTYPE_STRING,		&glDriver.fbo[4].format		},

	{"fbo_5_width",		VTYPE_INT,			&glDriver.fbo[5].width		},
	{"fbo_5_height",	VTYPE_INT,			&glDriver.fbo[5].height		},
	{"fbo_5_format",	VTYPE_STRING,		&glDriver.fbo[5].format		},

	{"fbo_6_width",		VTYPE_INT,			&glDriver.fbo[6].width		},
	{"fbo_6_height",	VTYPE_INT,			&glDriver.fbo[6].height		},
	{"fbo_6_format",	VTYPE_STRING,		&glDriver.fbo[6].format		},

	{"fbo_7_width",		VTYPE_INT,			&glDriver.fbo[7].width		},
	{"fbo_7_height",	VTYPE_INT,			&glDriver.fbo[7].height		},
	{"fbo_7_format",	VTYPE_STRING,		&glDriver.fbo[7].format		},

	{"fbo_8_width",		VTYPE_INT,			&glDriver.fbo[8].width		},
	{"fbo_8_height",	VTYPE_INT,			&glDriver.fbo[8].height		},
	{"fbo_8_format",	VTYPE_STRING,		&glDriver.fbo[8].format		},

	{"fbo_9_width",		VTYPE_INT,			&glDriver.fbo[9].width		},
	{"fbo_9_height",	VTYPE_INT,			&glDriver.fbo[9].height		},
	{"fbo_9_format",	VTYPE_STRING,		&glDriver.fbo[9].format		},

	{"fbo_10_width",	VTYPE_INT,			&glDriver.fbo[10].width		},
	{"fbo_10_height",	VTYPE_INT,			&glDriver.fbo[10].height	},
	{"fbo_10_format",	VTYPE_STRING,		&glDriver.fbo[10].format	},

	{"fbo_11_width",	VTYPE_INT,			&glDriver.fbo[11].width		},
	{"fbo_11_height",	VTYPE_INT,			&glDriver.fbo[11].height	},
	{"fbo_11_format",	VTYPE_STRING,		&glDriver.fbo[11].format	},

	{"fbo_12_width",	VTYPE_INT,			&glDriver.fbo[12].width		},
	{"fbo_12_height",	VTYPE_INT,			&glDriver.fbo[12].height	},
	{"fbo_12_format",	VTYPE_STRING,		&glDriver.fbo[12].format	},

	{"fbo_13_width",	VTYPE_INT,			&glDriver.fbo[13].width		},
	{"fbo_13_height",	VTYPE_INT,			&glDriver.fbo[13].height	},
	{"fbo_13_format",	VTYPE_STRING,		&glDriver.fbo[13].format	},

	{"fbo_14_width",	VTYPE_INT,			&glDriver.fbo[14].width		},
	{"fbo_14_height",	VTYPE_INT,			&glDriver.fbo[14].height	},
	{"fbo_14_format",	VTYPE_STRING,		&glDriver.fbo[14].format	},

	{"fbo_15_width",	VTYPE_INT,			&glDriver.fbo[15].width		},
	{"fbo_15_height",	VTYPE_INT,			&glDriver.fbo[15].height	},
	{"fbo_15_format",	VTYPE_STRING,		&glDriver.fbo[15].format	},

	{"fbo_16_width",	VTYPE_INT,			&glDriver.fbo[16].width		},
	{"fbo_16_height",	VTYPE_INT,			&glDriver.fbo[16].height	},
	{"fbo_16_format",	VTYPE_STRING,		&glDriver.fbo[16].format	},

	{"fbo_17_width",	VTYPE_INT,			&glDriver.fbo[17].width		},
	{"fbo_17_height",	VTYPE_INT,			&glDriver.fbo[17].height	},
	{"fbo_17_format",	VTYPE_STRING,		&glDriver.fbo[17].format	},

	{"fbo_18_width",	VTYPE_INT,			&glDriver.fbo[18].width		},
	{"fbo_18_height",	VTYPE_INT,			&glDriver.fbo[18].height	},
	{"fbo_18_format",	VTYPE_STRING,		&glDriver.fbo[18].format	},

	{"fbo_19_width",	VTYPE_INT,			&glDriver.fbo[19].width		},
	{"fbo_19_height",	VTYPE_INT,			&glDriver.fbo[19].height	},
	{"fbo_19_format",	VTYPE_STRING,		&glDriver.fbo[19].format	},

};

#define COMMANDS_NUMBER (sizeof(scriptCommand) / sizeof(tScriptCommand))

// ******************************************************************

// defined section commands
#define SECTION_IDENTIFIER	0
#define SECTION_START		1
#define SECTION_END			2
#define SECTION_LAYER		3
#define SECTION_BLEND		4
#define SECTION_ALPHA		5
#define SECTION_PARAM		6
#define SECTION_STRING		7
#define SECTION_SPLINE		8
#define SECTION_MODIFIER	9
#define SECTION_ENABLED    10


// defined section reserved keys
#define SECTION_COMMANDS_NUMBER 11

static char *scriptSectionCommand[SECTION_COMMANDS_NUMBER] = {

	"id", "start", "end", "layer", "blend", "alpha",
	"param", "string", "spline", "modify", "enabled"

};

// ******************************************************************

typedef struct {

	char *name;
	int code;

} glTable_t;

// ******************************************************************

#define BLEND_FUNC 15

glTable_t blendFunc[BLEND_FUNC] = {

	{ "ZERO",						GL_ZERO							},
	{ "ONE",						GL_ONE							},
	{ "DST_COLOR",					GL_DST_COLOR					},
	{ "ONE_MINUS_DST_COLOR",		GL_ONE_MINUS_DST_COLOR			},
	{ "SRC_ALPHA",					GL_SRC_ALPHA					},
	{ "ONE_MINUS_SRC_ALPHA",		GL_ONE_MINUS_SRC_ALPHA			},
	{ "DST_ALPHA",					GL_DST_ALPHA					},
	{ "ONE_MINUS_DST_ALPHA",		GL_ONE_MINUS_DST_ALPHA			},
	{ "CONSTANT_COLOR",				GL_CONSTANT_COLOR				},
	{ "ONE_MINUS_CONSTANT_COLOR",	GL_ONE_MINUS_CONSTANT_COLOR		},
	{ "CONSTANT_ALPHA",				GL_CONSTANT_ALPHA				},
	{ "ONE_MINUS_CONSTANT_ALPHA",	GL_ONE_MINUS_CONSTANT_ALPHA		},
	{ "SRC_ALPHA_SATURATE",			GL_SRC_ALPHA_SATURATE			},
	{ "SRC_COLOR",					GL_SRC_COLOR					},
	{ "ONE_MINUS_SRC_COLOR",		GL_ONE_MINUS_SRC_COLOR			},

	// NOTAS - OpenGL spec 1.2.1
	// DST_COLOR es especifico de source factor
	// SRC_COLOR es especifico de destination factor
	// pero... no quiere decir que no funcionen
};

// ******************************************************************

#define ALPHA_FUNC 8

glTable_t alphaFunc[ALPHA_FUNC] = {

	{ "NEVER",		GL_NEVER	},
	{ "LESS",		GL_LESS		},
	{ "EQUAL",		GL_EQUAL	},
	{ "LEQUAL",		GL_LEQUAL	},
	{ "GREATER",	GL_GREATER	},
	{ "NOTEQUAL",	GL_NOTEQUAL	},
	{ "GEQUAL",		GL_GEQUAL	},
	{ "ALWAYS",		GL_ALWAYS	}
};

// ******************************************************************

#define RENDER_MODE 13

glTable_t renderMode[RENDER_MODE] = {

	{ "vertexes",		RMODE_VERTEXES			},
	{ "wireframe",		RMODE_WIRE				},
	{ "color",			RMODE_FILL				},
	{ "flat",			RMODE_FLAT				},
	{ "shaded",			RMODE_SHADED			},
	{ "normals",		RMODE_NORMALS			},
	{ "texture",		RMODE_TEXTURE			},
	{ "environment",	RMODE_ENV				},
	{ "cell",			RMODE_CELL				},
	{ "outline",		RMODE_OUTLINE			},
	{ "bump",			RMODE_BUMPDOT3			},
	{ "glsl",			RMODE_GLSL				},
	{ "binormal",		RMODE_GLSL_BINORMAL		},
};

// ******************************************************************

typedef struct {
	
	char *name;
	int tex_iformat;		// internalformat
	int tex_format;
	int tex_type;

} glTexTable_t;

// ******************************************************************

#define TEXTURE_MODE 6

glTexTable_t textureModes[TEXTURE_MODE] = {
	{ "RGBA",				GL_RGBA8,		GL_RGBA,		GL_UNSIGNED_BYTE },
	{ "RGB",				GL_RGB8,		GL_RGB,			GL_UNSIGNED_BYTE },
	{ "RGBA16F",			GL_RGBA16F_ARB,	GL_RGBA,		GL_FLOAT },
	{ "RGB16F",				GL_RGB16F_ARB,	GL_RGB,			GL_FLOAT },
	{ "RGBA32F",			GL_RGBA32F_ARB,	GL_RGBA,		GL_FLOAT},
	{ "RGB32F",				GL_RGB32F_ARB,	GL_RGB,			GL_FLOAT},
};

// ******************************************************************

int getTexInternalFormatByName(char *name, glTexTable_t *table, int size) {
	
	int i;
	
	for (i=0; i<size; i++) {
		if (spz_strcmpi(name, table[i].name) == 0) {
			return table[i].tex_iformat;
		}
	}
	
	return -1;
}

// ******************************************************************

int getTexFormatByName(char *name, glTexTable_t *table, int size) {
	
	int i;
	
	for (i=0; i<size; i++) {
		if (spz_strcmpi(name, table[i].name) == 0) {
			return table[i].tex_format;
		}
	}
	
	return -1;
}

// ******************************************************************

int getTexTypeByName(char *name, glTexTable_t *table, int size) {
	
	int i;
	
	for (i=0; i<size; i++) {
		if (spz_strcmpi(name, table[i].name) == 0) {
			return table[i].tex_type;
		}
	}
	
	return -1;
}

// ******************************************************************

int getCodeByName(char *name, glTable_t *table, int size) {

	int i;

	for (i=0; i<size; i++) {
		if (spz_strcmpi(name, table[i].name) == 0) {
			return table[i].code;
		}
	}

	return -1;
}

// ******************************************************************

int getBlendCodeByName(char *name) {
	return getCodeByName(name, blendFunc, BLEND_FUNC);
}

// ******************************************************************

int getAlphaCodeByName(char *name) {
	return getCodeByName(name, alphaFunc, ALPHA_FUNC);
}

// ******************************************************************

int getRenderModeByName(char *name) {
	return getCodeByName(name, renderMode, RENDER_MODE);
}

// ******************************************************************

int getTextureFormatByName(char *name) {
	return getTexFormatByName(name, textureModes, TEXTURE_MODE);
}

// ******************************************************************

int getTextureInternalFormatByName(char *name) {
	return getTexInternalFormatByName(name, textureModes, TEXTURE_MODE);
}

// ******************************************************************

int getTextureTypeByName(char *name) {
	return getTexTypeByName(name, textureModes, TEXTURE_MODE);
}

// ******************************************************************

int getSectionByName(char *name) {

	int i;

	// get section index
	for (i=0; i<SECTIONS_NUMBER; i++) {
		if (spz_strcmpi(name,sectionFunction[i].scriptName) == 0) {
			return i;
		}
	}

	return -1;
}

// ******************************************************************

void dkernel_getArguments(int argc, char *argv[])
	{
	// deprecated

	/*
	char *lastSlash;
	int chars;

	demoSystem.argc = argc;
	demoSystem.argv = argv;

	if (argc > 1) {
		demoSystem.demoDir = argv[1];
	}
	else {
		lastSlash = strrchr(demoSystem.argv[0], '\\');
		if (lastSlash == NULL) {
			demoSystem.demoDir = ".";
		} else {
			chars = strlen(demoSystem.argv[0]) - strlen(lastSlash);
			demoSystem.demoDir = (char *) malloc(chars + 1);
			strncpy(demoSystem.demoDir, demoSystem.argv[0], chars);
			demoSystem.demoDir[chars] = 0;
		}
	}
	*/

	demoSystem.demoDir = ".";

	#ifdef WIN32
		demoSystem.hInstance = (long) GetModuleHandle(NULL);
	#endif
	}

// ******************************************************************

void dkernel_loadScriptData(const char* pScript, const char* pSource)
	{
	const char*		name = "";
	char			line[256], key[512], value[512], tmp[512], tmp2[512];
	tDemoSection*	section;
	int				lineNum, com, i, values;
	float			fvalue;
	unsigned int	startPosition = 0;

	// initialize script parse variables
	lineNum = 0;
	section = NULL;
	for (;;)
		{
		lineNum++;

		//line = calloc(256, sizeof(char));
		memset(line, '\0', 256);
		
		// get current script line
		// break if end of file
		
		// This function was returning startPosition > 0 when there weren't additional lines in the text variable, but only when
		// it was called in a very fast way (for example, loading two sections coming from the network very fast, one after another)
		// So we added a protection inside the function to return 0 if the startPosition is invalid (larger than the string length)
		startPosition = getNextLine(pScript, line, startPosition);

		// No more lines to process
		if (startPosition == 0)
			break;

		if (line[0] == '\0')
			continue;
		
		// fprintf(stdout,"    Parsing line %i... \n", lineNum);

		// Ignore comments or empty line
		if ((line[0] == ';') || (line[0] == '\n') || (line[0] == '\r'))
			{
			//fprintf(stdout,"comments found or empty line: ignoring this line.\n");
			continue;
			}
		
		// section type line
		if (line[0] == '[')
			{
			// The section type goes to the "key" variable
			sscanf(line, "[%s]", key);
			key[strlen(key) - 1] = 0;

			// create the new section...
			section = (tDemoSection *) malloc(sizeof(tDemoSection));
			memset(section, 0, sizeof(tDemoSection));

			strcpy(section->DataSource, pSource);

			// ...and queue it!
			if (demoSystem.demoSection != NULL)
				demoSystem.demoSection->prior = section;
			
			section->next = demoSystem.demoSection;
			demoSystem.demoSection = section;
			
			// by default the section is enabled and marked as not loaded
			section->enabled = 1;
			section->loaded = 0;
			
			dkernel_trace("*************** %s ***************", key);

			i = getSectionByName(key);
			if (i < 0)
				parse_error(name, lineNum, "Unknown section type.");

			section->staticSectionIndex = i;
			}
		else if (section)
			{
			// section body
			getKeyValue(line,key,value);

			// select local variable
			com = -1;
			for (i=0; i<SECTION_COMMANDS_NUMBER; i++)
				{
				if (spz_strcmpi(key,scriptSectionCommand[i]) == 0)
					{
					com = i;
					break;
					}
				}

			// if the command is not recognized
			// look at the first character (fColor, cSpline, sTexture)
			if (com == -1)
				{
				switch (key[0])
					{
					case 'f':
						com = SECTION_PARAM;
						break;
					case 's':
						com = SECTION_STRING;
						break;
					case 'c':
						com = SECTION_SPLINE;
						break;
					case 'm':
						com = SECTION_MODIFIER;
						break;
					}
				}

			// load variable
			switch (com)
				{
				// identifier
				case SECTION_IDENTIFIER:
					values = sscanf(value,"%s", (char *)&section->identifier);
					break;

				// enabled
				case SECTION_ENABLED:
					values = sscanf(value,"%i",&section->enabled);
					// if ((section->enabled != 1) || (section->enabled != 0)) parse_error(name, lineNum, "Invalid enabled state");
					dkernel_trace("section enabled state: %i", section->enabled);
					break;

				// start
				case SECTION_START:
					{
					values = sscanf(value,"%f",&section->startTime);

					if ((values != 1) || (section->startTime < 0))
						parse_error(name, lineNum, "Invalid start");

					dkernel_trace("section start: %f", section->startTime);
					break;
					}

				// end
				case SECTION_END:
					{
					values = sscanf(value, "%f", &section->endTime);

					if (values != 1)
						parse_error(name, lineNum, "Invalid end");

					section->duration = section->endTime - section->startTime;

					if (section->duration <= 0)
						parse_error(name, lineNum, "Invalid duration");

					dkernel_trace("section end: %f", section->endTime);
					break;
					}

				// layer
				case SECTION_LAYER:
					values = sscanf(value, "%i", &section->layer);

					if (values != 1)
						parse_error(name, lineNum, "Invalid layer");

					dkernel_trace("section layer: %i", section->layer);
					break;

				// blending function
				case SECTION_BLEND:
					{
					values = sscanf(value,"%s %s",tmp,tmp2);

					if (values != 2)
						parse_error(name, lineNum, "Invalid blend");

					section->sfactor = getBlendCodeByName(tmp);
					section->dfactor = getBlendCodeByName(tmp2);

					if ((section->sfactor == -1) || (section->dfactor == -1))
						parse_error(name, lineNum, "Invalid blend factor");

					section->hasBlend = TRUE;
					dkernel_trace("section blending: source %i and destination %i", section->sfactor, section->dfactor);
					break;
					}

				// alpha function
				case SECTION_ALPHA:
					{
					values = sscanf(value,"%s %f %f",tmp,&section->alpha1,&section->alpha2);
					switch (values)
						{
						case 2:
							section->alpha2 = section->alpha1;
							break;
						case 3:
							break;
						default:
							parse_error(name, lineNum, "Invalid alpha");
							break;
						}

					section->alphaFunc = getAlphaCodeByName(tmp);

					if (section->alphaFunc == -1)
						parse_error(name, lineNum, "Invalid alpha function");

					section->hasAlpha = TRUE;
					dkernel_trace("section alpha: from %f to %f", section->alpha1, section->alpha2);
					break;
					}

				// param
				case SECTION_PARAM:
					{
					if (section->paramNum >= SECTION_PARAMS)
						parse_error(name, lineNum, "Too many params.");

					values = getFloatVector(value,&section->param[section->paramNum],SECTION_PARAMS);
					section->paramNum += values;
					
					dkernel_trace("section parameter: %s = %f (%i found)", key, section->param[section->paramNum - 1], values);
					break;
					}

				// string
				case SECTION_STRING:
					{
					if (section->stringNum >= SECTION_STRINGS)
						parse_error(name, lineNum, "Too many strings");

					section->strings[section->stringNum] = strdup(value);
					section->stringNum++;
					
					dkernel_trace("found string \"%s\"", value);
					break;
					}

				// spline
				case SECTION_SPLINE:
					{
					if (section->splineNum >= SECTION_SPLINES)
						parse_error(name, lineNum, "Too many splines");

					values = sscanf(value, "%s %f", tmp, &fvalue);

					switch (values)
						{
						case 1:
							fvalue = section->duration;
							break;
						case 2:
							break;
						default:
							parse_error(name, lineNum, "Invalid spline");
							break;
						}

					section->splineFiles[section->splineNum] = strdup(tmp);
					section->splineDuration[section->splineNum] = fvalue;
					section->splineNum++;
					dkernel_trace("section: found spline");
					break;
					}

				/*
				// modifier
				case SECTION_MODIFIER:
					if (section->modifierNum >= SECTION_MODIFIERS)
						parse_error("Too many modifiers", name, lineNum);

					values = sscanf(value,"%d %d %s",
									&section->modifiers[section->modifierNum].spline,
									&section->modifiers[section->modifierNum].param,
									tmp);
					if (values != 3)
						parse_error("Invalid modifier", name, lineNum);

					if (strcmp(tmp,"BEAT") == 0) {
						section->modifiers[section->modifierNum].mode = 1;
					}
					else {
						section->modifiers[section->modifierNum].mode = 0;
					}

					section->modifierNum++;
					break;
				*/

				default:
					dkernel_trace("Examined line: \"%s\" but ", line);
					parse_error(name, lineNum, "Unknown section variable (id: %d) was found.", com);
					break;
				}
			}
		else
			{
			// global variables
			getKeyValue(line, key, value);

			// generic variable loading
			for (i = 0; i < COMMANDS_NUMBER; ++i)
				{
				if (spz_strcmpi(key, scriptCommand[i].cName) == 0)
					{
					switch(scriptCommand[i].vType)
						{
						case VTYPE_INT:
							sscanf(value,"%d",(int *)((char**)scriptCommand[i].vAddr));
							break;
						case VTYPE_FLOAT:
							sscanf(value,"%f",(float *)((char**)scriptCommand[i].vAddr));
							break;
						case VTYPE_STRING:
							*((char**)scriptCommand[i].vAddr) = strdup(value);
							break;
						default:
							parse_error(name, lineNum, "%d is not a valid variable type id.", scriptCommand[i].vType);
							break;
						}
					break;
					}
				}

			if (i >= COMMANDS_NUMBER)
				parse_error(pSource, lineNum, "%s is not a valid SPO Script variable.", key);
			}
		}
	}

// ******************************************************************

void loadScriptFile(const char* pPath)
	{
	char* pScript;

	dkernel_trace("Loading SPO Script %s...\n", pPath);
	pScript = loadFile(pPath);
	
	dkernel_loadScriptData(pScript, pPath);
	
	free(pScript);
	}

// ******************************************************************

#ifdef WIN32

void load_spos(char *dir)
	{
	struct _finddata_t FindData;
	intptr_t hFile;
	char fullpath[512];
	char ScriptRelativePath[512];
	strcpy(fullpath, dir);
	strcat(fullpath, "/*.spo");
	if( (hFile = _findfirst( fullpath, &FindData )) != -1L )
		{
		do
			{
			strcpy(ScriptRelativePath, dir);
			strcat(ScriptRelativePath, "\\");
			strcat(ScriptRelativePath, FindData.name);
			loadScriptFile(ScriptRelativePath);
			}
		while( _findnext( hFile, &FindData ) == 0 );
		}
	}

#endif

#ifdef __APPLE__

void load_spos(char *dir)
	{
	char spec[512];
	DIR *dirData;
	struct dirent *dirent;

	dirData = opendir(dir);
	
	if (!dirData) return;
	
	fprintf(stderr,"Directory %s opened.\n", dir);
	fprintf(stderr,"Begin scanning %s\n", dir);

 	while ((dirent = readdir(dirData)) != NULL) {

		// skip "." and ".."
		if ((spz_strcmpi(dirent->d_name,".") != 0) && (spz_strcmpi(dirent->d_name,"..") != 0)) {
			strcpy(spec, dir);
			strcat(spec, "/");
			strcat(spec, dirent->d_name);

			// recurse into a possible subdirectory
			// load_spos(spec);

			if (strstr(dirent->d_name,".spo")) {
				// try to load the script
				fprintf(stderr,"Loading Script file from %s\n", spec);
				loadScriptFile(spec);
				fprintf(stderr,"Script Loaded\n");
			}
		}
	}

	fprintf(stderr,"End reading from %s\n", dir);


	closedir(dirData);
	}


#endif

// ******************************************************************

void dkernel_loadScripts()
	{
	// initialize global kernel variables
	demoSystem.demoName = "Spontz Visuals Engine";
	demoSystem.configDialog = FALSE;
	demoSystem.debug = FALSE;
	demoSystem.record = FALSE;
	demoSystem.recordFps = 25.0f;
	demoSystem.loop = TRUE;
	demoSystem.sound = TRUE;
	demoSystem.bench = FALSE;
	demoSystem.startTime = 0.0f;
	demoSystem.endTime = 20.0f;
	demoSystem.slaveMode = FALSE;

	// initialize main queue
	demoSystem.demoSection = NULL;

	dkernel_trace("dkernel: variables inited to default state.\n");

	// initialize driver variables
	gldrv_create();

	dkernel_trace("OpenGL environment created.\n");

	#ifndef DEBUG
		#ifdef WIN32
			SetCurrentDirectoryA(demoSystem.demoDir);
		#endif
	#endif

	// load all scripts
	load_spos("data");
	
	dkernel_trace("Scripts loaded\n");

}

// ******************************************************************

void dkernel_configDialog() {

	if (demoSystem.configDialog) {
		// call to a procedure inside dialog.c
		// that displays the dialog and change config
#ifdef WIN32
		// dialogDisplay();
#endif
	}
}

// ******************************************************************

void init_control_vars() {

	// reset time
	demoSystem.runTime = demoSystem.startTime;

	// reset control time variables
	demoSystem.frameTime = 0;
	demoSystem.realFrameTime = 0;
	demoSystem.frameCount = 0;
	demoSystem.accumFrameCount = 0;
	demoSystem.accumFrameTime = 0;
	demoSystem.fps = 0;
	demoSystem.exitDemo = FALSE;

	// reset random number generator
	rand_seed(0,0);
}

// ******************************************************************
// queues management
// ******************************************************************
void loadSplines(tDemoSection *ds) {
	int i,j;
	float motionStepTime;
	Motion *motion;

	if (ds->splineNum > 0) {

		for (i=0; i<ds->splineNum; i++) {
			motion = motion_load(ds->splineFiles[i]);
			
			if (motion == NULL) return;
			
			motion->steps = ds->splineDuration[i];
			motionStepTime = motion->steps / (motion->keys - 1);
			
			for (j=0; j<motion->keys; j++) {
				motion->keylist[j].step = motionStepTime * j;
			}
			
			ds->splines[i] = motion;
		}
	}
}

void init_sectionQueues()
	{
	tDemoSection* ds;
	tDemoSection* ds_tmp;
	tDemoSection* loadingSection;
	tSectionFunction* loading;
	SDL_Event event;
	float startTime = 0.0f;
	int i;
	char isLast;

	// Set the demo state to loading
	demoSystem.state = DEMO_LOADING;

	if (demoSystem.debug)
		{
		fprintf(stdout, "\n%s", log_separator);
		fprintf(stdout, "loading: start\n");
		startTime = (float) SDL_GetTicks();
		}

	loadingSection = NULL;
	loading = NULL;
	ds = demoSystem.demoSection;

	while (ds)
		{
		if (ds->staticSectionIndex == 0)
			{
			loadingSection = ds;
			loading = &sectionFunction[loadingSection->staticSectionIndex];
			break;
			}
		ds = ds->next;
		}

	if (!loadingSection) {
		dkernel_trace("Loading section not found: using default loader");

		// create a new section as loader...
		loadingSection = (tDemoSection *) malloc(sizeof(tDemoSection));
		memset(loadingSection, 0, sizeof(tDemoSection));
		loading = &sectionFunction[loadingSection->staticSectionIndex];
	}
		
	// section accounting
	demoSystem.numSections = 0;
	demoSystem.numReadySections = 0;
	demoSystem.loadedSections = 0;

	// preload, load and init loading section
	mySection = loadingSection;
	loading->preload();
	loading->load();
	loading->init();
	loading->exec();

	// reset section queues
	demoSystem.readySection = NULL;
	demoSystem.runSection = NULL;

	if (demoSystem.debug)
		dkernel_trace("loading: section setup complete, %.2f seconds\n", 0.001f * ((float) SDL_GetTicks() - startTime));

	// view all sections looking for ready sections
	ds = demoSystem.demoSection;
	while (ds != NULL) {

		if (    (  demoSystem.slaveMode == 1  ) || (  ( (ds->startTime < demoSystem.endTime) || fabs(demoSystem.endTime) < FLT_EPSILON ) && ( ds->endTime > demoSystem.startTime )  )    )
			{
			demoSystem.numReadySections++;

			// load section splines (to avoid code load in the sections)
			loadSplines(ds);

			// first list element
			if (!demoSystem.readySection) {
				demoSystem.readySection = ds;

			// ordered insert on ready list
			} else {
				ds_tmp = demoSystem.readySection;
				isLast = FALSE;
				while ((!isLast) && (ds_tmp->startTime < ds->startTime)) {
					if (ds_tmp->nextRdy) ds_tmp = ds_tmp->nextRdy;
					else isLast = TRUE;
				}

				if (isLast) {
					ds_tmp->nextRdy = ds;
					ds->priorRdy = ds_tmp;
				} else {
					if (ds_tmp->priorRdy) (*(tDemoSection*)ds_tmp->priorRdy).nextRdy = ds;
					else demoSystem.readySection = ds;
					ds->priorRdy = ds_tmp->priorRdy;
					ds->nextRdy = ds_tmp;
					ds_tmp->priorRdy = ds;
				}
			}
		}
		ds = ds->next;
	}

	// all sections will be preloaded except loading section
	demoSystem.numSections += SECTIONS_NUMBER-1;

	if (demoSystem.debug)
		{
		fprintf(stdout, "loading: build ready queue, %.2f seconds\n", 0.001f * ((float) SDL_GetTicks() - startTime));
		fprintf(stdout, "loading: %d sections to be preloaded\n", demoSystem.numSections);
		}

	// preload all used demo sections
	for (i = 1; i < SECTIONS_NUMBER; ++i)
		{
		sectionFunction[i].preload();
		++demoSystem.loadedSections;

		// update loading
		loading->exec();

		// event handler
		if (SDL_PollEvent(&event)) eventHandler(event);

		if (demoSystem.exitDemo)
			{
			dkernel_closeDemo();
			exit(EXIT_SUCCESS);
			}
		}

	if (demoSystem.debug)
		{
		fprintf(stdout, "loading: sections preloaded, %.2f seconds\n", 0.001f * ((float) SDL_GetTicks() - startTime));
		fprintf(stdout, "loading: %d sections to be loaded\n", demoSystem.numReadySections);
		}

	#ifdef _DEBUG
		fprintf(stdout, "ready >");
	#endif

	// load all ready sections
	ds = demoSystem.readySection;
	while (ds != NULL)
		{
		#ifdef _DEBUG
			fprintf(stdout, " %s.load", sectionFunction[ds->staticSectionIndex].scriptName);
		#endif

		// section load
		mySection = ds;
		sectionFunction[ds->staticSectionIndex].load();
		demoSystem.loadedSections++;

		ds = ds->nextRdy;

		// update loading
		mySection = loadingSection;
		loading->exec();

		// event handler
		if (SDL_PollEvent(&event)) eventHandler(event);

		if (demoSystem.exitDemo)
			{
			dkernel_closeDemo();
			exit(EXIT_SUCCESS);
			}
		}

	if (demoSystem.debug)
		fprintf(stdout, "\nloading: sections loaded, %.2f seconds\n", 0.001f * ((float) SDL_GetTicks() - startTime));

	// end loading
	loading->end();
	}

// ******************************************************************

void reinit_sectionQueues() {

	tDemoSection *ds, *ds_tmp;
	char isLast;

	// reset section queues
	demoSystem.readySection = NULL;
	ds = demoSystem.runSection;
	demoSystem.runSection = NULL;
	while (ds != NULL) {

		mySection = ds;
		sectionFunction[ds->staticSectionIndex].end();
		ds = ds->nextRun;
	}

	// view all sections looking for ready sections
	ds = demoSystem.demoSection;
	while (ds != NULL) {

		// reset section vars
		ds->nextRdy = ds->priorRdy = NULL;
		ds->nextRun = ds->priorRun = NULL;
		ds->runTime = 0;

		if (((ds->startTime < demoSystem.endTime) || fabs(demoSystem.endTime) < FLT_EPSILON) && (ds->endTime > demoSystem.runTime)) {

			// first list element
			if (!demoSystem.readySection) {
				demoSystem.readySection = ds;

			// ordered insert on ready list
			} else {
				ds_tmp = demoSystem.readySection;
				isLast = FALSE;
				while ((!isLast) && (ds_tmp->startTime < ds->startTime)) {
					if (ds_tmp->nextRdy) ds_tmp = ds_tmp->nextRdy;
					else isLast = TRUE;
				}

				if (isLast) {
					ds_tmp->nextRdy = ds;
					ds->priorRdy = ds_tmp;
				} else {
					if (ds_tmp->priorRdy) (*(tDemoSection*)ds_tmp->priorRdy).nextRdy = ds;
					else demoSystem.readySection = ds;
					ds->priorRdy = ds_tmp->priorRdy;
					ds->nextRdy = ds_tmp;
					ds_tmp->priorRdy = ds;
				}
			}
		}
		ds = ds->next;
	}
}

// ******************************************************************

void process_sectionQueues() {

	char OGLError[1024];
	tDemoSection *ds, *ds_tmp;
	char isLast;

#ifdef _DEBUG
	fprintf(stdout, "\n\nsecond %f\n", demoSystem.runTime);
	fprintf(stdout, "run (finished sections) >");
#endif

	// loop run sections looking for finished sections
	ds = demoSystem.runSection;
	while (ds != NULL) {

		if (ds->endTime <= demoSystem.runTime) {

			// remove from run queue
			if (ds->priorRun) (*(tDemoSection*)ds->priorRun).nextRun = ds->nextRun;
			else demoSystem.runSection = ds->nextRun;
			if (ds->nextRun) (*(tDemoSection*)ds->nextRun).priorRun = ds->priorRun;

#ifdef _DEBUG
			fprintf(stdout, " %s.end", sectionFunction[ds->staticSectionIndex].scriptName);
#endif

			// execute end
			mySection = ds;
			sectionFunction[ds->staticSectionIndex].end();
		}

		ds = ds->nextRun;
	}

#ifdef _DEBUG
	fprintf(stdout, "\nready >");
#endif

	// view ready sections looking for run sections
	ds = demoSystem.readySection;
	while (ds != NULL) {

		// direct reject of all next sections
		if (ds->startTime > demoSystem.runTime) break;

		// if finished section do nothing
		if (ds->endTime <= demoSystem.runTime) {
#ifdef _DEBUG
			fprintf(stdout, " '%s' skipped", sectionFunction[ds->staticSectionIndex].scriptName);
#endif
		}

		// else execute init and add it to run queue
		else {
			// setup the runtime value
			ds->runTime = demoSystem.runTime - ds->startTime;

#ifdef _DEBUG
			fprintf(stdout, " %s.init", sectionFunction[ds->staticSectionIndex].scriptName);
#endif

			// section init
			mySection = ds;
			sectionFunction[ds->staticSectionIndex].init();

			// first element in run queue
			if (!demoSystem.runSection) {
				demoSystem.runSection = ds;

			} else {
				// ordered insert on run list
				ds_tmp = demoSystem.runSection;
				isLast = FALSE;
				while ((!isLast) && (ds_tmp->layer < ds->layer)) {
					if (ds_tmp->nextRun) ds_tmp = ds_tmp->nextRun;
					else isLast = TRUE;
				}

				if (isLast) {
					ds_tmp->nextRun = ds;
					ds->priorRun = ds_tmp;
				} else {
					if (ds_tmp->priorRun) (*(tDemoSection*)ds_tmp->priorRun).nextRun = ds;
					else demoSystem.runSection = ds;
					ds->priorRun = ds_tmp->priorRun;
					ds->nextRun = ds_tmp;
					ds_tmp->priorRun = ds;
				}
			}
		}

		// remove from ready queue
		if (ds->priorRdy) (*(tDemoSection*)ds->priorRdy).nextRdy = ds->nextRdy;
		else demoSystem.readySection = ds->nextRdy;
		if (ds->nextRdy) (*(tDemoSection*)ds->nextRdy).priorRdy = ds->priorRdy;

		ds = ds->nextRdy;
	}

	// prepare engine for render
	gldrv_initRender(TRUE);
	demoSystem.camera = NULL;

#ifdef _DEBUG
	fprintf(stdout, "\nrun >");
#endif

	// execute run sections
	ds = demoSystem.runSection;
	while (ds != NULL)
		{
		if (ds->endTime > demoSystem.runTime)
			{
			// section exec
			tex_reset_bind();
			mySection = ds;
			
			if ((ds->enabled) && (ds->loaded))
				{
				#ifdef _DEBUG
					dkernel_trace("dKernel: Rendering %s (%i)", sectionFunction[ds->staticSectionIndex].scriptName, ds->staticSectionIndex);
				#endif
					
				sectionFunction[ds->staticSectionIndex].exec();
				while (gl_drv_check_for_gl_errors(OGLError))
					dkernel_error("%s : The section has produced the following OGL error:\n\n%s", sectionFunction[ds->staticSectionIndex].scriptName, OGLError);
				}
			
			// add last frame time
			ds->runTime = demoSystem.runTime - ds->startTime;
			}
		else
			{
			dkernel_warn("Finished section not detected");
			}

		ds = ds->nextRun;
		}
	
	// Leave an empty line
	/*
	#ifdef _DEBUG
		dkernel_trace("");
	#endif
	*/

	// restore viewport
	gldrv_initRender(FALSE);

	// capture frame if we want to record a video
	if (demoSystem.record) gldrv_capture();

	// expand RTT to fullscreen
	gldrv_endRender();

	//if (demoSystem.drawFps) drawFps();
	//else if (demoSystem.drawTiming) drawTiming();

	// swap buffer
	gldrv_swap();
}

// ******************************************************************
// demo main functions
// ******************************************************************

void dkernel_initDemo() {
	// initialize graphics driver
	gldrv_init();

	// initialize fast events
	if (FE_Init() != 0)
		dkernel_error(FE_GetError());

	// initialize sound driver
	if (demoSystem.sound) sound_init();

	if (demoSystem.slaveMode) {
		// initialize network driver
		network_init();
	}
	
	// initialize global control variables
	init_control_vars();

	// prepare sections
	init_sectionQueues();

	// initialize debugging font
	// text_load_font(&debugFont, strDebugFont, 16, 16, "data/fonts/font.tga");

	// get initial sync timer values
	init_timer();
}

// ******************************************************************

// This function is executed once for each frame
void dkernel_doExec() {
	// capture mouse move and reset the position
	demoSystem.mouseButtons = SDL_GetMouseState(&demoSystem.mouseX, &demoSystem.mouseY);
	
	// Save the mouse position differences from the center of the window
	demoSystem.mouseXvar = (glDriver.width / 2) - demoSystem.mouseX;
	demoSystem.mouseYvar = (glDriver.height / 2) - demoSystem.mouseY;
	
	// Reset the mouse position if the left mouse button is pressed
	if (demoSystem.mouseButtons & SDL_BUTTON(1)) {
		SDL_ShowCursor(FALSE);
		SDL_WarpMouse(glDriver.width / 2, glDriver.height / 2);
	} else if (demoSystem.debug) {
		SDL_ShowCursor(TRUE);
	}
	
	// control exit demo (debug, loop) when end time arrives
	if ((demoSystem.endTime > 0) && (demoSystem.runTime > demoSystem.endTime)) {

		if (demoSystem.loop) {
			dkernel_restart();
		}
		else {
			if (demoSystem.debug) {
				demoSystem.runTime = demoSystem.endTime;
				dkernel_pause();
			}
			else {
				demoSystem.exitDemo = TRUE;
				return;
			}
		}
	}

	// non-play state
	if (demoSystem.state != DEMO_PLAY) {

		process_sectionQueues();
		pause_timer();

		if (demoSystem.state & DEMO_REWIND) {

			// decrease demo runtime
			demoSystem.runTime -= 10.0f * demoSystem.realFrameTime;
			if (demoSystem.runTime < demoSystem.startTime) {
				demoSystem.runTime = demoSystem.startTime;
				dkernel_pause();
			}

		} else if (demoSystem.state & DEMO_FASTFORWARD) {

			// increase demo runtime
			demoSystem.runTime += 10.0f * demoSystem.realFrameTime;
			if (demoSystem.runTime > demoSystem.endTime) {
				demoSystem.runTime = demoSystem.endTime;
				dkernel_pause();
			}
		}

		// reset section queues
		reinit_sectionQueues();

	// play state
	} else {

		// Prepare and remove the sections
		process_sectionQueues();
		
		// Update the timing information for the sections
		process_timer();
		
		// Update the beat information
		// update_beat();
	}

	// Default color
	glColor4f(1,1,1,1);
	
	// update sound driver once a frame
	if (demoSystem.sound) sound_update();
}

// ******************************************************************

void dkernel_mainLoop() {

	SDL_Event event;
	int result;
	
	if (demoSystem.debug) {
		fprintf(stdout, "\n%s", log_separator);
		fprintf(stdout, "main demo loop!\n");
		fprintf(stdout, "\n%s", log_separator);
	}

	demoSystem.state = DEMO_PLAY;
	
	while (!demoSystem.exitDemo) {
		if (FE_PollEvent(&event)) {
			result = eventHandler(event); 
		} else {
			dkernel_doExec();
		}
	}
}

// ******************************************************************

void dkernel_closeDemo() {

	char text[256];
	float score;

	// close sound driver
	if (demoSystem.sound) sound_end();

	// close graphics driver
	gldrv_close();

	// close network driver
	network_close();

	// write down the bench mark
	if (demoSystem.bench) {

		if (demoSystem.runTime > 0) {
			score = 10000 * (demoSystem.frameCount / demoSystem.runTime) / 60;
			sprintf(text,"Score: %.2f",score);

#ifdef WIN32
			// MessageBox(NULL,text,"Benchmark",MB_OK | MB_ICONINFORMATION);
#else
			fprintf(stdout,"%s\n",text);
#endif
		}
	}
	
	// Close demo abstraction layer
	SDL_Quit();
}

int dkernel_createSection(const char* pSectionScript, const char* pDataSource)
	{
	// **************************************************************************
	// * Creates a new section and adds it to the queue for inmediate rendering *
	// **************************************************************************

	// Load the script. After loading, the section will be stored in demoSystem.demoSection
	dkernel_loadScriptData(pSectionScript, pDataSource);
	
	// Activate the section we are loading
	mySection = demoSystem.demoSection;
	
	// Load the splines
	loadSplines(demoSystem.demoSection);

	// Preload and load the new section
	sectionFunction[demoSystem.demoSection->staticSectionIndex].preload();
	sectionFunction[demoSystem.demoSection->staticSectionIndex].load();

	// And, finally, refresh the queues
	reinit_sectionQueues();
	
	//process_sectionQueues();
	
	return demoSystem.demoSection->loaded; //1=OK, 0=NOK
	}

void dkernel_toggleSection(const char* identifiers) {
// ******************************************************************
// * Marks the desired section as enabled, activating its execution *
// ******************************************************************

	char* identifier;
	tDemoSection *ds;
	
	// Get a copy of the oiginal variable to work with it
	//identifier = calloc(strlen(identifiers) + 1, sizeof(char)); << this is a leak, strdup allocates memory
	identifier = strdup(identifiers);
	
	for (identifier = strtok(identifier, ",");
		identifier != NULL;
		identifier = strtok(NULL, ",")) {
				
			ds = demoSystem.demoSection;

			// Look for the section to disable
			while (ds != NULL) {
				if (strcmp(identifier, (char *)ds->identifier) == 0) {
					// We found the section, so enable it
					ds->enabled = !ds->enabled;
					// And exit the loop
					break;
				} else {
					// This is not the section we are looking for, so continue with the following
					ds = ds->next;
				}
			}
	}

	process_sectionQueues();
}

void dkernel_setSectionsStartTime(char* amount, char* identifiers) {
// *************************************************************
// * Sets the start time of a set of sections at the same time *
// *************************************************************
	char* identifier;
	tDemoSection *ds;
	float startTime;
	int i;

	// Get a floating point time increment
	startTime = (float)atof(amount);

	// Get a copy of the original variable to work with it
	// identifier = calloc(strlen(identifiers) + 1, sizeof(char));
	identifier = strdup(identifiers);
	
	for (identifier = strtok(identifier, ",");
		identifier != NULL;
		identifier = strtok(NULL, ",")) {
				
			ds = demoSystem.demoSection;

			// Look for the section to move in time
			while (ds != NULL) {
				if (strcmp(identifier, (char *)ds->identifier) == 0) {
					// We found the section, so set its startTime
					ds->startTime = startTime;
					
					// Update the section duration
					ds->duration = ds->endTime - ds->startTime;

					// If the section has splines, reload it. This way they are recalculated
					if (ds->splineNum > 0) {
						for (i=0; i<ds->splineNum; i++) {  
							ds->splineDuration[i] = ds->duration;
						}
						
						loadSplines(ds);
					}
					
					// And exit the loop
					break;
				} else {
					// This is not the section we are looking for, so continue with the following
					ds = ds->next;
				}
			}
	}

	//Update the screen
	reinit_sectionQueues();
}

void dkernel_setSectionsLayer(char* newLayer, char* identifier) {
// ************************************************************
// * Sets the layer of a single sections to the desired value *
// ************************************************************
	tDemoSection *ds;
	int theLayer;

	// Get the layer
	theLayer = atoi(newLayer);
				
	ds = demoSystem.demoSection;

	// Look for the section to move in time
	while (ds != NULL) {
		if (strcmp(identifier, (char *)ds->identifier) == 0) {
			// We found the section, so set its startTime
			ds->layer = theLayer;
					
			// And exit the loop
			break;
		} else {
			// This is not the section we are looking for, so continue with the following
			ds = ds->next;
		}
	}

	//Update the screen
	reinit_sectionQueues();
}

void dkernel_setSectionsEndTime(char* amount, char* identifiers) {
// ***********************************************************
// * Sets the end time of a set of sections at the same time *
// ***********************************************************
	char* identifier;
	tDemoSection *ds;
	float endTime;
	int i;

	// Get a floating point time increment
	endTime = (float)atof(amount);

	// Get a copy of the original variable to work with it
	// identifier = calloc(strlen(identifiers) + 1, sizeof(char));
	identifier = strdup(identifiers);
	
	for (identifier = strtok(identifier, ",");
		identifier != NULL;
		identifier = strtok(NULL, ",")) {
				
			ds = demoSystem.demoSection;

			// Look for the section to move in time
			while (ds != NULL) {
				if (strcmp(identifier, (char *)ds->identifier) == 0) {
					// We found the section, so set its endTime
					ds->endTime = endTime;

					// Update the section duration
					ds->duration = ds->endTime - ds->startTime;

					// If the section has splines, reload it. This way they are recalculated
					if (ds->splineNum > 0) {
						for (i=0; i<ds->splineNum; i++) {  
							ds->splineDuration[i] = ds->duration;
						}
						
						loadSplines(ds);
					}
										
					// And exit the loop
					break;
				} else {
					// This is not the section we are looking for, so continue with the following
					ds = ds->next;
				}
			}
	}

	//Update the screen
	reinit_sectionQueues();
}

void dkernel_updateSection(const char* pIdentifier, const char* pScript)
	{
	// ******************************************************************
	// * Updates the script for the desired section with the passed one *
	// ******************************************************************

	// Delete the current section
	dkernel_deleteSection(pIdentifier);

	// Create a new section
	dkernel_createSection(pScript, "Update Section Command");
	}

void dkernel_deleteSection(const char* identifier)
	{
	// *******************************************************
	// * Deletes the section and refreshes the section queue *
	// *******************************************************

	tDemoSection *ds, *prevds, *nextds;
	ds = demoSystem.demoSection;
	
	// Look for the section to delete
	while (ds != NULL) {
		
			if (strcmp(identifier, (char *)ds->identifier) == 0) {
			// We found the section, so let's first modify the linkage with the other sections
			
			//ds->endTime = -1;
			//ds->startTime = -1;
			
			// Handle the case in which we are deleting the last added section
			if (ds == demoSystem.demoSection) demoSystem.demoSection = ds->next;
			
			if (ds->prior != NULL) {
				prevds = ds->prior;
				prevds->next = ds->next;

				if (ds->next != NULL) {
					nextds = ds->next;
					nextds->prior = ds->prior;
				}
			}

			// And, finally, refresh the queues
			reinit_sectionQueues();
			
			// Safely delete the section
			free(ds->vars);
			free(ds);
			
			// Now exit the loop as we have finished
			break;
		} else {
			// This is not the section we are looking for, so continue with the following
			ds = ds->next;
		}
	}
}

void dkernel_play() {
// ********************************
// * Puts the engine in play mode *
// ********************************
	if (demoSystem.state != DEMO_PLAY) {
	
		demoSystem.state = DEMO_PLAY;

		// reinit section queues
		reinit_sectionQueues();

		// unpause the sound
		if (demoSystem.sound) sound_play();
	}
}

void dkernel_setStartTime(float theTime) {
// **********************************************
// * Sets the start time to the specified value *
// **********************************************
	
	// Correct the time if it has an invalud value
	if (theTime < 0) theTime = 0;
	else if (theTime > demoSystem.endTime) theTime = demoSystem.endTime;
	
	// Set the new time
	demoSystem.startTime = theTime;
}

void dkernel_setCurrentTime(float theTime) {
// ************************************************
// * Sets the current time to the specified value *
// ************************************************
	
	// Correct the time if it has an invalud value
	if (theTime < 0) theTime = 0;
	
	// Set the new time
	demoSystem.runTime = theTime;
}

void dkernel_setEndTime(float theTime) {
// ********************************************
// * Sets the end time to the specified value *
// ********************************************
	
	// Correct the time if it has an invalud value
	if (theTime < 0) theTime = 0;
	else if (theTime < demoSystem.startTime) theTime = demoSystem.startTime;
	
	// Set the new time
	demoSystem.endTime = theTime;
}

// ******************************************************************

void dkernel_pause() {

	demoSystem.state = DEMO_PAUSE;
	demoSystem.frameTime = 0;
	if (demoSystem.sound) sound_pause();
}

// ******************************************************************

void dkernel_rewind() {

	demoSystem.state = (demoSystem.state & DEMO_PAUSE) | DEMO_REWIND;
	if (demoSystem.sound) sound_pause();
}

// ******************************************************************

void dkernel_fastforward() {

	demoSystem.state = (demoSystem.state & DEMO_PAUSE) | DEMO_FASTFORWARD;
	if (demoSystem.sound) sound_pause();
}

// ******************************************************************

void dkernel_restart() {

	demoSystem.state = DEMO_PLAY;
	if (demoSystem.sound) {
		sound_end();
		sound_init();
	}

	init_control_vars();
	reinit_sectionQueues();
	init_timer();
}

// ******************************************************************

void dkernel_getSpline(int spline, float step, ChanVec resVec)
	{
	//	int i;

	// interpolate spline
	if (mySection->splines[spline] == NULL)
		dkernel_error("Spline not found for section %s", mySection->identifier);
	else
		MotionCalcStep (mySection->splines[spline], resVec, step);

	/*
	// apply modifiers
	for (i=0; i<mySection->modifierNum; i++) {
		if (mySection->modifiers[i].spline == spline+1) {
			switch (mySection->modifiers[i].mode) {

				// only one hardcoded modifier for now
				case 1:
					resVec[mySection->modifiers[i].param-1] += demoSystem.beat*10;
					break;
			}
		}
	*/
	}


// ******************************************************************

void dkernel_error(const char *error, ...)
	{
	va_list argptr;
	char text[1024];

	va_start(argptr,error);
	vsprintf(text,error,argptr);
	va_end(argptr);

	// end sound and video (only if we are not in debug mode)
	if (!demoSystem.debug)
		dkernel_closeDemo();

	// write down the error in the console
	fprintf(stderr,"Error: %s\n", text);

	SpzMessageBox("Engine Error", text);

	#ifdef WIN32
		OutputDebugStringA("Engine Error: ");
		OutputDebugStringA(text);
		OutputDebugStringA("\n");
	#endif

	// force exit (if we are in slave mode, try to continue)
	if (!demoSystem.debug)
		exit(EXIT_FAILURE);
	}

// ******************************************************************

void dkernel_warn(const char *warn, ...)
	{
	va_list argptr;
	char text[1024];

	va_start(argptr,warn);
	vsprintf(text,warn,argptr);
	va_end(argptr);

	// write down the warning
	if (demoSystem.debug)
		fprintf(stderr, "Warning: second %.2f, message '%s'\n", demoSystem.runTime, text);

	#ifdef WIN32
		OutputDebugStringA("Engine Warning: ");
		OutputDebugStringA(text);
		OutputDebugStringA("\n");
	#endif

	SpzMessageBox("Engine Warning", text);
	}

// ******************************************************************

void dkernel_trace(const char *message, ...)
	{
	va_list argptr;
	char text[1024];

	va_start(argptr,message);
	vsprintf(text,message,argptr);
	va_end(argptr);

	// write down the trace to the standard output
	if (demoSystem.slaveMode)
		fprintf(stdout, "Trace: %s\n", text);

	#ifdef WIN32
		OutputDebugStringA("Engine Kernel Trace: ");
		OutputDebugStringA(text);
		if (text[0]!='\0' || text[strlen(text)-1] != '\n')
			OutputDebugStringA("\n");
	#endif
	}

// ******************************************************************

void section_error(const char *error, ...)
	{
	va_list argptr;
	char text[1024];

	va_start(argptr,error);
	vsprintf(text,error,argptr);
	va_end(argptr);

	dkernel_error
		(
		"%s (%d, %f -> %f): %s",
		sectionFunction[mySection->staticSectionIndex].scriptName,
		mySection->layer, mySection->startTime, mySection->endTime, text
		);
	}

// ******************************************************************

void parse_error(const char* pFilename, int line, const char* pError, ...)
	{
	va_list argptr;
	char text[1024];

	va_start(argptr, pError);
	vsprintf(text, pError, argptr);
	va_end(argptr);

	dkernel_warn("%s(%d) : parse error : %s", pFilename, line, text);
	}

// ******************************************************************

/*const matrix_t*			get_sve_variable_matrix_4x4f(enum_sve_variable id) {
	return &demoSystem.m_VariableMatrix[id];
}


enum_sve_variable		get_sve_variable_id(const char* pString) {
	if (strcmp(pString, "World"))
		return sve_variable_matrix_world;
	else if (strcmp(pString, "View"))
		return sve_variable_matrix_view;
	else return sve_variable_unknown;
}

enum_sve_variable_type	get_sve_variable_type(enum_sve_variable id)
{
	static enum_sve_variable_type VariableType[MAX_SVE_VARIABLE_ID+1];
	static bool initialized = false;
	
	if (!initialized)
	{
		VariableType[sve_variable_unknown] = sve_variable_type_unknown;
		VariableType[sve_variable_matrix_world] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_view] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_projection] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_world_view] = sve_variable_type_matrix_4x4f;
		VariableType[sve_variable_matrix_world_projection] = sve_variable_type_matrix_4x4f;
		initialized = true;
	}
	
	return VariableType[id];
}

void					invalidate_sve_variable(enum_sve_variable id)
{
	//SL_NOT_IMPLEMENTED;
}

void					set_sve_variable_matrix_4x4f(enum_sve_variable id, const matrix_t* pValue)
{
	//invalidate_sve_variable(
	memcpy(&demoSystem.m_VariableMatrix[id], pValue, sizeof(matrix_t));
	
	switch(id)
	{
		case sve_variable_matrix_world:
			invalidate_sve_variable(sve_variable_matrix_world_inverse);
			invalidate_sve_variable(sve_variable_matrix_world_transpose);
			break;
		case sve_variable_matrix_view:
			invalidate_sve_variable(sve_variable_matrix_view_inverse);
			invalidate_sve_variable(sve_variable_matrix_view_transpose);
			break;
		case sve_variable_matrix_projection:
			invalidate_sve_variable(sve_variable_matrix_projection_inverse);
			invalidate_sve_variable(sve_variable_matrix_projection_transpose);
			break;
	}
} */