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
  IPaddress ip;                              // where to send messages
  char buf[100];                             // will store messages
  int i;
  int limit = 1000;
  int count = 0;

  mySDLInitOrQuit(SDL_INIT_EVENTTHREAD |     // initialize SDL. we need
                  SDL_INIT_VIDEO |           // events and it doesn't work
                  SDL_INIT_NOPARACHUTE);     // without video.

// The call to NET2_ResolveHost fills in the IPaddress with the IP
// address and port we are going to send messages to. You can put in
// any symbolic name and it will be resolved or you will get an
// error. I don't check for an error here because this is a sample,
// not production code, and if this fails, you have worse problems...

  NET2_ResolveHost(&ip, "localhost", 5555);  // talk to my self on 5555

  for (i = 0; i < limit; i++)                // send a BUNCH of messages
  {
    memset(buf, 0, sizeof(buf));             // clear the message buffer
    sprintf(buf, "UDP message #%08d", i);    // create a message

// The next line sends a UDP/IP message to the port and address stored
// in ip.

    NET2_UDPSend(&ip, buf, sizeof(buf));     // send the message
    count += sizeof(buf);
    //printf("%d\n", i);
    //SDL_Delay(1);                          // play with this to see
                                             // how it effects the
                                             // number of messages
                                             // that get through
  }

  printf("count=%d\n", count);               // print how many were sent

  mySDL_Quit();                              // clean up and quit
	return 0;
}
