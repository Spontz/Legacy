#include "../interface/demo.h"

void preload_saveBuffer()
	{
	}

void load_saveBuffer()
	{
	// Check for the right number of parameters in the script
	if (mySection->paramNum > 3)
		{
		section_error("saveBuffer: 3 params required.");
		return;
		}

	// Check for the right kind of parameters
	if (  (mySection->param[0] < 0.0f) || (mySection->param[0] > (float)RENDERING_BUFFERS)  )
		{
		section_error("saveBuffer: Invalid texture buffer number: %i", mySection->param[0]);
		return;
		}

	mySection->loaded = 1;
	}

void init_saveBuffer()
	{
	}

void render_saveBuffer()
	{
	char OGLError[1024];

	while(gl_drv_check_for_gl_errors(OGLError))
		section_error("OGL Error before render_saveBuffer:\n\n%s", OGLError);

	// Enable the buffer in which we are going to paint
	tex_bind(demoSystem.texRenderingBuffer[(int)mySection->param[0]]);

	while(gl_drv_check_for_gl_errors(OGLError))
		section_error("OGL Error in tex_bind in render_saveBuffer:\n\n%s", OGLError);

	// Copy the color buffer contents into the buffer we activated before
	gldrv_copyColorBuffer();

	// Clear the screen and depth buffers depending of the parameters passed by the user
	if (fabs(mySection->param[1] - 1.0f) < FLT_EPSILON)
		glClear(GL_COLOR_BUFFER_BIT);

	if (fabs(mySection->param[2] - 1.0f) < FLT_EPSILON)
		glClear(GL_DEPTH_BUFFER_BIT);
	}

void end_saveBuffer()
	{
	}
