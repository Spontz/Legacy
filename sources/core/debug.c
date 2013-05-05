/*
	debug.c: debug generation info
*/

#include "../main.h"
#include "debug.h"
#include "text.h"

// ******************************************************************

char *strDebugFont = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^-`abcdefghijklmnopqrstuvwxyz{|}~ Cª<º+'º'>¿ÀÁÂAAAAÇÈÉEEÌÍIIÑÒÓOOO0ÙÚUUÝpbàáaaaaaçèéeeìíiiñòóooooùúuuýyy ";
font debugFont;

char *stateStr[] = {"play", "play - RW", "play - FF",
					"paused", "paused - RW", "paused - FF"};

// ******************************************************************

void drawTiming() {

	char *state;

	if (demoSystem.state & DEMO_PAUSE) {
		if (demoSystem.state & DEMO_REWIND) state = stateStr[4];
		else if (demoSystem.state & DEMO_FASTFORWARD) state = stateStr[5];
		else state = stateStr[3];

	} else {
		if (demoSystem.state & DEMO_REWIND) state = stateStr[1];
		else if (demoSystem.state & DEMO_FASTFORWARD) state = stateStr[2];
		else state = stateStr[0];
	}

	glPrintf(debugFont, "fps %.0f\n%d - %.1f/%.1f\nsound %0.1f%% chan %d\ntexmem %.2fM\n%s",
				demoSystem.fps,demoSystem.frameCount,demoSystem.runTime,demoSystem.endTime,
				sound_cpu(),sound_channels(),(float)tex_get_used_memory()/1048576.0f,state);
}

// ******************************************************************

void drawFps() {

	glPrintf(debugFont, "fps %.0f", demoSystem.fps);
}
