/*
	text.c: text management library
*/

#include "../main.h"
#include "text.h"

// ******************************************************************

static void ViewOrtho();
static void ViewPerspective();

// ******************************************************************

// Carga la fuente deseada
// p -> Propiedades de la fuente, es una variable que se "llena" en esta funcion
// charset -> Juego de caracteres que forman la textura
// t_ID -> Identificador de textura
// charX, charY -> Numero de letras que hay en la textura a lo largo de X y de Y
// fname -> Filename, donde esta la textura
// ******************************************************************

void text_load_font (font *p, char *charset, int charX, int charY, char *fname) {

	p->filename = fname;
	p->charset = charset;

	p->texture_ID = tex_load(p->filename, USE_CACHE, 0);
	tex_properties(p->texture_ID, NO_MIPMAP | CLAMP);
	tex_upload(p->texture_ID, 1);

	p->charX = charX;
	p->charY = charY;
	p->posX = p->posY = 0.0f;
	p->R = p->G = p->B = p->A = 1.0f;
	p->size = (float) tex_array[p->texture_ID]->width / p->charX;
	p->spacing = 1.0f;
	

}

// ******************************************************************

void glDrawChar (font p, char c) {
	char *pStr = p.charset;
	int iPos = 0;
	int i, j;
	float coordX, coordY;

	while (*pStr != c && *pStr)	{
		pStr++;
		iPos++;
	}

	if (*pStr == 0)	return;

	i = iPos % (int)p.charX;						// Columna de la textura, empezando en 0
	j = iPos / (int)p.charY;						// Fila de la textura, empezando en 0

	coordX = (float) (i%(int)p.charX) / p.charX;			// Posición actual de X (textura) (de 0 a 1)
	coordY = (float) (j%(int)p.charY) / p.charY;			// Posición actual de Y (textura) (de 0 a 1)

	glPushMatrix();
		glTranslatef(p.posX,p.posY,0);					// Ens movem a l'dreta
		glBegin(GL_QUADS);								// Un quad per char
			glTexCoord2f(coordX,					coordY                 );	glVertex2f(  0.0f,   0.0f);
			glTexCoord2f(coordX,					coordY + 1.0f / p.charY);	glVertex2f(  0.0f, p.size);
			glTexCoord2f(coordX + 1.0f / p.charX,	coordY + 1.0f / p.charY);	glVertex2f(p.size, p.size);
			glTexCoord2f(coordX + 1.0f / p.charX,	coordY                 );	glVertex2f(p.size,   0.0f);
		glEnd();
	glPopMatrix();
}

// ******************************************************************

void glPrintf(font p, const char *string, ...) {
	char text[256];				// El que tindra el texte, max 256 caracters
	va_list ap;					// Punter a la llista d'arguments
	int i = 0;

	if (string == NULL)	return;

	va_start(ap, string);				// Busca variables
	vsprintf(text, string, ap);			// Converteix els símbols en chars
	va_end(ap);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	tex_bind(p.texture_ID);				// Seleccionamos la textura de la fuente

	ViewOrtho();

	glColor4f(p.R,p.G,p.B,p.A);
	while (text[i])	{
		// Return character handling
		if (text[i] == '\n') {
			p.posX = -p.size;
			p.posY += p.size;
		}

		// Character drawing
		glDrawChar (p, text[i]);
		p.posX += p.size * p.spacing;
		i++;
	}
	
	glDisable(GL_BLEND);

	ViewPerspective();
}

// ******************************************************************

void ViewOrtho() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, glDriver.width, glDriver.height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

// ******************************************************************

void ViewPerspective() {
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
