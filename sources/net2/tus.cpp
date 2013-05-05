#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "net2.h"
#include "fastevents.h"
#include "SDLUtils.h"
#include "trace.h"


//----------------------------------------
// 
// 
// 

int main(int argc, char **argv)
{
  SDL_Event ev;
  UDPpacket *p = NULL;
  char buf[1024];
  int len = 0;

  mySDLInitOrQuit(SDL_INIT_EVENTTHREAD | SDL_INIT_VIDEO);

  NET2_UDPAcceptOn(5555, 128);
  NET2_TCPAcceptOn(6666);

  while (FE_WaitEvent(&ev))
  {
    //printSDLEvent(&ev);

    switch (ev.type)
    {
    case SDL_USEREVENT:
      switch(NET2_GetEventType(&ev))
      {
      case NET2_TCPACCEPTEVENT:
        printf("accept(%d)\n", NET2_GetSocket(&ev));
        //printNET2Event(&ev);
        break;

      case NET2_TCPRECEIVEEVENT:
        printf("tcp recieve(%d)\n", NET2_GetSocket(&ev));
        //printNET2Event(&ev);
        while (0 != (len = NET2_TCPRead(NET2_GetSocket(&ev), buf, sizeof(buf))))
        {
        }
        break;

      case NET2_TCPCLOSEEVENT:
        printf("close(%d)\n", NET2_GetSocket(&ev));
        //printNET2Event(&ev);
        NET2_TCPClose(NET2_GetSocket(&ev));
        break;

      case NET2_UDPRECEIVEEVENT:
        printf("udp recieve\n");
        //printNET2Event(&ev);
        p = NET2_UDPRead(NET2_GetSocket(&ev));
        while (NULL != p)
        {
          //printf("%s\n", p->data);
          NET2_UDPFreePacket(p);
          p = NET2_UDPRead(NET2_GetSocket(&ev));
        }
        break;

      case NET2_ERROREVENT:
        printf("Error: %s(%d)\n", NET2_GetEventError(&ev), NET2_GetSocket(&ev));
        printNET2Event(&ev);
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
