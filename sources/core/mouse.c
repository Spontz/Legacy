/*
	mouse.c: mouse controls
*/

#include "../main.h"
#include "drivers/events.h"

// ******************************************************************

static float accel = 0, accel_strafe = 0;

// ******************************************************************
// mouse interaction
// ******************************************************************

void camera_axis_mov (camera_t *cam, int x, int y) {

	float dx, dy;

	dx = x / 16.0f;
	dy = y / 16.0f;

	if (fabs(dx) > FLT_EPSILON || fabs(dy) > FLT_EPSILON)
	{
		cam->angle.x += dy;
		//if (cam->angle.x >  90) cam->angle.x =  90;
		//if (cam->angle.x < -90) cam->angle.x = -90;
		cam->angle.y += dx;
	}
}

// ******************************************************************

void camera_front_mov (camera_t *cam, float accel) {

	vector_t v;

	v = cam->front;
	vector_scale (&v, accel, &v);
	vector_sub (&v, &cam->eye, &cam->eye);
	vector_sub (&v, &cam->target, &cam->target);
}

// ******************************************************************

void camera_strafe_mov (camera_t *cam, float accel) {

	vector_t v;

	v = cam->side;
	vector_scale (&v, accel, &v);
	vector_add (&v, &cam->eye, &cam->eye);
	vector_add (&v, &cam->target, &cam->target);
}

// ******************************************************************

void interactive_mov (camera_t *cam, float t) {
	float at;
	FILE * myFile;
	char * data;

// In order to execute this routine, no spline modifier key should be pressed.
	// This includes the number keys from 0 to 9:
	if ((demoSystem.debug) & !demoSystem.keys[KEY_NUM_1] & !demoSystem.keys[KEY_NUM_2] & !demoSystem.keys[KEY_NUM_3] & !demoSystem.keys[KEY_NUM_4] & !demoSystem.keys[KEY_NUM_5] & !demoSystem.keys[KEY_NUM_6] & !demoSystem.keys[KEY_NUM_7] & !demoSystem.keys[KEY_NUM_8] & !demoSystem.keys[KEY_NUM_9]) { //-V564

		// capture camera position
		if (demoSystem.keys[KEY_CAPTURE]) {
			myFile = fopen("camera.cam", "a");
			data = calloc(1024, sizeof(char));
			sprintf(data, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
				cam->   eye.x, cam->   eye.y, cam->   eye.z,
				cam-> angle.x, cam-> angle.y, cam-> angle.z,
				cam->target.x, cam->target.y, cam->target.z,
				cam->fov, cam->znear, cam->zfar, cam->roll);
			
			fwrite(data, sizeof(char), strlen(data), myFile);	
			free(data);
			fclose(myFile);
			
			demoSystem.keys[KEY_CAPTURE] = FALSE;
		}

		// move target
		if (demoSystem.mouseButtons & SDL_BUTTON(1))
			camera_axis_mov (cam, demoSystem.mouseXvar, demoSystem.mouseYvar);

		// forward move
		if (demoSystem.keys[KEY_FORWARD])
			accel += 50.0f*t;
		
		if (demoSystem.keys[KEY_BACKWARD])
			accel -= 50.0f*t;

		if (fabs(accel) > FLT_EPSILON) {
			if (accel > 15) accel = 15;
			if (accel < -15) accel = -15;

			camera_front_mov (cam, accel*t);

			at = 20.0f*t;
			if (accel > at) accel -= at;
			else if (accel < -at) accel += at;
			else accel = 0;
		}

		// strafe move
		if (demoSystem.keys[KEY_STRAFELEFT]) accel_strafe -= 50.0f*t;
		if (demoSystem.keys[KEY_STRAFERIGHT]) accel_strafe += 50.0f*t;

		if (fabs(accel_strafe) > FLT_EPSILON) {
			if (accel_strafe > 15) accel_strafe = 15;
			if (accel_strafe < -15) accel_strafe = -15;
			
			camera_strafe_mov (cam, accel_strafe*t);

			at = 20.0f*t;
			if (accel_strafe > at) accel_strafe -= at;
			else if (accel_strafe < -at) accel_strafe += at;
			else accel_strafe = 0;
		}

		// camera fov
		if (demoSystem.keys[KEY_FOVUP]) cam->fov++;
		if (demoSystem.keys[KEY_FOVDOWN]) cam->fov--;

		// camera roll
		if (demoSystem.keys[KEY_ROLLUP]) cam->roll--;
		if (demoSystem.keys[KEY_ROLLDOWN]) cam->roll++;

		// reset camera
		if (demoSystem.keys[KEY_CAMRESET]) {
			camera_reset(cam);
		}
	}
}
