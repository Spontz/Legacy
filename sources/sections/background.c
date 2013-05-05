#include "../interface/demo.h"

typedef enum
	{
	background_drawing_mode__fit_to_viewport = 0,	// ajusta la imagen para que ocupe todo el viewport (sin distorsion de aspect)
	background_drawing_mode__fit_to_content = 1		// ajusta la imagen para se vea completa (sin distorsion de aspect)
	} enum_background_drawing_mode;

typedef struct
	{
	rect2d_t						quad;
	rect2d_t						quad2;
	int								texture;
	enum_background_drawing_mode	mode;
	} background_section;

static background_section *local;

void preload_background()
	{
	}

void load_background()
	{
	// script validation
	if (mySection->stringNum != 1)
		{
		section_error("1 string needed");
		return;
		}

	if (mySection->paramNum != 1)
		{
		section_error("1 param needed");
		return;
		}

	local = (background_section*) malloc(sizeof(background_section));
	mySection->vars = (void *) local;

	switch((unsigned int)mySection->param[0])
		{
		case background_drawing_mode__fit_to_content:
			local->mode = background_drawing_mode__fit_to_content;
			break;
		case background_drawing_mode__fit_to_viewport:
			local->mode = background_drawing_mode__fit_to_viewport;
			break;
		default:
			section_error("Invalid value for param[0].");
		}

	// texture loading
	local->texture = tex_load(mySection->strings[0], USE_CACHE, FALSE);
	if (local->texture==-1)
		return;

	tex_properties(local->texture, CLAMP);
	tex_upload(local->texture, USE_CACHE);

	mySection->loaded = 1;
	}

void init_background()
	{
	}

void render_background()
	{
	int i;

	local = (background_section *) mySection->vars;

	glDisable(GL_DEPTH_TEST);
		{
		if (mySection->hasBlend)
			{
			glBlendFunc(mySection->sfactor, mySection->dfactor);
			glEnable(GL_BLEND);
			}
		
		if (mySection->hasAlpha)
			{
			float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(mySection->alphaFunc, alpha);
			}

		// compute rectangle coordinates
			{
			// hack : las texturas se cargan con resoluciones multiplo de dos
			float TextureAspectRatio = (float)tex_array[local->texture]->width / (float)tex_array[local->texture]->height;

			switch(local->mode)
				{
				case background_drawing_mode__fit_to_viewport:
					camera_2d_fit_to_viewport(TextureAspectRatio, &local->quad.x0, &local->quad.x1, &local->quad.y0, &local->quad.y1);
					break;
				case background_drawing_mode__fit_to_content:
					camera_2d_fit_to_content(TextureAspectRatio, &local->quad.x0, &local->quad.x1, &local->quad.y0, &local->quad.y1);
					break;
				default:
					section_error("Invalid background section drawing mode");
					break;
				}
			}

		// store vertex values
		local->quad.pc[0][0] = local->quad.x0;
		local->quad.pc[1][0] = local->quad.x1;
		local->quad.pc[2][0] = local->quad.x1;
		local->quad.pc[3][0] = local->quad.x0;

		local->quad.pc[0][1] = local->quad.y0;
		local->quad.pc[1][1] = local->quad.y0;
		local->quad.pc[2][1] = local->quad.y1;
		local->quad.pc[3][1] = local->quad.y1;

		local->quad.tc[0][0] = 0; local->quad.tc[0][1] = 1;
		local->quad.tc[1][0] = 1; local->quad.tc[1][1] = 1;
		local->quad.tc[2][0] = 1; local->quad.tc[2][1] = 0;
		local->quad.tc[3][0] = 0; local->quad.tc[3][1] = 0;

		camera_set2d();

		tex_bind(local->texture);
		glBegin(GL_QUADS);
			for (i=0; i<4; i++)
				{
				glTexCoord2fv(local->quad.tc[i]);
				glVertex2fv(local->quad.pc[i]);
				}
		glEnd();

		camera_restore();

		if (mySection->hasAlpha)
			glDisable(GL_ALPHA_TEST);

		if (mySection->hasBlend)
			glDisable(GL_BLEND);
		}
	glEnable(GL_DEPTH_TEST);
	}

// ******************************************************************

void end_background ()
	{
	}
