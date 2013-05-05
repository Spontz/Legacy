/*
	timing.c: portable timing code
*/

#include "../main.h"

// ******************************************************************
// executed before play the sound and execute the main loop

void init_timer () {

	demoSystem.beforeFrameTime = 0.001f * (float) SDL_GetTicks();
}

// ******************************************************************
// calculation of frames per second

void calculate_fps (float frameTime) {

	demoSystem.accumFrameTime += frameTime;
	demoSystem.accumFrameCount++;
	if (demoSystem.accumFrameTime > 0.3f) {
		demoSystem.fps = (float) demoSystem.accumFrameCount / demoSystem.accumFrameTime;
		demoSystem.accumFrameTime = 0;
		demoSystem.accumFrameCount = 0;
	}
}

// ******************************************************************
// executed in the main loop

void process_timer () {

	// frame time calculation
	if (demoSystem.record) {
		demoSystem.realFrameTime = 1.0f / demoSystem.recordFps;

	} else {
		demoSystem.afterFrameTime = 0.001f * (float) SDL_GetTicks();
		demoSystem.realFrameTime = demoSystem.afterFrameTime - demoSystem.beforeFrameTime;
		demoSystem.beforeFrameTime = demoSystem.afterFrameTime;
	}

	// advance sections and demo time
	demoSystem.frameTime = demoSystem.realFrameTime;
	demoSystem.runTime += demoSystem.frameTime;

	// frame count
	demoSystem.frameCount++;

	// fps calculation
	calculate_fps(demoSystem.frameTime);
}

// ******************************************************************

void pause_timer () {

	// frame time calculation
	demoSystem.afterFrameTime = 0.001f * (float) SDL_GetTicks();
	demoSystem.realFrameTime = demoSystem.afterFrameTime - demoSystem.beforeFrameTime;
	demoSystem.beforeFrameTime = demoSystem.afterFrameTime;

	// sections should not advance
	demoSystem.frameTime = 0;

	// frame count
	demoSystem.frameCount++;

	// fps calculation
	calculate_fps(demoSystem.realFrameTime);
}
