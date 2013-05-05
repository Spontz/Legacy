#ifndef __SPLINE_H
#define __SPLINE_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

/*
 * LightWave: A motion tracks has 9 channels (x, y, z position, h, p, b rotation angle,
 * and x, y, z scale), although this number can vary.  Envelopes work the
 * same way but track only one channel.
 */

#define NUM_CHAN 32
typedef float ChanVec[NUM_CHAN];

/*
 * Each KeyFrame in a motion has the value of the channels at that key,
 * tension, continuity and bias spline controls, a linear flag and the
 * frame at which the key is located (step).
 */
typedef struct st_KeyFrame {
	ChanVec cv;
	float   tens, cont, bias;
	int     linear;
	float   step;
} KeyFrame;

/*
 * A motion is just an array of `keys' KeyFrames and the total number of
 * steps in the motion.
 */
typedef struct st_Motion {
	KeyFrame *keylist;
	int      keys;
	float    steps;
	int      channels;
} Motion;

// ******************************************************************

void MotionCalcStep (Motion *mot, ChanVec resVec, float step);
Motion *motion_load(char *name);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
