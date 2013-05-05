#include "files.h"
#include "dkernel.h"
#include <stdio.h>
#include <stdlib.h>

// File loader
// Returns a pointer to an array with the file contents loaded in memory
char* loadFile (const char *path)
	{
	FILE * pFile;
	char *buffer;
	long lSize;
	size_t result;

	pFile = fopen ( path , "rb" );
	if (pFile == NULL)
		{
		dkernel_error("loadFile: File error: '%s'", path);
		return 0;
		}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	// we allocate one character more to ensure the returned string terminates in zero
	buffer = (char*) calloc (lSize + 1, sizeof(char));
	if (buffer == NULL) dkernel_error ("loadFile: Memory error");

	// copy the file into the buffer:
	result = fread (buffer, 1, lSize, pFile);
	if (result != lSize) dkernel_error ("loadFile: Reading error");

	/* the whole file is now loaded in the memory buffer. */

	// terminate
	fclose (pFile);

	return buffer;
	}

// File sizer
// Returns the size in bytes of the file
long sizeFile (const char* path)
	{
	FILE * pFile;
	long lSize;

	pFile = fopen ( path , "rb" );
	if (pFile == NULL)
		{
		dkernel_error("loadFile: File error: '%s'", path);
		return 0;
		}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);	// TODO: Could be optimized removing this line, it SHOULD not be necessary

	// terminate
	fclose (pFile);

	return lSize;
	}
