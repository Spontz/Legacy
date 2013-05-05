/*
	layers.c: load layers files for rendering scenes
*/

#include "../main.h"

// ******************************************************************

layer_t *layers_load(char *name) {

	char line[512], key[64], value[64], tmp[64], tmp2[64];
	layer_t *layer, *newLayer, *out;
	int lineNum, values;
	FILE *f;

	f = fopen(name,"rt");
	if (!f) dkernel_error("Error loading '%s'", name);

	lineNum = 0;
	layer = NULL;
	out = NULL;

	for (;;) {

		lineNum++;

		if (fgets(line, 512, f) == NULL) break;

		// comments or empty line
		if ((line[0] == ';') || (line[0] == '\n') || (line[0] == '\r')) continue;

		// an element begins
		if ((line[0] == '{')) {

			newLayer = malloc(sizeof(layer_t));
			memset(newLayer, 0, sizeof(layer_t));
			newLayer->lineWidth = 1.0f;

			// start the queue
			if (!out) out = newLayer;
			// or queue the new layer
			else layer->next = (layer_t *) newLayer;

			layer = newLayer;
		}

		// an element ends
		else if ((line[0] == '}')) {
		}

		// element definition
		else if (layer) {
			getKeyValue(line,key,value);

			// render mode
			if (spz_strcmpi(key,"mode") == 0) {
				layer->rMode = getRenderModeByName(value);
				if (layer->rMode == -1) parse_error(name, lineNum, "Unknown render mode.");
			}

			// blending mode
			else if (spz_strcmpi(key,"blend") == 0) {
				values = sscanf(value,"%s %s",tmp,tmp2);
				if (values != 2)
					parse_error(name, lineNum, "Invalid blend.");

				layer->sfactor = getBlendCodeByName(tmp);
				layer->dfactor = getBlendCodeByName(tmp2);
				if ((layer->sfactor == -1) || (layer->dfactor == -1))
					parse_error(name, lineNum, "Invalid blend factor.");

				layer->hasBlend = TRUE;
			}

			// alpha mode
			else if (spz_strcmpi(key,"alpha") == 0) {
				values = sscanf(value,"%s %f %f",tmp,&layer->alpha1,&layer->alpha2);
				switch (values) {
					case 2: layer->alpha2 = layer->alpha1; break;
					case 3: break;
					default: parse_error(name, lineNum, "Invalid alpha.");
				}

				layer->alphaFunc = getAlphaCodeByName(tmp);
				if (layer->alphaFunc == -1)
					parse_error(name, lineNum, "Invalid alpha function.");

				layer->hasAlpha = TRUE;
			}

			// modulate color
			else if (spz_strcmpi(key,"color") == 0)
				{
				values = sscanf(value,"%f %f %f %f",&layer->r,&layer->g,&layer->b,&layer->alpha);
				if (values != 4)
					parse_error(name, lineNum, "Invalid color.");
				}

			// texture
			else if (spz_strcmpi(key, "texture") == 0)
				{
				layer->texture = tex_load(value, USE_CACHE, 0);
				tex_properties(layer->texture, MODULATE);
				tex_upload(layer->texture, USE_CACHE);
				}

			// cullface
			else if (spz_strcmpi(key,"cullface") == 0)
				{
				if (spz_strcmpi(value,"front") == 0)
					layer->cullface = GL_FRONT;
				else if (spz_strcmpi(value,"back" ) == 0)
					layer->cullface = GL_BACK;
				else
					parse_error(name, lineNum, "Invalid cullface.");

				layer->hasCullface = TRUE;
				}

			// line width
			else if (spz_strcmpi(key,"linewidth") == 0)
				{
				values = sscanf(value,"%f",&layer->lineWidth);
				if (values != 1)
					parse_error(name, lineNum, "Invalid line width.");
				}

			// node where layer will be applied
			else if (spz_strcmpi(key, "node") == 0)
				{
				layer->node = strdup(value);
				}

			else
				{
				parse_error(name, lineNum, "%s is not a valid layer variable.", key);
				}
		}
	}

	fclose(f);

	return out;
}

// ******************************************************************

int layer_counter(layer_t *s) {

	layer_t *layer = s;
	int i = 0;

	while (layer) {
		layer = (layer_t *) layer->next;
		i++;
	}

	return i;
}
