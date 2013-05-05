#include "viewport.h"

#ifdef __cplusplus
extern "C" {
#endif

void spzViewport(GLint x, GLint y, GLsizei width, GLsizei height, const char* pFile, int line)
	{
	// dkernel_trace("%s(%d) : glViewport(%d, %d, %d, %d)", pFile, line, x, y, width, height);
	glViewport(x, y, width, height);
	}
#ifdef __cplusplus
}
#endif
