/*
	tga.c: TGA graphic format code
*/

#include <stdio.h>
#include <string.h>
#include "tga.h"

// ******************************************************************

typedef struct _TargaHeader {
	unsigned char id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char pixel_size, attributes;
} TargaHeader;

// ******************************************************************

void write_tga_rle_data (FILE *f, char *buffer, int width, int height) {

	static char salida[1024*3];
	char bufaux[1024*3];
	char *ptr, *aux, *end;
	short repeat, different;
	char cr, cb, cg;

	ptr = salida;
	end = buffer + width * height * 3 + 1;
	cr = buffer++[0];
	cg = buffer++[0];
	cb = buffer++[0];
	while (buffer < end) {

		repeat = 1;
		while ((cr == buffer[0]) && (cg == buffer[1]) && (cb == buffer[2])
				&& (repeat < 127)) {

			buffer += 3;
			repeat++;

			if (buffer >= end) break;
		}

		// run pixels
		if (repeat > 1) {
			*ptr++ = 128 | (repeat - 1);
			*ptr++ = cb;
			*ptr++ = cg;
			*ptr++ = cr;

			if (buffer < end) {
				cr = buffer[0];
				cg = buffer[1];
				cb = buffer[2];
				buffer += 3;
			}
		}

		if (buffer >= end) break;

		aux = bufaux;
		different = 0;
		while (((cr != buffer[0]) || (cg != buffer[1]) || (cb != buffer[2]))
				&& (different < 127)) {

			*aux++ = cb;
			*aux++ = cg;
			*aux++ = cr;
			cr = buffer++[0];
			cg = buffer++[0];
			cb = buffer++[0];
			different++;

			if (buffer >= end) break;
		}

		// non-run pixels
		if (different > 0) {
			*ptr++ = different - 1;
			memcpy (ptr, bufaux, 3*different);
			ptr += 3*different;
		}

		// flush output
		if (ptr-salida > width*3) {
			fwrite (salida, ptr-salida, 1, f);
			ptr = salida;
		}
	}

	// finish output
	fwrite (salida, ptr-salida, 1, f);
}

// ******************************************************************

void write_tga_header (FILE *f, int width, int height, int rle) {

	char header[18];

	memset (header, 0, 18);

	if (rle)
		header[2] = 10; // rle compressed
	else
		header[2] = 2; // uncompressed

	header[12] = width & 255;
	header[13] = width >> 8;
	header[14] = height & 255;
	header[15] = height >> 8;

	header[16] = 24;

	fwrite (header, 18, 1, f);
}

// ******************************************************************

void tga_save (char *name, char *buffer, int width, int height, int rle) {

	int i, j, temp;
	char *in;
	FILE *f;

	f = fopen (name, "wb");
	if (!f) return;

	write_tga_header (f, width, height, rle);

	if (rle) {

		// encode buffer
		write_tga_rle_data (f, buffer, width, height);
	}

	else {
		// swap rgb to bgr
		for (i=0; i<height; i++) {

			in = buffer + i * width * 3;
			for (j=0; j<width*3; j+=3) {
				temp = in[j];
				in[j] = in[j+2];
				in[j+2] = temp;
			}

			fwrite (in, width * 3, 1, f);
		}
	}

	fclose(f);
}

