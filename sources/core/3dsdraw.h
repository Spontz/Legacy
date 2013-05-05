#ifndef __3DSDRAW_H
#define __3DSDRAW_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#include "3dsload.h"

// ******************************************************************

void render_boundingbox(H3dsBoundingBox *box);
void render_boundingsphere(H3dsBoundingSphere *sphere);

// ******************************************************************

void render_onlyvertex3ds (H3dsScene *scene);
void render_vertex3ds (H3dsScene *scene);
void render_flat3ds (H3dsScene *scene);
void render_smooth3ds (H3dsScene *scene);
void render_facenormals3ds (H3dsScene *scene, float scale);
void render_vertexnormals3ds (H3dsScene *scene, float scale);
void render_allvertexnormals3ds (H3dsScene *scene, float scale);
void render_glsl_binormal_tangent_3ds(H3dsScene *scene);
void render_glsl_3ds (H3dsScene *scene);
void render_glsl_binormal3ds (H3dsScene *scene);
void render_texture3ds (H3dsScene *scene);
void render_normal3ds (H3dsScene *scene);
void render_bump3ds (H3dsScene *scene, GLuint texture, GLuint cube_map);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
