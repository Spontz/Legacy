#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	rect3d_t	quad;
	int			texture;
	tExpression Evaluator;			// A equation containing the calculations used to position the object
} image2_section;

static image2_section *local;

// ******************************************************************

void preload_image2 () {
}

// ******************************************************************

void load_image2 () {
	char *evalEquation;
	int equationSize;

	// script validation
	if ((mySection->stringNum != 5)||(mySection->paramNum   > 2)) {
		section_error("image2: 5 strings needed, up to 2 params needed");
		return;
	}

	local = malloc(sizeof(image2_section));
	mySection->vars = (void *) local;

	// load texture
	if (mySection->paramNum == 2 && fabs(mySection->param[0] - 1.0f) < FLT_EPSILON) {
		local->texture = demoSystem.texRenderingBuffer[(int)mySection->param[1]];
	} else {
		local->texture = tex_load (mySection->strings[0], USE_CACHE, 0);
		if (local->texture == -1)
			return;
		tex_upload (local->texture, USE_CACHE);
	}
	
	// Particle evaluator
	equationSize  = strlen(mySection->strings[1]);
	equationSize += strlen(mySection->strings[2]);
	equationSize += strlen(mySection->strings[3]);
	equationSize += strlen(mySection->strings[4]);
	evalEquation = calloc(equationSize+1, sizeof(char));
	strcpy(evalEquation, (char *) mySection->strings[1]); //-V525
	strcat(evalEquation, (char *) mySection->strings[2]);
	strcat(evalEquation, (char *) mySection->strings[3]);
	strcat(evalEquation, (char *) mySection->strings[4]);
	local->Evaluator.equation = evalEquation;
	
	initExpression(&local->Evaluator);

	// rect3D initialization
	// rect3d_init(&local->quad, demoSystem.rtt, glDriver.rttWidth, glDriver.rttHeight);
	rect3d_init(&local->quad, local->texture, tex_array[local->texture]->width, tex_array[local->texture]->height);
	mySection->loaded=1;
}

// ******************************************************************

void init_image2 () {
}

// ************************************************************

void render_image2 () {
	double width, height;
	
	local = (image2_section *) mySection->vars;
	
	exprValListAdd(local->Evaluator.v, "w0", (EXPRTYPE)tex_array[local->texture]->width );
	exprValListAdd(local->Evaluator.v, "h0", (EXPRTYPE)tex_array[local->texture]->height);
	
	//glDisable(GL_DEPTH_TEST);
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	}
	
	if (mySection->hasAlpha) {
		float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(mySection->alphaFunc, alpha);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		// Place the image in 3D space
		deprecated_place_object (local->Evaluator);
	
		// Retrieve the values and assign them
		exprValListGet(local->Evaluator.v, "w", &width  );
		exprValListGet(local->Evaluator.v, "h", &height );
		
		rect3d_init(&local->quad, local->texture, (int)width, (int)height);
		tex_properties(local->texture, NO_MIPMAP);
		rect3d_render(&local->quad);
	glPopMatrix();

	if (mySection->hasAlpha) glDisable(GL_ALPHA_TEST);
	if (mySection->hasBlend) glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_image2 () {
}
