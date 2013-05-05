#ifndef __3DSDRAWMODES_H
#define __3DSDRAWMODES_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

#include "3dsload.h"

// ******************************************************************

void render_objectLayers (H3dsScene	*object, layer_t *layers, int	enableDepthBufferClearing);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
