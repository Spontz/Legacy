#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "net2.h"
#include "fastevents.h"
#include "SDLUtils.h"
#include "trace.h"

int main(int argc, char **argv)
{
  SDL_Event ev;
  char buf[1024];
  int len = 0;
  int count = 0;

  mySDLInitOrQuit(SDL_INIT_EVENTTHREAD | 
                  SDL_INIT_VIDEO | 
                  SDL_INIT_NOPARACHUTE);

  if (-1 == NET2_TCPConnectTo("localhost", 6666))
  {
    printf("ConnectTo failed\n");
    exit(0);
  }

  while (FE_WaitEvent(&ev))
  {
    switch (ev.type)
    {
    case SDL_USEREVENT:
      switch(NET2_GetEventType(&ev))
      {
      case NET2_TCPACCEPTEVENT:
        printf("accept\n");
        break;

      case NET2_TCPRECEIVEEVENT:
        while (0 != (len = NET2_TCPRead(NET2_GetSocket(&ev), buf, sizeof(buf))))
        {
          int i;

          printf("len=%d\n", len);
          count += len;

          for (i = 0; i < len; i++)
          {
            putchar(buf[i]);
          }
          putchar('\n');


          if (-1 == NET2_TCPSend(NET2_GetSocket(&ev), buf, len))
          {
            printf("RECIEVE SEND FAILED\n");
            NET2_TCPClose(NET2_GetSocket(&ev));
            break;
          }
        }

        if (count > 10000)
        {
          exit(0);
        }

        break;

      case NET2_TCPCLOSEEVENT:
        printf("close\n");
        NET2_TCPClose(NET2_GetSocket(&ev));
        exit(0);
        break;

      case NET2_ERROREVENT:
        printf("Error: %s\n", NET2_GetEventError(&ev));
        exit(0);
        break;
      }
      break;
    case SDL_QUIT:
      mySDL_Quit();
      exit(0);
      break;
    }
  }

  mySDL_Quit();
}
