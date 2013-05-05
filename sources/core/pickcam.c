/*
	pickcam.c: select different cameras inside scene
*/

#include "../main.h"
#include "dkernel.h"

// ******************************************************************

pickcam_t *pickcam_load(char *name) {

	char line[256], key[64], value[64];
	pickcam_t *cam, *newCam, *out;
	int lineNum;
	FILE *f;

	f = fopen(name,"rt");
	if (!f) dkernel_error("Error loading '%s'",name);

	lineNum = 0;
	cam = NULL;
	out = NULL;

	for (;;) {

		lineNum++;

		if (fgets(line,256,f) == NULL) break;

		// comments or empty line
		if ((line[0] == ';') || (line[0] == '\n') || (line[0] == '\r')) continue;

		// an element begins
		if ((line[0] == '{')) {

			newCam = malloc(sizeof(pickcam_t));
			memset(newCam, 0, sizeof(pickcam_t));

			// start the queue
			if (!out) out = newCam;
			// or queue the new camera
			else cam->next = (pickcam_t *) newCam;

			cam = newCam;
		}

		// an element ends
		else if ((line[0] == '}')) {
		}

		// element definition
		else if (cam) {
			getKeyValue(line,key,value);

			// camera
			if (spz_strcmpi(key,"camera") == 0) {
				cam->camera = strdup(value);
			}

			// start
			else if (spz_strcmpi(key,"start") == 0) {
				sscanf(value,"%f",&cam->start);
			}

			// end
			else if (spz_strcmpi(key,"end") == 0) {
				sscanf(value,"%f",&cam->end);
			}

			else {
				parse_error(key, lineNum, "Unknown pickcam variable.");
			}
		}
	}

	fclose(f);

	return out;
}
