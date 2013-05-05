/*
	main.c: main demo code !!!
*/

#include "main.h"

// ******************************************************************

#ifdef WIN32
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdshow)
	{
	int argc = 0;
	char** argv=0;
#else
int main(int argc, char *argv[])
	{
#endif

	dkernel_trace("Inferno: Starting up...\n");

	dkernel_getArguments(argc, argv);
	dkernel_trace("Inferno: Got arguments\n");
	
	// Remove a previous camera.cam file in order to star capturing the camera from zero
	// In the future, the captured camera values will be sent to the demo editor instead of being written to a file
	remove("camera.cam");
	dkernel_trace("Inferno: Cleaning old camera files");
	
	dkernel_loadScripts();
	dkernel_trace("Inferno: Scripts Loaded\n");
	
	if (demoSystem.slaveMode) {
		dkernel_trace("Inferno: Running in network slave mode\n");
	} else {
		dkernel_trace("Inferno: Running in standalone mode\n");
	}
		
	dkernel_configDialog();
	dkernel_trace("Inferno: Config Dialog executed\n");
	
	dkernel_initDemo();
	dkernel_trace("Inferno: Engine inited successfully\n");

	dkernel_mainLoop();
	dkernel_trace("Inferno: Main loop ended successfully\n");

	dkernel_closeDemo();
	dkernel_trace("Inferno: Engine closed\n");

	return EXIT_SUCCESS;
	}
