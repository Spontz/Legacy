#include "../interface/demo.h"
#include "../core/drivers/events.h"

// ******************************************************************

typedef struct {
	rect3d_t	quad;
	int			texture;
	char		editMode;
	
	float Scale[3];
	float Rotate[3];
	float Translate[3];
} imageMatrix_section;

static imageMatrix_section *local;

// ******************************************************************

void preload_imageMatrix () {
}

// ******************************************************************

void load_imageMatrix() {

	// script validation
	if ((mySection->paramNum  != 5)||(mySection->stringNum != 1)||(mySection->splineNum != 2)) {
		section_error("imageMatrix: 5 params: 1 string and 2 splines needed");
		return;
	}

	local = malloc(sizeof(imageMatrix_section));
	mySection->vars = (void *) local;

	// Check if we want to use a predefined texture file or the contents of a
	// buffer already rendered and stored in the graphic card memory
	if (fabs(mySection->param[1]) < FLT_EPSILON)
		{
		// We are using a texture, so upload it to the graphic card memory
		local->texture = tex_load (mySection->strings[0], USE_CACHE, 0);
		if (local->texture == -1)
			return;
		tex_properties(local->texture, NO_MIPMAP | CLAMP);
		tex_upload (local->texture, USE_CACHE);
		rect3d_init(&local->quad, local->texture, tex_array[local->texture]->owidth, tex_array[local->texture]->oheight);
		}
	else if (fabs(mySection->param[1] - 1) < FLT_EPSILON)
		{
		// We are using a pre-rendered texture stored in a buffer
		local->texture = demoSystem.texRenderingBuffer[(int)mySection->param[2]];
		rect3d_init(&local->quad, local->texture, tex_array[local->texture]->width, tex_array[local->texture]->height);
		}
	else
		{
		// Unexpected value found
		section_error("imageMatrix: Invalid value for texture or buffer usage selection");
		return;
		}

	// Edit mode flag
	if (fabs(mySection->param[4] - 1) < FLT_EPSILON)
		local->editMode = 1;
	
	// Default values for the editable parameters
	local->Scale[0] = 0.0f;
	local->Scale[1] = 0.0f;
	local->Scale[2] = 0.0f;
	
	local->Rotate[0] = 0.f;
	local->Rotate[1] = 0.f;
	local->Rotate[2] = 0.f;
	
	local->Translate[0] = 0.f;
	local->Translate[1] = 0.f;
	local->Translate[2] = 0.f;
	mySection->loaded=1;
}

// ******************************************************************

void init_imageMatrix () {
}

// ******************************************************************

static void prepare_object (float step, int path) {
	
//	ChanVec resVec;
    local = (imageMatrix_section *) mySection->vars;

		// Because the buffered texture is larger than the screen size we must modify the texture coordinates
		// for the drawn quad. We will fit the quad boundaries with the visible buffer texture boundaries.
		// This way, those parts of the buffered texture that have not been drawn (because being out of the viewport)
		// won't appear reflected in the image matrix objects
	if (fabs(mySection->param[1] - 1) < FLT_EPSILON)
		{
		local->quad.tc[1][0] = (float)glDriver.vpWidth  / (float)tex_array[local->texture]->width ;
		local->quad.tc[2][0] = (float)glDriver.vpWidth  / (float)tex_array[local->texture]->width ;
		local->quad.tc[2][1] = (float)glDriver.vpHeight / (float)tex_array[local->texture]->height;
		local->quad.tc[3][1] = (float)glDriver.vpHeight / (float)tex_array[local->texture]->height;
		}
	
	/*
	// In order to execute this routine, we must be running in debug mode
	// and the editMode flag must be set to true
	if ((demoSystem.debug) & (local->editMode == 1)) {
				
		// Scale X
		if (demoSystem.keys[KEY_NUM_1]) {
			if (demoSystem.keys[KEY_MORE])   local->Scale[0] = local->Scale[0] + 0.1f;
			if (demoSystem.keys[KEY_LESS]) local->Scale[0] = local->Scale[0] - 0.1f;
		}

		// Scale Y
		if (demoSystem.keys[KEY_NUM_2]) {
			if (demoSystem.keys[KEY_MORE])   local->Scale[1] = local->Scale[1] + 0.1f;
			if (demoSystem.keys[KEY_LESS]) local->Scale[1] = local->Scale[1] - 0.1f;
		}
		
		// Scale Z
		if (demoSystem.keys[KEY_NUM_3]) {
			if (demoSystem.keys[KEY_MORE])   local->Scale[2] = local->Scale[2] + 0.1f;
			if (demoSystem.keys[KEY_LESS]) local->Scale[2] = local->Scale[2] - 0.1f;
		}

		// Rotation X
		if (demoSystem.keys[KEY_NUM_4]) {
			if (demoSystem.keys[KEY_MORE])   local->Rotate[0] = local->Rotate[0] + 0.5f;
			if (demoSystem.keys[KEY_LESS]) local->Rotate[0] = local->Rotate[0] - 0.5f;
		}
		
		// Rotation Y
		if (demoSystem.keys[KEY_NUM_5]) {
			if (demoSystem.keys[KEY_MORE])   local->Rotate[1] = local->Rotate[1] + 0.5f;
			if (demoSystem.keys[KEY_LESS]) local->Rotate[1] = local->Rotate[1] - 0.5f;
		}
		
		// Rotation Z
		if (demoSystem.keys[KEY_NUM_6]) {
			if (demoSystem.keys[KEY_MORE])   local->Rotate[2] = local->Rotate[2] + 0.5f;
			if (demoSystem.keys[KEY_LESS]) local->Rotate[2] = local->Rotate[2] - 0.5f;
		}
		
		// Translate X
		if (demoSystem.keys[KEY_NUM_7]) {
			if (demoSystem.keys[KEY_MORE])   local->Translate[0] = local->Translate[0] + 0.5f;
			if (demoSystem.keys[KEY_LESS]) local->Translate[0] = local->Translate[0] - 0.5f;
		}
		
		// Translate Y
		if (demoSystem.keys[KEY_NUM_8]) {
			if (demoSystem.keys[KEY_MORE])   local->Translate[1] = local->Translate[1] + 0.5f;
			if (demoSystem.keys[KEY_LESS]) local->Translate[1] = local->Translate[1] - 0.5f;
		}
		
		// Translate Z
		if (demoSystem.keys[KEY_NUM_9]) {
			if (demoSystem.keys[KEY_MORE])   local->Translate[2] = local->Translate[2] + 0.5f;
			if (demoSystem.keys[KEY_LESS]) local->Translate[2] = local->Translate[2] - 0.5f;
		}

		// Reset variations
		if (demoSystem.keys[KEY_RESET]) {
			local->Scale[0] = 0.f;
			local->Scale[1] = 0.f;
			local->Scale[2] = 0.f;
		
			local->Rotate[0] = 0.f;
			local->Rotate[1] = 0.f;
			local->Rotate[2] = 0.f;
		
			local->Translate[0] = 0.f;
			local->Translate[1] = 0.f;
			local->Translate[2] = 0.f;
		}
						
		// Reset the key states to allow only a single pulsation
		if (!demoSystem.keys[KEY_MODIFICATOR]) {
			demoSystem.keys[KEY_MORE] = 0;
			demoSystem.keys[KEY_LESS] = 0;
		}
	}

	dkernel_getSpline(path, step, resVec);

	if (path == 1) {
		glTranslatef (resVec[6] + local->Translate[0], resVec[7] + local->Translate[1], resVec[8] + local->Translate[2]);
		glRotatef    (resVec[3] + local->Rotate[0], 1, 0, 0);
		glRotatef    (resVec[4] + local->Rotate[1], 0, 1, 0);
		glRotatef    (resVec[5] + local->Rotate[2], 0, 0, 1);
		glScalef     (resVec[0] + local->Scale[0], resVec[1] + local->Scale[1], resVec[2] + local->Scale[2]);
	} else {
		glTranslatef (resVec[6], resVec[7], resVec[8]);
		glRotatef    (resVec[3], 1, 0, 0);
		glRotatef    (resVec[4], 0, 1, 0);
		glRotatef    (resVec[5], 0, 0, 1);
		glScalef     (resVec[0], resVec[1], resVec[2]);
	}
	
	if ((demoSystem.debug) & (local->editMode == 1) & (path == 1)) {
		// Capture current data
		if (demoSystem.keys[KEY_DUMP]) {
			fprintf(stdout, ";ScaleX ScaleY ScaleZ RotateX RotateY RotateZ TranslateX TranslateY TranslateZ\n");
			fprintf(stdout, "%f %f %f %f %f %f %f %f %F\n", resVec[0] + local->Scale[0], resVec[1] + local->Scale[1], resVec[2] + local->Scale[2], resVec[3] + local->Rotate[0], resVec[4] + local->Rotate[1], resVec[5] + local->Rotate[2], resVec[6] + local->Translate[0], resVec[7] + local->Translate[1], resVec[8] + local->Translate[2]);
			demoSystem.keys[KEY_DUMP] = 0;
		}
	} */
}

// ************************************************************

void render_imageMatrix ()
	{
	int i;
	
	local = (imageMatrix_section *) mySection->vars;
						
	if (fabs(mySection->param[3] - 1) < FLT_EPSILON)
		glDisable(GL_DEPTH_TEST);

	if (mySection->hasBlend)
		{
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
		}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// matrix preparation for the first object
	prepare_object(mySection->runTime, 0);

	// rendering of the following objects
	for (i=0; i<mySection->param[0]; i++)
		{
		AxisDrawing();

		rect3d_render(&local->quad);

		prepare_object(mySection->runTime, 1);
		}

	glPopMatrix();
	
	if (mySection->hasBlend)
		glDisable(GL_BLEND);

	if (fabs(mySection->param[2] - 1) < FLT_EPSILON)
		glEnable(GL_DEPTH_TEST);
	}

// ******************************************************************

void end_imageMatrix() {
}
