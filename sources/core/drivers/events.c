/*
	events.c: SDL events management
*/

#include "../../main.h"
#include "events.h"
#include "netdriver.h"

// ******************************************************************

int eventHandler(SDL_Event event) {
  char buf[4096];                           // a message buffer
  char *theResponse;
  int len = 0;                               // some counters
  int count = 0;
  int socks = 0;                             // the number of
                                             // connected sockets,
                                             // which is also the
                                             // number of connected
                                             // clients.

	switch (event.type) {

		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
			break;

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == KEY_EXIT) {
				demoSystem.exitDemo = 1;
				break;
			}
			
			if (event.key.keysym.sym == KEY_SCREENSHOT) {
				gldrv_screenshot();
				break;
			}

			if (demoSystem.debug) {

				if (event.key.keysym.sym == KEY_TIME) {
					fprintf(stdout, "%f\n", demoSystem.runTime);

				} else if (event.key.keysym.sym == KEY_PLAY_PAUSE) {
					if (demoSystem.state == DEMO_PLAY) {
						dkernel_pause();
					}
					else {
						dkernel_play();
					}

				} else if (event.key.keysym.sym == KEY_REWIND) {
					dkernel_rewind();

				} else if (event.key.keysym.sym == KEY_FASTFORWARD) {
					dkernel_fastforward();

				} else if (event.key.keysym.sym == KEY_STOP) {
					demoSystem.exitDemo = 1;

				} else if (event.key.keysym.sym == KEY_RESTART) {
					dkernel_restart();
				}
			}

			demoSystem.keys[event.key.keysym.sym] = TRUE; 
			
			break;

		case SDL_KEYUP:
			if ((event.key.keysym.sym == KEY_REWIND) ||
				(event.key.keysym.sym == KEY_FASTFORWARD)) {

				if (demoSystem.state & DEMO_PAUSE) dkernel_pause();
				else dkernel_play();
			}

			demoSystem.keys[event.key.keysym.sym] = FALSE;
			break;

		case SDL_QUIT:
			demoSystem.exitDemo = 1;
			break;


		case SDL_USEREVENT:
			switch(NET2_GetEventType(&event)) {

				case NET2_TCPACCEPTEVENT:
					// This next piece of code handles an accept event. This event tells
					// us that an connection has been accepted. Here, all we do is count
					// it. You would normally take some other action.
					//dkernel_trace("Net2: Accept(%d)\n", NET2_GetSocket(&event));
					//printNET2Event(&event);
					socks++;
					break;

				case NET2_TCPRECEIVEEVENT:
					// This next piece of code is for handling receive
					// events. This kind of event tells us we have input waiting
					// on a socket. You need to grab all of it. No that we get the
					// socket from the event. You can use the socket to tell you
					// which user sent the information to you.
					while (0 != (len = NET2_TCPRead(NET2_GetSocket(&event), buf, sizeof(buf)))) {
						count += len;
					}
					theResponse = process_message(buf);

					if (NET2_TCPSend(NET2_GetSocket(&event), theResponse, strlen(theResponse)) == -1)
						dkernel_trace("Net2: NET2_TCPSend: Error: %s", NET2_GetError());

					NET2_TCPClose(NET2_GetSocket(&event));
					socks--;
					count = 0;

					break;

				case NET2_TCPCLOSEEVENT:
					// If an error occurs on a socket or the other computer closes
					// the connection you will get a close event. When you get a
					// close event you must close the socket. Use the socket in
					// the event to tell you which connection went away.
					
					dkernel_trace("Net2: Close(%d)\n", NET2_GetSocket(&event));
					
					//printNET2Event(&event);
					NET2_TCPClose(NET2_GetSocket(&event));  // close the socket

					//dkernel_trace("Net2: count=%d\n", count);
					fflush(NULL);

					socks--;

					if (socks < 0)
					{
						dkernel_trace("events.c : negative number of socks");
						socks = 0;
					}

					count = 0;

					break;

					// Sometimes you will get errors. It is best to keep track of
					// them and try to figure out what is causing them.

				case NET2_ERROREVENT:
					dkernel_trace("Net2: Error: %s(%d)\n", NET2_GetEventError(&event), NET2_GetSocket(&event));
					printNET2Event(&event);
					break;

				default:
					dkernel_trace("Net2: Unknown Network Event");
					break;
			}

			break;

		default:
			break;
	}

	return 0;
}
