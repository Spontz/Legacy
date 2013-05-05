#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	// textures
	int tex_front, tex_back;

	// background quad
	rect2d_t rect_QUAD;

	// progress bar
	float red, green, blue, alpha;
	float barra[4], barWidth;

	// progress bar border
	float bred, bgreen, bblue, balpha;
	float border[4], borderWidth;
} loading_section;

static loading_section *local;
static char byDefault;

// ******************************************************************

void preload_loading() {

	// empty since no global variables are used between instances
}

// ******************************************************************

void load_loading() {

	// script validation
	if ((mySection->paramNum  != 17) || (mySection->stringNum !=  2)) {
		dkernel_trace("17 params and 2 strings needed. Using default values.");
		byDefault = 1;
	} else {
		byDefault = 0;
	}

	local = malloc(sizeof(loading_section));
	mySection->vars = (void *) local;

	// texture loading
	if (!byDefault) {
		local->tex_back = tex_load(mySection->strings[0], USE_CACHE, 0);
		tex_properties(local->tex_back, CLAMP); // it should be NO_MIPMAP
		tex_upload(local->tex_back, USE_CACHE);

		local->tex_front = tex_load(mySection->strings[1], USE_CACHE, 0);
		tex_properties(local->tex_front, CLAMP); // it should be NO_MIPMAP
		tex_upload(local->tex_front, USE_CACHE);

		// progress bar color
		local->red = mySection->param[0];
		local->green = mySection->param[1];
		local->blue = mySection->param[2];
		local->alpha = mySection->param[3];

		// progress bar position
		local->barra[0] = mySection->param[4];
		local->barra[1] = mySection->param[5];
		local->barra[2] = mySection->param[6];
		local->barra[3] = mySection->param[7];
		local->barWidth = local->barra[2] - local->barra[0];

		// progress bar border color
		local->bred = mySection->param[8];
		local->bgreen = mySection->param[9];
		local->bblue = mySection->param[10];
		local->balpha = mySection->param[11];

		// progress bar position and width
		local->border[0] = mySection->param[12];
		local->border[1] = mySection->param[13];
		local->border[2] = mySection->param[14];
		local->border[3] = mySection->param[15];
		local->borderWidth = mySection->param[16];

	} else {
		
		// progress bar color will be white
		local->red = 1;
		local->green = 1;
		local->blue = 1;
		local->alpha = 1;
		
		// progress bar position
		local->barra[0] = 0.25f;
		local->barra[1] = 0.37f;
		local->barra[2] = 0.75f;
		local->barra[3] = 0.40f;
		local->barWidth = local->barra[2] - local->barra[0];
		
		// progress bar border color will be white
		local->bred = 1;
		local->bgreen = 1;
		local->bblue = 1;
		local->balpha = 1;
		
		// border bar position and width will fit the default bar
		local->border[0] = local->barra[0];
		local->border[1] = local->barra[1];
		local->border[2] = local->barra[2];
		local->border[3] = local->barra[3];
		local->borderWidth = 0.001f;
	}
	
	// background quad
	local->rect_QUAD.x0 = 0.0f;
	local->rect_QUAD.x1 = 1.0f;
	local->rect_QUAD.y0 = 0.0f;
	local->rect_QUAD.y1 = 1.0f;
	
	local->rect_QUAD.pc[0][0] = local->rect_QUAD.x0;
	local->rect_QUAD.pc[1][0] = local->rect_QUAD.x1;
	local->rect_QUAD.pc[2][0] = local->rect_QUAD.x1;
	local->rect_QUAD.pc[3][0] = local->rect_QUAD.x0;
	
	local->rect_QUAD.pc[0][1] = local->rect_QUAD.y0;
	local->rect_QUAD.pc[1][1] = local->rect_QUAD.y0;
	local->rect_QUAD.pc[2][1] = local->rect_QUAD.y1;
	local->rect_QUAD.pc[3][1] = local->rect_QUAD.y1;
	
	local->rect_QUAD.tc[0][0] = 0; local->rect_QUAD.tc[0][1] = 1;
	local->rect_QUAD.tc[1][0] = 1; local->rect_QUAD.tc[1][1] = 1;
	local->rect_QUAD.tc[2][0] = 1; local->rect_QUAD.tc[2][1] = 0;
	local->rect_QUAD.tc[3][0] = 0; local->rect_QUAD.tc[3][1] = 0;
	
	mySection->loaded=1;
}

// ******************************************************************

void init_loading() {
}

// ******************************************************************

static void render_init () {

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	camera_set2d();
}

// ******************************************************************

static void render_end () {

	camera_restore();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void render_loading() {

	float zero2one;
	int i;

	local = (loading_section *) mySection->vars;

	if (demoSystem.loadedSections == 0) zero2one = 0;
	else zero2one = (float) demoSystem.loadedSections / (float) (demoSystem.numSections + demoSystem.numReadySections);

	glClearColor (0,0,0,0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// init loading render
	render_init ();

	if (!byDefault) {
		// background
		glColor4f (1,1,1,1);
		tex_bind (local->tex_back);
		glBegin (GL_QUADS);
			for (i=0; i<4; i++)	{
				glTexCoord2fv (local->rect_QUAD.tc[i]);
				glVertex2fv (local->rect_QUAD.pc[i]);
			}
		glEnd ();

		// foreground
		glColor4f (1,1,1,zero2one);
		tex_bind (local->tex_front);
		glBegin (GL_QUADS);
			for (i=0; i<4; i++)	{
				glTexCoord2fv (local->rect_QUAD.tc[i]);
				glVertex2fv (local->rect_QUAD.pc[i]);
			}
		glEnd ();
	}
	
	// progress bar
	glDisable (GL_TEXTURE_2D);

		// border frame
		glColor4f (local->bred,local->bgreen,local->bblue,local->balpha);
		glLineWidth(local->borderWidth * glDriver.width);
		glBegin (GL_LINE_LOOP);
			glVertex2f (local->border[0], local->border[1]);
			glVertex2f (local->border[0], local->border[3]);
			glVertex2f (local->border[2], local->border[3]);
			glVertex2f (local->border[2], local->border[1]);
		glEnd ();
		glLineWidth(1.0f);

		// bar
		glColor4f (local->red,local->green,local->blue,local->alpha);
		glBegin (GL_QUADS);
			glVertex2f (local->barra[0], local->barra[1]);
			glVertex2f (local->barra[0], local->barra[3]);
			glVertex2f (local->barra[0] + (local->barWidth * zero2one), local->barra[3]);
			glVertex2f (local->barra[0] + (local->barWidth * zero2one), local->barra[1]);
		glEnd ();

	glEnable (GL_TEXTURE_2D);

	// end loading render and swap
	render_end ();
	gldrv_swap ();
}

// ******************************************************************

void end_loading() {

	// local = (loading_section *) mySection->vars;

	// free textures
	// tex_free(local->tex_front);
	// tex_free(local->tex_back);

	// free section
	// free(local);
}
