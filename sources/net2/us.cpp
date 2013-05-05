#include <stdlib.h>

// The following libraries all have to be included to get all of SDL
// plus the net2 and fastevents libraries. I include my SDLUtils
// library because I have used my initialization and quit functions to
// simplify the code.

#include "SDL.h"
#include "net2.h"
#include "fastevents.h"
#include "SDLUtils.h"


int main(int argc, char **argv)
{
  SDL_Event ev;                              // storage for an SDL event
  UDPpacket *p = NULL;                       // used to get messages
  int count = 0;

  mySDLInitOrQuit(SDL_INIT_EVENTTHREAD |     // initialize SDL. We need
                  SDL_INIT_VIDEO |           // events and it doesn't work
                  SDL_INIT_NOPARACHUTE);     // without video.

// The following tells NET2 to start accepting messages on port
// 5555. It also tells NET2 that messages are no larger than 128 bytes
// long. That is all you have to do to start accepting UDP messages.

  NET2_UDPAcceptOn(5555, 128);

//   This is the normal SDL event loop. But, since we are using NET2
//   we have to also use the fastevents library. Everything else
//   should look normal. We just get an event and process it.

  while (FE_WaitEvent(&ev))
  {
    //printSDLEvent(&ev);

    switch (ev.type)
    {

    case SDL_QUIT:                           // this event tells us to stop
      printf("count = %d\n", count);         // how many messages were received
      mySDL_Quit();                          // use my utility to shut down
      exit(0);                               // good bye
      break;

// This part of the code handles user events. All NET2 events are user
// events. Not all user events are NET2 events.

    case SDL_USEREVENT:
      switch(NET2_GetEventType(&ev))         // get the event type
      {
      case NET2_UDPRECEIVEEVENT:

// This event tells us that there is at least 1 message waiting to be
// processed. There could be more than one. Notice that when we do a
// read we use the socket from the event.

        printf("udp recieve\n");
        //printNET2Event(&ev);
        p = NET2_UDPRead(NET2_GetSocket(&ev));      // get a message
        while (NULL != p)                           // if we get NULL we are done
        {
          count += p->len;
          printf("%08x(%d)%s\n",p->address.host, p->address.port, p->data);
          NET2_UDPFreePacket(p);                    // free the packet
          p = NET2_UDPRead(NET2_GetSocket(&ev));    // get the next message
        }
        break;

// For this example I just print any errors I get. Normally you would
// want to check to see if you received a valid socket and take action
// to clean things up. But, for this demo I just print the error and
// continue as it nothing has happened.

      case NET2_ERROREVENT:
        printf("Error: %s(%d)\n", NET2_GetEventError(&ev), NET2_GetSocket(&ev));
        //printNET2Event(&ev);
        break;
      }
      break;
    }
  }

  mySDL_Quit();   // Can we ever get here? if we do clean up and quit,
                  // we are done.
}
