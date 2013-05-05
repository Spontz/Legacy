#include "../interface/demo.h"

// ******************************************************************

// section local variables
typedef struct {
	vector_t origin, end;
	float red, green, blue;
	int numSegments;
	float Radius;
	float Width;
	char mode;

	float reseedingFrequence;
	vector_t *random_v;
	int	numStrokes;
} ray_section;

static ray_section *local;

static void recalculateRaySeed();

// ******************************************************************

void preload_ray () {
}

// ******************************************************************

static void prepare_object (float step, int path) {

	ChanVec resVec;

	// calculate spline in the current step
	dkernel_getSpline(path, step, resVec);

	local->origin.x = resVec[0];
	local->origin.y = resVec[1];
	local->origin.z = resVec[2];
	
	local->end.x = resVec[3];
	local->end.y = resVec[4];
	local->end.z = resVec[5];

	local->Radius = resVec[6];
	local->Width = resVec[7];
	local->reseedingFrequence = resVec[8];
	local->numStrokes = (int) resVec[9];
}

// ******************************************************************

static void prepare_colors (float step, int path) {

	ChanVec resVec;

	// calculate spline in the current step
	dkernel_getSpline(path, step, resVec);

	local->red = resVec[0];
	local->green = resVec[1];
	local->blue = resVec[2];
}

// ******************************************************************

void load_ray () {

	// script validation
	if (mySection->splineNum != 2) {
		section_error("2 splines needed");
		return;
	}
	
	local = malloc(sizeof(ray_section));
	mySection->vars = (void *) local;

	local->mode = (int) mySection->param[0];
	local->numSegments = (int) mySection->param[1];
	local->random_v = (vector_t *) malloc((local->numSegments + 1) * sizeof(vector_t));
	
	recalculateRaySeed();
	mySection->loaded=1;
}

// ******************************************************************

void init_ray () {
}

// ******************************************************************

void recalculateRaySeed() {

	int i;

	for (i=0; i<local->numSegments; i++) {
		local->random_v[i].x = 0.001f * (float)(myRand()%1000) * local->Radius * (float)pow(-1.0f, (myRand()%2));
		local->random_v[i].y = 0.001f * (float)(myRand()%1000) * local->Radius * (float)pow(-1.0f, (myRand()%2));
		local->random_v[i].z = 0.001f * (float)(myRand()%1000) * local->Radius * (float)pow(-1.0f, (myRand()%2));
	}
}

// ******************************************************************

void render_ray () {

	vector_t lastPoint, nextPoint;
	vector_t new_random_v;
	float alpha;
	int i, j, k;

	local = (ray_section *) mySection->vars;
	
	prepare_object (mySection->runTime, 0);
	prepare_colors (mySection->runTime, 1);

	glDisable(GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE);	
	glEnable(GL_BLEND);
		
	for (k=0; k<local->numStrokes; k++) {

		if (0.001 * (float)(myRand()%1000) < local->reseedingFrequence) recalculateRaySeed();

		lastPoint = local->origin;

		for (i=0; i<local->numSegments; i++) {
			new_random_v.x = 0.001f * (float)(myRand()%100) * local->Radius * (float)pow(-1.0f, (myRand()%2));
			new_random_v.y = 0.001f * (float)(myRand()%100) * local->Radius * (float)pow(-1.0f, (myRand()%2));
			new_random_v.z = 0.001f * (float)(myRand()%100) * local->Radius * (float)pow(-1.0f, (myRand()%2));

			local->random_v[i].x = local->random_v[i].x + new_random_v.x;
			local->random_v[i].y = local->random_v[i].y + new_random_v.y;
			local->random_v[i].z = local->random_v[i].z + new_random_v.z;

			if (local->mode == 0) {
					nextPoint.x = local->random_v[i].x + lastPoint.x + (local->end.x - local->origin.x) / local->numSegments;
					nextPoint.y = local->random_v[i].y + lastPoint.y + (local->end.y - local->origin.y) / local->numSegments;
					nextPoint.z = local->random_v[i].z + lastPoint.z + (local->end.z - local->origin.z) / local->numSegments;
			} else {
				if (i != local->numSegments - 1) {
					nextPoint.x = local->random_v[i].x + local->origin.x + (i+1) * (local->end.x - local->origin.x) / local->numSegments;
					nextPoint.y = local->random_v[i].y + local->origin.y + (i+1) * (local->end.y - local->origin.y) / local->numSegments;
					nextPoint.z = local->random_v[i].z + local->origin.z + (i+1) * (local->end.z - local->origin.z) / local->numSegments;
				} else {
					nextPoint = local->end;
				}
			}

			for (j=0; j<local->Width; j = j + 1) {	
				alpha = (j + 1) / local->Width;
				
				glLineWidth (local->Width - j);	
				glColor4f (local->red, local->green, local->blue, alpha);
				glBegin (GL_LINES);
					glVertex3f (lastPoint.x, lastPoint.y, lastPoint.z);
					glVertex3f (nextPoint.x, nextPoint.y, nextPoint.z);
				glEnd ();
			}

			lastPoint = nextPoint;
		}
	}

	glLineWidth(1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

// ******************************************************************

void end_ray () {
}
