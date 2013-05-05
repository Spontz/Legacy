#include "../interface/demo.h"

typedef struct
	{
	// A equation containing the calculations used to position the rendering QUAD
	tExpression evalPositioning;
	} renderBuffer_section;

static renderBuffer_section* pLocal;

void preload_renderBuffer()
	{
	}

void load_renderBuffer()
	{
	char OGLError[1024];

	char *positionEquation;
	int theSize;
	
	pLocal = malloc(sizeof(renderBuffer_section));

	if (!pLocal)
		{
		dkernel_error("insuficient memory");
		return;
		}
	else
		{
		mySection->vars = (void *) pLocal;
		
		// Check for the right number of parameters in the script
		if (mySection->paramNum > 4)
			{
			section_error("renderBuffer: 4 params required.");
			return;
			}

		// Check for the right kind of parameters
		if (  (mySection->param[0] < -1.0f) || (mySection->param[0] > (float)RENDERING_BUFFERS)  )
			{
			section_error("renderBuffer: Invalid texture buffer number: %i", mySection->param[0]);
			return;
			}

		if (fabs(mySection->param[3]) < FLT_EPSILON)
			{
			// QUAD rendering, so load the positioning equations
			theSize = strlen(mySection->strings[0]) + strlen(mySection->strings[1]) + strlen(mySection->strings[2]);
			positionEquation = calloc(theSize+1, sizeof(char));
			sprintf(positionEquation, "%s%s%s", mySection->strings[0], mySection->strings[1], mySection->strings[2]);
		
			pLocal->evalPositioning.equation = positionEquation;
			
			// Init the expression evaluation library
			initExpression(&pLocal->evalPositioning);
			}
		}

	while (gl_drv_check_for_gl_errors(OGLError))
		section_error("OGL Error in load_renderBuffer:\n\n%s", OGLError);

	mySection->loaded=1;
	}

void init_renderBuffer()
	{
	}

extern unsigned int get_closest_power_of_two(unsigned int value);

void draw_fucking_quad()
	{
	unsigned int i;
	float x0, y0, x1, y1;
	float fx, fy;
	rect2d_t quad;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	// store vertex values
	quad.pc[0][0] = x0;
	quad.pc[1][0] = x1;
	quad.pc[2][0] = x1;
	quad.pc[3][0] = x0;

	quad.pc[0][1] = y0;
	quad.pc[1][1] = y0;
	quad.pc[2][1] = y1;
	quad.pc[3][1] = y1;

	fx = (float)glDriver.vpWidth / (float)get_closest_power_of_two(glDriver.vpWidth);
	fy = (float)glDriver.vpHeight / (float)get_closest_power_of_two(glDriver.vpHeight);

	quad.tc[0][0] = 0;	quad.tc[0][1] = 0;
	quad.tc[1][0] = fx;	quad.tc[1][1] = 0;
	quad.tc[2][0] = fx;	quad.tc[2][1] = fy;
	quad.tc[3][0] = 0;	quad.tc[3][1] = fy;

	camera_set2d();

//			tex_bind(local->texture);
	glBegin(GL_QUADS);
	for (i=0; i<4; ++i)
		{
		glTexCoord2fv(quad.tc[i]);
		glVertex2fv(quad.pc[i]);
		}
	glEnd();

	camera_restore();
	}

void render_renderBuffer()
	{
	char OGLError[1024];

	pLocal = (renderBuffer_section *) mySection->vars;

	// Clear the screen and depth buffers depending of the parameters passed by the user
	if (fabs(mySection->param[1] - 1.0f) < FLT_EPSILON)
		glClear(GL_COLOR_BUFFER_BIT);
	if (fabs(mySection->param[2] - 1.0f) < FLT_EPSILON)
		glClear(GL_DEPTH_BUFFER_BIT);
	
	// Exit if shaders not supported (param==-1 when we want to use renderbuffer for shaders)
	if (!glDriver.ext.glslshaders && (int)mySection->param[0]==-1)
		return;
	
	// Enable the buffer in which we are going to paint
	if ((int)mySection->param[0]>=0)
		tex_bind(demoSystem.texRenderingBuffer[(int)mySection->param[0]]);
	
	// Check if the user wants to use some kind of blending state or not
	if (mySection->hasBlend)
		{
		// The user wants to use blending, so first of all let's activate the blending register
		glEnable(GL_BLEND);
		// And then set the start and end blending factors
		glBlendFunc(mySection->sfactor, mySection->dfactor);
		}
	
	// Deactivate the depth buffer calculations to avoid interferences with other objects
	glDisable(GL_DEPTH_TEST);
	
	if (fabs(mySection->param[3] - 1) < FLT_EPSILON)
		{
		// Fullscreen rendering
		// Set the camera matrix to a 2D one
		//camera_set2d();
		
		// Paint a textured quad covering all the screen area
		// The used texture will be the one corresponding to the previously used buffer
		// gldrv_texscreenquad();

		draw_fucking_quad();

		// Restore the camera
		//camera_restore();
		}
	else
		{
		// Rendering into a free quad
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		place_object(&pLocal->evalPositioning);

		draw_fucking_quad();


			// gldrv_texscreenquad();
			/*
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0);
				glVertex2f(-gldrv_get_aspect_ratio(), -1);

				glTexCoord2f(gldrv_get_viewport_aspect_ratio(), 0);
				glVertex2f(gldrv_get_aspect_ratio(), -1);

				glTexCoord2f(gldrv_get_viewport_aspect_ratio(),(1 / gldrv_get_viewport_aspect_ratio()));
				glVertex2f(gldrv_get_aspect_ratio(), 1);

				glTexCoord2f(0, (1 / gldrv_get_viewport_aspect_ratio()));
				glVertex2f(-gldrv_get_aspect_ratio(), 1);
			glEnd();
		*/
		glPopMatrix();
		}

	// Activate again the depth buffer calculation
	glEnable (GL_DEPTH_TEST);

	// If the section was using blending, deactivate the BLEND register
	if (mySection->hasBlend)
		glDisable(GL_BLEND);

	while (gl_drv_check_for_gl_errors(OGLError))
		section_error("OGL Error in render_renderBuffer:\n\n%s", OGLError);
	}

void end_renderBuffer()
	{
	}
