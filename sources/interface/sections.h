#ifndef __SECTIONS_H
#define __SECTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

void preload_loading();
void load_loading();
void init_loading();
void render_loading();
void end_loading();

// ******************************************************************

void preload_efxAccum();
void load_efxAccum();
void init_efxAccum();
void render_efxAccum();
void end_efxAccum();

void preload_efxBlackWhite();
void load_efxBlackWhite();
void init_efxBlackWhite();
void render_efxBlackWhite();
void end_efxBlackWhite();

void preload_efxBoxBlur();
void load_efxBoxBlur();
void init_efxBoxBlur();
void render_efxBoxBlur();
void end_efxBoxBlur();

void preload_efxFader();
void load_efxFader();
void init_efxFader();
void render_efxFader();
void end_efxFader();

void preload_efxFog();
void load_efxFog();
void init_efxFog();
void render_efxFog();
void end_efxFog();

void preload_efxGlow2();
void load_efxGlow2();
void init_efxGlow2();
void render_efxGlow2();
void end_efxGlow2();

void preload_efxHighPassFilter();
void load_efxHighPassFilter();
void init_efxHighPassFilter();
void render_efxHighPassFilter();
void end_efxHighPassFilter();

void preload_efxInverse();
void load_efxInverse();
void init_efxInverse();
void render_efxInverse();
void end_efxInverse();

void preload_efxMotionBlur();
void load_efxMotionBlur();
void init_efxMotionBlur();
void render_efxMotionBlur();
void end_efxMotionBlur();

void preload_efxMotionBlurAccum();
void load_efxMotionBlurAccum();
void init_efxMotionBlurAccum();
void render_efxMotionBlurAccum();
void end_efxMotionBlurAccum();

void preload_efxRadialBlur2();
void load_efxRadialBlur2();
void init_efxRadialBlur2();
void render_efxRadialBlur2();
void end_efxRadialBlur2();

void preload_efxRadialGlow();
void load_efxRadialGlow();
void init_efxRadialGlow();
void render_efxRadialGlow();
void end_efxRadialGlow();

void preload_efxRenderMesh2();
void load_efxRenderMesh2();
void init_efxRenderMesh2();
void render_efxRenderMesh2();
void end_efxRenderMesh2();

void preload_section_render_model();
void load_section_render_model();
void init_section_render_model();
void render_section_render_model();
void end_section_render_model();

// ******************************************************************

void preload_background();
void load_background();
void init_background();
void render_background();
void end_background();

/*
void preload_bsp();
void load_bsp();
void init_bsp();
void render_bsp();
void end_bsp();
*/

void preload_camera();
void load_camera();
void init_camera();
void render_camera();
void end_camera();

void preload_camera2();
void load_camera2();
void init_camera2();
void render_camera2();
void end_camera2();

void preload_camera3();
void load_camera3();
void init_camera3();
void render_camera3();
void end_camera3();

void preload_image();
void load_image();
void init_image();
void render_image();
void end_image();

void preload_image2();
void load_image2();
void init_image2();
void render_image2();
void end_image2();

void preload_image2D();
void load_image2D();
void init_image2D();
void render_image2D();
void end_image2D();

void preload_imageMatrix();
void load_imageMatrix();
void init_imageMatrix();
void render_imageMatrix();
void end_imageMatrix();

void preload_imagePart();
void load_imagePart();
void init_imagePart();
void render_imagePart();
void end_imagePart();

void preload_light();
void load_light();
void init_light();
void render_light();
void end_light();

void preload_objectShader();
void load_objectShader();
void init_objectShader();
void render_objectShader();
void end_objectShader();

void preload_objectShader2();
void load_objectShader2();
void init_objectShader2();
void render_objectShader2();
void end_objectShader2();

void preload_objectMorph();
void load_objectMorph();
void init_objectMorph();
void render_objectMorph();
void end_objectMorph();

void preload_objectMorph2();
void load_objectMorph2();
void init_objectMorph2();
void render_objectMorph2();
void end_objectMorph2();

void preload_particleMatrix2();
void load_particleMatrix2();
void init_particleMatrix2();
void render_particleMatrix2();
void end_particleMatrix2();

void preload_particleMatrix3();
void load_particleMatrix3();
void init_particleMatrix3();
void render_particleMatrix3();
void end_particleMatrix3();
	
void preload_lorentzAttractor();
void load_lorentzAttractor();
void init_lorentzAttractor();
void render_lorentzAttractor();
void end_lorentzAttractor();
	
void preload_objectMatrix();
void load_objectMatrix();
void init_objectMatrix();
void render_objectMatrix();
void end_objectMatrix();
	
void preload_particleTex2();
void load_particleTex2();
void init_particleTex2();
void render_particleTex2();
void end_particleTex2();

void preload_ray();
void load_ray();
void init_ray();
void render_ray();
void end_ray();

void preload_rayMatrix();
void load_rayMatrix();
void init_rayMatrix();
void render_rayMatrix();
void end_rayMatrix();

void preload_sound();
void load_sound();
void init_sound();
void render_sound();
void end_sound();

void preload_saveBuffer();
void load_saveBuffer();
void init_saveBuffer();
void render_saveBuffer();
void end_saveBuffer();

void preload_renderBuffer();
void load_renderBuffer();
void init_renderBuffer();
void render_renderBuffer();
void end_renderBuffer();

void preload_renderShadowMapping();
void load_renderShadowMapping();
void init_renderShadowMapping();
void render_renderShadowMapping();
void end_renderShadowMapping();

void preload_renderFbo();
void load_renderFbo();
void init_renderFbo();
void render_renderFbo();
void end_renderFbo();

void preload_fbobind();
void load_fbobind();
void init_fbobind();
void render_fbobind();
void end_fbobind();

void preload_fbounbind();
void load_fbounbind();
void init_fbounbind();
void render_fbounbind();
void end_fbounbind();

void preload_glslshaderbind();
void load_glslshaderbind();
void init_glslshaderbind();
void render_glslshaderbind();
void end_glslshaderbind();

void preload_glslshaderunbind();
void load_glslshaderunbind();
void init_glslshaderunbind();
void render_glslshaderunbind();
void end_glslshaderunbind();	

// ******************************************************************

// section 0 must be a loading section
// executed while loading demo sections

// sections functions references
tSectionFunction sectionFunction[] = {

	// loading section
	{"loading", &preload_loading, &load_loading, &init_loading, &render_loading, &end_loading },

	// built-in effects
	{"efxAccum",			&preload_efxAccum,				&load_efxAccum,				&init_efxAccum,				&render_efxAccum,			&end_efxAccum			},
	{"efxBlackWhite",		&preload_efxBlackWhite,			&load_efxBlackWhite,		&init_efxBlackWhite,		&render_efxBlackWhite,		&end_efxBlackWhite		},
	{"efxBoxBlur",			&preload_efxBoxBlur,			&load_efxBoxBlur,			&init_efxBoxBlur,			&render_efxBoxBlur,			&end_efxBoxBlur			},
	{"efxFader",			&preload_efxFader,				&load_efxFader,				&init_efxFader,				&render_efxFader,			&end_efxFader			},
	{"efxFog",				&preload_efxFog,				&load_efxFog,				&init_efxFog,				&render_efxFog,				&end_efxFog				},
	{"efxGlow2",			&preload_efxGlow2,				&load_efxGlow2,				&init_efxGlow2,				&render_efxGlow2,			&end_efxGlow2			},
	{"efxHighPassFilter",	&preload_efxHighPassFilter,		&load_efxHighPassFilter,	&init_efxHighPassFilter,	&render_efxHighPassFilter,	&end_efxHighPassFilter	},
	{"efxInverse",			&preload_efxInverse,			&load_efxInverse,			&init_efxInverse,			&render_efxInverse,			&end_efxInverse			},
	{"efxMotionBlur",		&preload_efxMotionBlur,			&load_efxMotionBlur,		&init_efxMotionBlur,		&render_efxMotionBlur,		&end_efxMotionBlur		},
	{"efxMotionBlurAccum",	&preload_efxMotionBlurAccum,	&load_efxMotionBlurAccum,	&init_efxMotionBlurAccum,	&render_efxMotionBlurAccum,	&end_efxMotionBlurAccum	},
	{"efxRadialBlur2",		&preload_efxRadialBlur2,		&load_efxRadialBlur2,		&init_efxRadialBlur2,		&render_efxRadialBlur2,		&end_efxRadialBlur2		},
	{"efxRadialGlow",		&preload_efxRadialGlow,			&load_efxRadialGlow,		&init_efxRadialGlow,		&render_efxRadialGlow,		&end_efxRadialGlow		},
	{"efxRenderMesh2",		&preload_efxRenderMesh2,		&load_efxRenderMesh2,		&init_efxRenderMesh2,		&render_efxRenderMesh2,		&end_efxRenderMesh2		},

	// built-in sections
	{"background",			&preload_background,			&load_background,			&init_background,			&render_background,				&end_background			},
	{"camera",				&preload_camera,				&load_camera,				&init_camera,				&render_camera,					&end_camera				},
	{"camera2",				&preload_camera2,				&load_camera2,				&init_camera2,				&render_camera2,				&end_camera2			},
	{"camera3",				&preload_camera3,				&load_camera3,				&init_camera3,				&render_camera3,				&end_camera3			},
	{"image",				&preload_image,					&load_image,				&init_image,				&render_image,					&end_image				},
	{"image2",				&preload_image2,				&load_image2,				&init_image2,				&render_image2,					&end_image2				},
	{"image2D",				&preload_image2D,				&load_image2D,				&init_image2D,				&render_image2D,				&end_image2D			},
	{"imageMatrix",			&preload_imageMatrix,			&load_imageMatrix,			&init_imageMatrix,			&render_imageMatrix,			&end_imageMatrix		},
	{"imagePart",			&preload_imagePart,				&load_imagePart,			&init_imagePart,			&render_imagePart,				&end_imagePart			},
	{"light",				&preload_light,					&load_light,				&init_light,				&render_light,					&end_light				},
	{"objectMorph",			&preload_objectMorph,			&load_objectMorph,			&init_objectMorph,			&render_objectMorph,			&end_objectMorph		},
	{"objectMorph2",		&preload_objectMorph2,			&load_objectMorph2,			&init_objectMorph2,			&render_objectMorph2,			&end_objectMorph2		},
	{"objectShader",		&preload_objectShader,			&load_objectShader,			&init_objectShader,			&render_objectShader,			&end_objectShader		},
	{"objectShader2",		&preload_objectShader2,			&load_objectShader2,		&init_objectShader2,		&render_objectShader2,			&end_objectShader2		},
	{"objectMatrix",		&preload_objectMatrix,			&load_objectMatrix,			&init_objectMatrix,			&render_objectMatrix,			&end_objectMatrix		},
	{"particleMatrix2",		&preload_particleMatrix2,		&load_particleMatrix2,		&init_particleMatrix2,		&render_particleMatrix2,		&end_particleMatrix2	},
	{"particleMatrix3",		&preload_particleMatrix3,		&load_particleMatrix3,		&init_particleMatrix3,		&render_particleMatrix3,		&end_particleMatrix3	},
	{"particleTex2",		&preload_particleTex2,			&load_particleTex2,			&init_particleTex2,			&render_particleTex2,			&end_particleTex2		},
	{"ray",					&preload_ray,					&load_ray,					&init_ray,					&render_ray,					&end_ray				},
	{"rayMatrix",			&preload_rayMatrix,				&load_rayMatrix,			&init_rayMatrix,			&render_rayMatrix,				&end_rayMatrix			},
	{"sound",				&preload_sound,					&load_sound,				&init_sound,				&render_sound,					&end_sound				},
	{"saveBuffer",			&preload_saveBuffer,			&load_saveBuffer,			&init_saveBuffer,			&render_saveBuffer,				&end_saveBuffer			},
	{"renderBuffer",		&preload_renderBuffer,			&load_renderBuffer,			&init_renderBuffer,			&render_renderBuffer,			&end_renderBuffer		},
	{"fbobind",				&preload_fbobind,				&load_fbobind,				&init_fbobind,				&render_fbobind,				&end_fbobind			},
	{"fbounbind",			&preload_fbounbind,				&load_fbounbind,			&init_fbounbind,			&render_fbounbind,				&end_fbounbind			},
	{"renderFbo",			&preload_renderFbo,				&load_renderFbo,			&init_renderFbo,			&render_renderFbo,				&end_renderFbo			},
	{"glslshaderbind",		&preload_glslshaderbind,		&load_glslshaderbind,		&init_glslshaderbind,		&render_glslshaderbind,			&end_glslshaderbind	    },
	{"glslshaderunbind",	&preload_glslshaderunbind,		&load_glslshaderunbind,		&init_glslshaderunbind,		&render_glslshaderunbind,		&end_glslshaderunbind	},
	{"renderShadowMapping",	&preload_renderShadowMapping,	&load_renderShadowMapping,	&init_renderShadowMapping,	&render_renderShadowMapping,	&end_renderShadowMapping}
};

#define SECTIONS_NUMBER ((sizeof(sectionFunction) / sizeof(tSectionFunction)))
// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
