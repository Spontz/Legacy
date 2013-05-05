/*
 *  windowmng.c : Window Management
 *  Inferno
 *
 *
 */

#include "windowmng.h"

#ifdef WIN32
	#include <windows.h>
#endif

#include "../core/dkernel.h"

#if defined( __APPLE__ ) && defined( __MACH__ )
	#include <Carbon/Carbon.h>
	#include <AGL/agl.h>
#endif



void SpzMessageBox( const char* title, const char* message )
	{
	static char *lastMessage;
		
	if (lastMessage != NULL)
		if (strcmp(lastMessage, message) == 0)
			return; // The message has been already displayed, so don't show it again (return the function)
		
	#if defined( __APPLE__ ) && defined( __MACH__ )
		CFStringRef titleStr = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
		CFStringRef messageStr = CFStringCreateWithCString(NULL, message, kCFStringEncodingUTF8);
		CFUserNotificationDisplayNotice (0, kCFUserNotificationCautionAlertLevel, NULL, NULL, NULL, titleStr, messageStr, CFSTR("OK"));
		CFRelease(titleStr);
		CFRelease(messageStr);
	#elif WIN32
		MessageBoxA( 0, message, title, 0 ); // Messagebox with an OK button
	#else
		#error Unsupported platform
	#endif

	// Save the message
	free(lastMessage);
	lastMessage = calloc(strlen(message) + 1, sizeof(char));
	lastMessage = SDL_strdup(message);
	}
