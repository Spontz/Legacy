#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	camera_t *cam;
	tExpression tParameters;
} camera_section2;

static camera_section2 *local;

// ******************************************************************

void preload_camera2() {
}

// ******************************************************************

void load_camera2() {
	char *equation;
	int equationSize;
	
	// script validation
	if (mySection->stringNum != 6) {
		section_error("6 strings needed");
		return;
	}
	
	local = malloc(sizeof(camera_section2));
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
	local->cam = camera_init(camera_type__look_at);
	
	// default camera values
	camera_locate(local->cam, 0, 0, 0);
	camera_angle(local->cam, 0, 0, 0);
	camera_target(local->cam, 0,  0,  -100);
	camera_fov_aspect(local->cam, 90.0f, gldrv_get_aspect_ratio());
	camera_zplanes(local->cam, 0.1f, 500.0f);
	camera_roll(local->cam, 0);
	mySection->loaded=1;
}

// ******************************************************************

void init_camera2 () {
}

// ******************************************************************

void render_camera2 () {
	double positionX, positionY, positionZ;
	double angleX, angleY, angleZ;
	double targetX, targetY, targetZ;
	double fov, zNear, zFar, roll;
	
	local = (camera_section2 *) mySection->vars;

	// for easy access to the current camera by any section
	demoSystem.camera = local->cam;

	// Refresh the camera information
	// Insert the default section values into the size equation
	insertSectionVariables(&local->tParameters);
	
	// Evaluate the equations
	local->tParameters.err = exprEval(local->tParameters.o, &local->tParameters.result);

	// Check for errors
	if(local->tParameters.err != EXPR_ERROR_NOERROR) section_error("cameraSection2: render_camera2: Expression evaluation Error (%d): %s", local->tParameters.err, local->tParameters.equation);

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
	
	exprValListGet(local->tParameters.v, "fov", &fov);
	exprValListGet(local->tParameters.v, "zNear", &zNear);
	exprValListGet(local->tParameters.v, "zFar", &zFar);
	exprValListGet(local->tParameters.v, "roll", &roll);
	
	// Apply the new parameters to the camera
	camera_locate(local->cam, (float)positionX, (float)positionY, (float)positionZ);
	camera_angle (local->cam, (float)angleX,    (float)angleY,    (float)angleZ   );
	camera_target(local->cam, (float)targetX,   (float)targetY,   (float)targetZ  );
	
	camera_fov_aspect(local->cam, (float)fov, gldrv_get_aspect_ratio());
	camera_zplanes(local->cam, (float)zNear, (float)zFar);
	camera_roll(local->cam, (float)roll);
	
	camera_glprojection (local->cam);
	camera_glmatrix (local->cam);
}

// ******************************************************************

void end_camera2 () {
}
