#include "../interface/demo.h"
#include "../core/drivers/events.h"

// ******************************************************************

//typedef struct {
//} glslshaderunbind_section;

//static glslshaderunbind_section *local;

// ******************************************************************

void preload_glslshaderunbind () {
}

// ******************************************************************

void load_glslshaderunbind () {

//	local = malloc(sizeof(glslshaderunbind_section));
//	mySection->vars = (void *) local;
	mySection->loaded=1;
}

// ******************************************************************

void init_glslshaderunbind () {
}

// ************************************************************

void render_glslshaderunbind () {
	// Exit, if demo is being run without shaders
	if (!glDriver.ext.glslshaders)
		return;
		
	// local = (glslshaderunbind_section *) mySection->vars;
		
	// Choose the proper program shader
	glslshad_reset_bind();
	glUseProgramObjectARB(0);
}

// ******************************************************************

void end_glslshaderunbind () {
}
