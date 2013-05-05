#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	camera_t *cam;
	float freeCamera;
} camera_section;

static camera_section *local;

// ******************************************************************

void preload_camera() {
}

// ******************************************************************

void load_camera() {

	// script validation
	if ((mySection->splineNum != 1)||(mySection->paramNum  != 1)) {
		section_error("1 spline and 1 float needed");
		return;
	}
	
	local = malloc(sizeof(camera_section));
	mySection->vars = (void *) local;

	// Create the camera object
	local->cam = camera_init(camera_type__look_at);
	
	// Free camera handling
	local->freeCamera = mySection->param[0];
	mySection->loaded=1;
}

// ******************************************************************

static void interpolate_camera (float step, camera_t *cam) {

	ChanVec resVec;

	dkernel_getSpline(0, step, resVec);

	// update the camera with the spline values
	camera_locate (cam, resVec[0], resVec[1], resVec[2]);
	camera_angle (cam, resVec[3], resVec[4], resVec[5]);
	camera_target (cam, resVec[6], resVec[7], resVec[8]);
	camera_fov_aspect (cam, resVec[9], gldrv_get_aspect_ratio());
	camera_zplanes (cam, resVec[10], resVec[11]);
	
	if (mySection->splines[0]->channels == 13) {
		camera_roll (cam, resVec[12]);
	}
}

// ******************************************************************

void init_camera () {
}

// ******************************************************************

void render_camera () {

	local = (camera_section *) mySection->vars;

	// for easy access to the current camera by any section
	demoSystem.camera = local->cam;

	// free camera?
	if (fabs(local->freeCamera - 1.0f) < FLT_EPSILON)
		interactive_mov (local->cam, demoSystem.realFrameTime);
	else if (mySection->splineNum > 0)
		interpolate_camera (mySection->runTime, local->cam);

	camera_glprojection (local->cam);
	camera_glmatrix (local->cam);
}

// ******************************************************************

void end_camera () {
}
