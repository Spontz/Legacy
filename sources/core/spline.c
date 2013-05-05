/*
	splice.c: load spline files
*/

#include "../main.h"

// ******************************************************************

/*
 * Compute Hermite spline coeficients for t, where 0 <= t <= 1.
 */
static void Hermite (float t, float *h1, float *h2, float *h3, float *h4)
{
	float t2, t3, z;

	// h1(s) =  2s^3 - 3s^2 + 1
	// h2(s) = -2s^3 + 3s^2
	// h3(s) =   s^3 - 2s^2 + s
	// h4(s) =   s^3 -  s^2

	t2 = t * t;
	t3 = t * t2;
	z = 3.0f * t2 - t3 - t3;

	*h1 = 1.0f - z;
	*h2 = z;
	*h3 = t3 - t2 - t2 + t;
	*h4 = t3 - t2;
}

// ******************************************************************

/*
 * Compute the motion channel vector for the given step.  Step can be
 * fractional but values correspond to frames.
 */
void MotionCalcStep (Motion *mot, ChanVec resVec, float step)
{
	KeyFrame *key0, *key1;
	float t, h1, h2, h3, h4, res, d10;
	float dd0a = 0, dd0b = 0, ds1a = 0, ds1b = 0;
	float adj0 = 0, adj1 = 0, dd0 = 0, ds1 = 0;
	float tlength;
	int i;

	/*
	 * If there is but one key, the values are constant.
	 */
	if (mot->keys == 1) {
		for (i = 0; i < mot->channels; i++)
			resVec[i] = mot->keylist[0].cv[i];
		return;
	}

	/*
	 * Get keyframe pair to evaluate. This should be within the range
	 * of the motion or this will raise an illegal access (fixed).
	 */
	key0 = mot->keylist;

	i = 1;
	while (step > key0[1].step) {

		if (i >= mot->keys) {
			step = key0[1].step;
			break;
		}

		key0++;
		i++;
	}

	key1 = key0 + 1;
	step -= key0->step;

	/*
	 * Get tween length and fractional tween position.
	 */
	tlength = key1->step - key0->step;
	t = step / tlength;

	/*
	 * Precompute spline coefficients.
	 */
	if (!key1->linear)
		{
		Hermite (t, &h1, &h2, &h3, &h4);
		dd0a = (1.0f - key0->tens) * (1.0f + key0->cont)
			 * (1.0f + key0->bias);
		dd0b = (1.0f - key0->tens) * (1.0f - key0->cont)
			 * (1.0f - key0->bias);
		ds1a = (1.0f - key1->tens) * (1.0f - key1->cont)
			 * (1.0f + key1->bias);
		ds1b = (1.0f - key1->tens) * (1.0f + key1->cont)
			 * (1.0f - key1->bias);

		if (fabs(key0->step) > FLT_EPSILON)
			adj0 = tlength / (key1->step - key0[-1].step);

		if (fabs(key1->step - mot->steps) < FLT_EPSILON)
			adj1 = tlength / (key1[1].step - key0->step);
		}

	/*
	 * Compute the channel components.
	 */
	for (i = 0; i < mot->channels; i++)
		{
		d10 = key1->cv[i] - key0->cv[i];

		if (!key1->linear)
			{
			if (fabs(key0->step) < FLT_EPSILON)
				dd0 = 0.5f * (dd0a + dd0b) * d10;
			else
				dd0 = adj0 * (dd0a * (key0->cv[i] - key0[-1].cv[i])	+ dd0b * d10);

			if (fabs(key1->step - mot->steps) < FLT_EPSILON)
				ds1 = 0.5f * (ds1a + ds1b) * d10;
			else
				ds1 = adj1 * (ds1a * d10 + ds1b * (key1[1].cv[i] - key1->cv[i]));

			res = key0->cv[i] * h1 + key1->cv[i] * h2 + dd0 * h3 + ds1 * h4;
			}
		else
			res = key0->cv[i] + t * d10;

		resVec[i] = res;
	}
}

// ******************************************************************

Motion *motion_load(char *name) {

	char line[512];
	KeyFrame *key, *tmp;
	Motion *motion;
	FILE *f;
	int chan;

	f = fopen(name,"rt");
	if (!f) {
		dkernel_error("Error loading '%s'",name);
		return NULL;
	}
	
	motion = malloc(sizeof(Motion));
	motion->keylist = NULL;
	motion->keys = 0;
	motion->steps = 0;
	motion->channels = 0;

	for (;;) {

		if (fgets(line,512,f) == NULL) break;

		// comments or empty line
		if ((line[0] == ';') || (line[0] == '\n') || (line[0] == '\r')) continue;

		tmp = motion->keylist;
		motion->keylist = malloc((motion->keys + 1) * sizeof(KeyFrame));
		memcpy(motion->keylist,tmp,motion->keys * sizeof(KeyFrame));
		free(tmp);

		key = &motion->keylist[motion->keys];
		chan = getFloatVector(line, key->cv, NUM_CHAN);

		if (motion->channels == 0) {
			if (chan == 0) dkernel_error("Spline: incorrect format in '%s'", name);
			motion->channels = chan;
		} else {
			if (motion->channels != chan) dkernel_error("Spline: incorrect channel in '%s'", name);
		}

		key->tens = 0;
		key->cont = 0;
		key->bias = 0;
		key->linear = 0;

		motion->keys++;
	}

	fclose(f);

	return motion;
}
