#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include "drivers/gldriver.h"

#ifdef __cplusplus
extern "C" {
#endif

void spzViewport(GLint x, GLint y, GLsizei width, GLsizei height, const char* pFile, int line);
#define SPZ_VIEWPORT(a, b, c, d) spzViewport(a, b, c, d, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif
