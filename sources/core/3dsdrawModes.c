/*
	3dsdrawmodes.c: 3ds render modes library
*/

#include "../main.h"
#include "3dsdraw.h"


// ******************************************************************

static void render_objectLayer(H3dsScene *object, layer_t *layer) {
	float ambient[4] = {0,0,0,1};
	float diffuse[4] = {layer->r, layer->g, layer->b, 1};
	float specular[4] = {layer->r, layer->g, layer->b, 1};

	// hidden surface removal init
	if (layer->hasCullface) {
		glEnable(GL_CULL_FACE);
		glCullFace(layer->cullface);
	}

	
	// ******************************************************************
	
	if (layer->rMode == RMODE_GLSL) {
		
		render_glsl_3ds(object);
	}
	
	// ******************************************************************
	
	else if (layer->rMode == RMODE_GLSL_BINORMAL) {
		
		render_glsl_binormal_tangent_3ds(object);
	}
	
	// ******************************************************************

	else if (layer->rMode == RMODE_VERTEXES) {

		glDisable(GL_TEXTURE_2D);
		glPointSize(layer->lineWidth);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		render_onlyvertex3ds(object);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPointSize(1.0f);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_WIRE) {

		glDisable(GL_TEXTURE_2D);
		glLineWidth(layer->lineWidth);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		render_vertex3ds(object);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1.0f);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_FILL) {

		glDisable(GL_TEXTURE_2D);
		render_vertex3ds(object);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_FLAT) {

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glShadeModel(GL_FLAT);
		render_flat3ds(object);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_SHADED) {

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glShadeModel(GL_SMOOTH);
		render_smooth3ds(object);
		glShadeModel(GL_FLAT);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_NORMALS) {

		glDisable(GL_TEXTURE_2D);
		glLineWidth(layer->lineWidth);
		render_facenormals3ds(object, 10.0f);
		glLineWidth(1.0f);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_TEXTURE) {

		render_texture3ds(object);

	}

	// ******************************************************************

	else if (layer->rMode == RMODE_ENV) {

		if (layer->texture > 0) {

			tex_bind(layer->texture);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			render_normal3ds(object);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
		}
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_CELL) {

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_1D);
			tex_reset_bind();
			tex_bind(layer->texture);
				glEnable(GL_TEXTURE_GEN_S);
				glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				render_normal3ds(object);
				glDisable(GL_TEXTURE_GEN_S);
			tex_reset_bind();
		glDisable(GL_TEXTURE_1D);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_OUTLINE) {

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(layer->lineWidth);
			render_vertex3ds(object);
			glLineWidth(1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
	}

	// ******************************************************************

	else if (layer->rMode == RMODE_BUMPDOT3) {

		if (cube_map == -1) {
			setupNormalizationCubeMap();
			if (cube_map == -1) dkernel_error("Unable to create cube map");
		}
		render_bump3ds(object, layer->texture, cube_map);
	}

	// ******************************************************************

	// hidden surface removal end
	if (layer->hasCullface) {
		glDisable(GL_CULL_FACE);
	}
}

// ******************************************************************

void render_objectLayers (H3dsScene	*object, layer_t *layers, int	enableDepthBufferClearing) {

	layer_t *layer;
	int i;

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthFunc(GL_LEQUAL);

	// loop layers
	layer = layers;
	i = 0;

	while (layer)
		{
		// clear depth buffer
		if (enableDepthBufferClearing == 1)
			glClear(GL_DEPTH_BUFFER_BIT);

		// set layer blending
		if (layer->hasBlend)
			glBlendFunc(layer->sfactor, layer->dfactor);
		else
			glBlendFunc(GL_ONE, GL_ZERO);

		// set color
		glColor4f(layer->r, layer->g, layer->b, layer->alpha);

		render_objectLayer(object, layer);
		layer = (layer_t *) layer->next;
		i++;
		}

	glDisable(GL_BLEND);
}