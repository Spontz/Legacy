#ifndef __MAIN_H
#define __MAIN_H

// ******************************************************************

#define TRUE  1
#define FALSE 0

#define USE_CACHE 1
#define NO_CACHE  0

// ******************************************************************

#ifdef WIN32
	#pragma warning( disable : 4996) // strdup #define strdup _strdup causes problems
	#define spz_strcmpi _strcmpi
#else
	#define spz_strcmpi strcasecmp
#endif

// #define _DEBUG 1

// ******************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <SDL.h>
#include <float.h>

// ******************************************************************

#include "windowmng/windowmng.h"

// ******************************************************************

#include "core/disk.h"
#include "core/drivers/gldriver.h"
#include "core/drivers/fmoddriver.h"
#include "core/drivers/netdriver.h"
#include "core/drivers/exprevaldriver.h"
#include "core/dkernel.h"

// ******************************************************************

#include "core/math3d.h"
#include "core/skybox.h"
#include "core/camera.h"
#include "core/positioning.h"
#include "core/particle.h"
#include "core/rays.h"
#include "core/texture.h"
#include "core/fbo.h"
#include "core/glslshader.h"
#include "core/layers.h"
#include "core/pickcam.h"
#include "core/3dsdraw.h"
#include "core/3dsdrawmodes.h"
#include "core/mouse.h"
#include "core/axis.h"
#include "core/rect3d.h"
#include "core/rect2d.h"
#include "core/spline.h"
#include "core/random.h"
#include "core/boxblur.h"
#include "core/highpassfilter.h"
#include "core/cubemap.h"
#include "core/parse.h"
#include "core/viewport.h"

// ******************************************************************

#endif
