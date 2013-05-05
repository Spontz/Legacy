#ifndef __HIGHPASSFILTER_H
#define __HIGHPASSFILTER_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

void render_highpassfilter (int tex, float threshold_R, float threshold_G, float threshold_B, int accum);
void render_highpassfilter_ext (int tex, float threshold_R, float threshold_G, float threshold_B, int accum);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
