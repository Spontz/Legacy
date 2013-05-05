#ifndef __TEXT_H
#define __TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

// Font properties
typedef struct {
	char *filename;		// Filename
	char *charset;		// Set of characters composing the texture
	int texture_ID;		// ID of texture
	int charX, charY;	// Num of letters in texture
	float posX,posY;	// Text position
	float size;			// Size of text
	float spacing;		// Spacing between letters
	float R,G,B,A;		// Color, Alpha
} font;

// ******************************************************************

extern font debugFont;
extern char *strDebugFont;

// ******************************************************************

void text_load_font (font *font_props, char *charset, int charX, int charY, char *fname);
void glPrintf(font properties, const char *string, ...);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
