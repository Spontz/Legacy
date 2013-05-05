/*
	netdriver.c: SDL Network init and setup
*/

#include "../platform.h"
#include "netdriver.h"
#include "../dkernel.h"

static network_connection *local;
// static char *additionalData;

void network_init() {
/***************************************************************
* init the SDL network library and open the port for listening *
****************************************************************/
	
	local = malloc(sizeof(network_connection));

	local->port = 28000;

	// NET2 must be inited before calling its functions
	mySDLInitOrQuit(SDL_INIT_EVENTTHREAD | SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

	dkernel_trace("Net2: SDLNet has been inited successfully");

	NET2_TCPAcceptOn(local->port);
}

char * getParamString(char *message, int requestedParameter) {
/*********************************************************************************************
* Returns the requested parameter from the passed message (first parameter is 1) as a string *
**********************************************************************************************/

	char *theParameter;
	int counter = 1;
	
	// isaac: strdup already allocates memory
	// theParameter = calloc(strlen(message) + 1, sizeof(char));
	
	theParameter = SPZ_STL(strdup)(message);
	
	for (theParameter = strtok(theParameter,"::");
		counter < requestedParameter;
		theParameter = strtok(NULL, "::"))
			counter++;
	
	return theParameter;
}

float getParamFloat(char *message, int requestedParameter) {
/************************************************************************************************************
* Returns the requested parameter from the passed message (first parameter is 1) as a floating point number *
*************************************************************************************************************/
	float theFloatResult;
	char *theStringResult;
	
	theStringResult = getParamString(message, requestedParameter);
	
	// Search for the parameter and transform it in a float
	sscanf(theStringResult, "%f", &theFloatResult);

	// Return the result
	return theFloatResult;
}

char * process_message(char *message) {
/*****************************
* Process the passed message *
******************************/
	// Incoming information
	char *identifier, *type, *action;

	// Outcoming information
	static char theResponse[4096];//char *theResponse[4096];
	char *theResult, *theInformation;

	identifier = getParamString(message, 1);
	type       = getParamString(message, 2);
	action     = getParamString(message, 3);

	theResult = "OK";
	theInformation = calloc(1024, sizeof(char));

	// ***********************
	// * COMMANDS PROCESSING *
	// ***********************
	if (strcmp(type, "command") == 0)
		{
		if      (strcmp(action, "pause"		  ) == 0) { dkernel_pause          ();							theResult = "OK"; }
		else if (strcmp(action, "play"        ) == 0) { dkernel_play           ();							theResult = "OK"; }
		else if (strcmp(action, "restart"     ) == 0) { dkernel_restart        ();							theResult = "OK"; }
		else if (strcmp(action, "startTime"   ) == 0) { dkernel_setStartTime   (getParamFloat(message, 4));	theResult = "OK"; }
		else if (strcmp(action, "currentTime" ) == 0) { dkernel_setCurrentTime (getParamFloat(message, 4));	theResult = "OK"; }
		else if (strcmp(action, "endTime"     ) == 0) { dkernel_setEndTime     (getParamFloat(message, 4));	theResult = "OK"; }
		else if (strcmp(action, "ping"        ) == 0) {                                                   ;	theResult = "OK"; }
		else
			{
			theResult = "NOK";
			sprintf((char *)theInformation, "Unknown command (%s)", message);
			}

		// ***********************
		// * SECTIONS PROCESSING *
		// ***********************
		}
	else if (strcmp(type, "section") == 0)
		{
		if (strcmp(action, "new") == 0)
			{
			int res = dkernel_createSection(getParamString(message, 4), "Network");
			if (res)
				{
				theResult = "OK";
				}
			else
				{
				theResult = "NOK";
				sprintf((char *)theInformation, "Section load failed");
				}
			}
		else if (strcmp(action, "toggle"        ) == 0) { dkernel_toggleSection         (getParamString(message, 4)							   ); theResult = "OK"; }
		else if (strcmp(action, "delete"        ) == 0) { dkernel_deleteSection         (getParamString(message, 4)                            ); theResult = "OK"; }
		else if (strcmp(action, "update"        ) == 0) { dkernel_updateSection         (getParamString(message, 4), getParamString(message, 5)); theResult = "OK"; }
		else if (strcmp(action, "setStartTime"  ) == 0) { dkernel_setSectionsStartTime  (getParamString(message, 4), getParamString(message, 5)); theResult = "OK"; }
		else if (strcmp(action, "setEndTime"    ) == 0) { dkernel_setSectionsEndTime    (getParamString(message, 4), getParamString(message, 5)); theResult = "OK"; }
		else if (strcmp(action, "setLayer"      ) == 0) { dkernel_setSectionsLayer      (getParamString(message, 4), getParamString(message, 5)); theResult = "OK"; }		
		else {
			theResult = "NOK";
			sprintf((char *)theInformation, "Unknown section (%s)", message);
		}
	}
	
	// Check for non-processed messages (If the result has not been set, the message has not been processed)
	if (strcmp(theResult, "") == 0) {
		theResult = "NOK";
		sprintf((char *)theInformation, "Unknown message (%s)", message);
	}
	
	// Create the response
	sprintf((char *)theResponse,"%s::%s::%f::%d::%f::%s", identifier, theResult, demoSystem.fps, demoSystem.state, demoSystem.runTime, (char *)theInformation);

	// Free memory
	free(theInformation);
	
	// and return the response (will be freed later)
	return theResponse;//return (char *)theResponse;
}

void network_close () {
/*******************************
* close the network connection *
********************************/
	
	dkernel_trace("Network closed.\n");	
	
	// clean up SDL Networking
    mySDL_Quit();
}