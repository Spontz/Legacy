#include <string.h>
#include <stdlib.h>

// The following libraries all have to be included to get all of SDL
// plus the net2 and fastevents libraries. I include my SDLUtils
// library because I have used my initialization and quit functions to
// simplify the code.

#include "SDL.h"
#include "net2.h"
#include "SDLUtils.h"

int main(int argc, char **argv)
{
  char buf[100];                             // storage for a message
  int i;                                     // a counter
  int s = -1;                                // the socket, just an int
  int count = 0;                             // how many messages to send
  int limit = 1000;

  mySDLInitOrQuit(SDL_INIT_EVENTTHREAD |     // initialize SDL. We need
                  SDL_INIT_VIDEO |           // events and it doesn't
                  SDL_INIT_NOPARACHUTE);     // work without video.

// Here we ask for a connection to the computer. In this case we are
// just trying to connect to the computer we are on at port 6666. We
// check to make sure we have a valid connection before going on with
// the rest of the program. Note that the socket id winds up in s.

  if (-1 == (s = NET2_TCPConnectTo("localhost", 6666)))
  {
    printf("ConnectTo failed\n");
    exit(0);
  }

// All we do here is create a message and send it. The message is just
// the words "TCP message" followed by a message number.

  for (i = 0; i < limit; i++)
  {
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "TCP message #%08d\n", i);  // create the message to send
    NET2_TCPSend(s, buf, sizeof(buf));       // send it and count it.
    count += sizeof(buf);
    //printf("%d\n", i);
    //SDL_Delay(1);
  }

  printf("count=%d\n", count);               // tell how many were sent

  mySDL_Quit();                              // clean up.
}
