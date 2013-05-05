#include <stdlib.h>

// The following libraries all have to be included to get all of SDL
// plus the net2 and fastevents libraries. I include my SDLUtils
// library because I have used my initialization and quit functions to
// simplify the code.

#include "SDL.h"
#include "fastevents.h"
#include "net2.h"
#include "SDLUtils.h"

int main(int argc, char **argv)
{
  SDL_Event ev;                              // an SDL event
  char buf[1024];                            // a message buffer
  int len = 0;                               // some counters
  int count = 0;
  int socks = 0;                             // the number of
                                             // connected sockets,
                                             // which is also the
                                             // number of connected
                                             // clients.

  mySDLInitOrQuit(SDL_INIT_EVENTTHREAD |     // initialize SDL. We need
                  SDL_INIT_VIDEO |           // events and it doesn't
                  SDL_INIT_NOPARACHUTE);     // work without video

// This is where we tell NET2 to start accepting TCP/IP connections on
// port 6666. This call does not wait for a connection to come in. It
// tells NET2 to do the waiting and it returns immediately. After this
// call returns your program will get an event any time a computer
// trys to connect to the port given in this call.

  NET2_TCPAcceptOn(6666);

  while (FE_WaitEvent(&ev))                  // wait for events
  {
    //printSDLEvent(&ev);

    switch (ev.type)
    {

// all NE2 events are SDL_USEREVENTs but not all user events are NET2
// events. If you use user events in your code you need to be careful
// to make sure you are not using the same event codes that NET2 uses.

    case SDL_USEREVENT:
      switch(NET2_GetEventType(&ev))
      {

// This next piece of code handles an accept event. This event tells
// us that an connection has been accepted. Here, all we do is count
// it. You would normally take some other action.

      case NET2_TCPACCEPTEVENT:
        printf("accept(%d)\n", NET2_GetSocket(&ev));
        printNET2Event(&ev);
        socks++;
        break;

        // This next piece of code is for handling receive
        // events. This kind of event tells us we have input waiting
        // on a socket. You need to grab all of it. No that we get the
        // socket from the event. You can use the socket to tell you
        // which user sent the information to you.

      case NET2_TCPRECEIVEEVENT:
        while (0 != (len = NET2_TCPRead(NET2_GetSocket(&ev), buf, sizeof(buf))))
        {
          count += len;
        }
        break;

        // If an error occurs on a socket or the other computer closes
        // the connection you will get a close event. When you get a
        // close event you must close the socket. Use the socket in
        // the event to tell you which connection went away.

      case NET2_TCPCLOSEEVENT:
        printf("close(%d)\n", NET2_GetSocket(&ev));
        printNET2Event(&ev);
        NET2_TCPClose(NET2_GetSocket(&ev));  // close the socket

        printf("count=%d\n", count); fflush(NULL);
        socks--;
        if (0 >= socks)
        {
          //exit(0);
        }
        //count = 0;
        break;

        // Sometimes you will get errors. It is best to keep track of
        // them and try to figure out what is causing them.

      case NET2_ERROREVENT:
        printf("Error: %s(%d)\n", NET2_GetEventError(&ev), NET2_GetSocket(&ev));
        printNET2Event(&ev);
        break;
      }
      break;

    case SDL_QUIT:                           // time to quit
      mySDL_Quit();                          // clean up and exit
      exit(0);
      break;
    }
  }

  mySDL_Quit();
}
