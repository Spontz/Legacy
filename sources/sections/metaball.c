#include "../interface/demo.h"

typedef struct {
	int		nblobs;
	float	*blob_px;
	float	*blob_py;
	float	*blob_vx;
	float	*blob_vy;
} metaball_section;

static metaball_section *local;

void preload_metaball () {
}

// ******************************************************************

void load_metaball () {
	int i;

	local = malloc(sizeof(metaball_section));
	mySection->vars = (void *) local;

	local->nblobs	=	(int)mySection->param[0];

	local->blob_px = malloc(local->nblobs*sizeof(float));
	local->blob_py = malloc(local->nblobs*sizeof(float));
	local->blob_vx = malloc(local->nblobs*sizeof(float));
	local->blob_vy = malloc(local->nblobs*sizeof(float));

	for (i=0; i<local->nblobs; ++i) {
		local->blob_px[i] = (float)(rand()%640);
		local->blob_py[i] = (float)(rand()%400);
		local->blob_vx[i] = (float)(rand()%200 + 100);
		local->blob_vy[i] = (float)(rand()%200 + 100);
	}
	
	mySection->loaded=1;
}

// ******************************************************************

void init_metaball () {

}

// ******************************************************************

void render_metaball () {	
	int i;
	int y;

	float	sc[400][640][3];

	local = (metaball_section *) mySection->vars;
	
	for (i=0; i<local->nblobs; ++i) {
		local->blob_px[i] += local->blob_vx[i]*demoSystem.frameTime;
		local->blob_py[i] += local->blob_vy[i]*demoSystem.frameTime;
		
		if(local->blob_px[i] < 0 || local->blob_px[i] > 640) local->blob_vx[i] *= -1;
		if(local->blob_py[i] < 0 || local->blob_py[i] > 400) local->blob_vy[i] *= -1;
	}
		
    for (y=0; y<400; ++y) {
		int x;
		for (x=0; x<640; ++x) {
			float m = 0.0f;
			int i;
			for (i=0; i<local->nblobs; ++i) {
				m += 1000000.0f/( (local->blob_py[i]-y)*(local->blob_py[i]-y) + (local->blob_px[i]-x)*(local->blob_px[i]-x) + 1 );
			}
			
			sc[y][x][0] = 1; // m/255.0f;
			sc[y][x][1] = 0; // m/255.0f;
			sc[y][x][2] = 1; //m/255.0f;
		}
	}
	
	camera_set2d();

	glPixelZoom(glDriver.width/640.0f, glDriver.height/400.0f);
	glPixelZoom(0.5, 0.5);
	glDrawPixels(640, 400, GL_RGB, GL_FLOAT, sc);
	
	camera_restore();
}

// ******************************************************************

void end_metaball () {
}
