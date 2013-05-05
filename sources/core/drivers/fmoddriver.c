/*
	fmoddriver.c: fmod interface
*/

#include "../../main.h"
#include "fmoddriver.h"

// ******************************************************************

#define sound_error() (dkernel_error("FMod: %s", FMOD_ErrorString(FSOUND_GetError())))

// ******************************************************************

void sound_init() {

	int i, driver, numDrivers;

    /*FMOD::System     *system;
    FMOD_RESULT       result;

    result = FMOD::System_Create(&system);
	ERRCHECK(result);
	
    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        dkernel_error("fmoddriver: You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        //getch();
        //return 0;
    }
	
	result = system->init(1, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result); */
	
	
	if (FSOUND_GetVersion() < FMOD_VERSION)
		dkernel_error("Wrong FMOD version\nIt should be %.02f", FMOD_VERSION);

#ifdef _WIN32
	if (!FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND)) sound_error();
#elif __APPLE__
	if (!FSOUND_SetOutput(FSOUND_OUTPUT_MAC)) sound_error();
#else
	if (!FSOUND_SetOutput(FSOUND_OUTPUT_OSS)) sound_error();
#endif

	i = 0;
	driver = FALSE;
	numDrivers = FSOUND_GetNumDrivers();

	while (!driver && (i < numDrivers)) {
		driver = FSOUND_SetDriver(i);
		if (driver) driver = FSOUND_Init(44100,32,0);
		i++;
	}

	if (!driver) sound_error();
}

// ******************************************************************

void sound_play() {

	FSOUND_SetPaused(FSOUND_ALL,FALSE);
}

// ******************************************************************

void sound_update() {
		
	FSOUND_Update();
}

// ******************************************************************

void sound_pause() {

	FSOUND_SetPaused(FSOUND_ALL,TRUE);
}

// ******************************************************************

void sound_stop() {

	FSOUND_StopSound(FSOUND_ALL);
}

// ******************************************************************

void sound_end() {

	FSOUND_Close();
}

// ******************************************************************

float sound_cpu() {

	return (float) FSOUND_GetCPUUsage();
}

// ******************************************************************

int sound_channels() {

	return FSOUND_GetChannelsPlaying();
}
