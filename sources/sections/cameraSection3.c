#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	camera_t *cam;
	camera_t *camSpline;

	tExpression tParameters;
	float freeCamera;
} camera_section3;

static camera_section3 *local;

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

void preload_camera3() {
}

// ******************************************************************

void load_camera3() {
	char *equation;
	int equationSize;
	
	// script validation
	if ((mySection->stringNum != 6)||(mySection->paramNum  != 1)) {
		section_error("6 strings and 1 float needed");
		return;
	}

	local = malloc(sizeof(camera_section3));
	mySection->vars = (void *) local;

	equationSize  = strlen(mySection->strings[0]) +1;
	equationSize += strlen(mySection->strings[1]    );
	equationSize += strlen(mySection->strings[2]    );
	equationSize += strlen(mySection->strings[3]    );
	equationSize += strlen(mySection->strings[4]    );
	equationSize += strlen(mySection->strings[5]    );
		
	equation = calloc(equationSize, sizeof(char));

	strcpy(equation, (char *) mySection->strings[0]);
	strcat(equation, (char *) mySection->strings[1]);
	strcat(equation, (char *) mySection->strings[2]);
	strcat(equation, (char *) mySection->strings[3]);
	strcat(equation, (char *) mySection->strings[4]);
	strcat(equation, (char *) mySection->strings[5]);

	local->tParameters.equation = equation;
	
	// Init the expression evaluation library
	initExpression(&local->tParameters);
	
	// Create the camera object
	local->cam       = camera_init(camera_type__look_at);
	local->camSpline = camera_init(camera_type__look_at); //-V656

	// Free camera handling
	local->freeCamera = mySection->param[0];
	mySection->loaded=1;
}

// ******************************************************************

void init_camera3 () {
}

// ******************************************************************

void render_camera3 () {
	double positionX, positionY, positionZ;
	double angleX, angleY, angleZ;
	double targetX, targetY, targetZ;
	double fov, zNear, zFar, roll;
			
	local = (camera_section3 *) mySection->vars;

	// Clear the demoengine camera
	demoSystem.camera = NULL;
	
	// Calculate the first approximation to the camera following the provided spline
	// free camera?
	if (fabs(local->freeCamera - 1.0f) < FLT_EPSILON)
		interactive_mov(local->camSpline, demoSystem.realFrameTime);
	else if (mySection->splineNum > 0)
		interpolate_camera(mySection->runTime, local->camSpline);
	
	// Refresh the camera information
	// Insert the default section values into the size equation
	insertSectionVariables(&local->tParameters);
	
	// Insert variables pertaining to the calculated spline camera
	exprValListAdd(local->tParameters.v , "cam_eye_x", (EXPRTYPE)local->camSpline->eye.x);
	exprValListAdd(local->tParameters.v , "cam_eye_y", (EXPRTYPE)local->camSpline->eye.y);
	exprValListAdd(local->tParameters.v , "cam_eye_z", (EXPRTYPE)local->camSpline->eye.z);
	
	exprValListAdd(local->tParameters.v , "cam_target_x", (EXPRTYPE)local->camSpline->target.x);
	exprValListAdd(local->tParameters.v , "cam_target_y", (EXPRTYPE)local->camSpline->target.y);
	exprValListAdd(local->tParameters.v , "cam_target_z", (EXPRTYPE)local->camSpline->target.z);
	
	exprValListAdd(local->tParameters.v , "cam_angle_x", (EXPRTYPE)local->camSpline->angle.x);
	exprValListAdd(local->tParameters.v , "cam_angle_y", (EXPRTYPE)local->camSpline->angle.y);
	exprValListAdd(local->tParameters.v , "cam_angle_z", (EXPRTYPE)local->camSpline->angle.z);
	
	exprValListAdd(local->tParameters.v , "cam_fov"  , (EXPRTYPE)local->camSpline->fov  );
	exprValListAdd(local->tParameters.v , "cam_znear", (EXPRTYPE)local->camSpline->znear);
	exprValListAdd(local->tParameters.v , "cam_zfar" , (EXPRTYPE)local->camSpline->zfar );
	exprValListAdd(local->tParameters.v , "cam_roll" , (EXPRTYPE)local->camSpline->roll );
	
	// Evaluate the equations
	local->tParameters.err = exprEval(local->tParameters.o, &local->tParameters.result);

	// Check for errors
	if(local->tParameters.err != EXPR_ERROR_NOERROR) section_error("cameraSection3: render_camera3: Expression evaluation Error (%d): %s", local->tParameters.err, local->tParameters.equation);

	// Retrieve the values and assign them
	exprValListGet(local->tParameters.v, "posX", &positionX);
	exprValListGet(local->tParameters.v, "posY", &positionY);
	exprValListGet(local->tParameters.v, "posZ", &positionZ);

	exprValListGet(local->tParameters.v, "angX", &angleX);
	exprValListGet(local->tParameters.v, "angY", &angleY);
	exprValListGet(local->tParameters.v, "angZ", &angleZ);
	
	exprValListGet(local->tParameters.v, "lookX", &targetX);
	exprValListGet(local->tParameters.v, "lookY", &targetY);
	exprValListGet(local->tParameters.v, "lookZ", &targetZ);
	
	exprValListGet(local->tParameters.v, "fov"  , &fov  );
	exprValListGet(local->tParameters.v, "zNear", &zNear);
	exprValListGet(local->tParameters.v, "zFar" , &zFar );
	exprValListGet(local->tParameters.v, "roll" , &roll );
	
	// Apply the new parameters to the camera
	camera_locate(local->cam, (float)positionX, (float)positionY, (float)positionZ );
	camera_angle (local->cam, (float)angleX   , (float)angleY   , (float)angleZ    );
	camera_target(local->cam, (float)targetX  , (float)targetY  , (float)targetZ   );
	
	camera_fov_aspect(local->cam, (float)fov, gldrv_get_aspect_ratio());
	camera_zplanes(local->cam, (float)zNear, (float)zFar);
	camera_roll(local->cam, (float)roll);

	// for easy access to the current camera by any section	
	demoSystem.camera = local->cam;
	
	// Set the new camera
	camera_glprojection (local->cam);
	camera_glmatrix (local->camSpline);
	camera_glmatrix (local->cam);
}

// ******************************************************************

void end_camera3 () {
}
