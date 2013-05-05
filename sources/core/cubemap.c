/*
	cubemap.c: create a normalization cube map texture
*/

#include "../main.h"

// ******************************************************************

int cube_map = -1;

// ******************************************************************

void generateNormalizationCubeMap() {

	int i, j, size = 32;
	float offset = 0.5f;
	float halfSize = 16.0f;
	vector_t tempVector, midVector = {offset, offset, offset};
	unsigned char *bytePtr;
	unsigned char *data;

	// allocate a temporary buffer
	data = (unsigned char *) malloc(32*32*3);

	// positive x
	bytePtr = data;
	for (j=0; j<size; j++) {

		for(i=0; i<size; i++) {
			tempVector.x = halfSize;
			tempVector.y = -(j+offset-halfSize);
			tempVector.z = -(i+offset-halfSize);

			vector_normalize(&tempVector);
			vector_scale(&tempVector, 0.5f, &tempVector);
			vector_add(&tempVector, &midVector, &tempVector);

			bytePtr[0] = (unsigned char) (tempVector.x*255);
			bytePtr[1] = (unsigned char) (tempVector.y*255);
			bytePtr[2] = (unsigned char) (tempVector.z*255);

			bytePtr += 3;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// negative x
	bytePtr = data;
	for (j=0; j<size; j++) {

		for(i=0; i<size; i++) {
			tempVector.x = -halfSize;
			tempVector.y = -(j+offset-halfSize);
			tempVector.z = i+offset-halfSize;

			vector_normalize(&tempVector);
			vector_scale(&tempVector, 0.5f, &tempVector);
			vector_add(&tempVector, &midVector, &tempVector);

			bytePtr[0] = (unsigned char) (tempVector.x*255);
			bytePtr[1] = (unsigned char) (tempVector.y*255);
			bytePtr[2] = (unsigned char) (tempVector.z*255);

			bytePtr += 3;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// positive y
	bytePtr = data;
	for (j=0; j<size; j++) {

		for(i=0; i<size; i++) {
			tempVector.x = i+offset-halfSize;
			tempVector.y = halfSize;
			tempVector.z = j+offset-halfSize;

			vector_normalize(&tempVector);
			vector_scale(&tempVector, 0.5f, &tempVector);
			vector_add(&tempVector, &midVector, &tempVector);

			bytePtr[0] = (unsigned char) (tempVector.x*255);
			bytePtr[1] = (unsigned char) (tempVector.y*255);
			bytePtr[2] = (unsigned char) (tempVector.z*255);

			bytePtr += 3;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// negative y
	bytePtr = data;
	for (j=0; j<size; j++) {

		for(i=0; i<size; i++) {
			tempVector.x = i+offset-halfSize;
			tempVector.y = -halfSize;
			tempVector.z = -(j+offset-halfSize);

			vector_normalize(&tempVector);
			vector_scale(&tempVector, 0.5f, &tempVector);
			vector_add(&tempVector, &midVector, &tempVector);

			bytePtr[0] = (unsigned char) (tempVector.x*255);
			bytePtr[1] = (unsigned char) (tempVector.y*255);
			bytePtr[2] = (unsigned char) (tempVector.z*255);

			bytePtr += 3;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// positive z
	bytePtr = data;
	for (j=0; j<size; j++) {

		for(i=0; i<size; i++) {
			tempVector.x = i+offset-halfSize;
			tempVector.y = -(j+offset-halfSize);
			tempVector.z = halfSize;

			vector_normalize(&tempVector);
			vector_scale(&tempVector, 0.5f, &tempVector);
			vector_add(&tempVector, &midVector, &tempVector);

			bytePtr[0] = (unsigned char) (tempVector.x*255);
			bytePtr[1] = (unsigned char) (tempVector.y*255);
			bytePtr[2] = (unsigned char) (tempVector.z*255);

			bytePtr += 3;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// negative z
	bytePtr = data;
	for (j=0; j<size; j++) {

		for(i=0; i<size; i++) {
			tempVector.x = -(i+offset-halfSize);
			tempVector.y = -(j+offset-halfSize);
			tempVector.z = -halfSize;

			vector_normalize(&tempVector);
			vector_scale(&tempVector, 0.5f, &tempVector);
			vector_add(&tempVector, &midVector, &tempVector);

			bytePtr[0] = (unsigned char) (tempVector.x*255);
			bytePtr[1] = (unsigned char) (tempVector.y*255);
			bytePtr[2] = (unsigned char) (tempVector.z*255);

			bytePtr += 3;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	free(data);
}

// ******************************************************************

void setupNormalizationCubeMap() {

	GLuint v = (GLuint)cube_map;
	glGenTextures(1, &v);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cube_map);
	generateNormalizationCubeMap();
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, 0);
}
