#ifndef __PICKCAM_H
#define __PICKCAM_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

typedef struct {

	void *next;
	char *camera;
	float start, end;

} pickcam_t;

// ******************************************************************

pickcam_t *pickcam_load(char *name);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
