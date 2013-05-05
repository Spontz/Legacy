#ifndef __EVENTS_H
#define __EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

// definition of engine control keys
#define KEY_EXIT			SDLK_ESCAPE
#define KEY_SCREENSHOT		SDLK_s
#define KEY_TIME			SDLK_RETURN

#define KEY_PLAY_PAUSE		SDLK_SPACE
#define KEY_REWIND			SDLK_F2
#define KEY_FASTFORWARD		SDLK_F3
#define KEY_STOP			SDLK_ESCAPE
#define KEY_RESTART			SDLK_F1

// definition of camera control keys
#define KEY_CAPTURE			SDLK_c
#define KEY_FORWARD			SDLK_h
#define KEY_BACKWARD		SDLK_n
#define KEY_STRAFELEFT		SDLK_b
#define KEY_STRAFERIGHT		SDLK_m
#define KEY_CAMRESET		SDLK_r
#define KEY_FOVUP			SDLK_u
#define KEY_FOVDOWN			SDLK_j
#define KEY_ROLLUP			SDLK_i
#define KEY_ROLLDOWN		SDLK_k

// definition of spline control keys
#define KEY_NUM_1			SDLK_1
#define KEY_NUM_2			SDLK_2
#define KEY_NUM_3			SDLK_3
#define KEY_NUM_4			SDLK_4
#define KEY_NUM_5			SDLK_5
#define KEY_NUM_6			SDLK_6
#define KEY_NUM_7			SDLK_7
#define KEY_NUM_8			SDLK_8
#define KEY_NUM_9			SDLK_9
#define KEY_NUM_0			SDLK_0

/*#define KEY_MORE			SDLK_q
#define KEY_LESS			SDLK_a
#define KEY_RESET			SDLK_r
#define KEY_DUMP			SDLK_d
#define KEY_MODIFICATOR		SDLK_LALT*/

// ******************************************************************

int eventHandler(SDL_Event event);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
