#ifndef __RANDOM_H
#define __RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

unsigned int myRand( void ); /* returns a random 32-bit integer */
void rand_seed( unsigned int, unsigned int ); /* seed the generator */

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
