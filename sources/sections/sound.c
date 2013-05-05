#include "../interface/demo.h"

// ******************************************************************

#ifdef WIN32
    #include <fmod.h>
    #include <fmod_errors.h>
#else
    #include "../fmod/macppc/inc/fmod.h"
    #include "../fmod/macppc/inc/fmod_errors.h"
	#include "../fmod/macppc/inc/wincompat.h"
#endif

// ******************************************************************

#define BUFFER_SAMPLES	512

#define DEFAULT_ENERGY	0.0f
#define BEAT_RATIO		1.4f
#define FADE_OUT		4.0f

// ******************************************************************

typedef struct {
	// Sound information
	FSOUND_STREAM *stream;
	int channel;
	char *data;
	
	// Beat Information
	float energy[BUFFER_SAMPLES];
	float default_energy;
	float beat_ratio;
	float fade_out;
	float intensity;
	int position;
} sound_section;

static sound_section *local;

// ******************************************************************

#define sound_error() (section_error("FMod: %s", FMOD_ErrorString(FSOUND_GetError())))

// ******************************************************************

void preload_sound () {
}

// ******************************************************************

void load_sound () {

	FILE *f;
	int length;

	// exit if no sound
	if (!demoSystem.sound) return;

	if (mySection->stringNum != 1) {
		section_error("1 string needed");
		return;
	}
	
	local = malloc(sizeof(sound_section));
	mySection->vars = (void *) local;

	// open sound file
	f = fopen(mySection->strings[0],"rb");
	if (!f) {
		section_error("file '%s' not found", mySection->strings[0]);
		return;
	}
	
	// get file size
	fseek(f,0,SEEK_END);
	length = ftell(f);
	fseek(f,0,SEEK_SET);

	// load file to memory
	local->data = (char *) malloc(length);
	fread(local->data,length,1,f);
	fclose(f);

	// create new stream
	local->stream = FSOUND_Stream_Open(local->data,FSOUND_NORMAL | FSOUND_MPEGACCURATE | FSOUND_LOADMEMORY,0,length);
	if (!local->stream) {
		sound_error();
		return;
	}
	// Beat detection specific information
	if (mySection->paramNum == 0); // default parameters will be used
	else if (mySection->paramNum != 3) {
		section_error("sound: 3 params needed");
		return;
	}
	mySection->loaded=1;
}

// ******************************************************************

void init_sound () {
	FSOUND_DSPUNIT *unit;
	int i;
	
	// exit if no sound
	if (!demoSystem.sound) return;

	// exit if demo is not playing
	if (demoSystem.state != DEMO_PLAY) return;

	local = (sound_section *) mySection->vars;

	// seek if the section doesn't start from the begining
	if (mySection->runTime > 0) {
		if (!FSOUND_Stream_SetTime(local->stream,(int)(mySection->runTime*1000.0f))) sound_error();
	}

	local->channel = FSOUND_Stream_Play(FSOUND_FREE,local->stream);
	if (local->channel < 0) sound_error();
	
	// Beat detection specific information
	unit = FSOUND_DSP_GetFFTUnit();
	FSOUND_DSP_SetActive(unit, TRUE);

	// get parameters
	if (mySection->paramNum == 0) {
		local->default_energy = DEFAULT_ENERGY;
		local->beat_ratio = BEAT_RATIO;
		local->fade_out = FADE_OUT;
	}
	else {
		local->default_energy = mySection->param[0];
		local->beat_ratio = mySection->param[1];
		local->fade_out = mySection->param[2];
	}

	// clean variables
	for (i=0; i<BUFFER_SAMPLES; i++) {
		local->energy[i] = local->default_energy;
	}
	
	local->intensity = 0;
	local->position = 1;
}

// ******************************************************************

void render_sound () {
	float *spectrum, instant, avg;
	int i;
			
	// Update the beat information in the demo structure
	local = (sound_section *) mySection->vars;

	spectrum = FSOUND_DSP_GetSpectrum();
	// deactivated sound?
	if (spectrum == NULL) return;

	// get instant energy
	instant = 0;
	for (i=0; i<512; i++) {
		instant += spectrum[i];
	}

	// calculate average energy in last samples
	avg = 0;
	for (i=0; i<BUFFER_SAMPLES; i++) {
		avg += local->energy[i];
	}
	avg /= (float) local->position;

	// instant sample is a beat?
	if ((instant / avg) > local->beat_ratio) {
		local->intensity = 1.0f;
	} else if (local->intensity > 0) {
		local->intensity -= local->fade_out * demoSystem.frameTime;
		if (local->intensity < 0) local->intensity = 0;
	}

	// updated kernel shared variable
	// to be used by kernel itself or another sections
	demoSystem.beat = local->intensity;

	// update energy buffer
	if (local->position < BUFFER_SAMPLES) {
		local->energy[local->position-1] = instant;
		local->position++;
	}
	else {
		for (i=1; i<BUFFER_SAMPLES; i++) {
			local->energy[i-1] = local->energy[i];
		}
		local->energy[BUFFER_SAMPLES-1] = instant;
	}
	
	// Spectrum drawing (Only in debug mode, when the timing information is also being displayed)
	if ((demoSystem.debug)) {
		glDisable(GL_DEPTH_TEST);
		
		if (mySection->hasBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(mySection->sfactor, mySection->dfactor);
		}
		
		camera_set2d();

		glDisable(GL_TEXTURE_2D);
			glColor4f(1,1,1,1);
			
			glBegin(GL_LINES);
			
				for (i=0; i<512; i++) {
					glVertex2f(i/640.0f, spectrum[i]);
					glVertex2f(i/640.0f, 0);
				}
			
			glEnd ();
		glEnable(GL_TEXTURE_2D);

		/* if (local->intensity > 0) {
			glColor4f(local->intensity, local->intensity, local->intensity, local->intensity);
			tex_bind(local->texture);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2f(0.9f,0.9f);

				glTexCoord2f(1,0);
				glVertex2f(1,0.9f);

				glTexCoord2f(1,1);
				glVertex2f(1,1);

				glTexCoord2f(0,1);
				glVertex2f(0.9f,1);
			glEnd();
		} */

		camera_restore();

		if (mySection->hasBlend) glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}

// ******************************************************************

void end_sound () {

	// exit if no sound
	if (!demoSystem.sound) return;

	local = (sound_section *) mySection->vars;

	FSOUND_Stream_Stop(local->stream);

	//FSOUND_Stream_Close(local->stream);
	//free(local->data);
	//free(local);
}
