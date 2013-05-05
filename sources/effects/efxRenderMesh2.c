/*
	efxRendermesh.c: color buffer modification
*/

#include "../interface/demo.h"
#include <math.h>

// ******************************************************************

#define GRID_RES 40 // Grid resolution

typedef struct {
	// texture & vertex coordinates
	float u, v;
	float x, y;
} tGridPoint;

typedef struct {
	// RGB + Alpha
	double r;
	double g;
	double b;
	double a;
} tGridColor;

typedef struct {
	tGridPoint points[GRID_RES+1][GRID_RES+1];
	
	tGridColor gridColor;
	
	tExpression deformation;
	tExpression RGBA;
} efxRenderMesh_section;

static efxRenderMesh_section *local;

// ******************************************************************

static void updateGridTexCoords() {
	double u, v;
	int row, col;

	for (row=0; row<GRID_RES+1; row++) {
		for (col=0; col<GRID_RES+1; col++) {
		
			// Insert the default values into the equation
			insertSectionVariables(&local->deformation);
			
			// Insert the custom values into the equation
			exprValListAdd(local->deformation.v, "x", (EXPRTYPE)local->points[row][col].x);
			exprValListAdd(local->deformation.v, "y", (EXPRTYPE)local->points[row][col].y);
			exprValListAdd(local->deformation.v, "r", (EXPRTYPE)sqrt(local->points[row][col].x * local->points[row][col].x + local->points[row][col].y * local->points[row][col].y));
			exprValListAdd(local->deformation.v, "p", (EXPRTYPE)atan(local->points[row][col].y / local->points[row][col].x));
			
			// Evaluate the equation
			local->deformation.err = exprEval(local->deformation.o, &local->deformation.result);
			if(local->deformation.err != EXPR_ERROR_NOERROR) {
				section_error("efxRenderMesh2: Expression Evaluation Error for deformation (%d)", local->deformation.err);
			}
			
			exprValListGet(local->deformation.v, "u", &u);
			exprValListGet(local->deformation.v, "v", &v);
				
			// map into the rtt texture
			local->points[row][col].u = (float) u * gldrv_get_viewport_aspect_ratio();
			local->points[row][col].v = (float) v * (1 / gldrv_get_viewport_aspect_ratio());
		}
	}
}

// ******************************************************************

void updateGridTextRGBA() {
	// Insert the default values into the equation
	insertSectionVariables(&local->RGBA);
		
	// Evaluate the equation
	local->RGBA.err = exprEval(local->RGBA.o, &local->RGBA.result);
	if(local->RGBA.err != EXPR_ERROR_NOERROR) section_error("efxRenderMesh2: Expression Evaluation Error for RGBA (%d)", local->RGBA.err);
	
	exprValListGet(local->RGBA.v, "red", &local->gridColor.r);
	exprValListGet(local->RGBA.v, "green", &local->gridColor.g);
	exprValListGet(local->RGBA.v, "blue", &local->gridColor.b);
	exprValListGet(local->RGBA.v, "alpha", &local->gridColor.a);
				
	// Update the glColor
	glColor4f((float)local->gridColor.r, (float)local->gridColor.g, (float)local->gridColor.b, (float)local->gridColor.a);
}

// ******************************************************************

static void drawTexture() {

	int row, col;

	glBegin(GL_QUADS);
	for (row=0; row<GRID_RES; row++) {
		for (col=0; col<GRID_RES; col++) {

			glTexCoord2fv(&local->points[row  ][col  ].u);
			glVertex2fv  (&local->points[row  ][col  ].x);

			glTexCoord2fv(&local->points[row+1][col  ].u);
			glVertex2fv  (&local->points[row+1][col  ].x);

			glTexCoord2fv(&local->points[row+1][col+1].u);
			glVertex2fv  (&local->points[row+1][col+1].x);

			glTexCoord2fv(&local->points[row  ][col+1].u);
			glVertex2fv  (&local->points[row  ][col+1].x);
		}
	}
	glEnd();
}

static void drawGrid() {
	int row, col;

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POINTS);
	for (row=0; row<GRID_RES; row++) {
		for (col=0; col<GRID_RES; col++) {
			glVertex2fv  (&local->points[row  ][col  ].u);
			glVertex2fv  (&local->points[row+1][col  ].u);
			glVertex2fv  (&local->points[row+1][col+1].u);
			glVertex2fv  (&local->points[row  ][col+1].u);
		}
	}
	glEnd();
	
	glEnable(GL_TEXTURE_2D);
}
// ******************************************************************

void preload_efxRenderMesh2 () {

}

// ******************************************************************

void load_efxRenderMesh2 () {
	int row, col;
	float step = 2.0f / (GRID_RES);
	
	// script validation
	if (mySection->stringNum != 2) {
		section_error("2 strings needed (equation for deformation and RGBA variation)");
		return;
	}
	
	local = malloc(sizeof(efxRenderMesh_section));
	
	mySection->vars = (void *) local;

	local->deformation.equation = (char *) mySection->strings[0];
	local->RGBA.equation        = (char *) mySection->strings[1];

	// Set the static position for the grid points
	for (row=0; row<GRID_RES+1; row++) {
		for (col=0; col<GRID_RES+1; col++) {
			local->points[row][col].x = -1.0f + (float) col * step;
			local->points[row][col].y =  1.0f - (float) row * step;
		}
	}
	
	// Init the default colors and alpha
	local->gridColor.r = 1.0f;
	local->gridColor.g = 1.0f;
	local->gridColor.b = 1.0f;
	local->gridColor.a = 1.0f;
	
	// Init the expression evaluation library
	initExpression(&local->deformation);
	initExpression(&local->RGBA);
	mySection->loaded=1;
}

// ******************************************************************

void init_efxRenderMesh2 () {

}

// ******************************************************************

void render_efxRenderMesh2 () {

	local = (efxRenderMesh_section *) mySection->vars;

	// Bind texture
	tex_bind(demoSystem.rtt);

	// Copy the current drawn screen to the texture
	gldrv_copyColorBuffer();

	updateGridTexCoords(); // Calculate the new texture coordinates

	glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}
	
	// camera_set2d();

	// Instead of a call to camera_set2d() we will adjust the screen boundaries manually
	// This way vwe center the 0,0 in the middle of the screen, very useful when doing
	// things with polar coordinates
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();
	glOrtho (-1,1, -1,1, -1,1);

	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();

	updateGridTextRGBA(); // Calculate the new glColor and alpha

	// Draw the transformed grid
	drawTexture();

	if (mySection->hasBlend) glDisable(GL_BLEND);
	
	if ((demoSystem.debug)) drawGrid();

	camera_restore();

	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxRenderMesh2 () {
	/*local = (efxRenderMesh_section *) mySection->vars;

	exprFree(local->o);
	exprFreeValList(local->v);
	exprFreeValList(local->c);
	exprFreeFuncList(local->f);*/
}
