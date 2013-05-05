#ifndef __CUBEMAP_H
#define __CUBEMAP_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

extern int cube_map;

// ******************************************************************

void generateNormalizationCubeMap();
void setupNormalizationCubeMap();

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
